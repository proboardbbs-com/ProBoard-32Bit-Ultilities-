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
#include "basedit.h"
#include "confcmds.h"
#include "fsys.h"
#include "strlib.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

const char *strNoName = "תתתÿ";

// the sorting function for the sorted collection
TBaseCollection::TBaseCollection(short aLimit, short aDelta, Boolean aSortMode):
	TSortedCollection(aLimit, aDelta)
{
	shouldSort = aSortMode;
	foundError = False;
}

int TBaseCollection::compare(void *key1, void *key2)
{
	if( shouldSort ){
		char *text1 = ((TTagEntry *)key1)->getText();
	    char *text2 = ((TTagEntry *)key2)->getText();
		return stricmp(&text1[TEXT_OFFS], &text2[TEXT_OFFS]);
    }
    else return -1;
}

void TBaseCollection::error(ccIndex, ccIndex)
{
	messageBox("\003Fatal memory error. Aborting...", mfError|mfOKButton);
    foundError = True;
}


// the base editor class
TBaseEdit::TBaseEdit(const char *aTitle):
	TDialog(TRect(1, 1, 78, 21), aTitle),
	TWindowInit(TDialog::initFrame),
    list(0), listBox(0)
{
	strcpy(findStr, "");
    findIndex = -1;
}

TBaseEdit::~TBaseEdit()
{
	if( list ){
    	list->freeAll();
		destroy( list );
    }
}

void TBaseEdit::findFirst(const char *title)
{
	if(cmCancel != inputBox(title,"Search string: ",findStr,sizeof(findStr)-1)){
    	findIndex = -1;
		findNext();
    }
}

void TBaseEdit::findNext()
{
	if( list->getCount() && EOS != findStr[0] ){
    	for(short i = findIndex + 1; i < list->getCount(); ++i ){
        	char *text = ((TTagEntry *)list->at(i))->getText();
        	if( NULL != stristr(&text[TEXT_OFFS], findStr) ){
            	findIndex = i;
            	listBox->focusItem(i);
                return;
			}
		}
        messageBox("\003Search string not found.", mfInformation|mfOKButton);
	}
}

void TBaseEdit::gotoRecNum(const char *title)
{
	char text[50] = "";
	if( cmCancel != inputBox(title, "Enter number: ", text, 6) ){
    	short ndx = atoi(text) - 1;
        if( 0 > ndx || ndx >= list->getCount() ){
        	sprintf(text, "\003'%d' is out of range.\n\003"
						"Must be from 1 to %d", ndx+1, list->getCount());
        	messageBox(text, mfError|mfOKButton);
		}
        else listBox->focusItem(ndx);
	}
}

void TBaseEdit::handleEvent(TEvent &event)
{
	TDialog::handleEvent(event);
}

void TBaseEdit::deleteRecord()
{
	if( list->getCount() ){
	    list->atFree(listBox->focused);
        listBox->setRange(list->getCount());
        listBox->drawView();
    }
}

void TBaseEdit::packRecords()
{
	static char *msg = "\003This operation is not reversible.\n \n"
					   "\003Pack records now?";

	if( list->getCount() ){
    	if( cmYes == messageBox(msg, mfWarning|mfYesButton|mfNoButton) ){
        	for( short i = 0; i < list->getCount(); ++i ){
            	char *cmpText = ((TTagEntry *)list->at(i))->getText();
            	if(!strcmp(strNoName, &cmpText[TEXT_OFFS])) list->atFree(i--);
			}
            listBox->setRange(list->getCount());
            listBox->drawView();
		}
	}
}

void TBaseEdit::moveTagged(Boolean sortOnMove)
{
    TBaseCollection *tagList;
    void            *atItem = 0;

	if( list->getCount() ){
    	// find the insertion point (scan from focused back)
        for( short i = listBox->focused; 0 < i; --i ){
        	if( ' ' == *((TTagEntry *)list->at(i))->getText() ){
            	atItem = list->at(i);
            	break;
			}
		}

        tagList = new TBaseCollection(100, 10, False);
        assert(0 != tagList);
        if( sortOnMove ){
        	tagList->setSort(True);
            tagList->duplicates = True;
		}

        // loop through the list and remove all tagged entries
        for( i = 0; i < list->getCount(); ++i ){
        	if( ' ' != *((TTagEntry *)list->at(i))->getText() ){
            	tagList->insert(list->at(i));
                list->atRemove(i--);
			}
		}

        // now, see where our insertion point is
        short atItemNum = atItem ? list->indexOf(atItem) + 1 : 0;
        short itemToFocus = atItemNum ? atItemNum - 1 : 0;

        for( i = 0; i < tagList->getCount(); ++i ){
        	list->atInsert(atItemNum++, tagList->at(i));
		}

        tagList->removeAll();
        delete tagList;
        listBox->setRange(list->getCount());
	    listBox->focusItem(itemToFocus);
	    listBox->drawView();
    }
}

