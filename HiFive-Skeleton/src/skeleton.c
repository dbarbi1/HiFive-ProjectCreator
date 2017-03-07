// See LICENSE for license details.

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include <string.h>
#include "hf_interrupts.h"
#include "encoding.h"
#include <unistd.h>
#include "stdatomic.h"

#define RTC_FREQUENCY 32768
#define LED_OFF 0
#define LED_ON 65535

void hifive_init (void);
void led_control(uint32_t rgb, uint32_t value); 


const char * instructions_msg = " \
\n\
               SIFIVE, INC.\n\
\n\
         5555555555555555555555555\n\
        5555                   5555\n\
       5555                     5555\n\
      5555                       5555\n\
     5555       5555555555555555555555\n\
    5555       555555555555555555555555\n\
   5555                             5555\n\
  5555                               5555\n\
 5555                                 5555\n\
5555555555555555555555555555          55555\n\
 55555           555555555           55555\n\
   55555           55555           55555\n\
     55555           5           55555\n\
       55555                   55555\n\
         55555               55555\n\
           55555           55555\n\
             55555       55555\n\
               55555   55555\n\
                 555555555\n\
                   55555\n\
                     5\n\
\n\
Drew's Skeleton program\n\
\n";

void print_instructions() {

  write (STDOUT_FILENO, instructions_msg, strlen(instructions_msg));

}


//handler for mtick timer
void tick_handler() {
  static int state = 1;

  if(state) {
    led_control(GREEN_LED_OFFSET, LED_ON/2);
    state = 0;
  } else  {
    led_control(GREEN_LED_OFFSET,LED_OFF);
    state = 1;
  }
}

/*
 *configures HiFive leds for blinking
 */
void led_init() {

    // Set up RGB PWM
    //clear config register
    PWM1_REG(PWM_CFG)   = 0;
    //turn on and compare to center
    PWM1_REG(PWM_CFG)   = (PWM_CFG_ENALWAYS) | (PWM_CFG_CMP2CENTER);
    PWM1_REG(PWM_COUNT) = 0;

    //dont use compare 0
    PWM1_REG(PWM_CMP0)  = 0;

    //set gpio to other funciton
    GPIO_REG(GPIO_IOF_SEL)    |= ( (1 << GREEN_LED_OFFSET) | (1 << BLUE_LED_OFFSET) | (1 << RED_LED_OFFSET));
    GPIO_REG(GPIO_IOF_EN )    |= ( (1 << GREEN_LED_OFFSET) | (1 << BLUE_LED_OFFSET) | (1 << RED_LED_OFFSET));
    GPIO_REG(GPIO_OUTPUT_XOR) &= ~( (1 << GREEN_LED_OFFSET) | (1 << BLUE_LED_OFFSET)| (1<< GREEN_LED_OFFSET));

    //turn off for now
    PWM1_REG(PWM_CMP1)  = 0; // PWM is low on the left, GPIO is low on the left side, LED is ON on the left.
    PWM1_REG(PWM_CMP2)  = 0; // PWM is high on the middle, GPIO is low in the middle, LED is ON in the middle.
    PWM1_REG(PWM_CMP3)  = 0; // PWM is low on the left, GPIO is low on the right, LED is on on the right.
}

/*
**Interrupt Service Routine Called when wake button is pushed
*/
void wake_ISR() {
  static int state = 0;
  const char* default_handler_message = "Wake Handler\n";
  write(STDOUT_FILENO, default_handler_message, strlen(default_handler_message));


  if(state) {
    led_control(RED_LED_OFFSET, LED_OFF);
    state = 0;
  }else   {
    led_control(RED_LED_OFFSET, LED_ON/2);
    state = 1;
  }
  //clear irq - interrupt pending register is write 1 to clear
  GPIO_REG(GPIO_FALL_IP) |= (1<<PIN_2_OFFSET);
}

/*
**configures the wake button for irq trigger
**requires that the wake pin is connected to pin2
*/
void wake_irq_init()  {

    //dissable hw io function
    GPIO_REG(GPIO_IOF_EN )    &=  ~(1 << PIN_2_OFFSET);

    //set to input
    GPIO_REG(GPIO_INPUT_EN)   |= (1<<PIN_2_OFFSET);
    GPIO_REG(GPIO_PULLUP_EN)  |= (1<<PIN_2_OFFSET);

    //set to interrupt on falling edge
    GPIO_REG(GPIO_FALL_IE)    |= (1<<PIN_2_OFFSET);

    enable_interrupt(INT_GPIO_BASE+PIN_2_OFFSET, 2, &wake_ISR);

}



void led_control(uint32_t rgb, uint32_t value)  {

  switch (rgb)
  {
      case GREEN_LED_OFFSET:
        PWM1_REG(PWM_CMP1) = value;
        break;
      case BLUE_LED_OFFSET:
        PWM1_REG(PWM_CMP2) = value;
        break;
      case RED_LED_OFFSET:
        PWM1_REG(PWM_CMP3) = value;
        break;  
      default:
        break;
  }


}


void hifive_init () {

    interrupts_init();

    //set the tick freq and handler
    set_mtime_interrupt(1000, &tick_handler);

    led_init();
    wake_irq_init();

    //print logo
    print_instructions();

}

int main(int argc, char **argv)
{

  hifive_init();

  while (1){ 
    asm("wfi");
  }

  return 0;

}
