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
#define Uses_TApplication
#define Uses_TBackground
#define Uses_TButton
#define Uses_TKeys
#define Uses_TDeskTop
#define Uses_TPalette
#define Uses_TDialog
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TRect
#define Uses_TStaticText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TChDirDialog
#define Uses_TSubMenu
#define Uses_MsgBox
#define Uses_fpstream
#include <tv.h>
#include <help.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>
#include <dos.h>
#include "fsys.h"
#include "stdini.h"

#ifdef __OVERLAY
	#define __VERAPP " (Overlay)"
#else
	#define __VERAPP ""
#endif

#define __VERSION "v4.0r1/Pro" __VERAPP
#define __NAME    "FileCFG"

#include "spawno.h"
#include "pblib.h"
#include "confcmds.h"
#include "tv/execprog.h"
#include "files.h"
#include "mesgs.h"
#include "groups.h"

const char *nameFILECTL = "FILEUTIL.CTL";
const char *nameMESGCTL = "MESGUTIL.CTL";
extern char *helpCtxStrings[];

struct{
	char startup[MAXPATH];
    char helpfile[MAXPATH];
    char allfix[MAXPATH];
    char fileMgr[MAXPATH];
    char amu[MAXPATH];
    char interecho[MAXPATH];
    char bwave[MAXPATH];
} paths;

struct{
	char allfix[80];
    char fileMgr[80];
    char amu[80];
    char interecho[80];
    char bwave[80];
} menu;

/////////////////////////////////////////////////////////////////////////////
// The TFileStatus is the status line with hints
/////////////////////////////////////////////////////////////////////////////
class THintStatusLine: public TStatusLine
{
public:
	THintStatusLine(const TRect &bounds, TStatusDef &aDefs);
    virtual const char *hint(ushort aHelpCtx);
};

THintStatusLine::THintStatusLine(const TRect &bounds, TStatusDef &aDefs):
	TStatusLine(bounds, aDefs)
{
}

const char *THintStatusLine::hint(ushort aHelpCtx)
{
	if( aHelpCtx < hcLast ) return helpCtxStrings[aHelpCtx];
    else return "";
}

/////////////////////////////////////////////////////////////////////////////
// the TFileCfg main application class
/////////////////////////////////////////////////////////////////////////////
class TFileCfg : public TApplication
{
public:
    TFileCfg();

    static  TMenuBar    *initMenuBar(TRect r);
    static  TStatusLine *initStatusLine(TRect r);

    void handleEvent(TEvent &event);
    void getEvent(TEvent &event);
    TPalette &getPalette() const;

    void aboutDialog();
    void dosShell();
    void changeDir();
    void checkBackup(const char *path);
    void showHelp(ushort aCtx);

    void editFileAreas();
    void editMessageAreas();
    void editGroups(TGroupEdit::enum_mode mode);

    void indexFiles(const char *cmd);
    void runProgram(const char *path);
    void procCtlFile(const char *name);

    void idle();

protected:
	char    proSysPath[MAXPATH];
    char   *ovl_path;
    Boolean helpActive;
};

