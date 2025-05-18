/******************************************************************************
 * @file     EBI_LCD_Module.c
 *
 * @brief    Subfunctins to display contents on LCD module thru EBI interface
 * @version  1.0.0
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NuMicro.h"

#include "EBI_LCD_Module.h"
#include "block.h"

/* LCD Module "RS" */
#define PH_CTRL_RS      1
#if PH_CTRL_RS
#define SET_RS          PH3 = 1;
#define CLR_RS          PH3 = 0;
#endif

/* LCD Module "RESET" */
#define SET_RST         PB6 = 1;
#define CLR_RST         PB6 = 0;

/* Block Size */
#define BLOCK_SIZE       10
#define PIXELS_PER_BLOCK (BLOCK_SIZE * BLOCK_SIZE)
#define BYTES_PER_BLOCK  (PIXELS_PER_BLOCK * 3)

volatile    uint8_t     Timer3_flag = 0;
volatile    uint8_t     Timer3_cnt = 0;

volatile    uint32_t    g_u32AdcIntFlag_TP;

extern uint8_t blocks[9][BYTES_PER_BLOCK];

/**
 * @brief       TMR3 IRQ handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer3 default IRQ, declared in startup_M480.s
 */
void TMR3_IRQHandler(void)
{
    /* Clear timer interrupt flag */
    TIMER3->INTSTS |= TIMER_INTSTS_TIF_Msk;

    /* Set Timer3_flag = 1 */
    Timer3_flag = 1;

    /* Timer3_cnt + 1 */
    Timer3_cnt = Timer3_cnt + 1;

}

/**
 * @brief       Initial Timer3
 *
 * @param       None
 *
 * @return      None
 *
 * @details     To initialize Timer3 as periodic mode @100ms and enable interrupt
 */
void Timer3_Init(void)
{
    /* 1/12MHz * 1200000 = 100ms */
    TIMER3->CMP = 1200000;

    /* Enable Timer3 interrupt and set Timer3 as Periodic Mode, prescale = 0 (/1) */
    TIMER3->CTL = TIMER_CTL_INTEN_Msk | TIMER_PERIODIC_MODE;

    /* Enable Timer3 IRQ */
    NVIC_EnableIRQ(TMR3_IRQn);

    /* Enable Timer3 */
    TIMER3->CTL |= TIMER_CTL_CNTEN_Msk;

    /* Clear Timer3_flag */
    Timer3_flag = 0;

    /* Reset Timer3_cnt */
    Timer3_cnt = 0;

}

/**
 * @brief       Write command to LCD
 *
 * @param       cmd: the command will be written
 *
 * @return      None
 *
 * @details     To write command to LCD thru the EBI interface
 */
#if PH_CTRL_RS
void LCD_WR_REG(uint16_t cmd)
{
    CLR_RS
    EBI0_WRITE_DATA16(0x00000000, cmd);
    SET_RS
}
#else
void LCD_WR_REG(uint16_t cmd)
{
    EBI0_WRITE_DATA16(0x00000000, cmd);
}
#endif

/**
 * @brief       Write data to LCD
 *
 * @param       dat: the data will be written
 *
 * @return      None
 *
 * @details     To write data to LCD thru the EBI interface
 */
void LCD_WR_DATA(uint16_t dat)
{
    EBI0_WRITE_DATA16(0x00030000, dat);

}

/**
 * @brief       Read back data from LCD
 *
 * @param       None
 *
 * @return      The read back data
 *
 * @details     To read data from LCD thru the EBI interface
 */
uint16_t LCD_RD_DATA(void)
{
    return EBI0_READ_DATA16(0x00030000);

}

/**
 * @brief       Set LCD window
 *
 * @param       x_s: the start position in the x-direction of window
 *              x_e: the end position in the x-direction of window
 *              y_s: the start position in the y-direction of window
 *              y_e: the end position in the y-direction of window
 * @return      None
 *
 * @details     To set the window area of LCD module
 */
void LCD_SetWindow(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
    /* X range */
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(x_s>>8);
    LCD_WR_DATA(x_s);
    LCD_WR_DATA(x_e>>8);
    LCD_WR_DATA(x_e);

    /* Y range */
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(y_s>>8);
    LCD_WR_DATA(y_s);
    LCD_WR_DATA(y_e>>8);
    LCD_WR_DATA(y_e);

    /* Memory write */
    LCD_WR_REG(0x2C);

}

/**
 * @brief       Initial ILI9341 LCD driver
 *
 * @param       None
 *
 * @return      None
 *
 * @details     To initialize the ILI9341 LCD driver
 */
