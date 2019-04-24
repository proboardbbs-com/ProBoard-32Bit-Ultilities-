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
#include "groups.h"
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

////////////////////////////////////////////////////////////////////////////
// Record initialization
////////////////////////////////////////////////////////////////////////////
TGroupRec::TGroupRec(short aNum, const char *aTagName):
	TTagEntry(aTagName)
{
	oldNum = aNum;
}

////////////////////////////////////////////////////////////////////////////
// The derived class
////////////////////////////////////////////////////////////////////////////
TGroupEdit::TGroupEdit(const char *aSysDir, enum_mode aMode):
	TBaseEdit( "Arrange Groups"),
    TWindowInit(TGroupEdit::initFrame),
    mode(aMode)
{
    TView *pv;

	// save the filename
    sprintf(fileName, "%s\\%s", aSysDir, mode == file ? fnFGROUPS : fnMGROUPS);
    maxNumRead = 0;

    // build the list box
	TScrollBar *sb = new TScrollBar(TRect(73,3,74,13));
    listBox = new TTagListBox(TRect(4,3,73,13), sb);
    listBox->options |= ofFramed;
    listBox->helpCtx = hcListBox;
    insert(sb);
    insert(listBox);
	insert(new TLabel(TRect(4,2,16,3), "Gr~o~ups ", listBox));

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
TGroupEdit::~TGroupEdit()
{
}

void TGroupEdit::shutDown()
{
	if( list ){
    	list->freeAll();
        destroy(list);
		list = 0;
    }
    TDialog::shutDown();
}

void TGroupEdit::loadData()
{
	ifstream      ifp;
    ofstream      ofp;
   	GROUPDEF      area;
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
    numRecs = short(ifp.tellg() / sizeof(GROUPDEF));
    ifp.seekg(0, ios::beg);
	list = new TBaseCollection(50, 10, False);
    makeProgBar("Loading Data", fileName, "Loading from", long(numRecs));

	for( i = 0; !ifp.eof(); ++i ){
    	ifp.read((uchar*)&area, sizeof(area));
	    if( ifp.eof() ) break;
        ofp.write((uchar*)&area, sizeof(area));
        updateProgBar(long(i+1));
	    sprintf(tagName, "%4d. %s", i+1, (EOS != *area.name) ? area.name : strNoName);
		TGroupRec *fa = new TGroupRec(i, tagName);
		list->insert(fa);
    }

    maxNumRead = list->getCount();
    ifp.close();
    ofp.close();
    isValid = True;
    destroyProgBar();
	listBox->newList(list);
    needSave = False;
}

void TGroupEdit::renumberRecords()
{
	char tagName[150];

    if( list->getCount() ){
		for( short i = 0; i < list->getCount(); ++i ){
	    	TGroupRec *rec = (TGroupRec *)list->at(i);
	        sprintf(tagName, "%4d. %s", i + 1, &rec->getText()[TEXT_OFFS]);
			rec->setText(tagName);
		}
	    listBox->drawView();
    }
}

void TGroupEdit::saveOldData()
{
	ofstream ofp;
	uchar    groups[255];
    Boolean  changed = False;
    char     path[MAXPATH];

    memset(groups, 0, sizeof(groups));
    for( short i = 0; i < list->getCount(); ++i ){
    	if( i != ((TGroupRec *)list->at(i))->oldNum ) changed = True;
    	groups[((TGroupRec *)list->at(i))->oldNum % 255] = i + 1;
    }

    if( changed ){
	    chext(path, fileName, ".FCB");
	    ofp.open(path, ios::out|ios::binary);
	    if( !ofp ){
	    	messageBox("\003Cannot save group information.",mfError|mfOKButton);
	        return;
		}
	    ofp.write(groups, sizeof(groups));
	    ofp.close();
    }
}

void TGroupEdit::saveData()
{
    ifstream      ifp;
    ofstream      ofp;
    char          newPath[MAXPATH];

    saveOldData();
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
        GROUPDEF fa;
        updateProgBar(long(i+1));
		TGroupRec *rec = (TGroupRec *)list->at(i);
        if( rec->oldNum < maxNumRead ){
        	ifp.seekg(long(rec->oldNum) * sizeof(GROUPDEF), ios::beg);
            ifp.read((uchar*)&fa, sizeof(fa));
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
    messageBox("\003Data successfully saved.", mfInformation|mfOKButton);

    char *msg = "\003Group numbers may have changed. Update areas?";
    if( cmYes == messageBox(msg, mfConfirmation|mfYesButton|mfNoButton) )
    	updateAreas();

    listBox->drawView();
    needSave = False;
}


void TGroupEdit::globalChange()
{
    short   i;
	Boolean foundTags = False;

    globalSaveMode = 0;
    for(i = 0; i < list->getCount(); ++i){
    	if( ' ' != *((TGroupRec *)list->at(i))->getText() ){
        	foundTags = True;
            break;
		}
	}
    if( !foundTags ){
    	messageBox("\003Global operations need tagged areas!",mfWarning|mfOKButton);
        return;
	}

    saveOldData();

	TMenuItem *itemList = new TMenuItem("Security ~f~lags", cmGlobFlags, kbNoKey, hcGlobFlags);
    itemList->append(new TMenuItem("Security ~l~evel", cmGlobLevel, kbNoKey, hcGlobLevel));
    TMenuBox *popup = new TMenuBox(TRect(6,4,35,8), new TMenu(*itemList), 0);

    if( popup ){
	    switch( TProgram::deskTop->execView(popup) ){
        	case cmGlobFlags:
            	if( cmCancel != getFlags((ulong)global.flags, (ulong)global.flagsNot) )
                    globalSaveMode = cmGlobFlags;
            break;
            case cmGlobLevel:
            	if( cmCancel != getLevel(global.level, "Global Level") )
                    globalSaveMode = cmGlobLevel;
            break;
		}
        destroy(popup);
        if( globalSaveMode ) saveGlobal();
    }
}

void TGroupEdit::saveGlobal()
{
    char      newPath[MAXPATH];
    GROUPDEF  fa;
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
    	TGroupRec *rec = (TGroupRec *)list->at(i);
        if( rec->oldNum < maxNumRead ){
        	ifp.seekg(long(rec->oldNum) * sizeof(GROUPDEF), ios::beg);
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

void TGroupEdit::newRecord()
{
    short    atItemNum = listBox->focused, itemNumRpt;
    char     tagName[150];

    if( list->getCount() < 256 ){
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
	    list->atInsert(atItemNum, new TGroupRec(32000, tagName));
	    listBox->setRange(list->getCount());
        listBox->focusItem(atItemNum);
	    listBox->drawView();
        needSave = True;
    }
}

Boolean TGroupEdit::valid(ushort command)
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

void TGroupEdit::editRecord()
{
	GROUPDEF fa;
	long     num = long(((TGroupRec *)list->at(listBox->focused))->oldNum);
    fstream  stream(fileName, ios::binary|ios::in|ios::out);

    if( needSave ){
    	messageBox("\003Records were added. Save first.",mfError|mfOKButton);
        return;
	}

    if( !stream ){
		messageBox("\003Cannot access file.", mfError|mfOKButton);
        return;
    }
    stream.seekg(num * sizeof(GROUPDEF), ios::beg);
    stream.read((uchar *)&fa, sizeof(fa));

    if( cmCancel != editGroup(fa) ){
    	if(cmYes == messageBox("\003Save changes?", mfConfirmation|mfYesButton|mfNoButton)){
	    	char buf[150];
		    stream.seekp(num * sizeof(GROUPDEF), ios::beg);
			stream.write((uchar *)&fa, sizeof(fa));
	        sprintf(buf, "%4d. %s", listBox->focused+1, EOS != fa.name[0] ? fa.name : strNoName);
	        ((TGroupRec *)list->at(listBox->focused))->setText(buf);
	        listBox->drawView();
        }
    }
    stream.close();		// not really necessary, but what the heck!
}

void TGroupEdit::updateAreas()
{
	ifstream       fp;
    ofstream       ofp;
    char           path[MAXPATH], temp[MAXPATH];
    short          numRecs, num;
    uchar          groups[255];

    chext(path, fileName, ".FCB");
    fp.open(path, ios::in|ios::binary);
    if( !fp ){
    	messageBox("\003You don't seem to have changed groups!",mfError|mfOKButton);
    	return;
	}
    fp.read(groups, sizeof(groups));
    fp.close();
    rmfile(path);

    if( TGroupEdit::file == mode ){
    	sprintf(temp, "%s%s", basedir(fileName), fnFILECFG);
        chext(path, temp, ".BKP"); rmfile(path); rename(temp, path);
	    fp.open(path, ios::in|ios::nocreate|ios::binary);
        ofp.open(temp, ios::out|ios::binary);
        if( fp && ofp ){
            FILEAREA fa;

            fp.seekg(0, ios::end);
            numRecs = short(fp.tellg() / sizeof(FILEAREA));
            fp.seekg(0, ios::beg);
        	makeProgBar("Update File Groups", temp, "Updating data in", numRecs);
            fp.read((uchar*)&fa, sizeof(fa)); num = 1;
            while( !fp.eof() ){
            	for(short j = 0; j < 4; ++j ){
	            	if( fa.groups[j] )
						fa.groups[j] = groups[fa.groups[j] - 1];
                }
	            ofp.write((uchar*)&fa, sizeof(fa));
	            fp.read((uchar*)&fa, sizeof(fa));
                updateProgBar(++num);
			}
		}
	}
    else{
    	sprintf(temp, "%s%s", basedir(fileName), fnMESSAGES);
        chext(path, temp, ".BKP"); rmfile(path); rename(temp, path);
	    fp.open(path, ios::in|ios::nocreate|ios::binary);
        ofp.open(temp, ios::out|ios::binary);
        if( fp && ofp ){
            MESSAGES fa;

            fp.seekg(0, ios::end);
            numRecs = short(fp.tellg() / sizeof(MESSAGES));
            fp.seekg(0, ios::beg);
        	makeProgBar("Update File Groups", temp, "Updating data in", numRecs);
            fp.read((uchar*)&fa, sizeof(fa)); num = 1;
            while( !fp.eof() ){
            	for(short j = 0; j < 4; ++j ){
	            	if( fa.groups[j] )
						fa.groups[j] = groups[fa.groups[j] - 1];
                }
	            ofp.write((uchar*)&fa, sizeof(fa));
	            fp.read((uchar*)&fa, sizeof(fa));
                updateProgBar(++num);
			}
		}
	}

    rmfile(path);
    fp.close();
    ofp.close();
    destroyProgBar();
    messageBox("\003Groups updated successfully.", mfInformation|mfOKButton);
}

void TGroupEdit::handleEvent(TEvent &event)
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
            case cmFindFirst : findFirst("Find Group by Name"); break;
            case cmFindNext  : findNext(); break;
            case cmGotoNum   : gotoRecNum("Go to Group Number"); break;
            case cmChangeRecs: globalChange(); break;
            case cmEdit      : editRecord(); break;
        	default          : listBox->select(); return;
		}
        clearEvent(event);
        listBox->select();
	}
}