TFileCfg::TFileCfg():
	TProgInit(TFileCfg::initStatusLine,
			  TFileCfg::initMenuBar,
			  TFileCfg::initDeskTop)
{
	char path[MAXPATH];

	ovl_path = getenv("TMP");
    if( !ovl_path ) ovl_path = getenv("TEMP");
    if( !ovl_path ) ovl_path = ".";

    init_SPAWNO(ovl_path, SWAP_ANY);

	strcpy(proSysPath, getenv("PROBOARD"));
    if( 0 != access(paths.allfix, 0) ) disableCommand(cmAllfix);
    if( 0 != access(paths.fileMgr, 0) ) disableCommand(cmFileMgr);
    if( 0 != access(paths.amu, 0) ) disableCommand(cmAMU);
    if( 0 != access(paths.interecho, 0) ) disableCommand(cmInterEcho);
    if( 0 != access(paths.bwave, 0) ) disableCommand(cmBlueWave);
    sprintf(path, "%s%s", paths.startup, nameFILECTL);
    if( 0 != access(path, 0) ) disableCommand(cmFileCtl);
    sprintf(path, "%s%s", paths.startup, nameMESGCTL);
    if( 0 != access(path, 0) ) disableCommand(cmMesgCtl);
    sprintf(path, "%s\\%s", proSysPath, fnFGROUPS);
    if( 0 != access(path, 0) ) disableCommand(cmFileGroup);
    sprintf(path, "%s\\%s", proSysPath, fnMGROUPS);
    if( 0 != access(path, 0) ) disableCommand(cmMesgGroup);
    sprintf(path, "%s\\%s", proSysPath, fnFILECFG);
    if( 0 != access(path, 0) ) disableCommand(cmFileAreas);
    sprintf(path, "%s\\%s", proSysPath, fnMESSAGES);
    if( 0 != access(path, 0) ) disableCommand(cmMesgAreas);

    helpActive = False;

    TEvent event;
    event.what = evCommand;
    event.message.command = cmAbout;
    putEvent(event);
}

TMenuBar *TFileCfg::initMenuBar(TRect bounds)
{
    bounds.b.y = bounds.a.y + 1;
    TMenuItem *fileExtMenu = new TMenuItem(menu.allfix, cmAllfix, kbNoKey, hcExtern);
    fileExtMenu->append(
		new TMenuItem(menu.fileMgr, cmFileMgr, kbNoKey, hcExtern, 0,
		new TMenuItem(menu.amu, cmAMU, kbNoKey, hcExtern)));
    TMenuItem *mesgExtMenu = new TMenuItem(menu.interecho, cmInterEcho, kbNoKey, hcExtern);
    mesgExtMenu->append(
    	new TMenuItem(menu.bwave, cmBlueWave, kbNoKey, hcExtern)
	);

    return new TMenuBar( bounds,
    	*new TSubMenu("~S~ystem", kbNoKey, hcSystem) +
            *new TMenuItem("Run ~P~roCFG", cmProCfg, kbNoKey, hcProCfg) +
            *new TMenuItem("~C~hange dir...", cmChDir, kbNoKey, hcChDir) +
            *new TMenuItem("~S~hell to DOS   ", cmDosShell, kbAltJ, hcShell, "Alt-J") +
            newLine() +
            *new TMenuItem("E~x~it", cmQuit, kbAltX, hcExit, "Alt-X") +
        *new TSubMenu("~F~iles", kbNoKey, hcMaint) +
        	*new TMenuItem("~A~rrange areas...", cmFileAreas, kbNoKey, hcFiles) +
            *new TMenuItem("Arrange ~g~roups...", cmFileGroup, kbNoKey, hcFileGroup) +
            *new TMenuItem("Re-~i~ndex filebase", cmReindex, kbNoKey, hcIndex) +
            newLine() +
            *new TMenuItem("~P~rocess control file", cmFileCtl, kbNoKey, hcFileCtl) +
  			*new TMenuItem("E~x~ternal programs   ", kbNoKey, new TMenu(*fileExtMenu), hcExtern) +
        *new TSubMenu("~M~essages", kbNoKey, hcMessage) +
        	*new TMenuItem("~A~rrange areas...", cmMesgAreas, kbNoKey, hcMesg) +
            *new TMenuItem("Arrange ~g~roups...", cmMesgGroup, kbNoKey, hcMesgGroup) +
            *new TMenuItem("Re-~i~ndex messagebase", cmReindexMsg, kbNoKey, hcReindexMsg) +
            newLine() +
            *new TMenuItem("~P~rocess control file", cmMesgCtl, kbNoKey, hcMesgCtl) +
            *new TMenuItem("E~x~ternal programs   ", kbNoKey, new TMenu(*mesgExtMenu), hcExtern) +
		*new TSubMenu("~H~elp", kbNoKey, hcHelp) +
        	*new TMenuItem("~C~ontents", cmContents, kbNoKey, hcContents) +
            *new TMenuItem("Usage ~o~verview", cmUsage, kbNoKey, hcUsage) +
            *new TMenuItem("~L~imits and caveats", cmLimits, kbNoKey, hcLimits) +
            *new TMenuItem("Ac~k~nowledgements", cmAcks, kbNoKey, hcAcks) +
            *new TMenuItem("Con~t~act info", cmContact, kbNoKey, hcContact) +
            newLine() +
	        *new TMenuItem("~A~bout", cmAbout, kbNoKey, hcAbout)
	);
}

