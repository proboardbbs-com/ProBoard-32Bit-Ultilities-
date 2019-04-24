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
#include "mesgs.h"
#include "confcmds.h"
#include "fsys.h"
#include "strlib.h"
#include "gadgets.h"
#include "tv/tvutil.h"
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define Uses_TMenu
#define Uses_TMenuBox
#define Uses_TMenuItem
#define Uses_TSubMenu
#include <tv.h>

#define MAXRECS 5001		/* max records per file */

////////////////////////////////////////////////////////////////////////////
// Record initialization
////////////////////////////////////////////////////////////////////////////
TMessageArea::TMessageArea(short aNum, const char *aTagName):
	TTagEntry(aTagName)
{
	oldNum = aNum;
}

////////////////////////////////////////////////////////////////////////////
// The derived class
////////////////////////////////////////////////////////////////////////////
TMessageAreaEdit::TMessageAreaEdit(const char *aFileName):
	TBaseEdit( "Arrange Message Areas"),
    TWindowInit(TMessageAreaEdit::initFrame)
{
    TView *pv;

	// save the filename
    strcpy(fileName, aFileName);
    maxNumRead = 0;

    // build the list box
	TScrollBar *sb = new TScrollBar(TRect(73,3,74,13));
    listBox = new TTagListBox(TRect(4,3,73,13), sb);
    listBox->options |= ofFramed;
    listBox->helpCtx = hcListBox;
    insert(sb);
    insert(listBox);
	insert(new TLabel(TRect(4,2,19,3), "Message ~a~reas ", listBox));

    // place some buttons in
	pv = new TButton(TRect(2,15,14,17), "~R~enumber", cmRenumber, bfNormal);
    pv->helpCtx = hcRenumber;
    insert(pv);
	pv = new TButton(TRect(14,15,26,17), "~I~nsert", cmNew, bfNormal);
    pv->helpCtx = hcInsert;
    insert(pv);
	pv = new TButton(TRect(26,15,38,17), "~D~elete", cmDelete, bfNormal);
    pv->helpCtx = hcDelete;
    insert(pv);
    pv = new TButton(TRect(38,15,50,17), "~F~ind", cmFindFirst, bfNormal);
    pv->helpCtx = hcFindFirst;
    insert(pv);
    pv = new TButton(TRect(50,15,62,17), "~G~oto", cmGotoNum, bfNormal);
    pv->helpCtx = hcGotoNum;
    insert(pv);
	pv = new TButton(TRect(62,15,74,17), "Sa~v~e", cmSave, bfNormal);
    pv->helpCtx = hcSave;
    insert(pv);
	pv = new TButton(TRect(2,17,14,19), "~P~ack", cmPack, bfNormal);
    pv->helpCtx = hcPack;
    insert(pv);
    pv = new TButton(TRect(14,17,26,19), "~M~ove", cmMove, bfNormal);
    pv->helpCtx = hcMove;
    insert(pv);
	pv = new TButton(TRect(26,17,38,19), "Move~S~ort", cmSort, bfNormal);
    pv->helpCtx = hcSort;
    insert(pv);
    pv = new TButton(TRect(38,17,50,19), "~N~ext", cmFindNext, bfNormal);
    pv->helpCtx = hcFindNext;
    insert(pv);
    pv = new TButton(TRect(50,17,62,19), "Glo~b~al", cmChangeRecs, bfNormal);
    pv->helpCtx = hcChangeRecs;
    insert(pv);
	pv = new TButton(TRect(62,17,74,19), "~E~dit", cmEdit, bfDefault);
    pv->helpCtx = hcEdit;
    insert(pv);

    // load some data
    loadData();
    foundAt = -1;
    strcpy(searchText, "");
    memset(&global, 0, sizeof(global));
    listBox->select();
}

TMessageAreaEdit::~TMessageAreaEdit()
{
}

void TMessageAreaEdit::shutDown()
{
	if( list ){
    	list->freeAll();
        destroy(list);
		list = 0;
    }
    TDialog::shutDown();
}

