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
#ifndef __CONFCMDS_H
#define __CONFCMDS_H

#include "filecfg.h"

const
    cmAllfix       = 150,
    cmFileMgr      = 151,
    cmAMU          = 152,
    cmFileCtl      = 153,
    cmMesgCtl      = 154,
    cmInterEcho    = 155,
    cmBlueWave     = 156,
    cmEdit         = 157,
    cmFileGroup    = 158,
    cmMesgGroup    = 159,
    cmFileAreas    = 160,
    cmMesgAreas    = 161,

	cmAbout        = 0x2001,
    cmChDir        = 0x2002,
    cmDosShell     = 0x2003,
    cmBakFile      = 0x2004,

    cmContents     = 0x3005,
    cmUsage        = 0x3006,
    cmAcks         = 0x3007,
    cmLimits       = 0x3008,
    cmContact      = 0x3009,

    cmReindex      = 0x4002,
    cmReindexMsg   = 0x4003,
    cmProCfg       = 0x4004,

    cmRenumber     = 0x5001,
    cmNew          = 0x5002,
    cmDelete       = 0x5003,
    cmSave         = 0x5004,
    cmMove         = 0x5005,
    cmPack         = 0x5006,
    cmSort         = 0x5007,
    cmFindFirst    = 0x5008,
    cmFindNext     = 0x5009,
	cmGotoNum      = 0x500a,
	cmChangeRecs   = 0x500b,
	cmGlobFlags    = 0x500c,
	cmGlobLevel    = 0x500d,
	cmGlobAllGrps  = 0x500e,
	cmGlobCDROM    = 0x500f,
	cmGlobLoc      = 0x5010,
	cmGlobList     = 0x5011,
	cmGlobTops     = 0x5012,
	cmGlobFree     = 0x5013,
	cmGlobWFlags   = 0x5014,
	cmGlobWLevel   = 0x5015,
	cmGlobSFlags   = 0x5016,
	cmGlobSLevel   = 0x5017,
	cmGlobMaxFiles = 0x5018,
	cmGlobMaxKB    = 0x5019,
	cmGlobKillDays = 0x501A,
	cmGlobKillRecv = 0x501B,
	cmGlobMaxMsgs  = 0x501C,
	cmGlobSysop    = 0x501D,
	cmGlobOrigin   = 0x501E,
	cmGlobMKind    = 0x501F,
	cmGlobMType    = 0x5020,
	cmGlobMBase    = 0x5021,
	cmGlobMName    = 0x5022,
	cmGlobAKA      = 0x5023;

const
	hlChDir       = 0;

#endif

