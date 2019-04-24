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
#include "files.h"
#include "confcmds.h"
#include "fsys.h"
#include "strlib.h"
#include "gadgets.h"
#include "tv/tvutil.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#define Uses_TMenu
#define Uses_TMenuBox
#define Uses_TMenuItem
#define Uses_TSubMenu
#include <tv.h>

#define MAXRECS 5001		/* max records per file */

////////////////////////////////////////////////////////////////////////////
// Record initialization
////////////////////////////////////////////////////////////////////////////
TFileArea::TFileArea(short aNum, const char *aTagName):
	TTagEntry(aTagName)
{
	oldNum = aNum;
}

////////////////////////////////////////////////////////////////////////////
// The derived class
////////////////////////////////////////////////////////////////////////////
TFileAreaEdit::TFileAreaEdit(const char *aFileName):
	TBaseEdit( "Arrange File Areas"),
    TWindowInit(TFileAreaEdit::initFrame)
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
	insert(new TLabel(TRect(4,2,16,3), "File ~a~reas ", listBox));

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

TFileAreaEdit::~TFileAreaEdit()
{
}

void TFileAreaEdit::shutDown()
{
	if( list ){
    	list->freeAll();
        destroy(list);
		list = 0;
    }
    TDialog::shutDown();
}

void TFileAreaEdit::loadData()
{
	ifstream      ifp;
    ofstream      ofp;
   	FILEAREA      area;
	char          tagName[150];
    short         i, numRecs;

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
    numRecs = short(ifp.tellg() / sizeof(FILEAREA));
    ifp.seekg(0, ios::beg);
	list = new TBaseCollection(1500, 250, False);
    makeProgBar("Loading Data", fileName, "Loading from", long(numRecs));

	for( i = 0; i < MAXRECS; ++i ){
    	ifp.read((uchar*)&area, sizeof(area));
	    if( ifp.eof() ) break;
        ofp.write((uchar*)&area, sizeof(area));
        updateProgBar(long(i+1));
	    sprintf(tagName, "%4d. %s", i+1, (EOS != *area.name) ? area.name : strNoName);
		TFileArea *fa = new TFileArea(i, tagName);
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

void TFileAreaEdit::renumberRecords()
{
	char tagName[150];

    if( list->getCount() ){
		for( short i = 0; i < list->getCount(); ++i ){
	    	TFileArea *rec = (TFileArea *)list->at(i);
	        sprintf(tagName, "%4d. %s", i + 1, &rec->getText()[TEXT_OFFS]);
			rec->setText(tagName);
		}
	    listBox->drawView();
    }
}

void TFileAreaEdit::saveData()
{
    ifstream      ifp;
    ofstream      ofp;
    char          newPath[MAXPATH];

    chext(newPath, fileName, ".BKP");
    rmfile(newPath);
    rename(fileName, newPath);

    ofp.open(fileName, ios::binary);
    ifp.open(newPath, ios::binary);
    if( !ofp || !ifp ){
    	messageBox("\003Unable to save the file to disk.", mfError|mfOKButton);
        return;
	}

    makeProgBar("Saving Data", fileName, "Saving to", long(list->getCount()));

    for( short i = 0; i < list->getCount(); ++i ){
    	char     aTagName[150];
        FILEAREA fa;
        updateProgBar(long(i+1));
		TFileArea *rec = (TFileArea *)list->at(i);
        if( rec->oldNum < maxNumRead ){
        	ifp.seekg(long(rec->oldNum) * sizeof(FILEAREA), ios::beg);
            ifp.read((uchar*)&fa, sizeof(fa));

            // has the special control file for multiple dirs?
            sprintf(aTagName, "%sFA_%d.CTL", basedir(fileName), rec->oldNum+1);
            if( stristr(fa.filepath, aTagName) ){
            	char oldpath[MAXPATH], newname[MAXPATH];
                sprintf(oldpath, "%sFA_%d.CTL", basedir(fileName), i+1);
            	if( 0 == access(oldpath, 0) ){	// duplicate name, rename
                	chext(newname, oldpath, ".CT$");
                	rename(oldpath, newname);
				}
                if( 0 != access(aTagName, 0) ){	// check if rename (dupe)
                	chext(newname, aTagName, ".CT$");
                	if( 0 == access(newname, 0) )
                    	rename(newname, oldpath);
				}
                else rename(aTagName, oldpath);	// simply rename it
                strcpy(fa.filepath, oldpath);
			}
        }
        else{
        	memset(&fa, 0, sizeof(fa));
		}
        ofp.write((uchar*)&fa, sizeof(fa));
        rec->oldNum = i;
        sprintf(aTagName, "%4d. %s", rec->oldNum+1, &rec->getText()[TEXT_OFFS]);
	    rec->setText(aTagName);
	}

    ifp.close();
    ofp.close();
    rmfile(newPath);

    destroyProgBar();
    needSave = False;
    messageBox("\003Data successfully saved.", mfInformation|mfOKButton);
    listBox->drawView();
}

void TFileAreaEdit::globalChange()
{
    short   i;
	Boolean foundTags = False;

    globalSaveMode = 0;
    for(i = 0; i < list->getCount(); ++i){
    	if( ' ' != *((TFileArea *)list->at(i))->getText() ){
        	foundTags = True;
            break;
		}
	}
    if( !foundTags ){
    	messageBox("\003Global operations need tagged areas!",mfWarning|mfOKButton);
        return;
	}

	TMenuItem *itemList = new TMenuItem("Security ~f~lags", cmGlobFlags, kbNoKey, hcGlobFlags);
    itemList->append(new TMenuItem("Security ~l~evel", cmGlobLevel, kbNoKey, hcGlobLevel, 0,
		new TMenuItem("~A~ll groups", cmGlobAllGrps, kbNoKey, hcGlobAllGrps, 0,
		new TMenuItem("~C~D-ROM areas", cmGlobCDROM, kbNoKey, hcGlobCDROM, 0,
		new TMenuItem("F~i~le location", cmGlobLoc, kbNoKey, hcGlobLoc, 0,
		new TMenuItem("File li~s~tings", cmGlobList, kbNoKey, hcGlobList, 0,
		new TMenuItem("~N~o top statistics", cmGlobTops, kbNoKey, hcGlobTops, 0,
		new TMenuItem("Define as f~r~ee", cmGlobFree, kbNoKey, hcGlobFree, 0,
		new TMenuItem("~M~ax files allowed", cmGlobMaxFiles, kbNoKey, hcGlobMaxFiles, 0,
		new TMenuItem("Max ~K~B per day", cmGlobMaxKB, kbNoKey, hcGlobMaxKB))))))))));
    TMenuBox *popup = new TMenuBox(TRect(6,4,35,16), new TMenu(*itemList), 0);

    if( popup ){
	    switch( TProgram::deskTop->execView(popup) ){
        	case cmGlobFlags:
            	if( cmCancel != getFlags(global.flags, (ulong)global.flagsNot) )
                    globalSaveMode = cmGlobFlags;
            break;
            case cmGlobLevel:
            	if( cmCancel != getLevel(global.level, "Global Level") )
                    globalSaveMode = cmGlobLevel;
            break;
            case cmGlobAllGrps:
            	if( cmCancel != getBool(global.allGroups, "\003Set areas to 'All Groups' to reset?") )
                	globalSaveMode = cmGlobAllGrps;
            break;
            case cmGlobCDROM:
            	if( cmCancel != getBool(global.copyLocal, "\003Mark areas as CD-ROM or unmark?") )
                	globalSaveMode = cmGlobCDROM;
            break;
            case cmGlobTops:
            	if( cmCancel != getBool(global.notops, "\003Include areas in top file lists or exclude?") ){
                	global.notops = !global.notops;	// reverse the boolean
                    globalSaveMode = cmGlobTops;
				}
            break;
            case cmGlobFree:
            	if( cmCancel != getBool(global.free, "\003Shoud these areas be free or not?") )
                	globalSaveMode = cmGlobFree;
            break;
            case cmGlobMaxFiles:
            	if(cmCancel != getNumber((ushort)global.maxfiles, "Enter max files limit:"))
                	globalSaveMode = cmGlobMaxFiles;
            break;
            case cmGlobMaxKB:
            	if(cmCancel != getNumber((ushort)global.maxkb, "Enter max KB limit:"))
                	globalSaveMode = cmGlobMaxKB;
            break;
            case cmGlobLoc:
            	if(cmCancel != inputBox("Change Location of Files",
					"Drive (or D:\\DIR):",
					global.filepath, sizeof(global.filepath) - 1))
                {
                	strupr(global.filepath);
                    if( 3 < strlen(global.filepath) ){
                    	if('\\' != global.filepath[strlen(global.filepath)-1])
                        	strcat(global.filepath, "\\");
					}
                	globalSaveMode = cmGlobLoc;
                }
            break;
            case cmGlobList:
            	if(cmCancel != inputBox("Change Location of Listings",
					"Drive (or D:\\DIR):",
					global.listpath, sizeof(global.listpath) - 1))
                {
                	strupr(global.listpath);
                    if( 3 < strlen(global.listpath) ){
                    	if('\\' != global.listpath[strlen(global.listpath)-1])
                        	strcat(global.listpath, "\\");
					}
                	globalSaveMode = cmGlobList;
                }
            break;
            default         : break;
		}
        destroy(popup);

        if( globalSaveMode ) saveGlobal();
    }
}

void TFileAreaEdit::saveGlobal()
{
    char      newPath[MAXPATH], buf[100];
    FILEAREA  fa;
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

    makeProgBar("Saving Global Data", fileName, "Saving to", long(list->getCount()));
    for(short i = 0; i < list->getCount(); ++i){
    	updateProgBar(long(i+1));
    	TFileArea *rec = (TFileArea *)list->at(i);
        if( rec->oldNum < maxNumRead ){
        	ifp.seekg(long(rec->oldNum) * sizeof(FILEAREA), ios::beg);
            ifp.read((uchar*)&fa, sizeof(fa));
		}
        else{
        	memset(&fa, 0, sizeof(fa));
		}
		rec->oldNum = i;
        if( ' ' != *rec->getText() ){	// tagged, process it!
        	switch( globalSaveMode ){
	            case cmGlobFlags:
                	fa.flags = global.flags;
                    fa.flagsNot = global.flagsNot;
                break;
                case cmGlobLevel   : fa.level = global.level; break;
                case cmGlobAllGrps : fa.allGroups = global.allGroups; break;
                case cmGlobCDROM   : fa.copyLocal = global.copyLocal; break;
                case cmGlobTops    : fa.notops = global.notops; break;
                case cmGlobFree    : fa.free = global.free; break;
                case cmGlobMaxFiles: fa.maxfiles = global.maxfiles; break;
                case cmGlobMaxKB   : fa.maxkb = global.maxkb; break;
                case cmGlobLoc:
                	if( 3 < strlen(global.filepath) ){
                    	sprintf(buf, "%s%s", global.filepath,&fa.filepath[3]);
                        strcpy(fa.filepath, buf);
                    }
                	else fa.filepath[0] = global.filepath[0];
                break;
                case cmGlobList:
                	if( 3 < strlen(global.listpath) ){
                    	sprintf(buf, "%s%s", global.listpath,&fa.listpath[3]);
                        strcpy(fa.listpath, buf);
                    }
                	else fa.listpath[0] = global.listpath[0];
                break;
			}
		}
        ofp.write((uchar*)&fa, sizeof(fa));
	}

    ifp.close();
    ofp.close();
    rmfile(newPath);
    destroyProgBar();
    needSave = False;
    messageBox("\003Global settings have been saved.", mfInformation|mfOKButton);
}

void TFileAreaEdit::newRecord()
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
	    list->atInsert(atItemNum, new TFileArea(32000, tagName));
	    listBox->setRange(list->getCount());
        listBox->focusItem(atItemNum);
	    listBox->drawView();
        needSave = True;
    }
}

