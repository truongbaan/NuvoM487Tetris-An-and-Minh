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
File        : PROGBAR_Private.h
Purpose     : Internal header file
---------------------------END-OF-HEADER------------------------------
*/

#ifndef PROGBAR_PRIVATE_H
#define PROGBAR_PRIVATE_H

#include "GUI_Private.h"
#include "WM_Intern.h"
#include "PROGBAR.h"
#include "WIDGET.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PROGBAR_SF_HORIZONTAL PROGBAR_CF_HORIZONTAL
#define PROGBAR_SF_VERTICAL   PROGBAR_CF_VERTICAL
#define PROGBAR_SF_USER       PROGBAR_CF_USER

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} PROGBAR_SKIN_PRIVATE;

typedef struct {
  const GUI_FONT * pFont;
  GUI_COLOR aBarColor[2];
  GUI_COLOR aTextColor[2];
  PROGBAR_SKIN_PRIVATE SkinPrivate;
} PROGBAR_PROPS;

typedef struct {
  WIDGET Widget;
  int v;
  WM_HMEM hpText;
  I16 XOff, YOff;
  I16 TextAlign;
  int Min, Max;
  PROGBAR_PROPS Props;
  WIDGET_SKIN const * pWidgetSkin;
  U8 Flags;
} PROGBAR_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define PROGBAR_INIT_ID(p) p->Widget.DebugId = WIDGET_TYPE_PROGBAR
#else
  #define PROGBAR_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  PROGBAR_Obj * PROGBAR_LockH(PROGBAR_Handle h);
  #define PROGBAR_LOCK_H(h)   PROGBAR_LockH(h)
#else
  #define PROGBAR_LOCK_H(h)   (PROGBAR_Obj *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern PROGBAR_PROPS PROGBAR__DefaultProps;

extern const WIDGET_SKIN PROGBAR__SkinClassic;
extern       WIDGET_SKIN PROGBAR__Skin;

extern WIDGET_SKIN const * PROGBAR__pSkinDefault;

/*********************************************************************
*
*       Public functions (internal)
*
**********************************************************************
*/
char * PROGBAR__GetTextLocked(const PROGBAR_Obj * pObj);
void   PROGBAR__GetTextRect  (const PROGBAR_Obj * pObj, GUI_RECT * pRect, const char * pText);
int    PROGBAR__Value2Pos    (const PROGBAR_Obj * pObj, int v);

#endif /* GUI_WINSUPPORT */
#endif /* PROGBAR_PRIVATE_H */

/*************************** End of file ****************************/
