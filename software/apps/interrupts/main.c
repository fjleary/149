// Blink app
//
// Blinks the LEDs on Buckler

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "software_interrupt.h"

#include "buckler.h"
#include "gpio.h"

uint8_t LED0 = 25;

void SWI1_EGU1_IRQHandler(void) {
    NRF_EGU1->EVENTS_TRIGGERED[0] = 0;
    printf("Software interrupt triggered.\n");
    nrf_delay_ms(1500);
    printf("Software interrupt finished.\n");
}

void GPIOTE_IRQHandler(void) {
    NRF_GPIOTE->EVENTS_IN[0] = 0;
    printf("BUTTON0 interrupt triggered.\n");
    gpio_set(LED0);
    nrf_delay_ms(500);
    gpio_clear(LED0);
    printf("BUTTON0 interrupt finished.\n");
}

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");

  uint8_t BUTTON0 = 28;
  uint8_t SWITCH0 = 22;

  gpio_config(BUTTON0, INPUT);
  gpio_config(SWITCH0, INPUT);
  gpio_config(LED0, OUTPUT);

  // MODE: Event
  NRF_GPIOTE->CONFIG[0] |= 1;
  // PSEL: BUTTON0
  NRF_GPIOTE->CONFIG[0] |= BUTTON0 << 8;
  // POLARITY: HiToLo
  NRF_GPIOTE->CONFIG[0] |= 2 << 16;

  // Enable the GPIOTE Interrupt for Event[0] using the INTENSET register.
  NRF_GPIOTE->INTENSET |= 1; 
  
  // Enable interrupts
  NVIC_EnableIRQ(GPIOTE_IRQn); 
  software_interrupt_init();
 
  // Set interrupt priorities
  NVIC_SetPriority(SWI1_EGU1_IRQn, 1);
  NVIC_SetPriority(GPIOTE_IRQn, 0);

  while(1) {
    // loop forever
    while (1) {
      __WFI();
      printf("Looping __WFI \n");
      nrf_delay_ms(1000);
      if (!gpio_read(SWITCH0)) break;
      printf("SWITCH value: %i \n", gpio_read(SWITCH0));
    }
    while (1) {
      printf("Looping\n");
      software_interrupt_generate();
      nrf_delay_ms(1000);
      if (gpio_read(SWITCH0))  break;
      printf("SWITCH0 value: %i\n", gpio_read(SWITCH0));
    } 
  } 
}