TStatusLine *TFileCfg::initStatusLine(TRect bounds)
{
	bounds.a.y = bounds.b.y - 1;
    return new THintStatusLine( bounds,
		*new TStatusDef( 0, 0xffff ) +
        	*new TStatusItem("~F1~ Help", kbF1, cmHelp)
	);
}

void TFileCfg::handleEvent(TEvent &event)
{
	char buf[MAXPATH];

	TApplication::handleEvent( event );
	if( event.what == evCommand ){
		switch( event.message.command ){

		    case cmAbout     : aboutDialog();      break;
        	case cmDosShell  : dosShell();         break;
            case cmChDir     : changeDir();        break;
            case cmReindex   : indexFiles("PBUTIL.EXE FI"); break;
            case cmReindexMsg: indexFiles("PBUTIL.EXE MI"); break;
            case cmFileAreas : editFileAreas();    break;
            case cmFileGroup : editGroups(TGroupEdit::file); break;
            case cmMesgAreas : editMessageAreas(); break;
            case cmMesgGroup : editGroups(TGroupEdit::mesg); break;
            case cmBakFile   : checkBackup(fnFILECFG); break;
            case cmAllfix    : runProgram(paths.allfix); break;
            case cmFileMgr   : runProgram(paths.fileMgr); break;
            case cmAMU       : runProgram(paths.amu); break;
            case cmInterEcho : runProgram(paths.interecho); break;
            case cmBlueWave  : runProgram(paths.bwave); break;
            case cmFileCtl   : procCtlFile(nameFILECTL); break;
            case cmMesgCtl   : procCtlFile(nameMESGCTL); break;
            case cmHelp      : showHelp(getHelpCtx()); break;
            case cmContents  : showHelp(hcContents); break;
            case cmUsage     : showHelp(hcUsage); break;
            case cmAcks      : showHelp(hcAcks); break;
            case cmContact   : showHelp(hcContact); break;
            case cmLimits    : showHelp(hcLimits); break;
            case cmProCfg    :
            	sprintf(buf, "%s\\PROCFG.EXE", proSysPath);
				runProgram(buf);
			break;

            default:
            	return;
        }
		clearEvent( event );
    }
}

void TFileCfg::aboutDialog()
{
	TDialog *pd = new TDialog(TRect( 0, 0, 46, 10 ), "About");
    if( pd ){
		pd->options |= ofCentered;
        pd->helpCtx = hcAbout;
		pd->insert( new TStaticText( TRect( 2, 2, 44, 6 ),
			"\003" __NAME " " __VERSION "\n"
			"\003Copyright (c) 1995-1996 by\n"
			"\003Branislav L. Slantchev\n"
			"\003A Product of Silicon Creations, Inc.\n"
			"\003This program is part of the PB-GNU Project"));
		pd->insert(new TButton(TRect(2,7,44,9), "~O~k", cmOK, bfDefault));
		TProgram::deskTop->execView( pd );
	}
	destroy( pd );
}

void TFileCfg::idle()
{
	TProgram::idle();
    if( !deskTop->current && !menuBar->getState(sfSelected) && !helpActive ){
    	TEvent event;
        event.what = evKeyboard;
        event.keyDown.keyCode = kbAltS;
        putEvent(event);
	}
}

