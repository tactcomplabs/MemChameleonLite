/* _MCL_DRIVER_C_
 *
 * MemChameleonLite Driver Routines
 *
 * Copyright 2017, Tactical Computing Laboratories, LLC
 * contact@tactcomplabs.com
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include <string.h>
#include "plic/plic_driver.h"
#include "encoding.h"
#include <unistd.h>
#include "stdatomic.h"
#include "mcl_const.h"

/* ---------------------------------------------------- FUNCTIONS */
typedef void (*function_ptr_t) (void);
void no_interrupt_handler (void) {};
function_ptr_t g_ext_interrupt_handlers[PLIC_NUM_INTERRUPTS];
extern uint32_t PacketFlitsInBytes(uint8_t byte);
extern int ProcessPacket();

/* ---------------------------------------------------- GLOBALS */
uint8_t *__STOR;
uint64_t __REG[MCL_NUM_REG];
uint64_t __COUNTER[MCL_NUM_COUNTER];
uint64_t __RAW[MCL_MAX_PACKET_SIZE];

plic_instance_t g_plic;
const char *splash_msg = " \
\n\
                ,(#%#(,\n\
         ,@@@@@@@@@@@@@@@@@@@@@,\n\
      @@@@@@@@@@@  %    @@@@@@@@@@@\n\
    @@@*       @   @@#  #@   @@@@@@@@\n\
  ,@@@@@@@   @@@   @@@&&@@   @@@@@@@@@,\n\
 .@@@@@@@@   @@@   @@@@@@@   @@@@@@@@@@.\n\
 @@@@@@@@@   @@@   @@#  #@   ..  #@@@@@@\n\
 @@@@@@@@@@@@@@@@  %    @@@@@@@@@@@@@@@@\n\
 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\
 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\
 ,@@@@@@ &@@@@@@@@@@@@@@@@@@@@%.(@@@@@@\n\
  &@@@@@    (@@@@@@@@@@@@@@@      @@@@\n\
   ,@@@@       ,@@@@@@@@@*       &@@@\n\
     @@@@      &@@@@@@@@@@  */##@@@@\n\
     @@@@@@@@@@@@@@   @@@@@@@@@@@@@@\n\
     ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.\n\
       &@@@@@@@@@@@@@@@@@@@@@@@&\n\
             @@@@@@@@@@@@@@@\n\
             @@& @@& @@& &@@\n\
             @@& @@& @@& &@@\n\
             @@& @@& @@& &@@\n\
             @@& @@& @@& &@@\n\
             @@& @@& @@& &@@\n\
\n\
             MemChameleonLite\n\
   TACTICAL COMPUTING LABORATORIES LLC\n\
\n\
";

/* ---------------------------------------------------- PRINT_SPLASH */
static void print_splash(){
  write( STDOUT_FILENO, splash_msg, strlen(splash_msg));
}

/* ---------------------------------------------------- RESET_COUNTERS */
static void reset_counters(){
  uint64_t i = 0x00ull;

  for( i=0; i<MCL_NUM_COUNTER; i++ ){
    __COUNTER[i] = 0x00ull;
  }
}

/* ---------------------------------------------------- BUTTON_0_HANDLER */
static void button_0_handler(void){
  /* -- reset packet counters */
  reset_counters();
}

/* ---------------------------------------------------- HANDLE_M_EXT_INTERRUPT */
void handle_m_ext_interrupt(){
  plic_source int_num  = PLIC_claim_interrupt(&g_plic);
  if((int_num >= 1) && (int_num < PLIC_NUM_INTERRUPTS)){
    g_ext_interrupt_handlers[int_num]();
  }else{
    exit(1+(uintptr_t)int_num);
  }
  PLIC_complete_interrupt(&g_plic, int_num);
}

/* ---------------------------------------------------- HANDLE__TIME_INTERRUPT */
void handle_m_time_interrupt(){
  clear_csr(mie, MIP_MTIP);

  /* reset the timer for 3s in the future */
  /* also clears the existing timer interrupt */
  volatile uint64_t * mtime       = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 1.5 * RTC_FREQUENCY;
  *mtimecmp = then;

  /* re-enable the timer interrupt */
  set_csr(mie, MIP_MTIP);
}