void TMessageAreaEdit::loadData()
{
   	MESSAGES  area;
    ifstream  ifp;
    ofstream  ofp;
	char      tagName[150];
    short     i, numRecs;

    chext(tagName, fileName, ".BAK");
    if( 0 != access(tagName, 0) ){
    	ofp.open(tagName, ios::binary);
        if( !ofp ){
        	messageBox("\003Unable to create the backup.", mfError|mfOKButton);
            isValid = False;
            return;
		}
	}

    ifp.open(fileName, ios::binary);
    if( !ifp ){
    	messageBox("\003Error reading the data file.", mfError|mfOKButton);
        isValid = False;
        return;
	}

    ifp.seekg(0, ios::end);
    numRecs = short(ifp.tellg() / sizeof(MESSAGES));
    ifp.seekg(0, ios::beg);

    makeProgBar("Loading Data", fileName, "Loading from", long(numRecs));
	list = new TBaseCollection(1500, 250, False);

	for( i = 0; i < MAXRECS; ++i ){
		ifp.read((uchar*)&area, sizeof(area));
	    if( ifp.eof() ) break;
        updateProgBar(i+1);
        ofp.write((uchar*)&area, sizeof(area));
	    sprintf(tagName, "%4d. %s", i+1, (EOS != *area.name) ? area.name : strNoName);
		TMessageArea *fa = new TMessageArea(i, tagName);
		list->insert(fa);
    }

    if( i == MAXRECS ){
    	messageBox("\003You have too many areas.\n\003Some areas will be lost.",
			mfWarning|mfOKButton);
	}
    maxNumRead = i + 1;
    ifp.close();
    ofp.close();
    isValid = True;
    destroyProgBar();
	listBox->newList(list);
    needSave = False;
}

void TMessageAreaEdit::renumberRecords()
{
	char tagName[150];

    if( list->getCount() ){
		for( short i = 0; i < list->getCount(); ++i ){
	    	TMessageArea *rec = (TMessageArea *)list->at(i);
	        sprintf(tagName, "%4d. %s", i + 1, &rec->getText()[TEXT_OFFS]);
			rec->setText(tagName);
		}
	    listBox->drawView();
    }
}

short TMessageAreaEdit::findReply(short origNum)
{
	short retval = 0;
	if( origNum ){
    	origNum--;
    	for( short i = 0; i < list->getCount(); ++i ){
        	TMessageArea *rec = (TMessageArea *)list->at(i);
        	if( origNum == rec->oldNum ){
				retval = i + 1;
                break;
            }
        }
	}
    return retval;
}

void TMessageAreaEdit::saveData()
{
    ifstream ifp;
    ofstream ofp;
    char     newPath[MAXPATH];
    short    i;

    chext(newPath, fileName, ".BKP");
    rmfile(newPath);
    rename(fileName, newPath);

    ifp.open(newPath, ios::binary);
    ofp.open(fileName, ios::binary);
    if( !ifp || !ofp ){
    	messageBox("\003Unable to save the file to disk.", mfError|mfOKButton);
        return;
	}

    makeProgBar("Saving Data", fileName, "Saving to", long(list->getCount()));
    for( i = 0; i < list->getCount(); ++i ){
        MESSAGES fa;
		TMessageArea *rec = (TMessageArea *)list->at(i);
        updateProgBar(long(i+1));
        if( rec->oldNum < maxNumRead ){
	        ifp.seekg(long(rec->oldNum) * sizeof(MESSAGES), ios::beg);
	        ifp.read((uchar*)&fa, sizeof(fa));
            fa.replyBoard = findReply(fa.replyBoard);
        }
        else{
        	memset(&fa, 0, sizeof(fa));
		}
        fa.areaNum = i + 1;
		ofp.write((uchar*)&fa, sizeof(fa));
	}

    ifp.close();
    ofp.close();
    rmfile(newPath);
    for( i = 0; i < list->getCount(); ++i )
    	((TMessageArea *)list->at(i))->oldNum = i + 1;
    renumberRecords();
	destroyProgBar();
    messageBox("\003Data successfully saved.", mfInformation|mfOKButton);
    listBox->drawView();
    needSave = False;
}

