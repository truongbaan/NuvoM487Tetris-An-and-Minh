/******************************************************************************
 * @file     main.c
 * @brief    This is a M480 EBI sample code to drive the LCD displaying and
 *           also utilize ADC to judge the position of finger touch.
 * @version  1.0.0
 * @Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "NuMicro.h"
#include "EBI_LCD_Module.h"
#include "startScreenImage.h"

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
extern uint8_t startScreen[240*320*3];
int i = 0, j = 0;
volatile char key = 'p';
volatile uint8_t keyPressed = 0;
volatile uint16_t seconds = 0;

// Tetris piece definitions
// 7 piece, each piece is 4x4 block big, 4 rotation
//if you want to increase block_size, you would need to redraw all of the piece too
int pieces[7][4][4][4] = { 
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
    {{{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{5, 5, 0, 0},
      {5, 5, 0, 0},
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

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(EBI_MODULE);
    CLK_EnableModuleClock(EADC_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_EnableModuleClock(TRNG_MODULE);

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
		
		SYS_LockReg();
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
		//disableSelectiveInterrupts();
    /* To check if PC.10 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PC, BIT10))
    {
        GPIO_CLR_INT_FLAG(PC, BIT10);
        printf("Joystick (Down) - PC.10 INT occurred.\n");
				keyPressed = 1;
				key = 's';
    }
		else if(GPIO_GET_INT_FLAG(PC, BIT9))
		{
				GPIO_CLR_INT_FLAG(PC, BIT9);
				printf("Joystick (Left) - PC.9 INT occured.\n");
				keyPressed = 1;
				key = 'a';
		}
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        PC->INTSRC = PC->INTSRC;
        printf("Un-expected interrupts.\n");
    }
		//enableSelectiveInterrupts();
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
		//disableSelectiveInterrupts();
    /* To check if PA.0 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PA, BIT0))
    {
        GPIO_CLR_INT_FLAG(PA, BIT0);
        printf("SW1 - PA.0 INT occurred.\n");
				keyPressed = 1;
				key = 'p';
    }
		else if(GPIO_GET_INT_FLAG(PA, BIT1))
    {
        GPIO_CLR_INT_FLAG(PA, BIT1);
        printf("SW2 - PA.1 INT occurred.\n");
				keyPressed = 1;
				key = 'v';
    }
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        PA->INTSRC = PA->INTSRC;
        printf("Un-expected interrupts.\n");
    }
		//enableSelectiveInterrupts();
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
		//disableSelectiveInterrupts();
    /* To check if PG.2 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PG, BIT2))
    {
        GPIO_CLR_INT_FLAG(PG, BIT2);
        printf("Joystick (Up) - PG.2 INT occurred.\n");
				keyPressed = 1;
				key = 'w';
    }
		else if(GPIO_GET_INT_FLAG(PG, BIT4))
    {
        GPIO_CLR_INT_FLAG(PG, BIT4);
        printf("Joystick (Right) - PG.4 INT occurred.\n");
				keyPressed = 1;
				key = 'd';
    }
		else if(GPIO_GET_INT_FLAG(PG, BIT3))
    {
        GPIO_CLR_INT_FLAG(PG, BIT3);
        printf("Joystick (Center) - PG.3 INT occurred.\n");
				//keyPressed = 1;
    }
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        PG->INTSRC = PG->INTSRC;
        printf("Un-expected interrupts.\n");
    }
		//enableSelectiveInterrupts();
}

// func to init the board when first start
void createBoard(int board[HEIGHT][WIDTH]) {
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            board[i][j] = 0; // 0 == empty cell
        }
    }
}

// func for print board
void printBoard(int board[HEIGHT][WIDTH], int score, int piece, int rotation, int posX, int posY)
{		
		int row, col;
    //system("cls");

    printf("Tetris Game   Score: %d\n\n", score);

    for (row = 0; row < HEIGHT; row++) {
        for (col = 0; col < WIDTH; col++) {
            if (board[row][col]) {// print out the block that has been occupied
                printf("[%d]", board[row][col]);
								LCD_DrawBlock(board[row][col] - 1, (col * 10) + 10, (row * 10) + 10);
            }
            else {
								LCD_DrawBlock(8, (col * 10) + 10, (row * 10) + 10);
                // Then check if the active piece covers this cell
                int localX = col - posX;
                int localY = row - posY;

                if ( localX >= 0 && localX < BLOCK_SIZE && localY >= 0 && localY < BLOCK_SIZE && pieces[piece][rotation][localY][localX] ) {
                    // draw falling piece
                    printf("[%d]", (piece + 1)); // use number to define color later
										LCD_DrawBlock(piece, (col * 10) + 10, (row * 10) + 10);
                }
                else {
                    // empty cell
                    printf("   "); // 3 width long due to 2 '[' and 1 digit (would be incrase base on the how big the screen and block size be)
                }
            }
        }
        printf("\n");
    }

    // bottom border
    for(j = 0; j < WIDTH; j++) {
        printf("---");
    }
    printf("\n");
}

// Function to generate a random Tetris piece
int generatePiece() {
    return rand() % 7;
}

// Function to check if a piece can move to a new position
int canMove(int board[HEIGHT][WIDTH], int piece, int rotation, int x, int y) {
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
void movePieceDown(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
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

void movePieceLeft(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *x = *x - 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *x = *x + 1;
    }
}

void movePieceRight(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *x = *x + 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *x = *x - 1;
    }
}

void rotatePiece(int board[HEIGHT][WIDTH], int *piece, int *rotation, int *x, int *y) {
    int newRotation = (*rotation + 1) % 4;
    if (canMove(board, *piece, newRotation, *x, *y)) {
        *rotation = newRotation;
    }
}

// Function to check for completed lines and remove them
int removeLines(int board[HEIGHT][WIDTH]) {
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
        }
    }
    return linesRemoved;
}

// Function to check for game over
int isGameOver(int board[HEIGHT][WIDTH]) {
    for (j = 0; j < WIDTH; j++) {
        if (board[0][j])
            return 1; // Game over if there is a block in the top row
    }
    return 0;
}

// Compute maximum drop distance for the current piece before collision
int computeDropDistance(int board[HEIGHT][WIDTH], int piece, int rotation, int x, int y) {
    int maxDrop = HEIGHT;  // upper bound
    // For each cell in the 4×4 piece
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
void hardDrop(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
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
int key_input(int board[HEIGHT][WIDTH],int piece,int *rotation, int *x, int *y, int *rotate_amount, int *pause) {
    //if (!_kbhit())
		if (keyPressed == 0)
       return 0;

    //char key = _getch();
		//char key;
		//disableSelectiveInterrupts();
    switch (key) {
        case 'a': case 'A':
            if(*pause) break;
            movePieceLeft(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
						//enableSelectiveInterrupts();
            break;
        case 'd': case 'D':
            if(*pause) break;
            movePieceRight(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
						//enableSelectiveInterrupts();
            break;
        case 's': case 'S':
            if(*pause) break;
            movePieceDown(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
						//enableSelectiveInterrupts();
            break;
        case 'w': case 'W':
            if(*pause) break;
            rotatePiece(board, &piece, rotation, x, y);
            (*rotate_amount)++;
						keyPressed = 0;
						key = 'r';
						//enableSelectiveInterrupts();
            return 2;
        case 'v': case 'V':
            if(*pause) break;
            hardDrop(board, x, y, piece, *rotation);
						keyPressed = 0;
						key = 'r';
						//enableSelectiveInterrupts();
            break;
        case 'p': case 'P':
						keyPressed = 0;
						key = 'r';
						//enableSelectiveInterrupts();
            return 3;
        case 27:  // ESC
            return 1;
				case 'r':
						keyPressed = 0;
						//enableSelectiveInterrupts();
						return 0;
        default:
						keyPressed = 0;
						return 0;
            break;
    }
    return 0;
}

void insertScore(int topscore[], int score)
{
	for (i = 0; i < MAX_STORE; i++)
	{
		if (topscore[i] < score) {
			for (j = MAX_STORE - 1; j > i; j--) {
				topscore[j] = topscore [j - 1];
			}
			topscore[i] = score;
			break;
		}
	}
}

void drawScoreScreen(int topscore[], int score)
{
	LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
	printf("Game Over!\n");
	insertScore(topscore, score);
	printf("Top score: \n");
	for (i = 0; i < MAX_STORE; i++)
	{
		printf("Score: %d\n", topscore[i]);
	}
	uint16_t x = 0, y = 0;
	while(1) {	
			if(Timer3_flag == 1) {
				Timer3_flag = 0;
				
				x = Get_TP_X();
				y = Get_TP_Y();
				
				if ((x != LCD_W - 1) && (y != LCD_H - 1)) {
					PH7 = 0;
					LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
					PH7 = 1;
					break;
				}
				else if (keyPressed == 1)
				{
					LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
					key = 'p';
					keyPressed = 0;
					break;
				}
			}
	}
}

/* Draws the game play time in format mm:ss in the game window */
void LCD_DisplayTime(int x, int y, int total_seconds, uint16_t f_color, uint16_t b_color)
{
	int minutes = total_seconds / 60;
	int seconds = total_seconds % 60;
	
	char time_str[6];	// "mm:ss" + null terminator
	
	if (minutes < 100 && seconds < 60) {
		sprintf(time_str, "%02d:%02d", minutes, seconds);
		LCD_PutString(x, y, (uint8_t *)time_str, f_color, b_color);

	} else {
		LCD_PutString(x, y, (uint8_t *)"99:60", f_color, b_color);
	}	
}

