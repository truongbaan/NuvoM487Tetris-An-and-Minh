/******************************************************************************
 * @file     main.c
 * @brief    This is a M480 Tetris game implementation.
 * @version  1.0.0
 * @Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "NuMicro.h"
#include "EBI_LCD_Module.h"
#include "startScreenImage_RGB565.h"

/*---------------------------------------------------------------------------*/
/* Define                                                                    */
/*---------------------------------------------------------------------------*/
// Game settings
#define WIDTH 14
#define HEIGHT 30
#define BLOCK_SIZE 4 // Each Tetris piece is made of 4 blocks
#define MAX_STORE 10

/*---------------------------------------------------------------------------*/
/* Global variables                                                          */
/*---------------------------------------------------------------------------*/
extern  volatile    uint8_t Timer3_flag;
extern  volatile    uint8_t Timer3_cnt;
extern const uint16_t startScreenRGB565[240*320];
int i = 0, j = 0;
volatile char key = 'p';
volatile uint8_t keyPressed = 0;
volatile uint16_t totalSeconds = 0;
volatile uint8_t spawnPieceDelayFlag = 0;
static int x = WIDTH/2 - 2;
static int y = 0;
static int piece;
static int rotation = 0;
static uint8_t board[HEIGHT][WIDTH];
const uint32_t LEVEL1_SPEED = 6000000;
const uint32_t LEVEL2_SPEED = 5400000;
const uint32_t LEVEL3_SPEED = 4800000;
const uint32_t LEVEL4_SPEED = 4200000;
const uint32_t LEVEL5_SPEED = 3600000;
const uint32_t LEVEL6_SPEED = 3000000;
const uint32_t LEVEL7_SPEED = 2400000;
const uint32_t LEVEL8_SPEED = 1800000;
const uint32_t LEVEL9_SPEED = 1200000;
const uint32_t LEVEL10_SPEED = 600000;

