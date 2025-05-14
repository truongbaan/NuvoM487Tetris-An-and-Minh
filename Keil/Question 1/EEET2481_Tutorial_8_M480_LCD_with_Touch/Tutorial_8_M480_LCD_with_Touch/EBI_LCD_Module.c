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
#include <math.h>
#include "EBI_LCD_Module.h"

/* LCD Module "RS" */
#define PH_CTRL_RS      1
#if PH_CTRL_RS
#define SET_RS          PH3 = 1;
#define CLR_RS          PH3 = 0;
#endif

/* LCD Module "RESET" */
#define SET_RST         PB6 = 1;
#define CLR_RST         PB6 = 0;

#define PI 3.1416

volatile    uint8_t     Timer3_flag = 0;
volatile    uint8_t     Timer3_cnt = 0;

volatile		uint8_t			Timer1_flag = 0;
volatile		uint8_t			Timer1_cnt = 0;

volatile    uint32_t    g_u32AdcIntFlag_TP;

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
 * @brief       TMR1 IRQ handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer1 default IRQ, declared in startup_M480.s
 */
void TMR1_IRQHandler(void)
{
    /* Clear timer interrupt flag */
    TIMER1->INTSTS |= (1 << 0);

    /* Set Timer1_flag = 1 */
    Timer1_flag = 1;

    /* Timer1_cnt + 1 */
    Timer1_cnt = Timer1_cnt + 1;

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
 * @brief       Initial Timer1
 *
 * @param       None
 *
 * @return      None
 *
 * @details     To initialize Timer1 as periodic mode @100ms and enable interrupt
 */
void Timer1_Init(void)
{
    /* 1/12MHz * 1200000 = 100ms */
    TIMER1->CMP = 1200000;

    /* Enable Timer1 interrupt and set Timer1 as Periodic Mode, prescale = 0 (/1) */
    //TIMER1->CTL = TIMER_CTL_INTEN_Msk | TIMER_PERIODIC_MODE;
		TIMER1->CTL = (1UL << 29) | (1UL << 27);
	
		/* Enable Timer1 IRQ */
    //NVIC_EnableIRQ(TMR1_IRQn);
		NVIC->ISER[1] |= (1 << (33 - 32));

    /* Enable Timer1 */
    //TIMER1->CTL |= TIMER_CTL_CNTEN_Msk;
		TIMER1->CTL |= (1 << 30);

    /* Clear Timer1_flag */
    Timer1_flag = 0;

    /* Reset Timer1_cnt */
    Timer1_cnt = 0;

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
    //CLR_RS
		PH->DOUT &= ~(1 << 3);
    //EBI0_WRITE_DATA16(0x00000000, cmd);
		*((volatile unsigned short *)(0x60000000UL + 0x00000000)) = cmd;
    //SET_RS
		PH->DOUT |= (1 << 3);
}
#else
void LCD_WR_REG(uint16_t cmd)
{
    EBI0_WRITE_DATA16(0x00000000, cmd);
		*((volatile unsigned short *)(0x60000000UL + 0x00000000)) = cmd;
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
    //EBI0_WRITE_DATA16(0x00030000, dat);
		*((volatile unsigned short *)(0x60000000UL + 0x00030000)) = dat;
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
    //return EBI0_READ_DATA16(0x00030000);
		return *((volatile unsigned short *)(0x60000000UL + 0x00030000));
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
    //SET_RST;
		PH->DOUT |= (1 << 3);
    CLK_SysTickDelay(5000);     // Delay 5ms

    //CLR_RST;
		PH->DOUT &= ~(1 << 3);
    CLK_SysTickDelay(20000);    // Delay 20ms

    //SET_RST;
		PH->DOUT |= (1 << 3);
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
 * @brief       Draw a checkered board pattern on the LCD screen
 *
 * @param       blockSize: the size of each square block in the checkered pattern
 * @param       color1: the first color of the checkered pattern
 * @param       color2: the second color of the checkered pattern
 * @return      None
 *
 * @details     Draws alternating colored squares across the screen using LCD_BlankArea
 */
void LCD_DrawCheckeredBoard(uint16_t blockSize, uint16_t color1, uint16_t color2)
{
    uint16_t xBlocks = 240 / blockSize;
    uint16_t yBlocks = 320 / blockSize;
    uint16_t x, y;

    for (y = 0; y < yBlocks; y++) {
        for (x = 0; x < xBlocks; x++) {
            uint16_t color = ((x + y) % 2 == 0) ? color1 : color2;
            LCD_BlankArea(x * blockSize, y * blockSize, blockSize, blockSize, color);
        }
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
 * @brief       Draw a circle on the LCD screen using pixels only
 *
 * @param       x0: X-coordinate of the circle center
 * @param       y0: Y-coordinate of the circle center
 * @param       radius: Radius of the circle
 * @param       color: 16-bit RGB565 color
 * @param       fill: 1 to fill the circle, 0 for outline only
 * @return      None
 */
void LCD_DrawCircle(int x0, int y0, int radius, uint16_t color, uint8_t fill)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y)
    {
        if (fill) {
            // Fill horizontal lines between symmetric points
						int xi;
            for (xi = -x; xi <= x; xi++) {
                LCD_DrawPixel(x0 + xi, y0 + y, color);
                LCD_DrawPixel(x0 + xi, y0 - y, color);
            }
            for (xi = -y; xi <= y; xi++) {
                LCD_DrawPixel(x0 + xi, y0 + x, color);
                LCD_DrawPixel(x0 + xi, y0 - x, color);
            }
        } else {
            // Outline only — plot 8 symmetric points
            LCD_DrawPixel(x0 + x, y0 + y, color);
            LCD_DrawPixel(x0 - x, y0 + y, color);
            LCD_DrawPixel(x0 + x, y0 - y, color);
            LCD_DrawPixel(x0 - x, y0 - y, color);
            LCD_DrawPixel(x0 + y, y0 + x, color);
            LCD_DrawPixel(x0 - y, y0 + x, color);
            LCD_DrawPixel(x0 + y, y0 - x, color);
            LCD_DrawPixel(x0 - y, y0 - x, color);
        }

        if (d < 0)
            d += 4 * x + 6;
        else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/**
 * @brief       Draw a line from (x0, y0) to (x1, y1) using Bresenham's algorithm
 *
 * @param       x0: Start X
 * @param       y0: Start Y
 * @param       x1: End X
 * @param       y1: End Y
 * @param       color: 16-bit RGB565 color
 * @return      None
 */
void LCD_DrawLine(int x0, int y0, int x1, int y1, uint16_t color)
{
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        LCD_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
 * @brief       Fill a triangle using horizontal scanline method
 *
 * @param       x0, y0: First vertex
 * @param       x1, y1: Second vertex
 * @param       x2, y2: Third vertex
 * @param       color: Fill color (RGB565)
 * @return      None
 */
void LCD_FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color)
{
    int16_t a, b, y, last;
    
    // Sort by Y order (y0 <= y1 <= y2)
    if (y0 > y1) { int t; t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t; }
    if (y1 > y2) { int t; t = y1; y1 = y2; y2 = t; t = x1; x1 = x2; x2 = t; }
    if (y0 > y1) { int t; t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t; }

    if (y0 == y2) {  // All on same line
        int sx = x0, ex = x0;
        if (x1 < sx) sx = x1;
        if (x2 < sx) sx = x2;
        if (x1 > ex) ex = x1;
        if (x2 > ex) ex = x2;
				int x;
        for (x = sx; x <= ex; x++)
            LCD_DrawPixel(x, y0, color);
        return;
    }

    int dx01 = x1 - x0, dy01 = y1 - y0;
    int dx02 = x2 - x0, dy02 = y2 - y0;
    int dx12 = x2 - x1, dy12 = y2 - y1;
    int sa = 0, sb = 0;

    if (y1 == y2)
        last = y1;   // Include y1 scanline
    else
        last = y1 - 1; // Exclude y1 scanline

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b) { int t = a; a = b; b = t; }
				int x;
        for (x = a; x <= b; x++)
            LCD_DrawPixel(x, y, color);
    }

    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (a > b) { int t = a; a = b; b = t; }
				int x;
        for (x = a; x <= b; x++)
            LCD_DrawPixel(x, y, color);
    }
}

/**
 * @brief       Draw a filled 5-pointed star
 *
 * @param       cx: X center of the star
 * @param       cy: Y center of the star
 * @param       radius: Outer radius of the star
 * @param       color: Fill color (RGB565)
 * @return      None
 */
void LCD_DrawFilledStar(int cx, int cy, int radius, uint16_t color)
{
    float angleStep = 2 * PI / 5.0;   // 72 degrees per point
    float innerRadius = radius * 0.5f;  // Distance to inner vertices
    int x[10], y[10];

    // Compute alternating outer and inner points of the star
		int i;
    for (i = 0; i < 10; i++) {
        float angle = -PI / 2 + i * angleStep / 2;
        float r = (i % 2 == 0) ? radius : innerRadius;
        x[i] = cx + (int)(r * cos(angle));
        y[i] = cy + (int)(r * sin(angle));
    }

    // Fill triangles between center and each star arm
    for (i = 0; i < 10; i++) {
        int next = (i + 1) % 10;
        LCD_FillTriangle(cx, cy, x[i], y[i], x[next], y[next], color);
    }
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
    //PB9 = 1;
    //PH4 = 0;
		
    //GPIO_SetMode(PB, BIT9, GPIO_MODE_OUTPUT);   // XR
    //GPIO_SetMode(PH, BIT4, GPIO_MODE_OUTPUT);   // XL
    //GPIO_SetMode(PH, BIT5, GPIO_MODE_INPUT);    // YD	
		PB->MODE &= ~(0b11 << 18);
		PB->MODE |= (0b01 << 18);
		PH->MODE &= ~((0b11 << 8) | (0b11 << 10));
		PH->MODE |= (0b01 << 8);
		PB->DOUT |= (1 << 9);
		PH->DOUT &= ~(1 << 4);

    /* Configure the GPB8 ADC analog input pins. */
    //SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
    //SYS->GPB_MFPH |= SYS_GPB_MFPH_PB8MFP_EADC0_CH8;
		SYS->GPB_MFPH &= ~((0xFUL << 0) | (0xFUL << 4));
		SYS->GPB_MFPH |= (0x01UL << 0);

    /* Disable the GPB8 digital input path to avoid the leakage current. */
    //GPIO_DISABLE_DIGITAL_PATH(PB, BIT8);
		PB->DINOFF |= (0x00000100UL <<16);
		
    /* Configure the sample module 1 for analog input channel 8 and software trigger source.*/
    //EADC_ConfigSampleModule(EADC, 1, EADC_SOFTWARE_TRIGGER, 8); // YU
		EADC->SCTL[1] &= ~((0x1UL << 5) | (0x1UL << 4) | (0x1FUL << 16) | (0xFUL << 0));
    EADC->SCTL[1] |= ((0x0UL << 16) | 8);

    /* Clear the A/D ADINT1 interrupt flag for safe */
    //EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF1_Msk);
		EADC->STATUS2 = (0x1UL << 1);

    /* Enable the sample module 1 interrupt. */
    //EADC_ENABLE_INT(EADC, BIT1);    //Enable sample module A/D ADINT1 interrupt.
    EADC->CTL |= (0x00000002UL << 2);
		//EADC_ENABLE_SAMPLE_MODULE_INT(EADC, 1, BIT1);    //Enable sample module 1 interrupt.
		EADC->INTSRC[1] |= 0x00000002UL;
    //NVIC_EnableIRQ(EADC01_IRQn);
		NVIC->ISER[1] |= (1 << (43 - 32));

    /* Reset the ADC interrupt indicator and trigger sample module 1 to start A/D conversion */
    g_u32AdcIntFlag_TP = 0;
    //EADC_START_CONV(EADC, BIT1);
		EADC->SWTRG = 0x00000002UL;

    /* Wait ADC interrupt (g_u32AdcIntFlag_TP will be set at IRQ_Handler function) */
    while(g_u32AdcIntFlag_TP == 0) {};
    //x_adc_in = EADC_GET_CONV_DATA(EADC, 1)>>2;
		x_adc_in = (EADC->DAT[1] & (0xfffful << 0)) >> 2;

    /*=== Calculate the X position ===*/
    X_pos = (x_adc_in - 170)/2.8;	// range of x_adc_in is [0:842]

    if(X_pos >= (LCD_W - 1)) X_pos = LCD_W - 1;

    printf("Position X : %d\n", X_pos);
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
    //PB8 = 1;
    //PH5 = 0;

    //GPIO_SetMode(PB, BIT8, GPIO_MODE_OUTPUT);   // YU
    //GPIO_SetMode(PH, BIT5, GPIO_MODE_OUTPUT);   // YD
    //GPIO_SetMode(PH, BIT4, GPIO_MODE_INPUT);    // XL
		PB->MODE &= ~(0b11 << 16);
		PB->MODE |= (0b01 << 16);
		PH->MODE &= ~((0b11 << 10) | (0b11 << 8));
		PH->MODE |= (0b01 << 10);
		PB->DOUT |= (1 << 8);
		PH->DOUT &= ~(1 << 5);
		
    /* Configure the GPB9 ADC analog input pins. */
    //SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
    //SYS->GPB_MFPH |= SYS_GPB_MFPH_PB9MFP_EADC0_CH9;
		SYS->GPB_MFPH &= ~((0xful << 0) | (0xful << 4));
		SYS->GPB_MFPH |= (0x01UL << 4);

    /* Disable the GPB9 digital input path to avoid the leakage current. */
    //GPIO_DISABLE_DIGITAL_PATH(PB, BIT9);
		PB->DINOFF |= (0x00000200UL << 16);
		
    /* Configure the sample module 2 for analog input channel 9 and software trigger source.*/
    //EADC_ConfigSampleModule(EADC, 2, EADC_SOFTWARE_TRIGGER, 9); // XR
		EADC->SCTL[2] &= ~((0x1ul << 5) | (0x1ul << 4) | (0x1ful << 16) | (0xful << 0));
    EADC->SCTL[2] |= ((0UL << 16) | 9);
		
    /* Clear the A/D ADINT1 interrupt flag for safe */
    //EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF1_Msk);
		EADC->STATUS2 = (0x1UL << 1);

    /* Enable the sample module 2 interrupt. */
    //EADC_ENABLE_INT(EADC, BIT2);    //Enable sample module A/D ADINT1 interrupt.
		EADC->CTL |= (0x00000004UL << 2);
    //EADC_ENABLE_SAMPLE_MODULE_INT(EADC, 1, BIT2);    //Enable sample module 2 interrupt.
    EADC->INTSRC[1] |= 0x00000004UL;
		//NVIC_EnableIRQ(EADC02_IRQn);
		NVIC->ISER[1] |= (1 << (46 - 32));

    /* Reset the ADC interrupt indicator and trigger sample module 2 to start A/D conversion */
    g_u32AdcIntFlag_TP = 0;
    //EADC_START_CONV(EADC, BIT2);
		EADC->SWTRG = 0x00000004UL;

    /* Wait ADC interrupt (g_u32AdcIntFlag_TP will be set at IRQ_Handler function) */
    while(g_u32AdcIntFlag_TP == 0) {};
    //y_adc_in = EADC_GET_CONV_DATA(EADC, 2)>>2;
		y_adc_in = (EADC->DAT[2] & (0xfffful << 0)) >> 2;

    /*=== Calculate the Y position ===*/
		Y_pos = (y_adc_in - 150)/2.34; // range of y_adc_in is [0:898.8]

    if(Y_pos >= (LCD_H - 1)) Y_pos = LCD_H - 1;

    printf("Position Y : %d\n", Y_pos);
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
    //EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF1_Msk);
		EADC->STATUS2 = (0x1UL << 1);

    g_u32AdcIntFlag_TP = 1;

}