/* Draws the next piece into the next piece window	*/
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
					LCD_DrawBlock(next_piece, (j * 10) + 185, (i * 10) + 35);
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
	LCD_PutString(160, 80, (uint8_t *)"SCORE", C_WHITE, C_BLACK);
	char buffer[12];	// Enough for a 32-bit int
	sprintf(buffer, "%d", score);	// Convert int score to string
	LCD_PutString(170, 95, (uint8_t *)buffer, C_WHITE, C_BLACK);
}

void drawTopScore(int topScore)
{
	LCD_PutString(150, 110, (uint8_t *)" TOP SCORE", C_WHITE, C_BLACK);
	char buffer[12];	// Enough for a 32-bit int
	sprintf(buffer, "%d", topScore);	// Convert int score to string
	LCD_PutString(170, 125, (uint8_t *)buffer, C_WHITE, C_BLACK);
}

int gameplay(int topscore[]){
    int board[HEIGHT][WIDTH];
    createBoard(board);
		
    int piece    = generatePiece();
    int rotation = 0;
    int x        = WIDTH/2 - 2;
    int y        = 0;
    int score    = 0;
    int gameSpeed    = 150;
    int linesRemoved = 0;
    int rotate_amount = 0;
    int pause = 1;
    int next_piece = generatePiece();

		
    while (!isGameOver(board)) {
        printBoard(board, score, piece, rotation, x, y);
				LCD_DrawGameBackground();
				drawNextPiece(next_piece);
				drawGameScore(score);
				drawTopScore(topscore[0]);

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
            continue;
        }
        if (key_result == 2){
					if(rotate_amount < 5){
						CLK_SysTickDelay(gameSpeed);
						continue;
					}
        }
				

        // auto moving down
        movePieceDown(board, &x, &y, piece, rotation);
        CLK_SysTickDelay(gameSpeed);

        // clear lines & update score/speed
        linesRemoved = removeLines(board);
        if (linesRemoved) {
            score += linesRemoved * 100;
            gameSpeed = (gameSpeed > 100) ? gameSpeed - 50 : 100;
        }

        // if landed, spawn new piece
        if (!canMove(board, piece, rotation, x, y)) {
            piece    = next_piece;
            next_piece = generatePiece();
            rotation = 0;
            x        = WIDTH/2 - 2;
            y        = 0;
            if (isGameOver(board)) {
                //printBoard(board, score, piece, rotation, x, y);
                //printf("Game Over!\n");
                
                //CLK_SysTickDelay(2000);
								drawScoreScreen(topscore, score);
                break;
            }
        }
        rotate_amount = 0;
    }

    return 1;
}

