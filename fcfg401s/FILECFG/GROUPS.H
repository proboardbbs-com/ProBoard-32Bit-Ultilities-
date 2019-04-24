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
#ifndef __GROUPS_H
#define __GROUPS_H

#include "basedit.h"
#include "pblib.h"

typedef FILEGROUP GROUPDEF;

class TGroupRec: public TTagEntry
{
public:
	TGroupRec(short aNum, const char *aName);
    short oldNum;
};

class TGroupEdit: public TBaseEdit
{
public:
	enum enum_mode { file, mesg };
	TGroupEdit(const char *sysDir, enum_mode aMode);
    ~TGroupEdit();

    void handleEvent(TEvent &event);
    void shutDown();

    void loadData();
    void saveData();
    void newRecord();
    void globalChange();
    void saveGlobal();
    void renumberRecords();
    void editRecord();

    Boolean valid(ushort command);

protected:
	void     editGlobal();
    void     updateAreas();
    void     saveOldData();

	Boolean   isValid, needSave;
    short     maxNumRead;
    ushort    globalSaveMode;
	char      fileName[MAXPATH];
    char      searchText[80];
    ccIndex   foundAt;
    enum_mode mode;
    GROUPDEF  global;
};

#endif
