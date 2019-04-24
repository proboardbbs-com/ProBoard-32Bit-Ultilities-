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
#include "medit.h"
#include "confcmds.h"
#include "tv/tvutil.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *_m_kind[4] = {"Local","Netmail","Echomail","Private Echomail"};
static char *_m_type[4] = {"Private/Public","Private Only","Public Only","To-All"};
static char *_m_name[4] = {"Real Names Only","Free Alias","Fixed Alias or Real Name","Fixed Alias Enforced"};
static char *_m_base[4] = {"Hudson","Squish","Fido *.MSG","JAM"};
static char *_m_ysno[2] = {"No", "Yes"};

#pragma warn -use
TMesgEdit::TMesgEdit():
	TDialog(TRect(0,0,74,22), "Edit Message Area"),
    TWindowInit(TMesgEdit::initFrame)
{
	MESSAGES ma;
    long     n = 0;
    ushort   cbxFlags = cbxDisposesList|cbxNoTransfer|cbxOnlyList;
    TCombo   *p;

    m_msgKind = new TSItem(_m_kind[0], new TSItem(_m_kind[1],
				new TSItem(_m_kind[2], new TSItem(_m_kind[3], 0))));
    m_msgType = new TSItem(_m_type[0], new TSItem(_m_type[1],
				new TSItem(_m_type[2], new TSItem(_m_type[3], 0))));
    m_msgName = new TSItem(_m_name[0], new TSItem(_m_name[1],
				new TSItem(_m_name[2], new TSItem(_m_name[3], 0))));
    m_msgBase = new TSItem(_m_base[0], new TSItem(_m_base[1],
				new TSItem(_m_base[2], new TSItem(_m_base[3], 0))));

    akaDlg = 0;
    insert(areaName = new TInputLine(TRect(18,2,50,3), sizeof(ma.name) - 1));
    areaName->helpCtx = hcMesgAreaName;
    insert(new TLabel(TRect(2,2,17,3), "~N~ame         :", areaName));
    insert(path = new TInputLine(TRect(18,3,50,4), sizeof(ma.path) - 1));
    path->helpCtx = hcMesgPath;
    insert(new TLabel(TRect(2,3,17,4), "~P~ath         :", path));
    insert(msgKind = new TInputLine(TRect(18,4,44,5), 26));
    msgKind->helpCtx = hcMesgKind;
    p = new TCombo(TRect(44,4,45,5), msgKind, cbxFlags, m_msgKind);
    p->activateChar(13); insert(p); msgKind->setData(_m_kind[0]);
    insert(new TLabel(TRect(2,4,18,5), "Message ~K~ind :", msgKind));
    insert(msgType = new TInputLine(TRect(18,5,44,6), 26));
    msgType->helpCtx = hcMesgType;
    p = new TCombo(TRect(44,5,45,6), msgType, cbxFlags, m_msgType);
    p->activateChar(13); insert(p); msgType->setData(_m_type[0]);
    insert(new TLabel(TRect(2,5,17,6), "Message ~T~ype :", msgType));
    insert(msgName = new TInputLine(TRect(18,6,44,7), 26));
    msgName->helpCtx = hcMesgName;
    p = new TCombo(TRect(44,6,45,7), msgName, cbxFlags, m_msgName);
    p->activateChar(13); insert(p); msgName->setData(_m_name[0]);
    insert(new TLabel(TRect(2,6,17,7), "Name ~O~ptions :", msgName));
    insert(msgBase = new TInputLine(TRect(18,7,44,8), 26));
    msgBase->helpCtx = hcMesgBase;
    p = new TCombo(TRect(44,7,45,8), msgBase, cbxFlags, m_msgBase);
    p->activateChar(13); insert(p); msgBase->setData(_m_base[0]);
    insert(new TLabel(TRect(2,7,17,8), "Message ~B~ase :", msgBase));

    insert(readLevel = new TInputLong(TRect(18,9,25,10), 6, 0, 65535L, 0));
    readLevel->helpCtx = hcReadLevel;
    insert(new TLabel(TRect(2,9,17,10), "~R~ead Level   :", readLevel));
    insert(writeLevel = new TInputLong(TRect(18,10,25,11), 6, 0, 65535L, 0));
    writeLevel->helpCtx = hcWriteLevel;
    insert(new TLabel(TRect(2,10,17,11), "~W~rite Level  :", writeLevel));
    insert(sysopLevel = new TInputLong(TRect(18,11,25,12), 6, 0, 65535L, 0));
    sysopLevel->helpCtx = hcSysopLevel;
    insert(new TLabel(TRect(2,11,17,12), "~S~ysop Level  :", sysopLevel));
	readLevel->setData(&n); writeLevel->setData(&n); sysopLevel->setData(&n);

    insert(origin = new TInputLine(TRect(18,13,72,14), sizeof(ma.origin)-1));
    origin->helpCtx = hcMesgOrigin;
    insert(new TLabel(TRect(2,13,17,14), "Or~i~gin Line  :", origin));
    insert(aka = new TInputLine(TRect(18,14,43,15), 24));
    aka->helpCtx = hcMesgAka;
    insert(new TLabel(TRect(2,14,17,15), "Use ~A~KA      :", aka));
    insert(echo = new TInputLine(TRect(18,15,43,16), sizeof(ma.echoTag)-1));
    echo->helpCtx = hcMesgEcho;
    insert(new TLabel(TRect(2,15,17,16), "Echo~M~ail Tag :", echo));
    insert(qwk = new TInputLine(TRect(18,16,43,17), sizeof(ma.qwkTag)-1));
    qwk->helpCtx = hcMesgQwk;
    insert(new TLabel(TRect(2,16,17,17), "~Q~WK Area Name:", qwk));
    insert(sysop = new TInputLine(TRect(18,17,43,18), sizeof(ma.sysop)-1));
    sysop->helpCtx = hcMesgSysop;
    insert(new TLabel(TRect(2,17,17,18), "Sysop Nam~e~   :", sysop));
    insert(reply = new TInputLong(TRect(18,18,25,19), 6, 0, 10000, 0));
    reply->helpCtx = hcMesgReply;
    insert(new TLabel(TRect(2,18,17,19), "Repl~y~ Area   :", reply));
    insert(hudson = new TInputLong(TRect(18,19,25,20), 6, 0, 200, 0));
    hudson->helpCtx = hcMesgHudson;
    insert(new TLabel(TRect(2,19,17,20), "~H~udson Board :", hudson));
    reply->setData(&n); hudson->setData(&n);

    for( short i = 0; i < 4; ++i ){
    	char buf[20];
    	groups[i] = new TInputLong(TRect(64,2+i,70,3+i), 4, 0, 255, 0);
        groups[i]->helpCtx = hcMesgGroupNum;
        insert(groups[i]);
        sprintf(buf, "Group #~%-d~  :", i+1);
        insert(new TLabel(TRect(50,2+i,64,3+i), buf, groups[i]));
        groups[i]->setData(&n);
	}

    insert(allGroups = new TInputLine(TRect(64,7,69,8), 5));
    allGroups->helpCtx = hcMesgAllGroups;
    p = new TCombo(TRect(69,7,70,8), allGroups, cbxFlags,
		new TSItem(_m_ysno[0], new TSItem(_m_ysno[1], 0)));
    p->activateChar(13); insert(p); allGroups->setData(_m_ysno[0]);
    insert(new TLabel(TRect(50,7,64,8), "All ~G~roups:", allGroups));

    insert(readFlags = new TFlagView(TRect(38,9,72,10), 0, 0));
    readFlags->helpCtx = hcReadFlags;
    insert(new TLabel(TRect(26,9,38,10), "Rea~d~ Flags:", readFlags));
    insert(writeFlags = new TFlagView(TRect(38,10,72,11), 0, 0));
    writeFlags->helpCtx = hcWriteFlags;
    insert(new TLabel(TRect(25,10,38,11), "Write ~F~lags:", writeFlags));
    insert(sysopFlags = new TFlagView(TRect(38,11,72,12), 0, 0));
    sysopFlags->helpCtx = hcSysopFlags;
    insert(new TLabel(TRect(25,11,38,12), "Sysop F~l~ags:", sysopFlags));

    insert(daysKill = new TInputLong(TRect(65,14,72,15), 6, 0, 65535L, 0));
    daysKill->helpCtx = hcMesgKillDays;
    insert(new TLabel(TRect(43,14,64,15), "Kill after num days:", daysKill));
    insert(recvKill = new TInputLong(TRect(65,15,72,16), 6, 0, 65535L, 0));
    recvKill->helpCtx = hcMesgKillRecv;
    insert(new TLabel(TRect(43,15,64,16), "Kill recei~v~ed after:", recvKill));
    insert(maxMsgs = new TInputLong(TRect(65,16,72,17), 6, 0, 65535L, 0));
    maxMsgs->helpCtx = hcMesgMax;
    insert(new TLabel(TRect(43,16,64,17), "Ma~x~ number messages:", maxMsgs));
    insert(age = new TInputLong(TRect(65,17,72,18), 6, 0, 255, 0));
    age->helpCtx = hcMesgMinAge;
    insert(new TLabel(TRect(43,17,64,18), "Minim~u~m access age :", age));
    daysKill->setData(&n); recvKill->setData(&n); maxMsgs->setData(&n);
    age->setData(&n);

    TButton *ptr = new TButton(TRect(44,19,58,21), "OK", cmOK, bfNormal);
    ptr->helpCtx = hcOKButton;
    insert(ptr);
    ptr = new TButton(TRect(58,19,72,21), "Cancel", cmCancel, bfNormal);
    ptr->helpCtx = hcCancelButton;
    insert(ptr);

    akaDlg = new TAkaSelect(getenv("PROBOARD"));
    akaDlg->helpCtx = hcAkaDlg;
    options |= ofCentered;
}
#pragma warn .use

