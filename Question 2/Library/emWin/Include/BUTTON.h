/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2023  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.46 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws. Knowledge of the
source code may not be used to write a similar product. 
This file may
only be used in accordance with the following terms:

The  software has  been licensed by SEGGER Software GmbH to Nuvoton Technology Corporation
at the address: No. 4, Creation Rd. III, Hsinchu Science Park, Taiwan
for the purposes  of  creating  libraries  for its 
Arm Cortex-A, Arm Cortex-M and  Arm9 32-bit microcontrollers, commercialized and distributed by Nuvoton Technology Corporation
under  the terms and conditions  of  an  End  User  
License  Agreement  supplied  with  the libraries.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              Nuvoton Technology Corporation, No. 4, Creation Rd. III, Hsinchu Science Park, 30077 Hsinchu City, Taiwan
Licensed SEGGER software: emWin
License number:           GUI-00735
License model:            emWin License Agreement, signed February 27, 2018
Licensed platform:        Cortex-M, Cortex-A, and ARM9 32-bit series microcontroller designed and manufactured by Nuvoton Technology Corporation.
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2018-03-26 - 2025-09-04
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : BUTTON.h
Purpose     : BUTTON public header file (API)
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef BUTTON_H
#define BUTTON_H

#include "WM.h"
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Create flags
*/
/* For compatibility only ! */
#define BUTTON_CF_HIDE   WM_CF_HIDE
#define BUTTON_CF_SHOW   WM_CF_SHOW
#define BUTTON_CF_MEMDEV WM_CF_MEMDEV

/*********************************************************************
*
*       BUTTON color indexes
*
*  Description
*    Color indexes for BUTTON widget.
*/
#define BUTTON_CI_UNPRESSED 0    // Color for disabled state.
#define BUTTON_CI_PRESSED   1    // Color for pressed state.
#define BUTTON_CI_DISABLED  2    // Color for unpressed state.

/*********************************************************************
*
*       BUTTON bitmap indexes
*
*  Description
*    Bitmap indexes for BUTTON widget.
*/
#define BUTTON_BI_UNPRESSED 0    // Bitmap for disabled state.
#define BUTTON_BI_PRESSED   1    // Bitmap for pressed state.
#define BUTTON_BI_DISABLED  2    // Bitmap for unpressed state.

/*********************************************************************
*
*       States
*/
#define BUTTON_STATE_FOCUS      WIDGET_STATE_FOCUS
#define BUTTON_STATE_PRESSED    WIDGET_STATE_USER0
#define BUTTON_STATE_TOGGLEMODE WIDGET_STATE_USER1
#define BUTTON_STATE_TOGGLED    WIDGET_STATE_USER2

/*********************************************************************
*
*       Skinning property indices
*/
#define BUTTON_SKINFLEX_PI_PRESSED     0
#define BUTTON_SKINFLEX_PI_FOCUSED     1
#define BUTTON_SKINFLEX_PI_ENABLED     2
#define BUTTON_SKINFLEX_PI_DISABLED    3

#define BUTTON_SKINFLEX_PI_FOCUSSED BUTTON_SKINFLEX_PI_FOCUSED

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM BUTTON_Handle;

typedef struct {
  GUI_COLOR aColorFrame[3];
  GUI_COLOR aColorUpper[2];
  GUI_COLOR aColorLower[2];
  int Radius;
} BUTTON_SKINFLEX_PROPS;

/*********************************************************************
*
*       Create function(s)

  Note: the parameters to a create function may vary.
         Some widgets may have multiple create functions
*/

