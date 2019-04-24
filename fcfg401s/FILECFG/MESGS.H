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
#ifndef __MESGS_H
#define __MESGS_H
#include "basedit.h"
#include "pblib.h"

class TMessageArea: public TTagEntry
{
public:
	TMessageArea(short aNum, const char *aTagName);
    short  oldNum;
};

class TMessageAreaEdit: public TBaseEdit
{
public:
	TMessageAreaEdit(const char *aFileName);
    ~TMessageAreaEdit();

    virtual void shutDown();
    virtual void handleEvent(TEvent &event);

    void loadData();
    void saveData();
    void newRecord();
    void globalChange();
    void saveGlobal();
    void renumberRecords();
    void editRecord();

    Boolean valid(ushort command);

protected:
	ushort   editGlobal();
    ushort   globalSaveMode;
    short    findReply(short origNum);

	Boolean  isValid, needSave;
    ccIndex  foundAt;
    char     searchText[80];
    short    maxNumRead;
	char     fileName[MAXPATH];
    MESSAGES global;
};

#endif