/*	Tetris Piece Definitions 														*/
/*	7 Pieces, each piece is 4x4 blocks with 4 rotations	*/
const uint8_t pieces[7][4][4][4] = { 
    // I-piece : 1
    {{{1, 1, 1, 1},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0}},

     {{0, 0, 0, 0},
      {0, 0, 0, 0},
      {1, 1, 1, 1},
      {0, 0, 0, 0}},

     {{0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0}}},

    // J-piece : 2
    {{{2, 0, 0, 0},
      {2, 2, 2, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 2, 2, 0},
      {0, 2, 0, 0},
      {0, 2, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {2, 2, 2, 0},
      {0, 0, 2, 0},
      {0, 0, 0, 0}},

     {{0, 2, 0, 0},
      {0, 2, 0, 0},
      {2, 2, 0, 0},
      {0, 0, 0, 0}}},

    // Z-piece : 3
    {{{3, 3, 0, 0},
      {0, 3, 3, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 3, 0},
      {0, 3, 3, 0},
      {0, 3, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {3, 3, 0, 0},
      {0, 3, 3, 0},
      {0, 0, 0, 0}},

     {{0, 3, 0, 0},
      {3, 3, 0, 0},
      {3, 0, 0, 0},
      {0, 0, 0, 0}}},

    // L-piece : 4
    {{{0, 0, 4, 0},
      {4, 4, 4, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 4, 4, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {4, 4, 4, 0},
      {4, 0, 0, 0},
      {0, 0, 0, 0}},

     {{4, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 0, 0, 0}}},

    // O-piece : 5
    {{{0, 5, 5, 0},
      {0, 5, 5, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 5, 5, 0},
      {0, 5, 5, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 5, 5, 0},
      {0, 5, 5, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 5, 5, 0},
      {0, 5, 5, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}},

    // S-piece : 6
    {{{0, 6, 6, 0},
      {6, 6, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 6, 0, 0},
      {0, 6, 6, 0},
      {0, 0, 6, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {0, 6, 6, 0},
      {6, 6, 0, 0},
      {0, 0, 0, 0}},

     {{6, 0, 0, 0},
      {6, 6, 0, 0},
      {0, 6, 0, 0},
      {0, 0, 0, 0}}},

    // T-piece : 7
    {{{0, 7, 0, 0},
      {7, 7, 7, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 7, 0, 0},
      {0, 7, 7, 0},
      {0, 7, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {7, 7, 7, 0},
      {0, 7, 0, 0},
      {0, 0, 0, 0}},

     {{0, 7, 0, 0},
      {7, 7, 0, 0},
      {0, 7, 0, 0},
      {0, 0, 0, 0}}}
};


/*---------------------------------------------------------------------------*/
/* Functions                                                                 */
/*---------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
	
    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_CLKDIV0_HCLK(1));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_192MHZ);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_PCLK0DIV2 | CLK_PCLKDIV_PCLK1DIV2;
	
		CLK->PWRCTL |= CLK_PWRCTL_LXTEN_Msk;
    while((CLK->STATUS & CLK_STATUS_LXTSTB_Msk) == 0);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
    CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV0_EADC(8));
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0);
		CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
		CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HXT, 0);
		CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(EBI_MODULE);
    CLK_EnableModuleClock(EADC_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);
		CLK_EnableModuleClock(TMR0_MODULE);
		CLK_EnableModuleClock(TMR1_MODULE);
		CLK_EnableModuleClock(TMR2_MODULE);


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /*=== EBI (LCD module) mult-function pins ===*/
    /* EBI AD0~5 pins on PG.9~14 */
    SYS->GPG_MFPH &= ~(SYS_GPG_MFPH_PG9MFP_Msk  | SYS_GPG_MFPH_PG10MFP_Msk |
                       SYS_GPG_MFPH_PG11MFP_Msk | SYS_GPG_MFPH_PG12MFP_Msk |
                       SYS_GPG_MFPH_PG13MFP_Msk | SYS_GPG_MFPH_PG14MFP_Msk);
    SYS->GPG_MFPH |= (SYS_GPG_MFPH_PG9MFP_EBI_AD0  | SYS_GPG_MFPH_PG10MFP_EBI_AD1 |
                      SYS_GPG_MFPH_PG11MFP_EBI_AD2 | SYS_GPG_MFPH_PG12MFP_EBI_AD3 |
                      SYS_GPG_MFPH_PG13MFP_EBI_AD4 | SYS_GPG_MFPH_PG14MFP_EBI_AD5);

    /* EBI AD6, AD7 pins on PD.8, PD.9 */
    SYS->GPD_MFPH &= ~(SYS_GPD_MFPH_PD8MFP_Msk | SYS_GPD_MFPH_PD9MFP_Msk);
    SYS->GPD_MFPH |= (SYS_GPD_MFPH_PD8MFP_EBI_AD6 | SYS_GPD_MFPH_PD9MFP_EBI_AD7);

    /* EBI AD8, AD9 pins on PE.14, PE.15 */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_EBI_AD8 | SYS_GPE_MFPH_PE15MFP_EBI_AD9);

    /* EBI AD10, AD11 pins on PE.1, PE.0 */
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE1MFP_Msk | SYS_GPE_MFPL_PE0MFP_Msk);
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE1MFP_EBI_AD10 | SYS_GPE_MFPL_PE0MFP_EBI_AD11);

    /* EBI AD12~15 pins on PH.8~11 */
    SYS->GPH_MFPH &= ~(SYS_GPH_MFPH_PH8MFP_Msk  | SYS_GPH_MFPH_PH9MFP_Msk |
                       SYS_GPH_MFPH_PH10MFP_Msk | SYS_GPH_MFPH_PH11MFP_Msk);
    SYS->GPH_MFPH |= (SYS_GPH_MFPH_PH8MFP_EBI_AD12  | SYS_GPH_MFPH_PH9MFP_EBI_AD13 |
                      SYS_GPH_MFPH_PH10MFP_EBI_AD14 | SYS_GPH_MFPH_PH11MFP_EBI_AD15);

    /* Configure PH.3 as Output mode for LCD_RS (use GPIO PH.3 to control LCD_RS) */
    GPIO_SetMode(PH, BIT3, GPIO_MODE_OUTPUT);
    PH3 = 1;

    /* EBI RD and WR pins on PE.4 and PE.5 */
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE4MFP_Msk | SYS_GPE_MFPL_PE5MFP_Msk);
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE4MFP_EBI_nWR | SYS_GPE_MFPL_PE5MFP_EBI_nRD);

    /* EBI CS0 pin on PD.14 */
    SYS->GPD_MFPH &= ~SYS_GPD_MFPH_PD14MFP_Msk;
    SYS->GPD_MFPH |= SYS_GPD_MFPH_PD14MFP_EBI_nCS0;

    /* Configure PB.6 and PB.7 as Output mode for LCD_RST and LCD_Backlight */
    GPIO_SetMode(PB, BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT7, GPIO_MODE_OUTPUT);
    PB6 = 1;
    PB7 = 0;

    /*=== UART mult-function pins ===*/
    /* Set PD.2 and PD.3 multi-function pins for UART0 RXD, TXD */
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD2MFP_Msk | SYS_GPD_MFPL_PD3MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD2MFP_UART0_RXD | SYS_GPD_MFPL_PD3MFP_UART0_TXD);
		SYS->GPB_MFPH &= ~(0xful << 16) | (0xful << 20);	// Clear current settings PB.12 [19:16], PB.13 [23:20]
		SYS->GPB_MFPH |= (0x06UL << 16) | (0x06UL << 20);	// UART0 - MFP6
		
		/* Configure PB.2 output for wave form testing */
		//GPIO_SetMode(PB, BIT2, GPIO_MODE_OUTPUT);
		
		SYS_LockReg();
}