void ILI9341_Initial(void)
{
    /* Hardware reset */
    SET_RST;
    CLK_SysTickDelay(5000);     // Delay 5ms

    CLR_RST;
    CLK_SysTickDelay(20000);    // Delay 20ms

    SET_RST;
    CLK_SysTickDelay(40000);    // Delay 40ms

    /* Initial control registers */
    LCD_WR_REG(0xCB);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x02);

    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC1);
    LCD_WR_DATA(0x30);

    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x85);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x78);

    LCD_WR_REG(0xEA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x81);

    LCD_WR_REG(0xF7);
    LCD_WR_DATA(0x20);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x23);

    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x10);

    LCD_WR_REG(0xC5);
    LCD_WR_DATA(0x3e);
    LCD_WR_DATA(0x28);

    LCD_WR_REG(0xC7);
    LCD_WR_DATA(0x86);

    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x48);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);

    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x18);

    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x82);
    LCD_WR_DATA(0x27);

    LCD_WR_REG(0xF2);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x26);
    LCD_WR_DATA(0x01);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x31);
    LCD_WR_DATA(0x2B);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x4E);
    LCD_WR_DATA(0xF1);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x14);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x31);
    LCD_WR_DATA(0xC1);
    LCD_WR_DATA(0x48);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x31);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x0F);

    LCD_WR_REG(0x11);
    CLK_SysTickDelay(200000);   // Delay 200ms

    LCD_WR_REG(0x29);           //Display on

    printf("Initial ILI9341 LCD Module done.\n\n");

}

/**
 * @brief       Put an 8x16 character to LCD screen
 *
 * @param       x: the start position in the x-direction
 *              y: the start position in the y-direction
 *              c: the character of ASCII
 *              fcolor: the front color
 *              bcolor: the background color
 * @return      None
 *
 * @details     To show an 8x16 character on the LCD screen
 */
void LCD_PutChar8x16(uint16_t x, uint16_t y, uint8_t c, uint32_t fColor, uint32_t bColor)
{
    uint32_t    i, j;
    uint8_t     m;

    for(i=0; i<16; i++) {
        m = Font8x16[c*16+i];
        LCD_SetWindow(x, x+7, y+i, y+i);
        for(j=0; j<8; j++) {
            if((m & 0x80) == 0x80) {
                LCD_WR_DATA(fColor);
            } else {
                LCD_WR_DATA(bColor);
            }
            m <<= 1;
        }
    }

}

/**
 * @brief       Put a string to LCD screen
 *
 * @param       x: the start position in the x-direction
 *              y: the start position in the y-direction
 *              *s: the point of string
 *              fcolor: the front color
 *              bcolor: the background color
 * @return      None
 *
 * @details     To show a string on the LCD screen
 */
void LCD_PutString(uint16_t x, uint16_t y, uint8_t *s, uint32_t fColor, uint32_t bColor)
{
    uint8_t     l = 0;

    while(*s) {
        if(*s < 0x80) {
            LCD_PutChar8x16(x+l*8, y, *s, fColor, bColor);
            s++;
            l++;
        }
    }

}

/**
 * @brief       Put a larger 16x32 number (0~9) to LCD screen
 *
 * @param       x: the start position in the x-direction
 *              y: the start position in the y-direction
 *              c: the number 0~9
 *              fcolor: the front color
 *              bcolor: the background color
 * @return      None
 *
 * @details     To show a larger 16x32 number on the LCD screen
 */
void LCD_PutChar16x32(uint16_t x, uint16_t y, uint16_t c, uint32_t fColor, uint32_t bColor)
{
    uint32_t    i, j;
    uint16_t    m;

    for(i=0; i<32; i++) {
        m = Font16x32[c*32+i];
        LCD_SetWindow(x, x+15, y+i, y+i);
        for(j=0; j<16; j++) {
            if((m & 0x8000) == 0x8000) {
                LCD_WR_DATA(fColor);
            } else {
                LCD_WR_DATA(bColor);
            }
            m <<= 1;
        }
    }

}

/**
 * @brief       Blank an area of LCD screen
 *
 * @param       X: the X start position of this area, 0~239
 *              Y: the Y start position of this area, 0~319
 *              W: the width of this area, X+W < 240
 *              H: the height of this area, Y+H < 320
 *              color: the color will be filled into this area
 * @return      None
 *
 * @details     To fill a fixed color into an area on the LCD screen
 */
void LCD_BlankArea(uint16_t X, uint16_t Y, uint16_t W, uint16_t H, uint16_t color)
{
    uint16_t    i, j;

    LCD_SetWindow(X, X+W-1, Y, Y+H-1);
    for(j=0; j<H; j++) {
        for(i=0; i<W; i++)
            LCD_WR_DATA(color);
    }

}

