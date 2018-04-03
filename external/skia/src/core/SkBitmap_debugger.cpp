/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkBitmapDevice.h"
#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkDevice.h"
#include "SkImageEncoder.h"
#include "SkTime.h"

#include "cutils/properties.h"

#include <unistd.h>

static bool isBitmapDumpSwitchOn(void)  {
    
    char value[PROPERTY_VALUE_MAX];

    property_get("debug.skia.dump_enable", value, "0");

    bool isBitmapDumpSwitchOn = (atoi(value) == 0) ? false : true;

	return isBitmapDumpSwitchOn;
}

void saveToFileWithPid(const SkBitmap& bm, int preTag, int pid = 0) {
	SkTime::DateTime dt;
	SkTime::GetDateTime(&dt);
	SkMSec now = SkTime::GetMSecs();
	int ms = now % 1000;	/* millisecond */

	if (0 == pid) {
		pid = getpid();
	}
	
	SkString filename;	
	// png name formate: pid_HH_MM_SS_millisecond.png	
	filename.printf("/sdcard/dump/%04d%02d%02d%02d%03d%d.png",
	                              pid, dt.fHour, dt.fMinute, dt.fSecond, ms, preTag);

    bool res = SkImageEncoder::EncodeFile(filename.c_str(), bm, SkImageEncoder::kPNG_Type, 100);
	if (true == res) {
		SkDebugf("Dump bitmap with pid success, fileName: %s\n", filename.c_str());
	} else {
		SkDebugf("Dump bitmap with pid failed!\n");
	}
}

void SkBaseDevice::dumpBitmapWithPid(int preTag, int pid) {
	SkDebugf("Do not support dumpbitmap!\n");
}

void SkBitmapDevice::dumpBitmapWithPid(int preTag, int pid) {
	saveToFileWithPid(fBitmap, preTag, pid);
}

void SkCanvas::dumpBitmapWithPid(int preTag, int pid) {
	SkBaseDevice *d = getTopDevice();

	SkASSERT(d);

	if (isBitmapDumpSwitchOn()) {
		d->dumpBitmapWithPid(preTag, pid);
	} else {
		SkDebugf("BitmapDumpSwitch is off, please ture in on!");
	}
}