/* Timer0 to track game play time */
void Timer0_Init(void)
{
		/* Open Timer0 in periodic mode, enable interrupt and 1 interrupt tick per second */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);
    TIMER_EnableInt(TIMER0);
		NVIC_EnableIRQ(TMR0_IRQn);
}

/* Timer1 to control the game speed delay */
void Timer1_Init(void)
{
		/* 12MHz Timer1 Clock Frequency */
		TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 12000000);
		/* 1/12MHz * 1200000 = 100ms */
		TIMER_SET_CMP_VALUE(TIMER1, LEVEL1_SPEED);
		TIMER_EnableInt(TIMER1);
		NVIC_EnableIRQ(TMR1_IRQn);
}
/* Timer2 to control the piece spawn delay */
void Timer2_Init(void)
{
		TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 12000000);
		TIMER_SET_CMP_VALUE(TIMER2, LEVEL1_SPEED);
		TIMER_EnableInt(TIMER2);
		NVIC_EnableIRQ(TMR2_IRQn);
}
void disableSelectiveInterrupts()
{
	NVIC_DisableIRQ(GPA_IRQn);
	NVIC_DisableIRQ(GPC_IRQn);
	NVIC_DisableIRQ(GPG_IRQn);
}

void enableSelectiveInterrupts()
{
	NVIC_EnableIRQ(GPA_IRQn);
	NVIC_EnableIRQ(GPC_IRQn);
	NVIC_EnableIRQ(GPG_IRQn);
}

/**
 * @brief       GPIO PC IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PC default IRQ, declared in startup_M480.s.
 */