/**
 * @brief       Draw a 240x320 RGB image pixel by pixel
 *
 * @param       img: Pointer to an array of size 240*320*3 (RGB888 format)
 * @return      None
 *
 * @details     Converts each RGB888 pixel to RGB565 and sends it to LCD
 */
void LCD_DrawRGBImage(const uint8_t *img)
{
    uint32_t totalPixels = 240 * 320;
    uint32_t i;
    uint8_t r, g, b;
    uint16_t color;

    LCD_SetWindow(0, 239, 0, 319);  // Set the full screen as the active window

    for (i = 0; i < totalPixels; i++) {
        r = img[i * 3];
        g = img[i * 3 + 1];
        b = img[i * 3 + 2];

        // Convert RGB888 to RGB565
        color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

        LCD_WR_DATA(color);
    }
}

/**
 * @brief       Draw a pixel on the LCD
 *
 * @param       x: X-coordinate
 * @param       y: Y-coordinate
 * @param       color: 16-bit RGB565 color
 * @return      None
 */
void LCD_DrawPixel(int x0, int y0, uint16_t color)
{
	LCD_SetWindow(x0, x0 + 1, y0, y0 + 1);
	LCD_WR_DATA(color);
}

/**
 * @brief       Draw a rectangle on the LCD
 *
 * @param       x: Top-left X coordinate
 * @param       y: Top-left Y coordinate
 * @param       width: Width of the rectangle
 * @param       height: Height of the rectangle
 * @param       color: RGB565 color
 * @param       fill: 1 to fill the rectangle, 0 for outline only
 * @return      None
 */
void LCD_DrawRectangle(int x, int y, int width, int height, uint16_t color, uint8_t fill)
{
    if (fill) {
				int j;
        for (j = 0; j < height; j++) {
						int i;
            for (i = 0; i < width; i++) {
                LCD_DrawPixel(x + i, y + j, color);
            }
        }
    } else {
        // Top and bottom edges
				int i;
        for (i = 0; i < width; i++) {
            LCD_DrawPixel(x + i, y, color);             	// Top
            LCD_DrawPixel(x + i, y + height - 1, color); 	// Bottom
        }
        // Left and right edges
				int j;
        for (j = 0; j < height; j++) {
            LCD_DrawPixel(x, y + j, color);             // Left
            LCD_DrawPixel(x + width - 1, y + j, color); // Right
        }
    }
}

/**
 * @brief       Converts 8-bit RGB to 16-bit RGB value
 *
 * @param       r: 8-bit red value (0-255)
 * @param       g: 8-bit green value (0-255)
 * @param       b: 8-bit blue value (0-255)
 * @return      16-bit color value
 *
 * @details     Calculates the 16-bit RGB value from 8-bit RGB value
 */
uint16_t RGB16B(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) |   // top 5 bits of red
           ((g & 0xFC) << 3) |   // top 6 bits of green
           (b >> 3);             // top 5 bits of blue
}

/**
 * @brief       Draw a block on the LCD
 *
 * @param  block_index  Index of the block in the `blocks` array (0–7)
 * @param  x0           X coordinate of the top-left pixel on screen
 * @param  y0           Y coordinate of the top-left pixel on screen
 *
 * @return      None
 */
void LCD_DrawBlock(int block_index, int x0, int y0)
{
    // Validate index
    //if (block_index < 0 || block_index >= 9) {
    //   return;
    //}

    const uint8_t *sprite = blocks[block_index];
		
		int dy;
    for (dy = 0; dy < BLOCK_SIZE; ++dy) {
				int dx;
        for (dx = 0; dx < BLOCK_SIZE; ++dx) {
            int idx = (dy * BLOCK_SIZE + dx) * 3;
            uint8_t r = (uint8_t)sprite[idx + 0];
            uint8_t g = (uint8_t)sprite[idx + 1];
            uint8_t b = (uint8_t)sprite[idx + 2];
						LCD_DrawPixel(x0 + dx, y0 + dy, RGB16B(r,g,b));
        }
    }
}

void LCD_DrawGameBackground(void)
{
	int i;
	for (i = 0; i < 24; i++) {
		LCD_DrawBlock(7,i*10, 0);
		int j;
		for (j = 0; j < 32; j++) {
			if (i == 0 || i == 23 || i == 15) {
				LCD_DrawBlock(7,i*10,j*10);
			}
			if (j == 31) {
				LCD_DrawBlock(7,i*10,j*10);
			}
			if (j == 6 && i > 15) {
				LCD_DrawBlock(7,i*10,j*10);
			}
			if (j == 27 && i > 15) {
				LCD_DrawBlock(7,i*10,j*10);
			}
		}
	}
}

/**
 * @brief       Get X position from touch panel thru the ADC input
 *
 * @param       None
 *
 * @return      The X position on LCD screen
 *
 * @details     To get the X position when finger touching on the LCD screen
 */
