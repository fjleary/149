// Blink app
//
// Blinks the LEDs on Buckler

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"

#include "buckler.h"
#include "gpio.h"

int main(void) {
  typedef struct {
    uint32_t OUT;
    uint32_t OUTSET;
    uint32_t OUTCLR;
    uint32_t IN;
    uint32_t DIRECT;
    uint32_t DIRSET;
    uint32_t DIRCLR;
    uint32_t LATCH;
    uint32_t DETECTMODE;
    uint32_t reserved[118];
    uint32_t PIN_CNF[32];
  } gpio_struct_t;

  ret_code_t error_code = NRF_SUCCESS;

  gpio_struct_t* gpio = 0x50000504;
  uint32_t* GPIO_OUT = (uint32_t*) 0x50000504;
  uint32_t* GPIO_DIR = (uint32_t*) 0x50000514;
  uint8_t LED0 = 25;
  uint8_t LED1 = 24;
  uint8_t LED2 = 23;
  uint8_t BUTTON0 = 28;
  uint8_t SWITCH0 = 22;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");

  printf("GPIO_OUT address: %p\n", GPIO_OUT);
  printf("GPIO_OUT value: %i\n", *GPIO_OUT);
  printf("GPIO_DIR address: %p\n", GPIO_DIR);
  printf("GPIO_DIR value: %i\n", *GPIO_DIR);

  // Set IO direction
  gpio->PIN_CNF[22] = 0x00020000;
  gpio_config(BUTTON0, INPUT);
  //*GPIO_DIR = *GPIO_DIR | (1 << LED2);
  gpio_config(LED0, OUTPUT);
  gpio_config(LED1, OUTPUT);
  gpio_config(LED2, OUTPUT);

  // loop forever
  while (1) {
    // Toggle LED ON
    *GPIO_OUT |= (1 << LED2);

    // Read BUTTON0 and SWITCH0
    uint32_t SWITCH0_state = (gpio->IN >> 22) & 0x1;
    uint32_t BUTTON0_state = (gpio->IN >> 28) & 0x1;
    printf("SWITCH0 value: %i\n", SWITCH0_state);
    printf("BUTTON0 value: %i\n", BUTTON0_state);
    printf("\n");

    // Use library functions to read button and LED
    bool lib_button = gpio_read(BUTTON0);
    bool lib_switch = gpio_read(SWITCH0);
    printf("lib_button value: %i\n", lib_button);
    if (lib_button) {
      gpio_set(LED0);
    } else {
      gpio_clear(LED0);
    }
    if (lib_switch) {
      gpio_set(LED1);
    } else {
      gpio_clear(LED1);
    }

    nrf_delay_ms(10);
    // Toggle LED OFF
    *GPIO_OUT &= ~(1 << LED2);
    
    nrf_delay_ms(10);
  }
}