void TMessageAreaEdit::globalChange()
{
    short   i;
	Boolean foundTags = False;

    globalSaveMode = 0;
    for(i = 0; i < list->getCount(); ++i){
    	if( ' ' != *((TMessageArea *)list->at(i))->getText() ){
        	foundTags = True;
            break;
		}
	}
    if( !foundTags ){
    	messageBox("\003Global operations need tagged areas!",mfWarning|mfOKButton);
        return;
	}

	TMenuItem *itemList = new TMenuItem("~R~ead flags", cmGlobFlags, kbNoKey, hcGlobFlags);
    itemList->append(new TMenuItem("Read ~l~evel", cmGlobLevel, kbNoKey, hcGlobLevel, 0,
    	new TMenuItem("~W~rite flags", cmGlobWFlags, kbNoKey, hcGlobWFlags, 0,
		new TMenuItem("Write l~e~vel", cmGlobWLevel, kbNoKey, hcGlobWLevel, 0,
		new TMenuItem("~S~ysop flags", cmGlobSFlags, kbNoKey, hcGlobSFlags, 0,
		new TMenuItem("Sysop le~v~el", cmGlobSLevel, kbNoKey, hcGlobSLevel, 0,
		new TMenuItem("All ~g~roups (re)set", cmGlobAllGrps, kbNoKey, hcGlobAllGrps, 0,
		new TMenuItem("~K~ill after", cmGlobKillDays, kbNoKey, hcGlobKillDays, 0,
		new TMenuItem("K~i~ll received", cmGlobKillRecv, kbNoKey, hcGlobKillRecv, 0,
		new TMenuItem("Ma~x~ messages", cmGlobMaxMsgs, kbNoKey, hcGlobMaxMsgs, 0,
		new TMenuItem("Sysop ~n~ame", cmGlobSysop, kbNoKey, hcGlobSysop, 0,
		new TMenuItem("~O~rigin line", cmGlobOrigin, kbNoKey, hcGlobOrigin, 0,
		new TMenuItem("~M~essage kind", cmGlobMKind, kbNoKey, hcGlobMKind, 0,
		new TMenuItem("Message area ~t~ype", cmGlobMType, kbNoKey, hcGlobMType, 0,
		new TMenuItem("Message base ~f~ormat", cmGlobMBase, kbNoKey, hcGlobMBase, 0,
		new TMenuItem("Name o~p~tions", cmGlobMName, kbNoKey, hcGlobMName, 0,
		new TMenuItem("Set system ~A~KA", cmGlobAKA, kbNoKey, hcGlobAKA)))))))))))))))));
    TMenuBox *popup = new TMenuBox(TRect(6,3,35,21), new TMenu(*itemList), 0);

    if( popup ){
	    switch( TProgram::deskTop->execView(popup) ){
        	case cmGlobFlags:
            	if(cmCancel != getFlags(global.readFlags, global.readFlagsNot))
                	globalSaveMode = cmGlobFlags;
            break;
            case cmGlobWFlags:
            	if(cmCancel != getFlags(global.writeFlags, global.writeFlagsNot))
                	globalSaveMode = cmGlobWFlags;
            break;
            case cmGlobSFlags:
            	if(cmCancel != getFlags(global.sysopFlags, global.sysopFlagsNot))
                	globalSaveMode = cmGlobSFlags;
            break;
            case cmGlobLevel:
            	if(cmCancel != getLevel(global.readLevel, "Read Access Level"))
                	globalSaveMode = cmGlobLevel;
			break;
            case cmGlobWLevel:
            	if(cmCancel != getLevel(global.writeLevel, "Write Access Level"))
                	globalSaveMode = cmGlobWLevel;
            break;
            case cmGlobSLevel:
            	if(cmCancel != getLevel(global.sysopLevel, "Sysop Access Level"))
                	globalSaveMode = cmGlobSLevel;
            break;
            case cmGlobAllGrps:
            	if(cmCancel != getBool(global.allGroups, "\003Set areas to 'All Groups' or reset?"))
                	globalSaveMode = cmGlobAllGrps;
            break;
            case cmGlobKillDays:
            	if(cmCancel != getNumber(global.msgKillDays, "Days to keep messages:"))
                	globalSaveMode = cmGlobKillDays;
            break;
            case cmGlobKillRecv:
            	if(cmCancel != getNumber(global.rcvKillDays, "Days to keep received:"))
                	globalSaveMode = cmGlobKillRecv;
            break;
            case cmGlobMaxMsgs:
            	if(cmCancel != getNumber(global.maxMsgs, "Maximum messages in area:"))
                	globalSaveMode = cmGlobMaxMsgs;
            break;
            case cmGlobSysop:
            	if(cmCancel != inputBox("Sysop Name for Areas", "Enter new name:",
						global.sysop, sizeof(global.sysop)))
                	globalSaveMode = cmGlobSysop;
            break;
            case cmGlobOrigin:
            	if(cmCancel != inputBox("Default Origin Line", "Enter new origin line:",
						global.origin, sizeof(global.origin)))
                	globalSaveMode = cmGlobOrigin;
            break;
            case cmGlobMKind:
            	if(cmCancel != getMsgBaseInfo(global.msgKind, enKind))
                	globalSaveMode = cmGlobMKind;
            break;
            case cmGlobMType:
            	if(cmCancel != getMsgBaseInfo(global.msgType, enType))
                	globalSaveMode = cmGlobMType;
            break;
            case cmGlobMBase:
            	if(cmCancel != getMsgBaseInfo(global.msgBaseType, enBase))
                	globalSaveMode = cmGlobMBase;
            break;
            case cmGlobMName:
            	if(cmCancel != getMsgBaseInfo(global.flags, enName))
                	globalSaveMode = cmGlobMName;
            break;
            case cmGlobAKA:
            	if( cmCancel != getAka(global.aka) )
                	globalSaveMode = cmGlobAKA;
            break;
            default         : break;
		}
        destroy(popup);

        if( globalSaveMode ) saveGlobal();
    }
}