/* ---------------------------------------------------- RESET_DRIVER */
static void reset_driver(){

  int i = 0;
  unsigned long long m = 0;

  /* -- stage 1: disable the machine and interrupts until reset is complete */
  clear_csr(mie, MIP_MEIP);
  clear_csr(mie, MIP_MTIP);
  for( i=0; i<PLIC_NUM_INTERRUPTS; i++ ){
    g_ext_interrupt_handlers[i] = no_interrupt_handler;
  }

  /* -- stage 2: splash the logo */
#ifndef DISABLE_SPLASH
  print_splash();
#endif

  /* -- stage 3: reset the counters */
  reset_counters();

  /* -- stage 4: setup the buttons */
#ifdef HAS_BOARD_BUTTONS
  g_ext_interrupt_handlers[INT_DEVICE_BUTTON_0] = button_0_handler;
  PLIC_enable_interrupt (&g_plic, INT_DEVICE_BUTTON_0);
  PLIC_set_priority(&g_plic, INT_DEVICE_BUTTON_0, 1);
  GPIO_REG(GPIO_RISE_IE) |= (1 << BUTTON_0_OFFSET);
#endif

  volatile uint64_t * mtime       = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 1.5*RTC_FREQUENCY;
  *mtimecmp = then;

  /* -- stage 5: init the registers */
  __REG[MCL_CUB_REG]    = (uint64_t)(MCL_CUB);
  __REG[MCL_BLOCK_REG]  = (uint64_t)(MCL_MAX_BLOCK_SZ);

  __STOR = malloc( sizeof( uint8_t ) * MCL_SIZE );
  if( __STOR == NULL ){
    exit(0xF);
  }

  /* -- stage 6: init the backing memory */
  for( m=0; m<MCL_SIZE; m++ ){
    __STOR[m] = 0x0;
  }

  /* -- stage 7: enable the machine external bits & timers */
  set_csr(mie, MIP_MEIP);
  set_csr(mie, MIP_MTIP);
  set_csr(mstatus, MSTATUS_MIE);
}