void GPC_IRQHandler(void)
{
    /* To check if PC.10 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PC, BIT10))
    {
        GPIO_CLR_INT_FLAG(PC, BIT10);
        //printf("Joystick (Down) - PC.10 INT occurred.\n");
				keyPressed = 1;
				key = 's';
    }
		else if(GPIO_GET_INT_FLAG(PC, BIT9))
		{
				GPIO_CLR_INT_FLAG(PC, BIT9);
				//printf("Joystick (Left) - PC.9 INT occured.\n");
				keyPressed = 1;
				key = 'a';
		}
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        PC->INTSRC = PC->INTSRC;
        //printf("Un-expected interrupts.\n");
    }
}

/**
 * @brief       GPIO PA IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PA default IRQ, declared in startup_M480.s.
 */
void GPA_IRQHandler(void)
{
    /* To check if PA.0 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PA, BIT0))
    {
        GPIO_CLR_INT_FLAG(PA, BIT0);
        //printf("SW1 - PA.0 INT occurred.\n");
				keyPressed = 1;
				key = 'p';
    }
		else if(GPIO_GET_INT_FLAG(PA, BIT1))
    {
        GPIO_CLR_INT_FLAG(PA, BIT1);
        //printf("SW2 - PA.1 INT occurred.\n");
				keyPressed = 1;
				key = 'v';
    }
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        PA->INTSRC = PA->INTSRC;
        //printf("Un-expected interrupts.\n");
    }
}

/**
 * @brief       GPIO PG IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PG default IRQ, declared in startup_M480.s.
 */
void GPG_IRQHandler(void)
{
    /* To check if PG.2 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PG, BIT2))
    {
        GPIO_CLR_INT_FLAG(PG, BIT2);
        //printf("Joystick (Up) - PG.2 INT occurred.\n");
				keyPressed = 1;
				key = 'w';
    }
		else if(GPIO_GET_INT_FLAG(PG, BIT4))
    {
        GPIO_CLR_INT_FLAG(PG, BIT4);
        //printf("Joystick (Right) - PG.4 INT occurred.\n");
				keyPressed = 1;
				key = 'd';
    }
		//else if(GPIO_GET_INT_FLAG(PG, BIT3))
    //{
    //    GPIO_CLR_INT_FLAG(PG, BIT3);
    //    printf("Joystick (Center) - PG.3 INT occurred.\n");
		//		//keyPressed = 1;
    //}
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        PG->INTSRC = PG->INTSRC;
        //printf("Un-expected interrupts.\n");
    }
}

void Default_Handler(void)
{
		uint32_t irq_num = SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk;
    printf("??  Default_Handler: Unhandled IRQ #%lu\n", (unsigned long)(irq_num & 0x1FF));
    while (1);  // Trap unexpected interrupts
}

// func to init the board when first start
void createBoard(uint8_t board[HEIGHT][WIDTH]) {
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            board[i][j] = 0; // 0 == empty cell
        }
    }
}

// func for print board
void printBoard(uint8_t board[HEIGHT][WIDTH], int score, int piece, int rotation, int posX, int posY)
{		
		uint8_t row, col;

    for (row = 0; row < HEIGHT; row++) {
        for (col = 0; col < WIDTH; col++) {
            if (board[row][col]) {// print out the block that has been occupied
								LCD_DrawBlock(board[row][col] - 1, (col * 10) + 10, (row * 10) + 10);
            }
            else {
                // Then check if the active piece covers this cell
                uint8_t localX = col - posX;
                uint8_t localY = row - posY;

                if ( localX >= 0 && localX < BLOCK_SIZE && localY >= 0 && localY < BLOCK_SIZE && pieces[piece][rotation][localY][localX] ) {
                    // draw falling piece
										LCD_DrawBlock(piece, (col * 10) + 10, (row * 10) + 10);
                }
								else {
									LCD_DrawBlock(9, (col * 10) + 10, (row * 10) + 10);
								}
            }
        }
    }
}

// Function to generate a random Tetris piece
int generatePiece() {
	return TIMER_GetCounter(TIMER3) % 7;
    //return rand() % 7;
}

// Function to check if a piece can move to a new position
int canMove(uint8_t board[HEIGHT][WIDTH], int piece, int rotation, int x, int y) {
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            if (pieces[piece][rotation][i][j]) {
                int newX = x + j;
                int newY = y + i;

                if (newX < 0 || newX >= WIDTH || newY >= HEIGHT || board[newY][newX])
                    return 0; // Out of bounds or collision
                if(newY < 0) //ignore checking the top of the board
                    continue;
            }
        }
    }
    return 1; // The piece can move
}

// movement functions
void movePieceDown(uint8_t board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *y = *y + 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *y = *y - 1; // Move it back to the original position if it can't move down
        // Lock the piece on the board
        for (i = 0; i < BLOCK_SIZE; i++) {
            for (j = 0; j < BLOCK_SIZE; j++) {
                if (pieces[piece][rotation][i][j]) {
                    board[*y + i][*x + j] = (piece + 1);
                }
            }
        }
    }
}

void movePieceLeft(uint8_t board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *x = *x - 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *x = *x + 1;
    }
}

void movePieceRight(uint8_t board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *x = *x + 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *x = *x - 1;
    }
}

void rotatePiece(uint8_t board[HEIGHT][WIDTH], int *piece, int *rotation, int *x, int *y) {
    int newRotation = (*rotation + 1) % 4;
    if (canMove(board, *piece, newRotation, *x, *y)) {
        *rotation = newRotation;
    }
}

// Function to check for completed lines and remove them
int removeLines(uint8_t board[HEIGHT][WIDTH]) {
    int linesRemoved = 0;
    for (i = HEIGHT - 1; i >= 0; i--) {
        int lineComplete = 1;
        for (j = 0; j < WIDTH; j++) {
            if (!board[i][j]) {
                lineComplete = 0;
                break;
            }
        }
        if (lineComplete) {
            linesRemoved++;
            // Shift all rows above down by one row
						int k, l;
            for (k = i - 1; k >= 0; k--) {
                for (l = 0; l < WIDTH; l++) {
                    board[k + 1][l] = board[k][l];
                }
            }
            // Clear the top row
            for (l = 0; l < WIDTH; l++) {
                board[0][l] = 0;
            }
						i++;
        }
    }
    return linesRemoved;
}

// Function to check for game over
int isGameOver(uint8_t board[HEIGHT][WIDTH]) {
    for (j = 0; j < WIDTH; j++) {
        if (board[0][j])
            return 1; // Game over if there is a block in the top row
    }
    return 0;
}

// Compute maximum drop distance for the current piece before collision
int computeDropDistance(uint8_t board[HEIGHT][WIDTH], int piece, int rotation, int x, int y) {
    int maxDrop = HEIGHT;  // upper bound
    // For each cell in the 4�4 piece
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            if (!pieces[piece][rotation][i][j]) continue;
            int boardX = x + j;
            int boardY = y + i;
            // How far can this single block fall?
            int drop = 0;
            while (true) {
                int newY = boardY + drop + 1;
                // stop at bottom or if next cell is occupied
                if (newY >= HEIGHT || board[newY][boardX]) break;
                drop++;
            }
            // We need to stop one row before collision, so drop is correct
            if (drop < maxDrop) 
                maxDrop = drop;
        }
    }
    return maxDrop;
}

// Lock the piece into the board at its final spot
void hardDrop(uint8_t board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    int drop = computeDropDistance(board, piece, rotation, *x, *y);
    *y += drop;
    // Now immediately lock it in place
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            if (pieces[piece][rotation][i][j]) {
                board[*y + i][*x + j] = (piece + 1);
            }
        }
    }
}

// Returns true if ESC was pressed (signal to exit game loop)
int key_input(uint8_t board[HEIGHT][WIDTH], int piece, int *rotation, int *x, int *y, int *rotate_amount, int *pause) {
		if (!keyPressed)
       return 0;

    switch (key) {
        case 'a': case 'A':
            if(*pause) break;
            movePieceLeft(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
            break;
        case 'd': case 'D':
            if(*pause) break;
            movePieceRight(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
            break;
        case 's': case 'S':
            if(*pause) break;
            movePieceDown(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
            break;
        case 'w': case 'W':
            if(*pause) break;
            rotatePiece(board, &piece, rotation, x, y);
            (*rotate_amount)++;
						keyPressed = 0;
						key = 'r';
            return 2;
        case 'v': case 'V':
            if(*pause) break;
            hardDrop(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
            break;
        case 'p': case 'P':
						keyPressed = 0;
						key = 'r';
            return 3;
        case 27:  // ESC
            return 1;
				case 'r':
						keyPressed = 0;
						return 0;
        default:
						keyPressed = 0;
						return 0;
            break;
    }
    return 0;
}

void insertScore(uint8_t topscore[], int score)
{
	if (score > 0) {
		// Find where the new score fits
		for (i = 0; i < MAX_STORE; i++)
		{
			if (score > topscore[i]) {
				// Shift all lower scores down
				for (j = MAX_STORE - 1; j > i; j--) {
					topscore[j] = topscore [j - 1];
				}
				// Insert the new score
				topscore[i] = score;
				break;
			}
		}
	}
}

void drawScoreScreen(uint8_t topscore[], int score)
{
	LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
	//printf("Game Over!\n");
	/* Add score to top score */
	insertScore(topscore, score);
	
	/* Draw border */
	for (i = 0; i < 24; i++) {
		LCD_DrawBlock(7,i*10, 0);
		for (j = 0; j < 32; j++) {
			if (i == 0 || i == 23) {
				LCD_DrawBlock(7,i*10,j*10);
			}
			if (j == 31) {
				LCD_DrawBlock(7,i*10,j*10);
			}
		}
	}
	
	/* Print the top score text */
	LCD_PutString(80, 40, (uint8_t *)"HIGH SCORES", C_WHITE, C_BLACK);
	//printf("Top score: \n");
	
	/* Print the list of top scores */
	for (i = 0; i < MAX_STORE; i++)
	{
		if (topscore[i] > 0) {
			printf("Score: %d\n", topscore[i]);
			char buffer[12];	// Enough for a 32-bit int
			sprintf(buffer, "%d", topscore[i]);	// Convert int score to string
			LCD_PutString(80, (i * 20) + 65, (uint8_t *)buffer, C_WHITE, C_BLACK);
		} 
		else {
			printf("Score: %d\n", topscore[i]);
			LCD_PutString(80, (i * 20) + 65, (uint8_t *)"0", C_WHITE, C_BLACK);
		}
	}
	
	/* Wait for user input to return to game play */
	while(1) {	
		if (keyPressed && key == 'p')
		{
			LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
			key = 'p';
			keyPressed = 0;
			break;
		}
	}
	/* Reset game play speed to level 1 */
	TIMER_SET_CMP_VALUE(TIMER1, LEVEL1_SPEED);
	LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
}

/* Draws thee sen next piece into the next piece window	*/
void drawNextPiece(int next_piece)
{
	/* Clear the next piece display window */
	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 3; j++)
		{
			LCD_DrawBlock(8, (i + 16) * 10, (j + 3) * 10);
		}
	}
	
	/* Display next piece text */
	LCD_PutString(180, 15, (uint8_t *)"NEXT", C_WHITE, C_BLACK);

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			switch (next_piece) {
				case 0:	// I-piece
					if (pieces[next_piece][0][i][j])
					LCD_DrawBlock(next_piece, (j * 10) + 175, (i * 10) + 40);
					break;
					
				case 1:	// J-piece
					if (pieces[next_piece][0][i][j])
					LCD_DrawBlock(next_piece, (j * 10) + 180, (i * 10) + 35);
					break;
					
				case 2:	// Z-piece
					if (pieces[next_piece][0][i][j])
					LCD_DrawBlock(next_piece, (j * 10) + 180, (i * 10) + 35);
					break;
					
				case 3:	// L-piece 
					if (pieces[next_piece][0][i][j])
					LCD_DrawBlock(next_piece, (j * 10) + 180, (i * 10) + 35);
					break;
					
				case 4:	// O-piece
					if (pieces[next_piece][0][i][j])
					LCD_DrawBlock(next_piece, (j * 10) + 175, (i * 10) + 35);
					break;
					
				case 5:	// S-piece
					if (pieces[next_piece][0][i][j])
					LCD_DrawBlock(next_piece, (j * 10) + 180, (i * 10) + 35);
					break;
					
				case 6:	// T-piece
					if (pieces[next_piece][0][i][j])
					LCD_DrawBlock(next_piece, (j * 10) + 180, (i * 10) + 35);
					break;
					
				default:
					break;
			}
		}
	}
}