/* Initialize GPIO for Joystick, SW1, SW2  */
void GPIO_Init(void)
{
	/* Joystick (Down) 		-	PC.10				*/
	/* Joystick (Up)			-	PG.2				*/
	/* Joystick (Right) 	- PG.4				*/
	/* Joystick (Left)		- PC.9				*/
	/* Joystick (Center)	- PG.3				*/
	/* SW1								- PA.0				*/
	/* SW2								- PA.1				*/
	
	GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT);
	GPIO_SetMode(PA, BIT1, GPIO_MODE_INPUT);
	GPIO_SetMode(PG, BIT2, GPIO_MODE_INPUT);
	GPIO_SetMode(PG, BIT3, GPIO_MODE_INPUT);
	GPIO_SetMode(PG, BIT4, GPIO_MODE_INPUT);
	GPIO_SetMode(PC, BIT9, GPIO_MODE_INPUT);
	GPIO_SetMode(PC, BIT10, GPIO_MODE_INPUT); 
	
	GPIO_EnableInt(PA, 0, GPIO_INT_RISING);
	GPIO_EnableInt(PA, 1, GPIO_INT_RISING);
	GPIO_EnableInt(PG, 2, GPIO_INT_RISING);
	GPIO_EnableInt(PG, 3, GPIO_INT_RISING);
	GPIO_EnableInt(PG, 4, GPIO_INT_RISING);
	GPIO_EnableInt(PC, 9, GPIO_INT_RISING);
	GPIO_EnableInt(PC, 10, GPIO_INT_RISING);

  NVIC_EnableIRQ(GPA_IRQn);
	NVIC_EnableIRQ(GPG_IRQn);
	NVIC_EnableIRQ(GPC_IRQn);
	
	//GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
  //GPIO_ENABLE_DEBOUNCE(PA, BIT0);
  //GPIO_ENABLE_DEBOUNCE(PA, BIT1);
	//GPIO_ENABLE_DEBOUNCE(PG, BIT2);
  //GPIO_ENABLE_DEBOUNCE(PG, BIT3);
	//GPIO_ENABLE_DEBOUNCE(PG, BIT4);
  //GPIO_ENABLE_DEBOUNCE(PC, BIT9);
	//GPIO_ENABLE_DEBOUNCE(PC, BIT10);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint16_t x = 0, y = 0;
    char acString[32];

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();
	
		GPIO_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 57600);

    printf("\n");
    printf("+---------------------------------------------------------+\n");
    printf("|            						M487 Tetris        								|\n");
    printf("+---------------------------------------------------------+\n");
    printf("HXT clock %d Hz\n", CLK_GetHXTFreq());
    printf("CPU clock %d Hz\n", CLK_GetCPUFreq());

    /* Initialize EBI bank0 to access external LCD Module */
    EBI_Open(EBI_BANK0, EBI_BUSWIDTH_16BIT, EBI_TIMING_NORMAL, 0, EBI_CS_ACTIVE_LOW);
    EBI->CTL0 |= EBI_CTL0_CACCESS_Msk;
    EBI->TCTL0 |= (EBI_TCTL0_WAHDOFF_Msk | EBI_TCTL0_RAHDOFF_Msk);
    printf("\n[EBI CTL0:0x%08X, TCLT0:0x%08X]\n\n", EBI->CTL0, EBI->TCTL0);

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
		
    /* Init Timer3 */
    Timer3_Init();
		
		/* Draw the start screen image */
		LCD_DrawRGBImage(startScreen);
		
    /* waiting 3s */
    Timer3_cnt = 0;
    while(Timer3_cnt < 5) {};

		int gamestate = 0;
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
				else if (keyPressed == 1)
				{
					LCD_BlankArea(0, 0, LCD_W, LCD_H, C_BLACK);
					key = 'p';
					gamestate = 1;
					keyPressed = 0;
				}
			}
		}
		
    while(1) {
			int top_score[10] = {0,0,0,0,0,0,0,0,0,0};
			LCD_DrawGameBackground();
			if(!gameplay(top_score)){
					break;
			}
    }

}



/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/