void TFileCfg::runProgram(const char *cmdline)
{
	char curPath[MAXPATH], newPath[MAXPATH];

    strupr(strcpy(newPath, basedir(cmdline)));
    getcwd(curPath, sizeof(curPath));
    if( curPath[0] != newPath[0] ) setdisk(newPath[0] - 'A');
    chdir(newPath);
	suspend();
    system("cls");
    cout << "\nExecuting: '" << cmdline << "'\n" << endl;
    systemo(ovl_path, cmdline);
    resume();
    redraw();
    if( curPath[0] != newPath[0] ) setdisk(curPath[0] - 'A');
    chdir(curPath);
}

void TFileCfg::indexFiles(const char *cmd)
{
	char  cmdline[MAXPATH];

    sprintf(cmdline, "%s\\%s", proSysPath, cmd);
    TView *dlg = validView( new TExecProg("Rebuilding the index:", cmdline) );
    if( dlg ){
    	dlg->helpCtx = hcIndex;
    	TProgram::deskTop->execView(dlg);
        destroy(dlg);
	}
}

void TFileCfg::dosShell()
{
	char cmd[250];

	suspend();
    system("cls");
    cout << "Type 'EXIT' to return to " << __NAME << " " << __VERSION << endl;
    sprintf(cmd, "%s /K set prompt=[%s %s]$_$p$g",
					getenv("COMSPEC"), __NAME, __VERSION );
    systemo(ovl_path, cmd);
    resume();
    redraw();
}

void TFileCfg::changeDir()
{
	TView *dlg = validView(new TChDirDialog(cdNormal|cdHelpButton, hlChDir));

    if( dlg ){
    	dlg->helpCtx = hcChDir;
    	TProgram::deskTop->execView(dlg);
        destroy(dlg);
	}
}

void TFileCfg::checkBackup(const char *name)
{
	char path[MAXPATH], temp[150];
    sprintf(temp, "%s\\%s", proSysPath, name);
    chext(path, temp, ".BAK");
    if( 0 == access(path, 0) ){
    	sprintf(temp, "\003Old backup found. Remove now?\n\003'%s'", path);
		if( cmYes == messageBox(temp,mfConfirmation|mfYesButton|mfNoButton) )
	    	rmfile(path);
        else
        	messageBox("\003Current original will be overwritten!",
						mfInformation|mfOKButton);
	}
}

void TFileCfg::editFileAreas()
{
	char path[MAXPATH];

    checkBackup(fnFILECFG);
    sprintf(path, "%s\\%s", proSysPath, fnFILECFG);
	TFileAreaEdit *dlg = (TFileAreaEdit *)validView(new TFileAreaEdit(path));
    if( dlg ){
    	TProgram::deskTop->execView(dlg);
        destroy(dlg);
	}
}

void TFileCfg::editMessageAreas()
{
	char path[MAXPATH];

    checkBackup(fnMESSAGES);
    sprintf(path, "%s\\%s", proSysPath, fnMESSAGES);
    TMessageAreaEdit *dlg = (TMessageAreaEdit *)validView(new TMessageAreaEdit(path));
    if( dlg ){
    	TProgram::deskTop->execView(dlg);
        destroy(dlg);
	}
}

void TFileCfg::editGroups(TGroupEdit::enum_mode mode)
{
	checkBackup(TGroupEdit::file == mode ? fnFGROUPS : fnMGROUPS);
	TGroupEdit *dlg = new TGroupEdit(proSysPath, mode);
    if( dlg ){
    	TProgram::deskTop->execView(dlg);
        destroy(dlg);
	}
}

void TFileCfg::procCtlFile(const char *name)
{
	char cmdBuf[255];
    sprintf(cmdBuf, "%s%s", paths.startup, name);
    if( 0 == access(cmdBuf, 0) ){
    	FILE *fp = fopen(cmdBuf, "rt");
        if( !fp ) return;
        fgets(cmdBuf, sizeof(cmdBuf), fp);
        while( !feof(fp) ){
            if( ';' != *cmdBuf ){	// not a comment
            	if( '\n' == cmdBuf[strlen(cmdBuf) - 1])
                	cmdBuf[strlen(cmdBuf) - 1] = EOS;	// get rid of it
                runProgram(cmdBuf);
			}
            fgets(cmdBuf, sizeof(cmdBuf), fp);
		}
        fclose(fp);
	}
}

