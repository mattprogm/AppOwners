//
//  pinMap.c
//  AppOwners
//
//  Created by Matheus Henrique on 14/04/25.
//

#include "pinMap.h"


#define PIN_LED             GPIO_NUM_1
#define PIN_LED             GPIO_NUM_2
#define PIN_LED             GPIO_NUM_3
#define PIN_LED             GPIO_NUM_4
#define PIN_LED             GPIO_NUM_7
#define PIN_LED             GPIO_NUM_6



#define PIN_PULSE_DRY_1     GPIO_NUM_13
#define PIN_PULSE_DRY_2     GPIO_NUM_14

//GPIOS USBC Dragino
#define PIN_PULSE_DRY_3     GPIO_NUM_12  // SDA
#define PIN_PULSE_DRY_4     GPIO_NUM_11  // SCL


#define RS485_RX_GPIO       GPIO_NUM_18
#define RS485_TX_GPIO       GPIO_NUM_17

#define LORA_MISO_GPIO     GPIO_NUM_16
#define LORA_MOSI_GPIO     GPIO_NUM_15
#define LORA_SCK_GPIO      GPIO_NUM_6
#define LORA_RST_GPIO      GPIO_NUM_5
#define LORA_NSS_GPIO      GPIO_NUM_7
#define LORA_DIO0_GPIO     GPIO_NUM_9
#define LORA_DIO1_GPIO     GPIO_NUM_46
#define LORA_DIO2_GPIO     GPIO_NUM_3


#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_NUM I2C_NUM_1
#define SHT31_ADDRESS 0x44


#define DEBOUNCE_DELAY 140000