Boolean TFileAreaEdit::valid(ushort command)
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

void TFileAreaEdit::editRecord()
{
	FILEAREA fa;
	long     num = long(((TFileArea *)list->at(listBox->focused))->oldNum);
    fstream  stream(fileName, ios::binary|ios::in|ios::out);

    if( needSave ){
    	messageBox("\003You have inserted record(s). Save first.",mfError|mfOKButton);
        return;
	}

    if( !stream ){
		messageBox("\003Cannot access file.", mfError|mfOKButton);
        return;
    }
    stream.seekg(num * sizeof(FILEAREA), ios::beg);
    stream.read((uchar *)&fa, sizeof(fa));

    if( cmCancel != editFileArea(fa) ){
    	if(cmYes == messageBox("\003Save changes?", mfConfirmation|mfYesButton|mfNoButton)){
	    	char buf[150];
		    stream.seekp(num * sizeof(FILEAREA), ios::beg);
			stream.write((uchar *)&fa, sizeof(fa));
	        sprintf(buf, "%4d. %s", listBox->focused+1, EOS != fa.name[0] ? fa.name : strNoName);
	        ((TFileArea *)list->at(listBox->focused))->setText(buf);
	        listBox->drawView();
        }
    }
    stream.close();		// not really necessary, but what the heck!
}

void TFileAreaEdit::handleEvent(TEvent &event)
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

