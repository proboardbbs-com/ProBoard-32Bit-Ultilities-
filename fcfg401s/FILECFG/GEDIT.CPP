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
#include "gedit.h"
#include "confcmds.h"

#pragma warn -use
TGroupEditDlg::TGroupEditDlg():
	TDialog(TRect(0,0,60,11), "Edit Group"),
    TWindowInit(TGroupEditDlg::initFrame)
{
	FILEGROUP gp;

	name = new TInputLine(TRect(18,2,57,3), sizeof(gp.name) - 1);
    name->helpCtx = hcGroupName;
    insert(name);
    insert(new TLabel(TRect(2,2,17,3), "~G~roup name   :", name));
    flag = new TFlagView(TRect(18,5,52,6), 0, 0);
    flag->helpCtx = hcGroupFlags;
    insert(flag);
    insert(new TLabel(TRect(2,5,17,6), "Access ~f~lags :", flag));
    level = new TInputLong(TRect(18,6,25,7), 6, 0, 65535L, 0);
    level->helpCtx = hcGroupLevel;
    insert(level);
    insert(new TLabel(TRect(2,6,17,7), "Access ~l~evel :", level));

    TButton *p = new TButton(TRect(38,8,48,10), "O~K~", cmOK, bfNormal);
    p->helpCtx = hcOKButton;
    insert(p);
    p = new TButton(TRect(48,8,58,10), "Cancel", cmCancel, bfNormal);
    p->helpCtx = hcCancelButton;
    insert(p);
	options |= ofCentered;
}
#pragma warn .use

void TGroupEditDlg::setData(void *rec)
{
	FILEGROUP *p = (FILEGROUP *)rec;
    TFlagData flg = { p->flags, p->flagsNot };
    long     dummy = (long)p->level;

	name->setData(p->name);
    flag->setData(&flg);
    level->setData(&dummy);
}

void TGroupEditDlg::getData(void *rec)
{
	FILEGROUP *p = (FILEGROUP *)rec;
    TFlagData flg;
    long     dummy;

    name->getData(p->name);
    flag->getData(&flg);
    level->getData(&dummy);
    p->flags = flg.flags;
    p->flagsNot = flg.flagsNot;
    p->level = ushort(dummy);
}

#pragma warn -use
ushort TGroupEditDlg::dataSize()
{
	FILEGROUP gp;
	return sizeof(gp.name) + 2 * sizeof(ulong) + sizeof(ushort);
}
#pragma warn .use

void TGroupEditDlg::handleEvent(TEvent &event)
{
	// enter pressed on the flags selection
	if( evKeyboard == event.what && kbEnter == event.keyDown.keyCode ){
    	if( flag == current ){
			if( cmCancel != getFlags((ulong)flag->flags, (ulong)flag->flagsRev) )
	        	flag->drawView();
		}
	}
	TDialog::handleEvent(event);
}

