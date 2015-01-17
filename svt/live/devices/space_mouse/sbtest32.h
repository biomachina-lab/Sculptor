/*----------------------------------------------------------------------
 *  sbtest32.h  -- Basic Program to init spaceball, read its data and 
 *                  print it out.
 *
 *  $Id: sbtest32.h,v 1.2 2007-01-03 15:35:50 sbirmanns Exp $
 *
 *  Written By Elio Querze
 *  
 *
 *----------------------------------------------------------------------
 *
 * (C) 1998-2001 3Dconnexion. All rights reserved. 
 * Permission to use, copy, modify, and distribute this software for all
 * purposes and without fees is hereby grated provided that this copyright
 * notice appears in all copies.  Permission to modify this software is granted
 * and 3Dconnexion will support such modifications only is said modifications are
 * approved by 3Dconnexion.
 *
 */

static char SbTestCvsId[]="(C) 1998 Spacetec IMC Corporation: $Id: sbtest32.h,v 1.2 2007-01-03 15:35:50 sbirmanns Exp $";
/* Global Variables */
 
#ifndef SBTEST32_H
#define SBTEST32_H

HDC         hdc;          /* Handle to Device Context used to draw on screen */
HWND        hWndMain;     /* Handle to Main Window */
SiHdl       devHdl;       /* Handle to Spaceball Device */

/* Functions --See C File for Additional Details */

LRESULT  WINAPI HandleNTEvent (HWND hWnd, unsigned msg, WPARAM wParam, LPARAM lParam);
int   DispatchLoopNT(); 
void  CreateSPWindow(int, int, int, int, char *);
int   SbInit();
void  SbMotionEvent(SiSpwEvent *pEvent);
void  SbZeroEvent();
void  SbButtonPressEvent(int buttonnumber);
void  SbButtonReleaseEvent(int buttonnumber);
#endif