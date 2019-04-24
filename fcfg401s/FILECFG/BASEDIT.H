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
#ifndef __BASEDIT_H
#define __BASEDIT_H

#define Uses_TDialog
#define Uses_TEvent
#define Uses_TLabel
#define Uses_TButton
#define Uses_MsgBox
#define Uses_TCollection
#define Uses_TSortedCollection
#define Uses_TApplication
#define Uses_TDeskTop
#include <tv.h>

#include "tv/taglist.h"
#include "pblib.h"

extern const char *strNoName;
#define TEXT_OFFS 7

class TBaseCollection: public TSortedCollection
{
public:
	TBaseCollection(short aLimit, short aDelta, Boolean aSortMode);
    virtual int compare(void *key1, void *key2);
    void        setSort(Boolean aSortMode){ shouldSort = aSortMode; }
    void        error(ccIndex code, ccIndex);
    Boolean     getError() const { return foundError; }

private:
	Boolean shouldSort;
    Boolean foundError;

	void  writeItem(void *, opstream&){ return; }
    void *readItem(ipstream&){ return 0; }
};

class TBaseEdit: public TDialog
{
public:
	TBaseEdit(const char *aTitle);
    ~TBaseEdit();

    virtual void handleEvent(TEvent &event);

    virtual void deleteRecord();
    virtual void packRecords();
    virtual void moveTagged(Boolean sortOnMove);
    virtual void findFirst(const char *text);
    virtual void findNext();
    virtual void gotoRecNum(const char *text);

protected:
	ccIndex          findIndex;
    char             findStr[81];
    TTagListBox     *listBox;
    TBaseCollection *list;
};

#endif
