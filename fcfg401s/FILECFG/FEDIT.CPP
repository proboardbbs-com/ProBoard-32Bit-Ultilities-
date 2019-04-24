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
#include "fedit.h"
#include "tv/tvutil.h"
#include "confcmds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *_rep[2] = {"No", "Yes"};
static const char *_dt[3] = {"MM/DD/YY", "DD/MM/YY", "YY/MM/DD"};
static const uchar hlFilesBBS = 155, hlGetDir = 156;

ushort getFilePath(char *path)
{
	ushort retval = cmCancel;

    TFileDialog *dlg = new TFileDialog("*.*", "Select Listing File",
		"~L~isting file:", fdOpenButton|fdHelpButton, hlFilesBBS);
    if( dlg ){
    	dlg->helpCtx = hcFileDialog;
    	retval = TProgram::deskTop->execView(dlg);
        if( cmCancel != retval ) dlg->getFileName(path);
        TObject::destroy(dlg);
	}
    return retval;
}

ushort getDirectory(char *path)
{
	ushort retval = cmCancel;

    TGetDirDialog *dlg = new TGetDirDialog(cdNormal|cdHelpButton, hlGetDir);
    if( dlg ){
    	dlg->helpCtx = hcChDir;
    	retval = TProgram::deskTop->execView(dlg);
        if( cmCancel != retval ) dlg->getData(path);
        TObject::destroy(dlg);
	}
    return retval;
}

#pragma warn -use
TFileEdit::TFileEdit():
	TDialog(TRect(0,0,66,21), "File Area Editor"),
    TWindowInit(TFileEdit::initFrame)
{
	FILEAREA  fa;
    long      dummy = 0;
    ushort    cbxFlags = cbxDisposesList|cbxNoTransfer|cbxOnlyList;
    TCombo   *cp;

	data.areaName = new TInputLine(TRect(18,2,62,3), sizeof(fa.name) - 1);
    data.areaName->helpCtx = hcAreaName; insert(data.areaName);
    insert(new TLabel(TRect(2,2,17,3), "Area ~n~ame    :", data.areaName));
    data.fileLoc = new TInputLine(TRect(18,4,62,5), sizeof(fa.filepath) - 1);
    data.fileLoc->helpCtx = hcFileLoc; insert(data.fileLoc);
    insert(new TLabel(TRect(2,4,17,5), "~F~ile location:", data.fileLoc));
    data.fileList = new TInputLine(TRect(18,5,62,6), sizeof(fa.listpath) - 1);
    data.fileList->helpCtx = hcFileList; insert(data.fileList);
    insert(new TLabel(TRect(2,5,17,6), "~L~isting file :", data.fileList));

	data.flagView = new TFlagView(TRect(18,7,52,8), 0, 0);
    data.flagView->helpCtx = hcFileFlag; insert(data.flagView);
    insert(new TLabel(TRect(2,7,17,8), "~A~ccess flags :", data.flagView));
    data.level = new TInputLong(TRect(18,8,25,9), 6, 0, 65535L, 0);
    data.level->helpCtx = hcFileLevel; data.level->setData(&dummy);
	insert(data.level);
    insert(new TLabel(TRect(2,8,17,9), "Access le~v~el :", data.level));

    data.local = new TInputLine(TRect(18,10,24,11), 6);
    data.local->helpCtx = hcFileLocal; data.local->setData("No");
    insert(data.local);
    cp = new TCombo(TRect(24,10,25,11),data.local,cbxFlags,new TSItem("No",new TSItem("Yes",0)));
    cp->activateChar(13); insert(cp);
    insert(new TLabel(TRect(2,10,17,11), "~C~opy local   :", data.local));
    data.tops = new TInputLine(TRect(18,11,24,12), 6);
    data.tops->helpCtx = hcFileTops; data.tops->setData("Yes");
    insert(data.tops);
    cp = new TCombo(TRect(24,11,25,12),data.tops,cbxFlags,new TSItem("No",new TSItem("Yes",0)));
    cp->activateChar(13); insert(cp);
    insert(new TLabel(TRect(2,11,17,12), "In ~t~opfiles  :", data.tops));
    data.freeArea = new TInputLine(TRect(18,12,24,13), 6);
	data.freeArea->helpCtx = hcFileFree; data.freeArea->setData("No");
    insert(data.freeArea);
    cp = new TCombo(TRect(24,12,25,13),data.freeArea,cbxFlags,new TSItem("No",new TSItem("Yes",0)));
	cp->activateChar(13); insert(cp);
    insert(new TLabel(TRect(2,12,17,13), "F~r~ee area    :", data.freeArea));

    for( short i = 0; i < 4; ++i ){
    	char buf[20];
    	data.groups[i] = new TInputLong(TRect(18,14+i,25,15+i), 4, 0, 255, 0);
        data.groups[i]->helpCtx = hcFileGrpSel;
        data.groups[i]->setData(&dummy);
        insert(data.groups[i]);
        sprintf(buf, "Group #~%-d~     :", i+1);
        insert(new TLabel(TRect(2,14+i,17,15+i), buf, data.groups[i]));
	}
    data.allGroups = new TInputLine(TRect(18,18,24,19), 6);
	data.allGroups->helpCtx = hcFileAllGroups;
    data.allGroups->setData("No");
    insert(data.allGroups);
    cp = new TCombo(TRect(24,18,25,19),data.allGroups,cbxFlags,new TSItem("No",new TSItem("Yes",0)));
	cp->activateChar(13); insert(cp);
    insert(new TLabel(TRect(2,18,17,19), "All ~g~roups   :", data.allGroups));

    data.maxFiles = new TInputLong(TRect(50,10,62,11), 6, 0, 65535L, 0);
    data.maxFiles->helpCtx = hcFileMaxFiles;
    data.maxFiles->setData(&dummy);
    insert(data.maxFiles);
    insert(new TLabel(TRect(36,10,49,11), "~M~ax files  :", data.maxFiles));
    data.maxKb = new TInputLong(TRect(50,11,62,12), 6, 0, 65535L, 0);
    data.maxKb->helpCtx = hcFileMaxKb;
	data.maxKb->setData(&dummy);
    insert(data.maxKb);
    insert(new TLabel(TRect(36,11,49,12), "Ma~x~ Kb     :", data.maxKb));
    data.minAge = new TInputLong(TRect(50,12,62,13), 4, 0, 255, 0);
    data.minAge->helpCtx = hcFileMinAge;
    data.minAge->setData(&dummy);
    insert(data.minAge);
    insert(new TLabel(TRect(36,12,49,13), "M~i~n age    :", data.minAge));

    data.dateFormat = new TInputLine(TRect(50,14,61,15), 10);
    data.dateFormat->helpCtx = hcFileDateFormat;
    data.dateFormat->setData("MM/DD/YY");
    insert(data.dateFormat);
    cp = new TCombo(TRect(61,14,62,15), data.dateFormat, cbxFlags,
		new TSItem("MM/DD/YY", new TSItem("DD/MM/YY", new TSItem("YY/MM/DD", 0))));
	cp->activateChar(13); insert(cp);
    insert(new TLabel(TRect(36,14,49,15), "Date f~o~rmat:", data.dateFormat));

    TButton *p = new TButton(TRect(42,18,52,20), "O~K~", cmOK, bfNormal);
    p->helpCtx = hcOKButton;
    insert(p);
    p = new TButton(TRect(52,18,62,20), "Cancel", cmCancel, bfNormal);
    p->helpCtx = hcCancelButton;
    insert(p);

    options |= ofCentered;
}
#pragma warn .use

