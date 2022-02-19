/* memory layout for an STM32F446RE */

MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
	RAM  (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
}

ENTRY(main)

/* output sections */

SECTIONS
{
	/*program code into FLASH */
	.text : ALIGN(4)
	{
		*(.isr_vector)   	     /* Vector table */
		*(.text)          		 /* Program data */
		KEEP(*(.isr_vector))  
	} >FLASH

	/* Initialized global and static variables (which
	   we don't have any in this example) into SRAM */
	.data :
	{
		*(.data)
	} >RAM
}