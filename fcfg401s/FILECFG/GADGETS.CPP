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
#include "gadgets.h"
#include "confcmds.h"
#include <stdio.h>
#include <stdlib.h>

static TDialog      *__pBarDlg = 0;
static TProgressBar *__pbar = 0;

ushort getLevel(ushort &level, char *title)
{
	char   buf[10];
    ushort retval;
	sprintf(buf, "%u", level);
    retval = inputBox(title, "Enter level number:", buf, 6);
    if( cmCancel != retval ) level = (ushort)atoi(buf);
    return retval;
}

ushort getBool(bool &toggle, char *text)
{
	ushort retval;

    retval = messageBox(text, mfConfirmation|mfYesNoCancel);
    if( cmYes == retval ) toggle = 1;
    else if( cmNo == retval ) toggle = 0;
    return retval;
}

ushort getNumber(ushort &aNum, char *text)
{
	ushort retval;
    char   buf[10];

    sprintf(buf, "%u", aNum);
    retval = inputBox("Enter New Number", text, buf, 6);
    if( cmCancel != retval ) aNum = (ushort)atoi(buf);
    return retval;
}

ushort getMsgBaseInfo(byte &info, eMsgInfo which)
{
    static const struct{
    	char   *title;
        ushort  helpCtx;
		char   *text[4];
	} strings[4] = {
    {{"Message Kinds"}, { hcMesgKind },
	 {"1. Local","2. Netmail","3. Echomail","4. Private Echomail"}
	},
    {{"Message Types"}, { hcMesgType },
	 {"1. Private/Public","2. Public Only","3. Private Only","4. To-All"}
	},
    {{"Base Formats"}, { hcMesgBase },
	 {"1. Hudson","2. Squish","3. Fido *.MSG","4. JAM"}
	},
	{{"Name Options"}, { hcMesgName },
     {"1. Real names","2. Free alias","3. Alias or name","4. Alias only"}
	}};
    ushort             retval;
	TDialog           *dlg  = new TDialog(TRect(0,0,26,11), strings[which].title);
	TStringCollection *list = new TStringCollection(4,0);
    TListBox          *lbox = new TListBox(TRect(2,2,24,6), 1, 0);

    for(short i = 0; i < 4; ++i) list->insert(strings[which].text[i]);
    lbox->newList(list);
    lbox->helpCtx = strings[which].helpCtx;
    dlg->insert(lbox);
    dlg->insert(new TButton(TRect(3,8,13,10), "O~K~", cmOK, bfDefault));
    dlg->insert(new TButton(TRect(13,8,23,10), "~C~ancel", cmCancel, bfNormal));
    dlg->options |= ofCentered;
    lbox->options |= ofFramed;
    lbox->select();
    if( cmCancel != (retval = TProgram::deskTop->execView(dlg)) ){
    	info = lbox->focused;
	}
    TObject::destroy(dlg);
    return retval;
}

void makeProgBar(char *title, char *fileName, char *text, long count)
{
	char buf[150];
    sprintf(buf,"\003%s '%s'\n\003This may take a while.", text, fileName);
    __pBarDlg = new TDialog(TRect(0,0,50,8), title);
    if( __pBarDlg ){
    	__pbar = new TProgressBar(TRect(2,5,48,6), count);
        __pbar->options |= ofCenterX;
    	__pBarDlg->flags &= ~(wfMove | wfClose);
        __pBarDlg->options |= ofCentered;
	    __pBarDlg->insert(new TStaticText(TRect(2,2,48,4), buf));
        __pBarDlg->insert(__pbar);
        TProgram::application->insert(__pBarDlg);
    }
    TMouse::hide();
}

void updateProgBar(long count)
{
	if( __pbar ) __pbar->update(count);
}

void destroyProgBar()
{
	if( __pBarDlg ) TObject::destroy(__pBarDlg);
    __pBarDlg = 0;
    __pbar = 0;
    TMouse::show();
}

ushort editFileArea(FILEAREA &fa)
{
	return execDialog(new TFileEdit, &fa);
}

ushort editGroup(FILEGROUP &gp)
{
	return execDialog(new TGroupEditDlg, &gp);
}

ushort editMesgArea(MESSAGES &ma)
{
	return execDialog(new TMesgEdit, &ma);
}