void drawGameScore(int score)
{
	LCD_PutString(170, 80, (uint8_t *)"SCORE", C_WHITE, C_BLACK);
	char buffer[12];	// Enough for a 32-bit int
	sprintf(buffer, "%d", score);	// Convert int score to string
	LCD_PutString(170, 95, (uint8_t *)buffer, C_WHITE, C_BLACK);
}

void drawTopScore(int topScore)
{
	LCD_PutString(170, 130, (uint8_t *)"HISCORE", C_WHITE, C_BLACK);
	char buffer[12];	// Enough for a 32-bit int
	sprintf(buffer, "%d", topScore);	// Convert int score to string
	LCD_PutString(170, 145, (uint8_t *)buffer, C_WHITE, C_BLACK);
}

void drawLevel(uint8_t level)
{
	LCD_PutString(170, 180, (uint8_t *)"LEVEL", C_WHITE, C_BLACK);
	char buffer[12];	// Enough for a 32-bit int
	sprintf(buffer, "%d", level);	// Convert int score to string
	LCD_PutString(170, 195, (uint8_t *)buffer, C_WHITE, C_BLACK);
}

/* Draws the game play time in format mm:ss in the game window */
void drawTime(uint16_t total_seconds)
{
	uint8_t minutes = total_seconds / 60;
	uint8_t seconds = total_seconds % 60;
	
	char time_str[6];	// "mm:ss" + null terminator
	
	LCD_PutString(170, 230, (uint8_t *)"TIME", C_WHITE, C_BLACK);
	
	if (minutes < 100 && seconds < 60) {
		sprintf(time_str, "%02d:%02d", minutes, seconds);
		LCD_PutString(170, 245, (uint8_t *)time_str, C_WHITE, C_BLACK);
	} else {
		LCD_PutString(170, 245, (uint8_t *)"99:60", C_WHITE, C_BLACK);
	}
}