uint16_t Get_TP_X(void)
{
    uint16_t    x_adc_in;
    uint16_t    X_pos;

    /*=== Get X from ADC input ===*/
    PB9 = 1;
    PH4 = 0;
    GPIO_SetMode(PB, BIT9, GPIO_MODE_OUTPUT);   // XR
    GPIO_SetMode(PH, BIT4, GPIO_MODE_OUTPUT);   // XL
    GPIO_SetMode(PH, BIT5, GPIO_MODE_INPUT);    // YD

    /* Configure the GPB8 ADC analog input pins. */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB8MFP_EADC0_CH8;

    /* Disable the GPB8 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT8);

    /* Configure the sample module 1 for analog input channel 8 and software trigger source.*/
    EADC_ConfigSampleModule(EADC, 1, EADC_SOFTWARE_TRIGGER, 8); // YU

    /* Clear the A/D ADINT1 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF1_Msk);

    /* Enable the sample module 1 interrupt. */
    EADC_ENABLE_INT(EADC, BIT1);    //Enable sample module A/D ADINT1 interrupt.
    EADC_ENABLE_SAMPLE_MODULE_INT(EADC, 1, BIT1);    //Enable sample module 1 interrupt.
    NVIC_EnableIRQ(EADC01_IRQn);

    /* Reset the ADC interrupt indicator and trigger sample module 1 to start A/D conversion */
    g_u32AdcIntFlag_TP = 0;
    EADC_START_CONV(EADC, BIT1);

    /* Wait ADC interrupt (g_u32AdcIntFlag_TP will be set at IRQ_Handler function) */
    while(g_u32AdcIntFlag_TP == 0) {};
    x_adc_in = EADC_GET_CONV_DATA(EADC, 1)>>2;

    /*=== Calculate the X position ===*/
    X_pos = (x_adc_in - 170)/2.8;	// range of x_adc_in is [0:842]

    if(X_pos >= (LCD_W - 1)) X_pos = LCD_W - 1;

    //printf("Position X: %d\n", X_pos);
    return X_pos;

}

/**
 * @brief       Get Y position from touch panel thru the ADC input
 *
 * @param       None
 *
 * @return      The Y position on LCD screen
 *
 * @details     To get the Y position when finger touching on the LCD screen
 */
uint16_t Get_TP_Y(void)
{
    uint16_t    y_adc_in;
    uint16_t    Y_pos;

    /*=== Get Y from ADC input ===*/
    PB8 = 1;
    PH5 = 0;
    GPIO_SetMode(PB, BIT8, GPIO_MODE_OUTPUT);   // YU
    GPIO_SetMode(PH, BIT5, GPIO_MODE_OUTPUT);   // YD
    GPIO_SetMode(PH, BIT4, GPIO_MODE_INPUT);    // XL

    /* Configure the GPB9 ADC analog input pins. */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB9MFP_EADC0_CH9;

    /* Disable the GPB9 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT9);

    /* Configure the sample module 2 for analog input channel 9 and software trigger source.*/
    EADC_ConfigSampleModule(EADC, 2, EADC_SOFTWARE_TRIGGER, 9); // XR

    /* Clear the A/D ADINT1 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF1_Msk);

    /* Enable the sample module 2 interrupt. */
    EADC_ENABLE_INT(EADC, BIT2);    //Enable sample module A/D ADINT1 interrupt.
    EADC_ENABLE_SAMPLE_MODULE_INT(EADC, 1, BIT2);    //Enable sample module 2 interrupt.
    NVIC_EnableIRQ(EADC02_IRQn);

    /* Reset the ADC interrupt indicator and trigger sample module 2 to start A/D conversion */
    g_u32AdcIntFlag_TP = 0;
    EADC_START_CONV(EADC, BIT2);

    /* Wait ADC interrupt (g_u32AdcIntFlag_TP will be set at IRQ_Handler function) */
    while(g_u32AdcIntFlag_TP == 0) {};
    y_adc_in = EADC_GET_CONV_DATA(EADC, 2)>>2;

    /*=== Calculate the Y position ===*/
			Y_pos = (y_adc_in - 150)/2.34; // range of y_adc_in is [0:898.8]

    if(Y_pos >= (LCD_H - 1)) Y_pos = LCD_H - 1;

    //printf("Position Y : %d \n", Y_pos);
    return Y_pos;

}

/**
 * @brief       ADC01 IRQ handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The ADC01 default IRQ, declared in startup_M480.s
 */
void EADC01_IRQHandler(void)
{
    /* Clear the A/D ADINT1 interrupt flag */
    EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF1_Msk);

    g_u32AdcIntFlag_TP = 1;

}