TMesgEdit::~TMesgEdit()
{
	if( akaDlg ) destroy(akaDlg);
    akaDlg = 0;
}

ushort TMesgEdit::dataSize()
{
	return sizeof(MESSAGES);
}

void TMesgEdit::setData(void *rec)
{
	MESSAGES *p = (MESSAGES *)rec;
    char      buf[100];
    long      n;

    areaName->setData(p->name);
    path->setData(p->path);
    n = p->msgKind; msgKind->setData(_m_kind[(int)n % 4]);
    n = p->msgType; msgType->setData(_m_type[(int)n % 4]);
    n = p->flags; msgName->setData(_m_name[(int)n % 4]);
    n = p->msgBaseType; msgBase->setData(_m_base[(int)n % 4]);
    n = p->readLevel; readLevel->setData(&n);
    readFlags->flags = p->readFlags;
    readFlags->flagsRev = p->readFlagsNot;
    n = p->writeLevel; writeLevel->setData(&n);
    writeFlags->flags = p->writeFlags;
    writeFlags->flagsRev = p->writeFlagsNot;
    n = p->sysopLevel; sysopLevel->setData(&n);
    sysopFlags->flags = p->sysopFlags;
    sysopFlags->flagsRev = p->sysopFlagsNot;
    origin->setData(p->origin);
    echo->setData(p->echoTag);
    qwk->setData(p->qwkTag);
    n = p->replyBoard; reply->setData(&n);
    sysop->setData(p->sysop);
    if( !p->allGroups ) allGroups->setData("No");
    else allGroups->setData("Yes");
    for( short i = 0; i < 4; ++i ){
    	n = p->groups[i];
        groups[i]->setData(&n);
	}
    n = p->msgKillDays; daysKill->setData(&n);
    n = p->rcvKillDays; recvKill->setData(&n);
    n = p->maxMsgs; maxMsgs->setData(&n);
    n = p->hudsonBase; hudson->setData(&n);
    n = p->minAge; age->setData(&n);

    akaDlg->setData(&p->aka);
    akaDlg->getAkaStr(buf);
    aka->setData(buf);
}

