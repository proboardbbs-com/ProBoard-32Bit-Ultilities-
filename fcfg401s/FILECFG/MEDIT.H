/*
 * FileCFG v4.0r1/Pro by Silicon Creations, Inc.
 * Copyright (C) 1995-1996 Branislav L. Slantchev
 * All Rights Reserved, All Wrongs Deserved!
 *
 * This file is released under the terms and conditions of the
 * GNU General Public License (see the file 'Copying.Doc' for
 * more information).
 *
 * The author can be contacted at 73023.262@compuserve.com
 *
 * Please, see Credits.Doc for credits and acknowledgements.
 * Also, refer to Compile.Doc for compilation instructions.
 *
 * You need PB-Lib v2.0r1 (Full Release) with both the normal
 * and overlay versions in order to be able to compile the
 * source code. You also need TurboVision 1.03 with all the
 * patches applied (see Patches.Doc for details) and recompiled
 * in both the normal and overlay versions (see Overlay.Doc).
*/
#ifndef __MEDIT_H
#define __MEDIT_H


#define Uses_TDialog
#define Uses_TButton
#define Uses_TEvent
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TFileDialog
#define Uses_TStringCollection
#define Uses_TCombo
#define Uses_TInputLong
#define Uses_TListBox
#define Uses_TSItem
#define Uses_TProgram
#define Uses_TDeskTop
#include <tv.h>
#include "tv/tcombo.h"
#include "tv/tinplong.h"
#include "tv/tgetdir.h"
#include "tv/flagsel.h"
#include "tv/akasel.h"
#include "pblib.h"

class TMesgEdit: public TDialog
{
public:
	TMesgEdit();
    ~TMesgEdit();

    void   setData(void *rec);
    void   getData(void *rec);
    ushort dataSize();
    void   handleEvent(TEvent &event);

protected:
	TSItem      *m_msgKind;
    TSItem      *m_msgType;
    TSItem      *m_msgName;
    TSItem      *m_msgBase;

	TInputLine  *areaName;
	TInputLine  *path;
	TInputLine  *msgKind;
	TInputLine  *msgType;
	TInputLine  *msgName;
	TInputLine  *msgBase;
	TInputLong  *readLevel;
	TFlagView   *readFlags;
	TInputLong  *writeLevel;
	TFlagView   *writeFlags;
	TInputLong  *sysopLevel;
	TFlagView   *sysopFlags;
	TInputLine  *origin;
	TInputLine  *aka;
	TInputLine  *echo;
	TInputLine  *qwk;
	TInputLong  *reply;
	TInputLine  *sysop;
	TInputLong  *groups[4];
	TInputLine  *allGroups;
	TInputLong  *daysKill;
	TInputLong  *recvKill;
	TInputLong  *maxMsgs;
    //
    TInputLong  *hudson;
    TInputLong  *age;
    //
    TAkaSelect  *akaDlg;
};

#endif