TPalette& TFileCfg::getPalette() const
{
	static TPalette color(cpColor cHelpColor, sizeof(cpColor cHelpColor)-1);
	static TPalette blackwhite(cpBlackWhite cHelpBlackWhite,
	       						sizeof(cHelpBlackWhite) - 1);
	static TPalette monochrome(cpMonochrome cHelpMonochrome,
    							sizeof(cHelpMonochrome) - 1);

	static TPalette *palettes[] = { &color, &blackwhite, &monochrome };
	return *(palettes [appPalette]);
}

void TFileCfg::getEvent(TEvent &event)
{
    TProgram::getEvent(event);
    if( evCommand == event.what && cmHelp == event.message.command ){
    	showHelp(getHelpCtx());
        clearEvent(event);
    }
}

void TFileCfg::showHelp(ushort aHelpCtx)
{
    if( True == helpActive || menuBar->getState(sfSelected) ) return;
	fpstream &ifp = *new fpstream(paths.helpfile, ios::in|ios::binary);
    THelpFile *help = new THelpFile(ifp);
	if( !help ){
		messageBox("\003Could not open help file.", mfError|mfOKButton);
        delete help;
        return;
	}
	else{
    	helpActive = True;
		THelpWindow *wnd = new THelpWindow(help, aHelpCtx);
		wnd->helpCtx = hcHelpOnHelp;
		if( validView(wnd) != 0 ){
			TProgram::deskTop->execView(wnd);
			destroy(wnd);
            helpActive = False;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// the main function
////////////////////////////////////////////////////////////////////////////
#include <conio.h>

static void copyright()
{
	clrscr();
    cout << __NAME << " " << __VERSION << endl
		<< "   þ Compiled on " << __DATE__ << " at " << __TIME__ << endl
		<< "   þ Copyright (c) 1995-1996 Branislav L. Slantchev" << endl
		<< "   þ A Product of Silicon Creations, Inc. (PB-GNU)\n" << endl;
    exit(EXIT_SUCCESS);
}

#pragma exit copyright 31
#pragma argsused
int main(int argc, char *argv[])
{
    char inipath[MAXPATH];

    if( !getenv("PROBOARD")){
		cerr << "\n'PROBOARD' environment not set, aborting..." << endl;
        exit(EXIT_FAILURE);
	}

    strcpy(paths.startup, basedir(*argv));
    chext(paths.helpfile, *argv, ".HLP");

	sprintf(inipath, "%s\\SILICON.INI", getenv("PROBOARD"));
    IniReadStr(inipath, "FileCFG", "Allfix", paths.allfix, "ASETUP.EXE");
    IniReadStr(inipath, "FileCFG", "AllfixMenu", menu.allfix, "All~f~ix setup");
    IniReadStr(inipath, "FileCFG", "FileMgr", paths.fileMgr, "FEBBS.EXE");
    IniReadStr(inipath, "FileCFG", "FileMenu", menu.fileMgr, "File area ~m~anager");
    IniReadStr(inipath, "FileCFG", "AMU", paths.amu, "AMUCFG.EXE");
    IniReadStr(inipath, "FileCFG", "AMUMenu", menu.amu, "AMU ~s~etup");
    IniReadStr(inipath, "FileCFG", "InterEcho", paths.interecho, "IESETUP.EXE");
    IniReadStr(inipath, "FileCFG", "IEMenu", menu.interecho, "InterEcho ~s~etup");
    IniReadStr(inipath, "FileCFG", "BlueWave", paths.bwave, "BWSETUP.EXE");
    IniReadStr(inipath, "FileCFG", "BWMenu", menu.bwave, "~B~lueWave setup");

#ifdef __OVERLAY
    _ovrbuffer = 0x4000;
    if( 0 != _OvrInitExt(0, 0) ) _OvrInitEms(0, 0, 0);
#endif

	TFileCfg fileCfg;
	fileCfg.run();
	return 0;
}

