/******************************************************************************
 * @file     main.c
 * @brief    This is a M480 EBI code rewritten to draw a GUI, star, 
 * 					 rectangle, circle and image
 * @version  1.0.0
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "EBI_LCD_Module.h"
#include "image_data.h"


/*---------------------------------------------------------------------------*/
/* Define                                                                    */
/*---------------------------------------------------------------------------*/
#define HXTSTB 	1 << 0
#define HXTEN 	1 << 0
#define PLLSTB	1 << 2
	
/*---------------------------------------------------------------------------*/
/* Global variables                                                          */
/*---------------------------------------------------------------------------*/
extern 	volatile		uint8_t Timer1_flag;
extern 	volatile		uint8_t Timer1_cnt;
extern uint8_t image[240*320*3];
extern uint8_t image2[240*320*3];

/*---------------------------------------------------------------------------*/
/* Functions                                                                 */
/*---------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
		PF->MODE &= ~((0x00 << 4) | (0x00 << 6));
		CLK->PWRCTL |= HXTEN;
		

    /* Waiting for 12MHz clock ready */
		while (!(CLK->STATUS & HXTSTB));

    /* Switch HCLK clock source to HXT */
		CLK->CLKDIV0 &= ~(0x0F);
		CLK->CLKDIV0 |= (0UL << 0);
		
    /* Set core clock (192 MHz) as PLL_CLOCK from PLL */
		CLK->PLLCTL &= (~(1 << 19));
		CLK->PLLCTL &= (~(1 << 16));
		
		/* NF = 8, NR = 1, NO = 1	*/
		/* FBDIV[8:0] = 6, INDIV[13:9] = 0, OUTDIV[15:14] = 00 	*/
		CLK->PLLCTL &= ~(0xFFFF << 0);	// Clear PLLCTL
		CLK->PLLCTL |= (0x6UL << 0);
		CLK->PLLCTL |= (0UL << 9);
		CLK->PLLCTL |= (0UL << 14);
		
		CLK->PLLCTL &= (~(1 << 18));
		while (!(CLK->STATUS & PLLSTB));	// Wait for PLL to stabilize
		CLK->CLKSEL0 &= ~(0x07 << 0);			// Clear PLL output
		CLK->CLKSEL0 |= (0x02 << 0);			// Select PLLOUT -> HCLK

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
		CLK->PCLKDIV = (0x1UL << 0) | (0x1UL << 4);

    /* Select IP clock source 		*/
		/* UART0 Clock Configuration 	*/
		CLK->CLKSEL1 |= (0UL << 24);	// UART0 HXT clock source
		CLK->CLKDIV0 |= (0UL << 8);		// UART0 clock divider 1
		
		/* EADC Clock Configuration		*/
		CLK->CLKDIV0 &= ~(0x0FF << 16);		// Clear current settings
		CLK->CLKDIV0 |= (7UL << 16); 			// EADC clock divider is (7+1) --> ADC clock is PCLK1 = HCLK/2 -> 96/8 = 12 MHz
		
		/* TM1 Clock Selection HXT */
		CLK->CLKSEL1 &= ~(0b111 << 12);		// Clear TM1 clock selection
		CLK->CLKSEL1 |= (0UL << 12);			// Set TM1 clock source HXT
		
		/* Set TM1 Prescale	*/
		TIMER1->CTL &= ~(0xFF << 0);	// Set TM1 prescale 1
		
    /* Enable IP clock */		
		CLK->APBCLK0 |= (1UL << 16);  // Enable UART0 clock
		CLK->AHBCLK |= (1UL << 3);    // Enable EBI clock
		CLK->APBCLK0 |= (1UL << 28);  // Enable EADC clock
		CLK->APBCLK0 |= (1UL << 3);		// Enable TM1 clock

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /*=== EBI (LCD module) mult-function pins ===*/
    /* EBI AD0~5 pins on PG.9~14 */
    SYS->GPG_MFPH &= ~((0xful << 4) | (0xful << 8) | (0xful << 12) |
											 (0xful << 16) | (0xful << 20) | (0xful << 24));
		SYS->GPG_MFPH |= ((0x02UL << 4) | (0x02UL << 8) | (0x02UL << 12) |
											(0x02UL << 16) | (0x02UL << 20) | (0x02UL << 24));
		
    /* EBI AD6, AD7 pins on PD.8, PD.9 */
		SYS->GPD_MFPH &= ~((0xful << 0) | (0xful << 4));
		SYS->GPD_MFPH |= (0x02UL << 0) | (0x02UL << 4);

    /* EBI AD8, AD9 pins on PE.14, PE.15 */
		SYS->GPE_MFPH &= ~((0xful << 24) | (0xful << 28));
		SYS->GPE_MFPH |= (0x02UL << 24) | (0x02UL << 28);

    /* EBI AD10, AD11 pins on PE.1, PE.0 */
		SYS->GPE_MFPL &= ~((0xful << 4) | (0xful << 0));
		SYS->GPE_MFPL |= (0x02UL << 4) | (0x02UL << 0);

    /* EBI AD12~15 pins on PH.8~11 */
		SYS->GPH_MFPH &= ~((0xful << 0) | (0xful << 4) | (0xful << 8) | (0xful << 12));
		SYS->GPH_MFPH |= (0x02UL << 0) | (0x02UL << 4) | (0x02UL << 8) | (0x02UL << 12);

    /* Configure PH.3 as Output mode for LCD_RS (use GPIO PH.3 to control LCD_RS) */
		PH->MODE &= ~(0b11 << 6);
		PH->MODE |= (0b01 << 6);
		PH->DOUT |= (1 << 3);

    /* EBI RD and WR pins on PE.4 and PE.5 */
		SYS->GPE_MFPL &= ~((0xful << 16) | (0xful << 20));
		SYS->GPE_MFPL |= (0x02UL << 16) | (0x02UL << 20);

    /* EBI CS0 pin on PD.14 */
		SYS->GPD_MFPH &= ~(0xful << 24);
		SYS->GPD_MFPH |= (0x02UL << 24);

    /* Configure PB.6 and PB.7 as Output mode for LCD_RST and LCD_Backlight */
		PB->MODE &= ~((0b11 << 12) | (0b11 << 14));
		PB->MODE |= (0b01 << 12) | (0b01 << 14);
		PB->DOUT |= (1 << 6);
		PB->DOUT &= ~(1 << 7);

    /*=== UART mult-function pins ===*/
    /* Set PB.12 and PD.13 multi-function pins for UART0 RXD, TXD */
		SYS->GPB_MFPH &= ~(0xful << 16) | (0xful << 20);	// Clear current settings PB.12 [19:16], PB.13 [23:20]
		SYS->GPB_MFPH |= (0x06UL << 16) | (0x06UL << 20);	// UART0 - MFP6
		
		/* Lock Protected Registers */
		SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 57600-8n1 for print message */
	
		/* Set UART line configuration */
		UART0->LINE |= (0b11 << 0);			// 8 data bit
		UART0->LINE &= ~(1 << 2);				// One stop bit
		UART0->LINE &= ~(1 << 3);				// No parity bit
		
		/* Set UART Rx and RTS trigger level */
		UART0->FIFO |= (1 << 1);				// Clear RX FIFO
		UART0->FIFO |= (1 << 2);				// Clear TX FIFO
		UART0->FIFO &= ~(0xful << 16);	// FIFO Trigger level is 1 byte
		
		/* UART0 baud rate 57600 bps */
		UART0->BAUD &= ~(0b11 << 28);		// Mode 0
		UART0->BAUD &= ~(0xFFFF << 0);
		UART0->BAUD |= 11;
		
    printf("\n");
    printf("+---------------------------------------------------------+\n");
    printf("|            M487 LCD Display with Touch Function         |\n");
    printf("+---------------------------------------------------------+\n");
    printf("HXT clock %d Hz\n", CLK_GetHXTFreq());
    printf("CPU clock %d Hz\n", CLK_GetCPUFreq());
		printf("HCLK clock %d Hz\n", CLK_GetHCLKFreq());
		printf("PCLK0 clock %d Hz\n", CLK_GetPCLK0Freq());

    /* Initialize EBI bank0 to access external LCD Module */
		EBI->CTL0 |= (0x1ul << 1); 	// Set Data Width to 16-bit
		EBI->CTL0 |= 0; 						// Set Bus Mode
		EBI->CTL0 = (EBI->CTL0 & ~((0x7ul << 8) | (0x7ul << 16))) | // Read current CTL0, clear MCLKDIV and TALE fields
		(0x1UL << 8) | 						// Set MCLK Prescaler
		(0UL << 2) | 							// Set CS Active Level (0 for active low)
		(0x1UL << 0) | 						// Enable EBI Bank
		(0x3U << 16);							// Set TALE for normal timing (extend ALE by 3 MCLK cycles)
		EBI->TCTL0 = 0x03003318U; // Apply to EBI->TCTL0
		
		EBI->CTL0 |= (0x1ul << 4);
		EBI->TCTL0 |= (0x1ul << 23) | (0x1ul << 22);
    printf("\n[EBI CTL0:0x%08X, TCLT0:0x%08X]\n\n", EBI->CTL0, EBI->TCTL0);

    /* Init LCD Module */
    ILI9341_Initial();

    /* PB.7 BL_CTRL pin */
		PB->DOUT |= (1 << 7);

    /* Set PH7/PH6 as output mode for LED1/LED2 */
		PH->MODE &= ~((0b11 << 12) | (0b11 << 14));
		PH->MODE |= (0b01 << 12) | (0b01 << 14);
		PH->DOUT |= (1 << 7) | (1 << 6);

    /* Init ADC for TP */
    /* Set input mode as single-end and enable the A/D converter */
		EADC->CTL &= ~(0x1ul << 8);
    EADC->CTL |= ((0UL << 8) | (0x1ul << 0));
		while (!(EADC->PWRM & (0x1ul << 0))){};

		
		/* Init Timer1 */
		Timer1_Init();

    /*=== Show the 1st display ===*/
		Timer1_cnt = 0;
		while(Timer1_cnt < 5) {};
			
		uint16_t x = 0, y = 0;
		char acString[32];
			
    uint8_t draw_mode = 0; // 0 = show user interface, 1 = shape drawn
		uint8_t touch_active = 0;
		LCD_DrawRGBImage(image);

		while (1) {
			if (Timer1_flag == 1) {
				Timer1_flag = 0;

				/* Touch scanning */
				x = Get_TP_X();
				y = Get_TP_Y();

				/* Check if touch is detected */
				if ((x != LCD_W - 1) && (y != LCD_H - 1)) {
					if (!touch_active) {  // New touch detected (edge)
						touch_active = 1;
						PH->DOUT &= ~(1 << 7);

						if (draw_mode == 0) {
							/* Draw a Circle */
							if (x < 105 && y < 238 && y > 184) {
								LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
								LCD_DrawCircle(240 / 2, 320 / 2, 50, C_BLUE, 1);
								draw_mode = 1;
							}
							/* Draw a star */
							else if (x < 105 && y < 142 && y > 88) {
								LCD_BlankArea(0, 0, LCD_W, LCD_H, C_RED);
								LCD_DrawFilledStar(240 / 2, 320 / 2, 75, C_YELLOW);
								draw_mode = 1;
							}
							/* Draw an RGB image */
							else if (x > 133 && y < 142 && y > 88) {
								LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
								LCD_DrawRGBImage(image2);
								draw_mode = 1;
							}
							/* Draw a rectangle */
							else if (x > 133 && y < 238 && y > 184) {
								LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
								LCD_DrawRectangle((240/2) - (200/2), (320/2) - (150/2), 200, 125, C_RED, 1);
								draw_mode = 1;
							}
							else {
								LCD_DrawRGBImage(image);
							}
						} else {
							// If shape is showing, any new touch returns to image
							LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
							LCD_DrawRGBImage(image);
							draw_mode = 0;
						}

						PH->DOUT |= (1 << 7);
					}
				} else {
					touch_active = 0;  // Touch released
				}
			}
		}

}