static byte which_str(char *buf, char **array, short num)
{
	for( short i = 0; i < num; ++i )
		if( !stricmp(array[i], buf) ) return byte(i);
    return 0;
}

void TMesgEdit::getData(void *rec)
{
	MESSAGES *p = (MESSAGES *)rec;
    char      buf[100];
    long      n;

    areaName->getData(p->name);
    path->getData(p->path);
    msgKind->getData(buf); p->msgKind = which_str(buf,_m_kind,4);
    msgType->getData(buf); p->msgType = which_str(buf,_m_type,4);
    msgName->getData(buf); p->flags = which_str(buf,_m_name,4);
    msgBase->getData(buf); p->msgBaseType = which_str(buf,_m_base,4);
    readLevel->getData(&n); p->readLevel = ushort(n);
    p->readFlags = readFlags->flags;
    p->readFlagsNot = readFlags->flagsRev;
    writeLevel->getData(&n); p->writeLevel = ushort(n);
    p->writeFlags = writeFlags->flags;
    p->writeFlagsNot = writeFlags->flagsRev;
    sysopLevel->getData(&n); p->sysopLevel = ushort(n);
    p->sysopFlags = sysopFlags->flags;
    p->sysopFlagsNot = sysopFlags->flagsRev;
    origin->getData(p->origin);
    echo->getData(p->echoTag);
    qwk->getData(p->qwkTag);
    reply->getData(&n); p->replyBoard = ushort(n);
    sysop->getData(p->sysop);
    allGroups->getData(buf); p->allGroups = which_str(buf,_m_ysno,2);
    for( short i = 0; i < 4; ++i ){
    	groups[i]->getData(&n);
    	p->groups[i] = byte(n);
	}
    daysKill->getData(&n); p->msgKillDays = ushort(n);
    recvKill->getData(&n); p->rcvKillDays = ushort(n);
    maxMsgs->getData(&n); p->maxMsgs = ushort(n);
    hudson->getData(&n); p->hudsonBase = byte(n);
    age->getData(&n); p->minAge = byte(n);

    akaDlg->getData(&p->aka);
}

void TMesgEdit::handleEvent(TEvent &event)
{
	char buf[100];

	if( evKeyboard == event.what && kbEnter == event.keyDown.keyCode ){
		if( aka == current ){
			if( cmCancel != TProgram::deskTop->execView(akaDlg) ){
				akaDlg->getAkaStr(buf);
	    	    aka->setData(buf);
		        aka->drawView();
            }
		}
        else if( readFlags == current ){
        	getFlags(readFlags->flags, readFlags->flagsRev);
            readFlags->drawView();
		}
        else if( writeFlags == current ){
        	getFlags(writeFlags->flags, writeFlags->flagsRev);
            writeFlags->drawView();
		}
        else if( sysopFlags == current ){
        	getFlags(sysopFlags->flags, sysopFlags->flagsRev);
            sysopFlags->drawView();
		}
        else for( short i = 0; i < 4; ++i ){
       		if( groups[i] == current ){
           		uchar num = (uchar)atoi(groups[i]->data);
            	if( cmCancel != getGroup(num, TGroupSelect::mesg) ){
                	long temp = (long)num;
                	groups[i]->setData(&temp);
                    groups[i]->drawView();
   	            }
            }
        }
    }
    TDialog::handleEvent(event);
}

