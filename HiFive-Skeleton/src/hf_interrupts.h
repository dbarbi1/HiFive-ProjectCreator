// See LICENSE for license details.

#ifndef PLIC_SYSTEM_H
#define PLIC_SYSTEM_H

//typedef for plic handler function pointers
typedef void (*interrupt_function_ptr_t) (void);



/*Entry Point for PLIC Interrupt Handler*/
void handle_m_ext_interrupt();
/*Entry Point for Machine Timer Interrupt Handler*/
void handle_m_time_interrupt();

void set_mtime_interrupt(uint64_t msecs, interrupt_function_ptr_t tick_handler);

/*
 *enables the plic and programs handlers
**/
void interrupts_init( ) ;




#endif 
