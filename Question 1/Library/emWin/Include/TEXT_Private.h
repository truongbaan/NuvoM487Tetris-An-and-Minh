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
File        : TEXT.h
Purpose     : TEXT include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef TEXT_PRIVATE_H
#define TEXT_PRIVATE_H

#include "WM_Intern.h"
#include "TEXT.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  const GUI_FONT * pFont;
  GUI_COLOR TextColor;
  GUI_COLOR BkColor;
  GUI_WRAPMODE WrapMode;
#if GUI_SUPPORT_ROTATION
  const GUI_ROTATION * pLCD_Api;
#endif
  GUI_COLOR FrameColor;
} TEXT_PROPS;

typedef struct {
  WIDGET Widget;
  TEXT_PROPS Props;
  WM_HMEM hpText;
  I16 Align;
  I16 xPosText, yPosText;
} TEXT_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define TEXT_INIT_ID(p) p->Widget.DebugId = WIDGET_TYPE_TEXT
#else
  #define TEXT_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  TEXT_OBJ * TEXT_LockH(TEXT_Handle h);
  #define TEXT_LOCK_H(h)   TEXT_LockH(h)
#else
  #define TEXT_LOCK_H(h)   (TEXT_OBJ *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Module internal data
*
**********************************************************************
*/
extern TEXT_PROPS TEXT__DefaultProps;

#endif   /* if GUI_WINSUPPORT */
#endif   /* TEXT_PRIVATE_H */

/*************************** End of file ****************************/