void drawPause(void)
{
	LCD_PutString(175, 290, (uint8_t *)"PAUSE", C_WHITE, C_BLACK);
}

void drawUnpause(void)
{
	LCD_PutString(175, 290, (uint8_t *)"PAUSE", C_BLACK, C_BLACK);
}


/* Draws the game over sign */
void drawGameOver(void)
{
	/* Draw the game over sign */
	LCD_DrawRectangle((240/2) - 35, (320/2) - 20, 70, 40, C_BLACK, 1);
	LCD_DrawRectangle((240/2) - 35, (320/2) - 20, 70, 40, 0x4208, 0);
	LCD_PutString(105, 145, (uint8_t *)"GAME", C_WHITE, C_BLACK);
	LCD_PutString(105, 160, (uint8_t *)"OVER", C_WHITE, C_BLACK);
	
	/* Wait for user input to return to game play */
	while(1) {	
		if (keyPressed && key == 'p')
		{
			LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
			//key = 'p';
			keyPressed = 0;
			break;
		}
	}
	LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
}

void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
        totalSeconds++;
				//PB2 ^= 1;
    }
}

void TMR1_IRQHandler(void)
{
	movePieceDown(board, &x, &y, piece, rotation);
	TIMER_ClearIntFlag(TIMER1);
	//PB2 ^= 1;
}

void TMR2_IRQHandler(void)
{
	spawnPieceDelayFlag = 1;
	TIMER_ClearIntFlag(TIMER2);
}