void TMessageAreaEdit::saveGlobal()
{
    char      newPath[MAXPATH];
    short     i;
    MESSAGES  fa;
    ifstream  ifp;
    ofstream  ofp;

	if( !globalSaveMode ) return;

    if( cmYes != messageBox("\003Update the global settings?",
							mfConfirmation|mfYesButton|mfNoButton) )
    {
    	return;
    }

    chext(newPath, fileName, ".BKP");
    rmfile(newPath);
    rename(fileName, newPath);

    ifp.open(newPath, ios::binary);
    ofp.open(fileName, ios::binary);
    if( !ifp || !ofp ){
    	messageBox("\003Unable to update the settings.",mfError|mfOKButton);
        return;
	}

    makeProgBar("Save Global Data", fileName, "Saving to", long(list->getCount()));
    for(i = 0; i < list->getCount(); ++i){
    	TMessageArea *rec = (TMessageArea *)list->at(i);
        updateProgBar(long(i+1));
        if( rec->oldNum < maxNumRead ){
        	ifp.seekg(long(rec->oldNum) * sizeof(MESSAGES), ios::beg);
            ifp.read((uchar*)&fa, sizeof(fa));
            fa.replyBoard = findReply(fa.replyBoard);
		}
        else{
        	memset(&fa, 0, sizeof(fa));
		}
        if( ' ' != *rec->getText() ){	// tagged, process it!
        	switch( globalSaveMode ){
	            case cmGlobFlags  :
					fa.readFlags = global.readFlags;
                    fa.readFlagsNot = global.readFlagsNot;
                break;
                case cmGlobWFlags:
                	fa.writeFlags = global.writeFlags;
                    fa.writeFlagsNot = global.writeFlagsNot;
                break;
                case cmGlobSFlags:
                	fa.sysopFlags = global.sysopFlags;
                    fa.sysopFlagsNot = global.sysopFlagsNot;
                break;
                case cmGlobLevel   : fa.readLevel = global.readLevel; break;
                case cmGlobWLevel  : fa.writeLevel = global.writeLevel; break;
                case cmGlobSLevel  : fa.sysopLevel = global.sysopLevel; break;
                case cmGlobAllGrps : fa.allGroups = global.allGroups; break;
                case cmGlobKillDays: fa.msgKillDays = global.msgKillDays; break;
                case cmGlobKillRecv: fa.rcvKillDays = global.rcvKillDays; break;
                case cmGlobMaxMsgs : fa.maxMsgs = global.maxMsgs; break;
                case cmGlobSysop   : strcpy(fa.sysop, global.sysop); break;
                case cmGlobOrigin  : strcpy(fa.origin, global.origin); break;
                case cmGlobMKind   : fa.msgKind = global.msgKind; break;
                case cmGlobMType   : fa.msgType = global.msgType; break;
                case cmGlobMBase   : fa.msgBaseType = global.msgBaseType; break;
                case cmGlobMName   : fa.flags = global.flags; break;
                case cmGlobAKA     : fa.aka = global.aka; break;
			}
		}
        ofp.write((uchar*)&fa, sizeof(fa));
	}

    ifp.close();
    ofp.close();
    rmfile(newPath);
    for( i = 0; i < list->getCount(); ++i )
    	((TMessageArea *)list->at(i))->oldNum = i + 1;
    renumberRecords();
    destroyProgBar();
    needSave = False;
    messageBox("\003Global settings have been saved.", mfInformation|mfOKButton);
}

