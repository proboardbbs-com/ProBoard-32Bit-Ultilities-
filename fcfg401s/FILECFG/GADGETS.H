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
#ifndef __GADGETS_H
#define __GADGETS_H

#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TObject
#define Uses_MsgBox
#define Uses_TStringCollection
#define Uses_TListBox
#define Uses_TDialog
#define Uses_TButton
#define Uses_TLabel
#define Uses_TMouse
#include "tv/flagsel.h"
#include "tv/akasel.h"
#include "tv/tprogbar.h"
#include "fedit.h"
#include "gedit.h"
#include "medit.h"
#include <tv.h>
#include "pblib.h"

enum eMsgInfo { enKind, enType, enBase, enName };	// for getMsgInfo

ushort getLevel(ushort &level, char *title);
ushort getBool(bool &toggle, char *text);
ushort getNumber(ushort &aNum, char *text);
ushort getMsgBaseInfo(byte &info, eMsgInfo which);
void   makeProgBar(char *title, char *fileName, char *text, long count);
void   updateProgBar(long count);
void   destroyProgBar();
ushort editFileArea(FILEAREA &fa);
ushort editMesgArea(MESSAGES &ma);
ushort editGroup(FILEGROUP &gp);

#endif