ushort TFileEdit::dataSize()
{
	return sizeof(FILEAREA);
}

void TFileEdit::setData(void *rec)
{
	FILEAREA *fa = (FILEAREA *)rec;
    long dummy;

    data.areaName->setData(fa->name);
    data.fileLoc->setData(fa->filepath);
    data.fileList->setData(fa->listpath);
    data.flagView->flags = fa->flags;
    data.flagView->flagsRev = fa->flagsNot;
    dummy = long(fa->level);
    data.level->setData(&dummy);
    data.local->setData((void *)_rep[!(!fa->copyLocal)]);
    data.tops->setData((void *)_rep[!fa->notops]);
    data.freeArea->setData((void *)_rep[!(!fa->free)]);
    for(short i = 0; i < 4; ++i ){
    	dummy = long(fa->groups[i]);
		data.groups[i]->setData(&dummy);
    }
    data.allGroups->setData((void *)_rep[!(!fa->allGroups)]);
    dummy = long(fa->maxfiles);
    data.maxFiles->setData(&dummy);
    dummy = long(fa->maxkb);
    data.maxKb->setData(&dummy);
    dummy = long(fa->minAge);
    data.minAge->setData(&dummy);
    data.dateFormat->setData((void *)_dt[fa->dateFormat % 3]);
}

void TFileEdit::getData(void *rec)
{
	FILEAREA *fa = (FILEAREA *)rec;
    long dummy;

    data.areaName->getData(fa->name);
    data.fileLoc->getData(fa->filepath);
    data.fileList->getData(fa->listpath);
    fa->flags = data.flagView->flags;
    fa->flagsNot = data.flagView->flagsRev;
    data.level->getData(&dummy);
    fa->level = ushort(dummy);
    for(short i = 0; i < 4; ++i ){
		data.groups[i]->getData(&dummy);
        fa->groups[i] = uchar(dummy);
    }
    data.maxFiles->getData(&dummy);
    fa->maxfiles = ushort(dummy);
    data.maxKb->getData(&dummy);
    fa->maxkb = ushort(dummy);
    data.minAge->getData(&dummy);
    fa->minAge = uchar(dummy);

    for( i = 0; i < 1; ++i ){
    	if( !strcmp(data.local->data, _rep[i]) ) fa->copyLocal = i;
        if( !strcmp(data.tops->data, _rep[i]) ) fa->notops = !i;
        if( !strcmp(data.freeArea->data, _rep[i]) ) fa->free = i;
        if( !strcmp(data.allGroups->data, _rep[i]) ) fa->allGroups = i;
    }

    for( i = 0; i < 3; ++i )
    	if( !strcmp(data.dateFormat->data, _dt[i]) ){
        	fa->dateFormat = i;
            break;
		}
}

void TFileEdit::handleEvent(TEvent &event)
{
	// enter pressed on the flags selection
	if( evKeyboard == event.what && kbEnter == event.keyDown.keyCode ){
    	if( data.flagView == current ){
			if( cmCancel != getFlags((ulong)data.flagView->flags,
				(ulong)data.flagView->flagsRev) )
			{
	        	data.flagView->drawView();
    	    }
		}
        else if( data.fileList == current ){
        	if( cmCancel != getFilePath(data.fileList->data) ){
            	data.fileList->drawView();
			}
		}
        else if( data.fileLoc == current ){
        	if( cmCancel != getDirectory(data.fileLoc->data) ){
            	data.fileLoc->drawView();
			}
		}
        else for( short i = 0; i < 4; ++i ){
        	if( data.groups[i] == current ){
            	uchar num = (uchar)atoi(data.groups[i]->data);
            	if( cmCancel != getGroup(num, TGroupSelect::file) ){
                	long temp = (long)num;
                	data.groups[i]->setData(&temp);
                    data.groups[i]->drawView();
                }
            }
        }
	}
	TDialog::handleEvent(event);
}