int gameplay(uint8_t topscore[]){
		totalSeconds = 0;
    createBoard(board);
		piece = generatePiece();
		rotation = 0;
    int score    = 0;
    int gameSpeed    = 500000;
    int linesRemoved = 0;
    int rotate_amount = 0;
    int pause = 1;
    int next_piece = generatePiece();
		int level = 1;
		drawNextPiece(next_piece);
		
    while (!isGameOver(board)) {
        printBoard(board, score, piece, rotation, x, y);
				LCD_DrawGameBackground();
				drawGameScore(score);
				drawTopScore(topscore[0]);
				TIMER_Start(TIMER1);
			
				/* Calculate and draw level */
				level = (score / 5) + 1;
				if (level > 10) {
					level = 10;
				}
				drawLevel(level);
				
				/* Draw the time mm:ss */
				drawTime(totalSeconds);
				

        // handle input; if ESC, break out and end game
        int key_result = key_input(board, piece, &rotation, &x, &y, &rotate_amount, &pause);

        if (key_result == 1) {
            printf("Exiting game...\n");
            return 0;
        }
        if (key_result == 3){
            pause = abs(pause - 1);
        }
        if (pause){
					CLK_SysTickDelay(gameSpeed);
					TIMER_Stop(TIMER0);
					TIMER_DisableInt(TIMER1);
					TIMER_DisableInt(TIMER0);
					drawPause();
					continue;
        } else {
					TIMER_Start(TIMER0);
					TIMER_EnableInt(TIMER1);
					TIMER_EnableInt(TIMER0);
					drawUnpause();
				}
        if (key_result == 2){
					if(rotate_amount < 5){
						CLK_SysTickDelay(gameSpeed);
						continue;
					}
        }
				
				/* if landed, spawn new piece	*/
        if (!canMove(board, piece, rotation, x, y)) {
					printBoard(board, score, piece, rotation, x, y);
					TIMER_Start(TIMER2);
					//PB2 ^= 1;
					while(!spawnPieceDelayFlag){};
					TIMER_Stop(TIMER2);
					TIMER2->CNT = 0;
					piece    = next_piece;
					//PB2 ^= 1;
					next_piece = generatePiece();
					drawNextPiece(next_piece);
					rotation = 0;
					x        = WIDTH/2 - 2;
					y        = 0;
					spawnPieceDelayFlag = 0;
					TIMER1->CNT = 0;
        }
						
        // clear lines & update score/speed
        linesRemoved = removeLines(board);
				printBoard(board, score, piece, rotation, x, y);
        if (linesRemoved) {
					score += linesRemoved;
					
					/* Set game speed based on level*/
					switch (level) {
						case 1:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL1_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL1_SPEED);
							break;
						case 2:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL2_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL2_SPEED);
							break;
						case 3:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL3_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL3_SPEED);
							break;
						case 4:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL4_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL4_SPEED);
							break;
						case 5:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL5_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL5_SPEED);
							break;
						case 6:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL6_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL6_SPEED);
							break;
						case 7:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL7_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL7_SPEED);
							break;
						case 8:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL8_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL8_SPEED);
							break;
						case 9:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL9_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL9_SPEED);
							break;
						case 10:
							TIMER_SET_CMP_VALUE(TIMER1, LEVEL10_SPEED);
							TIMER_SET_CMP_VALUE(TIMER2, LEVEL10_SPEED);
							break;
					}
        }
				
				/* Check if game is over */
				if (isGameOver(board)) {
					drawGameOver();
					drawScoreScreen(topscore, score);
					TIMER_Stop(TIMER1);
					TIMER1->CNT = 0;
					break;
				}
        rotate_amount = 0;
    }

    return 1;
}

/* Initialize GPIO for Joystick, SW1, SW2  	*/
/* Joystick (Down) 		-	PC.10								*/
/* Joystick (Up)			-	PG.2								*/
/* Joystick (Right) 	- PG.4								*/
/* Joystick (Left)		- PC.9								*/
/* Joystick (Center)	- PG.3								*/
/* SW1								- PA.0								*/
/* SW2								- PA.1								*/
void GPIO_Init(void)
{
	GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT);
	GPIO_SetMode(PA, BIT1, GPIO_MODE_INPUT);
	GPIO_SetMode(PG, BIT2, GPIO_MODE_INPUT);
	//GPIO_SetMode(PG, BIT3, GPIO_MODE_INPUT);
	GPIO_SetMode(PG, BIT4, GPIO_MODE_INPUT);
	GPIO_SetMode(PC, BIT9, GPIO_MODE_INPUT);
	GPIO_SetMode(PC, BIT10, GPIO_MODE_INPUT); 
	
	GPIO_EnableInt(PA, 0, GPIO_INT_RISING);
	GPIO_EnableInt(PA, 1, GPIO_INT_RISING);
	GPIO_EnableInt(PG, 2, GPIO_INT_RISING);
	//GPIO_EnableInt(PG, 3, GPIO_INT_RISING);
	GPIO_EnableInt(PG, 4, GPIO_INT_RISING);
	GPIO_EnableInt(PC, 9, GPIO_INT_RISING);
	GPIO_EnableInt(PC, 10, GPIO_INT_RISING);

  NVIC_EnableIRQ(GPA_IRQn);
	NVIC_EnableIRQ(GPG_IRQn);
	NVIC_EnableIRQ(GPC_IRQn);
	
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_32);
  GPIO_ENABLE_DEBOUNCE(PA, BIT0);
	GPIO_ENABLE_DEBOUNCE(PA, BIT1);
	GPIO_ENABLE_DEBOUNCE(PG, BIT2);
  //GPIO_ENABLE_DEBOUNCE(PG, BIT3);
	GPIO_ENABLE_DEBOUNCE(PG, BIT4);
  GPIO_ENABLE_DEBOUNCE(PC, BIT9);
	GPIO_ENABLE_DEBOUNCE(PC, BIT10);
}