/* ---------------------------------------------------- MAIN */
int main( int argc, char **argv ){

  /* -- setup the GPIOs */
#ifdef HAS_BOARD_BUTTONS
  GPIO_REG(GPIO_OUTPUT_EN)  &= ~((0x1 << BUTTON_0_OFFSET));
  GPIO_REG(GPIO_PULLUP_EN)  &= ~((0x1 << BUTTON_0_OFFSET));
  GPIO_REG(GPIO_INPUT_EN)   |=  ((0x1 << BUTTON_0_OFFSET));
#endif

  /* -- setup the LEDs */
  GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_OFFSET)
                                  | (0x1<< GREEN_LED_OFFSET)
                                  | (0x1 << BLUE_LED_OFFSET));
  GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_OFFSET)
                                  | (0x1<< GREEN_LED_OFFSET)
                                  | (0x1 << BLUE_LED_OFFSET));
  GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << BLUE_LED_OFFSET);
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1<< RED_LED_OFFSET)
                                   | (0x1<< GREEN_LED_OFFSET));

  /* -- setup the request pins */
  /*
   * The packet I/O interface
   * is setup using the GPIO pins
   * as an 8-bit (1-byte) block.
   * Reading a single HMC FLIT off
   * the GPIO requires to read cycles.
   * All packets are ordered (LSB to MSB).
   * The lowest GPIO pin contains the LSB.
   *
   * -- Pin Mapping --
   * [Bit]  : [Pin]
   *  0     : GPIO_PIN_0
   *  1     : GPIO_PIN_1
   *  2     : GPIO_PIN_2
   *  3     : GPIO_PIN_3
   *  4     : GPIO_PIN_4
   *  5     : GPIO_PIN_5
   *  6     : GPIO_PIN_6
   *  7     : GPIO_PIN_7
   *
   */
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_0_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_1_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_2_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_3_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_4_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_5_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_6_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (0x1 << PIN_7_OFFSET);

  uint32_t rqst_mask = ((0x1 << PIN_0_OFFSET)|(0x1 << PIN_1_OFFSET)|
                        (0x1 << PIN_2_OFFSET)|(0x1 << PIN_3_OFFSET)|
                        (0x1 << PIN_4_OFFSET)|(0x1 << PIN_5_OFFSET)|
                        (0x1 << PIN_6_OFFSET)|(0x1 << PIN_7_OFFSET));

  /* -- setup the response pins */
  /*
   * The packet I/O interface
   * is setup using the GPIO pins
   * as an 8-bit (1-byte) block.
   * Writing a single HMC FLIT to
   * the GPIO requires to read cycles.
   * All packets are ordered (LSB to MSB).
   * The lowest GPIO pin contains the LSB.
   *
   * -- Pin Mapping --
   * [Bit]  : [Pin]
   *  0     : GPIO_PIN_8
   *  1     : GPIO_PIN_9
   *  2     : GPIO_PIN_10
   *  3     : GPIO_PIN_11
   *  4     : GPIO_PIN_12
   *  5     : GPIO_PIN_13
   *  6     : GPIO_PIN_15
   *  7     : GPIO_PIN_16
   *
   */
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_8_OFFSET);
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_9_OFFSET);
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_10_OFFSET);
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_11_OFFSET);
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_12_OFFSET);
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_13_OFFSET);
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_15_OFFSET);
  GPIO_REG(GPIO_INPUT_EN) |= (0x1 << PIN_16_OFFSET);

  uint32_t rsp_mask = ((0x1 << PIN_8_OFFSET)|(0x1 << PIN_9_OFFSET)|
                       (0x1 << PIN_10_OFFSET)|(0x1 << PIN_11_OFFSET)|
                       (0x1 << PIN_12_OFFSET)|(0x1 << PIN_13_OFFSET)|
                       (0x1 << PIN_15_OFFSET)|(0x1 << PIN_16_OFFSET));

  /* -- setup the PLIC */
  PLIC_init(&g_plic,
            PLIC_BASE_ADDR,
            PLIC_NUM_INTERRUPTS,
            PLIC_NUM_PRIORITIES );

  /* -- initiate driver & board reset */
  reset_driver();

  /* -- u-controller loop */
  uint32_t i    = 0;
  uint8_t byte  = 0x0;  /* byte buffer for GPIO */
  uint32_t iter = 0x0;  /* byte iterator */
  uint32_t curb = 8;    /* current byte position in the packet */
  uint32_t curw = 0;    /* current word position in the packet */
  uint32_t rsp  = 0;    /* response length in bytes */
  while(1){

    /* -- stage1 : pull the first byte of the transfer */
    byte = (uint8_t)(atomic_fetch_xor_explicit(&GPIO_REG(GPIO_OUTPUT_VAL),
                              rqst_mask, memory_order_relaxed));

    /* -- stage2 : decode the command structure */
    iter = PacketFlitsInBytes(byte);
    if( iter == 0 ){
      /* error occurred */
      write( STDOUT_FILENO, "PACKET ERROR", strlen("PACKET_ERROR"));
    }

    /* -- stage3 : pull the remainder of the bytes */
    __RAW[0] |= (uint64_t)(byte);
    for( i=1; i<iter; i++ ){
      byte = (uint8_t)(atomic_fetch_xor_explicit(&GPIO_REG(GPIO_OUTPUT_VAL),
                                              rqst_mask, memory_order_relaxed));
      __RAW[curw] |= ((uint64_t)(byte)<<curb);
      curb+=8;
      if( curb == 64 ){
        curb = 0;
        curw++;
      }
    }

    /* -- stage4 : process the packet */
    if( (rsp = ProcessPacket()) < 0 ){
      /* error occurred */
      write( STDOUT_FILENO, "PACKET ERROR", strlen("PACKET_ERROR"));
    }

    /* -- stage5 : send the response */
    curb = 0;
    curw = 1;
    for( i=0; i<rsp; i++ ){
      byte = ((__RAW[curw]>>curb)&0xFF);
      (uint8_t)(atomic_fetch_xor_explicit(&GPIO_REG(GPIO_INPUT_VAL),
                                    (((uint64_t)(byte)<<8)&rsp_mask),
                                    memory_order_relaxed));
      curb+=8;
      if( curb == 64 ){
        curb = 0;
        curw++;
      }
    }

    /* -- stage6 : cleanup */
    for( i=0; i<MCL_MAX_PACKET_SIZE; i++ ){
      __RAW[i] = 0x00ull;
    }
    curb = 8;
    curw = 0;
  }

  return 0;
}

/* EOF */
