/*
 * main.c
 *
 *  Created on: Oct 18, 2021
 *      Author: peppe
 */


typedef unsigned long uint32_t;

/* Memory and peripheral start addresses (common to all STM32 MCUs) */
#define FLASH_BASE  0x08000000
#define SRAM_BASE   0x20000000
#define PERIPH_BASE 0x40000000

/* Work out end of RAM address as initial stack pointer
* (specific of a given STM32 MCU */
#define SRAM_SIZE 128*1000 // STM32F446RE has 128 KB of RAM

#define SRAM_END (SRAM_BASE + SRAM_SIZE)

/* RCC peripheral addresses applicable to GPIOA
* (specific of a given STM32 MCU */
#define RCC_BASE    (PERIPH_BASE + 0x23800)
#define RCC_APB1ENR ((uint32_t*)(RCC_BASE + 0x30))

/* GPIOA peripheral addresses
* (specific of a given STM32 MCU) */
#define GPIOA_BASE  (PERIPH_BASE + 0x20000)
#define GPIOA_MODER ((uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR   ((uint32_t*)(GPIOA_BASE + 0x14))
#define GPIOA_PUPDR ((uint32_t*)(GPIOA_BASE + 0x0C))

/* User functions */
int main(void);
void delay(uint32_t count);


/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
		(uint32_t *)SRAM_END, // initial stack pointer
		(uint32_t *)main	  // main as Reset_Handler
};


int main() {
	/* Enable clock on GPIOA peripheral */
	*RCC_APB1ENR  = 0x1;
	/* Configure the PA5 as output pull-up */
	*GPIOA_MODER |= 0x400; // Sets MODER[11:10] = 0x1
	*GPIOA_PUPDR |= 0x400; // Sets PUPDR[11:10] = 0x1

	while(1) {
		*GPIOA_ODR = 0x20;
		delay(1000000);
		*GPIOA_ODR = 0x0;
		delay(1000000);
	}

}


void delay(uint32_t count) {
	while(count--);
}
