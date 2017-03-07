// See LICENSE for license details.
//#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include "plic/plic_driver.h"
#include "hf_interrupts.h"
#include "encoding.h"
#include <unistd.h>
#include <string.h>

//mtime tick freq
static uint64_t mtime_tick_freq;
interrupt_function_ptr_t mtick_handler;


// Instance data for the PLIC.
plic_instance_t g_plic;

//empty function for unimplemented handler
static void default_handler () { 
  const char* default_handler_message = "Default Handler\n";
  write(STDOUT_FILENO, default_handler_message, strlen(default_handler_message));
}
//interrupt vectors
interrupt_function_ptr_t g_ext_interrupt_handlers[PLIC_NUM_INTERRUPTS];


/*Entry Point for PLIC Interrupt Handler*/
void handle_m_ext_interrupt(){
  plic_source int_num  = PLIC_claim_interrupt(&g_plic);
  if ((int_num >=1 ) && (int_num < PLIC_NUM_INTERRUPTS)) {
    g_ext_interrupt_handlers[int_num]();
  }
  else {
    exit(1 + (uintptr_t) int_num);
  }
  PLIC_complete_interrupt(&g_plic, int_num);
}


/*Entry Point for Machine Timer Interrupt Handler*/
void handle_m_time_interrupt(){

  clear_csr(mie, MIP_MTIP);

  // Reset the timer for 3s in the future.
  // This also clears the existing timer interrupt.

  volatile uint64_t * mtime       = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + mtime_tick_freq;
  *mtimecmp = then;

  //call tick tick function
  mtick_handler();

  // Re-enable the timer interrupt.
  set_csr(mie, MIP_MTIP);

}


void set_mtime_interrupt(uint64_t msecs, interrupt_function_ptr_t tick_handler) {

  //floor of RTC_FREQ/1000
  mtime_tick_freq = msecs*(RTC_FREQ/1000);
  mtick_handler = tick_handler;

  // Set the machine timer to go off in 3 seconds.
  volatile uint64_t * mtime       = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + mtime_tick_freq;
  *mtimecmp = then; 

    // Enable the Machine-Timer bit in MIE
    set_csr(mie, MIP_MTIP);
}

//enables interrupt and assigns handler
void enable_interrupt(uint32_t int_num, uint32_t int_priority, interrupt_function_ptr_t handler) {
    g_ext_interrupt_handlers[int_num] = handler;
    PLIC_set_priority(&g_plic, int_num, int_priority);
    PLIC_enable_interrupt (&g_plic, int_num);
}

/*
 *enables the plic and programs handlers
**/
void interrupts_init(  ) {

    // Disable the machine & timer interrupts until setup is done.
    clear_csr(mie, MIP_MEIP);
    clear_csr(mie, MIP_MTIP);


  /**************************************************************************
   * Set up the PLIC
   *
   *************************************************************************/
  PLIC_init(&g_plic,
	    PLIC_BASE_ADDR,
	    PLIC_NUM_INTERRUPTS,
	    PLIC_NUM_PRIORITIES);

  //assign interrupts to defaul handler
  for (int ii = 0; ii < PLIC_NUM_INTERRUPTS; ii ++){
    g_ext_interrupt_handlers[ii] = default_handler;;
  }

      // Enable the Machine-External bit in MIE
    set_csr(mie, MIP_MEIP);

    // Enable interrupts in general.
    set_csr(mstatus, MSTATUS_MIE);

}