void TMessageAreaEdit::newRecord()
{
    short    atItemNum = listBox->focused, itemNumRpt;
    char     tagName[150];

    if( list->getCount() < MAXRECS ){
	    itemNumRpt = atItemNum + 1;
	    if( atItemNum == list->getCount() - 1){
        	char *msg = "\003Do you want to append the entry?";
	    	if(cmYes == messageBox(msg,mfConfirmation|mfYesButton|mfNoButton)){
	        	atItemNum++;
	            itemNumRpt++;
			}
		}
	    sprintf(tagName, "%4d. %s", itemNumRpt, strNoName);
        // make sure the oldNum id is higher than the # read
	    list->atInsert(atItemNum, new TMessageArea(32000, tagName));
	    listBox->setRange(list->getCount());
        listBox->focusItem(atItemNum);
	    listBox->drawView();
        needSave = True;
    }
}

Boolean TMessageAreaEdit::valid(ushort command)
{
	static char *msg = "\003File has not been saved.\n \n"
					   "\003Do you want to save it now?";

	Boolean retval = Boolean(TBaseEdit::valid(command) && isValid);
    if( retval && cmCancel == command &&
		cmYes == messageBox(msg, mfWarning|mfYesButton|mfNoButton) )
	{
		saveData();
    }
    return retval;
}

void TMessageAreaEdit::editRecord()
{
	MESSAGES fa;
	long     num = long(((TMessageArea *)list->at(listBox->focused))->oldNum);
    fstream  stream(fileName, ios::binary|ios::in|ios::out);

    if( needSave ){
    	messageBox("\003Records were added. Save first",mfError|mfOKButton);
        return;
	}

    if( !stream ){
		messageBox("\003Cannot access file.", mfError|mfOKButton);
        return;
    }
    stream.seekg(num * sizeof(fa), ios::beg);
    stream.read((uchar *)&fa, sizeof(fa));

    if( cmCancel != editMesgArea(fa) ){
    	if(cmYes == messageBox("\003Save changes?", mfConfirmation|mfYesButton|mfNoButton)){
	    	char buf[150];
		    stream.seekp(num * sizeof(fa), ios::beg);
			stream.write((uchar *)&fa, sizeof(fa));
	        sprintf(buf, "%4d. %s", listBox->focused+1, EOS != fa.name[0] ? fa.name : strNoName);
	        ((TMessageArea *)list->at(listBox->focused))->setText(buf);
	        listBox->drawView();
        }
    }
    stream.close();		// not really necessary, but what the heck!
}

void TMessageAreaEdit::handleEvent(TEvent &event)
{
	if( evKeyboard == event.what ){
    	if( kbEnter == event.keyDown.keyCode && listBox == current ){
	    	editRecord();
            clearEvent(event);
        }
        else if( ' ' == event.keyDown.charScan.charCode )
        	event.keyDown.keyCode = kbEnter;
	}

	TBaseEdit::handleEvent(event);

    if( evKeyboard == event.what ){
    	switch( event.keyDown.keyCode ){
        	case kbDel: deleteRecord(); break;
        	case kbIns: newRecord(); break;
            default   : listBox->select(); return;
		}
        clearEvent(event);
        listBox->select();
	}
    else if( evCommand == event.what ){
    	switch( event.message.command ){
            case cmDelete    : deleteRecord(); break;
            case cmPack      : packRecords(); break;
            case cmMove      : moveTagged(False); break;
            case cmSort      : moveTagged(True); break;
        	case cmSave      : saveData(); break;
            case cmRenumber  : renumberRecords(); break;
            case cmNew       : newRecord(); break;
            case cmFindFirst : findFirst("Find File Area by Name"); break;
            case cmFindNext  : findNext(); break;
            case cmGotoNum   : gotoRecNum("Go to Area Number"); break;
            case cmChangeRecs: globalChange(); break;
            case cmEdit      : editRecord(); break;
        	default          : listBox->select(); return;
		}
        clearEvent(event);
        listBox->select();
	}
}