/* Debugging HardFault Handler */
void HardFault_HandlerC(uint32_t *hardfault_args)
{
    uint32_t r0  = hardfault_args[0];
    uint32_t r1  = hardfault_args[1];
    uint32_t r2  = hardfault_args[2];
    uint32_t r3  = hardfault_args[3];
    uint32_t r12 = hardfault_args[4];
    uint32_t lr  = hardfault_args[5];
    uint32_t pc  = hardfault_args[6];
    uint32_t psr = hardfault_args[7];

    printf("==== HardFault Handler ====\n");
    printf("R0  = 0x%08X\n", r0);
    printf("R1  = 0x%08X\n", r1);
    printf("R2  = 0x%08X\n", r2);
    printf("R3  = 0x%08X\n", r3);
    printf("R12 = 0x%08X\n", r12);
    printf("LR  = 0x%08X\n", lr);
    printf("PC  = 0x%08X\n", pc);
    printf("PSR = 0x%08X\n", psr);

    // Optional: halt here
    while (1);
}

__attribute__((naked))
void HardFault_Handler(void)
{
    __asm volatile
    (
        "TST LR, #4\n"                // Test bit 2 of LR to determine stack used
        "ITE EQ\n"
        "MRSEQ R0, MSP\n"             // If 0, use MSP
        "MRSNE R0, PSP\n"             // Else, use PSP
        "B HardFault_HandlerC\n"
    );
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint16_t x = 0, y = 0;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();
	
		/* Init GPIOs */
		GPIO_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 57600);

    //printf("\n");
    //printf("+---------------------------------------------------------+\n");
    //printf("|            						M487 Tetris        								|\n");
    //printf("+---------------------------------------------------------+\n");
    //printf("HXT clock %d Hz\n", CLK_GetHXTFreq());
    //printf("CPU clock %d Hz\n", CLK_GetCPUFreq());

    /* Initialize EBI bank0 to access external LCD Module */
    EBI_Open(EBI_BANK0, EBI_BUSWIDTH_16BIT, EBI_TIMING_NORMAL, 0, EBI_CS_ACTIVE_LOW);
    EBI->CTL0 |= EBI_CTL0_CACCESS_Msk;
    EBI->TCTL0 |= (EBI_TCTL0_WAHDOFF_Msk | EBI_TCTL0_RAHDOFF_Msk);
    //printf("\n[EBI CTL0:0x%08X, TCLT0:0x%08X]\n\n", EBI->CTL0, EBI->TCTL0);

    /* Init LCD Module */
    ILI9341_Initial();

    /* PB.7 BL_CTRL pin */
    PB7 = 1;

    /* Set PH7/PH6 as output mode for LED1/LED2 */
    GPIO_SetMode(PH, BIT7|BIT6, GPIO_MODE_OUTPUT);
    PH7 = 1;
    PH6 = 1;

    /* Init ADC for TP */
    /* Set input mode as single-end and enable the A/D converter */
    EADC_Open(EADC, EADC_CTL_DIFFEN_SINGLE_END);
		
		/* Init Timers */
		Timer0_Init();
		Timer1_Init();
		Timer2_Init();
    Timer3_Init();
		
		//SysTick_Config(SystemCoreClock / 1000);

		/* Draw the start screen image */
		LCD_DrawRGB565Image(startScreenRGB565);
		
    /* waiting 0.5s */
    Timer3_cnt = 0;
    while(Timer3_cnt < 5) {};

		uint8_t gamestate = 0;
		while(gamestate == 0) {
			if(Timer3_flag == 1) {
				Timer3_flag = 0;
				
				x = Get_TP_X();
				y = Get_TP_Y();
				
				if ((x != LCD_W - 1) && (y != LCD_H - 1)) {
					PH7 = 0;
					LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
					PH7 = 1;
					gamestate = 1;
				}
			}
			if (keyPressed && key == 'p')
			{
				LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
				gamestate = 1;
				keyPressed = 0;
			}
			/* Start the game in paused mode */
			key = 'p';
			
			LCD_DrawRectangle(25,237,190,40,C_WHITE,0);
			CLK_SysTickDelay(16000000);
			LCD_DrawRectangle(25,237,190,40,C_BLUE,0);
			CLK_SysTickDelay(16000000);
		}
		
		LCD_BlankArea(0,0, LCD_W, LCD_H, C_BLACK);
		
		static uint8_t top_score[MAX_STORE] = {0};
    while(1) {
			LCD_DrawGameBackground();
			if(!gameplay(top_score)){
					break;
			}
    }
}