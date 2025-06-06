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
File        : FRAMEWIN_Private.h
Purpose     : FRAMEWIN private header file
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef FRAMEWIN_PRIVATE_H
#define FRAMEWIN_PRIVATE_H

#include "WM_Intern.h"
#include "FRAMEWIN.h"
#include "WIDGET.h"
#include "GUI_HOOK.h"

#if GUI_WINSUPPORT

/******************************************************************
*
*       Config defaults
*
*******************************************************************
*/
//
//Support for 3D effects
//
#ifndef FRAMEWIN_CLIENTCOLOR_DEFAULT
  #if WIDGET_USE_FLEX_SKIN
    #define FRAMEWIN_CLIENTCOLOR_DEFAULT GUI_WHITE
  #else
    #define FRAMEWIN_CLIENTCOLOR_DEFAULT GUI_GRAY_C0
  #endif
#endif

//
// Default for top frame size
//
#ifndef FRAMEWIN_TITLEHEIGHT_DEFAULT
  #define FRAMEWIN_TITLEHEIGHT_DEFAULT 0
#endif

//
// Default for left/right/top/bottom frame size
//
#ifndef FRAMEWIN_BORDER_DEFAULT
  #define FRAMEWIN_BORDER_DEFAULT 3
#endif

//
// Default for inner frame size
//
#ifndef FRAMEWIN_IBORDER_DEFAULT
  #define FRAMEWIN_IBORDER_DEFAULT 1
#endif

//
// Default font
//
#ifndef FRAMEWIN_DEFAULT_FONT
  #if WIDGET_USE_FLEX_SKIN
    #if   WIDGET_USE_SCHEME_SMALL
      #define FRAMEWIN_DEFAULT_FONT &GUI_Font13_1
    #elif WIDGET_USE_SCHEME_MEDIUM
      #define FRAMEWIN_DEFAULT_FONT &GUI_Font16_1
    #elif WIDGET_USE_SCHEME_LARGE
      #define FRAMEWIN_DEFAULT_FONT &GUI_Font24_1
    #endif
  #else
    #if   WIDGET_USE_SCHEME_SMALL
      #define FRAMEWIN_DEFAULT_FONT &GUI_Font8_1
    #elif WIDGET_USE_SCHEME_MEDIUM
      #define FRAMEWIN_DEFAULT_FONT &GUI_Font13_1
    #elif WIDGET_USE_SCHEME_LARGE
      #define FRAMEWIN_DEFAULT_FONT &GUI_Font16_1
    #endif
  #endif
#endif

//
// Default bar color when framewin is active
//
#ifndef FRAMEWIN_BARCOLOR_ACTIVE_DEFAULT
  #define FRAMEWIN_BARCOLOR_ACTIVE_DEFAULT GUI_BLUE
#endif

//
// Default bar color when framewin is inactive
//
#ifndef FRAMEWIN_BARCOLOR_INACTIVE_DEFAULT
  #define FRAMEWIN_BARCOLOR_INACTIVE_DEFAULT GUI_DARKGRAY
#endif

//
// Default frame color
//
#ifndef FRAMEWIN_FRAMECOLOR_DEFAULT
  #define FRAMEWIN_FRAMECOLOR_DEFAULT GUI_GRAY_AA
#endif

//
// Default text color when framewin is active
//
#ifndef FRAMEWIN_TEXTCOLOR_INACTIVE_DEFAULT
  #if WIDGET_USE_FLEX_SKIN
    #define FRAMEWIN_TEXTCOLOR_INACTIVE_DEFAULT GUI_BLACK
  #else
    #define FRAMEWIN_TEXTCOLOR_INACTIVE_DEFAULT GUI_WHITE
  #endif
#endif

//
// Default text color when framewin is inactive
//
#ifndef FRAMEWIN_TEXTCOLOR_ACTIVE_DEFAULT
  #if WIDGET_USE_FLEX_SKIN
    #define FRAMEWIN_TEXTCOLOR_ACTIVE_DEFAULT GUI_BLACK
  #else
    #define FRAMEWIN_TEXTCOLOR_ACTIVE_DEFAULT GUI_WHITE
  #endif
#endif

//
// Default text alignment
//
#ifndef FRAMEWIN_TEXTALIGN_DEFAULT
  #define FRAMEWIN_TEXTALIGN_DEFAULT GUI_TA_LEFT
#endif

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  unsigned              (* pfGetBordersize)(FRAMEWIN_Handle hObj, unsigned Index);
  WIDGET_DRAW_ITEM_FUNC  * pfDrawSkin;
} FRAMEWIN_SKIN_PRIVATE;

typedef struct {
  const GUI_FONT      * pFont;
  GUI_COLOR             aBarColor[2];
  GUI_COLOR             aTextColor[2];
  GUI_COLOR             ClientColor;
  FRAMEWIN_SKIN_PRIVATE SkinPrivate;
  I16                   TitleHeight;
  I16                   BorderSize;
  I16                   IBorderSize;
  I16                   TextAlign;
} FRAMEWIN_PROPS;

typedef struct {
  WIDGET                  Widget;
  FRAMEWIN_PROPS          Props;
  WIDGET_SKIN const     * pWidgetSkin;
  WIDGET_DRAW_ITEM_FUNC * pfDrawItem;      // Only for drawing the title bar...
  WM_CALLBACK           * cb;
  WM_HWIN                 hClient;
  WM_HWIN                 hMenu;
  WM_HWIN                 hText;
  GUI_RECT                rRestore;
  U16                     Flags;
  WM_HWIN                 hFocusedChild;   // Handle to focused child .. default none (0)
  WM_DIALOG_STATUS      * pDialogStatus;
  GUI_HOOK              * pFirstHook;
} FRAMEWIN_Obj;

typedef struct {
  I16      TitleHeight;
  I16      MenuHeight;
  GUI_RECT rClient;
  GUI_RECT rTitleText;
} FRAMEWIN_POSITIONS;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define FRAMEWIN_INIT_ID(p) (p->Widget.DebugId = WIDGET_TYPE_FRAMEWIN)
#else
  #define FRAMEWIN_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  FRAMEWIN_Obj * FRAMEWIN_LockH(FRAMEWIN_Handle h);
  #define FRAMEWIN_LOCK_H(h)   FRAMEWIN_LockH(h)
#else
  #define FRAMEWIN_LOCK_H(h)   (FRAMEWIN_Obj *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern FRAMEWIN_PROPS      FRAMEWIN__DefaultProps;

extern const WIDGET_SKIN   FRAMEWIN__SkinClassic;
extern       WIDGET_SKIN   FRAMEWIN__Skin;

extern const WIDGET_SKIN * FRAMEWIN__pSkinDefault;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void     FRAMEWIN__CalcPositions  (FRAMEWIN_Handle hObj, FRAMEWIN_POSITIONS * pPos);
int      FRAMEWIN__CalcTitleHeight(FRAMEWIN_Obj * pObj);
void     FRAMEWIN__UpdatePositions(FRAMEWIN_Handle hObj);
void     FRAMEWIN__UpdateButtons  (FRAMEWIN_Handle hObj, int OldHeight, int OldBorderSizeL, int OldBorderSizeR, int OldBorderSizeT);
void     FRAMEWIN__GetTitleLimits (FRAMEWIN_Handle hObj, int * pxMin, int * pxMax);
unsigned FRAMEWIN__GetBorderSize  (FRAMEWIN_Handle hObj, unsigned Index);

#endif   // GUI_WINSUPPORT
#endif   // FRAMEWIN_PRIVATE_H

/*************************** End of file ****************************/