BUTTON_Handle BUTTON_Create        (int x0, int y0, int xSize, int ySize, int ID, int Flags);
BUTTON_Handle BUTTON_CreateAsChild (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags);
BUTTON_Handle BUTTON_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
BUTTON_Handle BUTTON_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
BUTTON_Handle BUTTON_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR        BUTTON_GetDefaultBkColor   (unsigned Index);
const GUI_FONT * BUTTON_GetDefaultFont      (void);
int              BUTTON_GetDefaultTextAlign (void);
GUI_COLOR        BUTTON_GetDefaultTextColor (unsigned Index);
void             BUTTON_SetDefaultBkColor   (GUI_COLOR Color, unsigned Index);
GUI_COLOR        BUTTON_SetDefaultFocusColor(GUI_COLOR Color);
void             BUTTON_SetDefaultFont      (const GUI_FONT * pFont);
void             BUTTON_SetDefaultTextAlign (int Align);
void             BUTTON_SetDefaultTextColor (GUI_COLOR Color, unsigned Index);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void BUTTON_Callback(WM_MESSAGE *pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
GUI_COLOR          BUTTON_GetBkColor         (BUTTON_Handle hObj, unsigned int Index);
const GUI_BITMAP * BUTTON_GetBitmap          (BUTTON_Handle hObj,unsigned int Index);
const GUI_FONT   * BUTTON_GetFont            (BUTTON_Handle hObj);
GUI_COLOR          BUTTON_GetFrameColor      (BUTTON_Handle hObj);
WIDGET           * BUTTON_GetpWidget         (BUTTON_Handle hObj);
void               BUTTON_GetText            (BUTTON_Handle hObj, char * pBuffer, int MaxLen);
GUI_COLOR          BUTTON_GetTextColor       (BUTTON_Handle hObj, unsigned int Index);
int                BUTTON_GetTextAlign       (BUTTON_Handle hObj);
int                BUTTON_GetUserData        (BUTTON_Handle hObj, void * pDest, int NumBytes);
unsigned           BUTTON_IsPressed          (BUTTON_Handle hObj);
void               BUTTON_SetBitmap          (BUTTON_Handle hObj, unsigned int Index, const GUI_BITMAP * pBitmap);
void               BUTTON_SetBitmapEx        (BUTTON_Handle hObj, unsigned int Index, const GUI_BITMAP * pBitmap, int x, int y);
void               BUTTON_SetBkColor         (BUTTON_Handle hObj, unsigned int Index, GUI_COLOR Color);
void               BUTTON_SetBMP             (BUTTON_Handle hObj, unsigned int Index, const void * pBitmap);
void               BUTTON_SetBMPEx           (BUTTON_Handle hObj, unsigned int Index, const void * pBitmap, int x, int y);
void               BUTTON_SetFont            (BUTTON_Handle hObj, const GUI_FONT * pfont);
void               BUTTON_SetFrameColor      (BUTTON_Handle hObj, GUI_COLOR Color);
void               BUTTON_SetState           (BUTTON_Handle hObj, int State);                                    /* Not to be doc. */
void               BUTTON_SetPressed         (BUTTON_Handle hObj, int State);
GUI_COLOR          BUTTON_SetFocusColor      (BUTTON_Handle hObj, GUI_COLOR Color);
void               BUTTON_SetStreamedBitmap  (BUTTON_Handle hObj, unsigned int Index, const GUI_BITMAP_STREAM * pBitmap);
void               BUTTON_SetStreamedBitmapEx(BUTTON_Handle hObj, unsigned int Index, const GUI_BITMAP_STREAM * pBitmap, int x, int y);
int                BUTTON_SetText            (BUTTON_Handle hObj, const char* s);
void               BUTTON_SetTextAlign       (BUTTON_Handle hObj, int Align);
void               BUTTON_SetTextColor       (BUTTON_Handle hObj, unsigned int Index, GUI_COLOR Color);
void               BUTTON_SetTextOffset      (BUTTON_Handle hObj, int xPos, int yPos);
void               BUTTON_SetToggleMode      (BUTTON_Handle hObj, int OnOff);
void               BUTTON_SetSelfDrawEx      (BUTTON_Handle hObj, unsigned int Index, GUI_DRAW_SELF_CB * pDraw, int x, int y); /* Not to be doc. */
void               BUTTON_SetSelfDraw        (BUTTON_Handle hObj, unsigned int Index, GUI_DRAW_SELF_CB * pDraw);               /* Not to be doc. */
void               BUTTON_SetReactOnLevel    (void);
void               BUTTON_SetReactOnTouch    (void);
int                BUTTON_SetUserData        (BUTTON_Handle hObj, const void * pSrc, int NumBytes);
int                BUTTON_Toggle             (BUTTON_Handle hObj);

#define BUTTON_SetFocussable BUTTON_SetFocusable
#define BUTTON_SetFocusable  WIDGET_SetFocusable

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void BUTTON_GetSkinFlexProps     (BUTTON_SKINFLEX_PROPS * pProps, int Index);
void BUTTON_SetSkinClassic       (BUTTON_Handle hObj);
void BUTTON_SetSkin              (BUTTON_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int  BUTTON_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void BUTTON_SetSkinFlexProps     (const BUTTON_SKINFLEX_PROPS * pProps, int Index);
void BUTTON_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * BUTTON_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define BUTTON_SKIN_FLEX    BUTTON_DrawSkinFlex

#if defined(__cplusplus)
  }
#endif

#endif   // GUI_WINSUPPORT
#endif   // BUTTON_H

/*************************** End of file ****************************/
