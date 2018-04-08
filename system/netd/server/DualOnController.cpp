/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_V 1

#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <cutils/properties.h>

#define LOG_TAG "DualOnController"
#include <cutils/log.h>
#include <logwrap/logwrap.h>

#include "DualOnController.h"
#include "NetdConstants.h"
#include "RouteController.h"
#include "Fwmark.h"

const char* DualOnController::LOCAL_NAT_POSTROUTING = "dualon_nat_POSTROUTING";
const char* DualOnController::LOCAL_MANGLE_OUTPUT = "dualon_mangle_OUTPUT";
/* M: Need to synchronized with RouteController.cpp*/
const char* DualOnController::CONNMARK1 = "CONNMARK1";
const char* DualOnController::CONNMARK2 = "CONNMARK2";
const char* DualOnController::MARK1 = "0x5A";
const char* DualOnController::MARK2 = "0x5B";

#define LOG_TAG "DualOnController"

DualOnController::DualOnController() {
}

DualOnController::~DualOnController() {
}

int DualOnController::setupIptablesHooks(void) {
    ALOGE("start setupIptablesHooks");
    int res = 0;

    res |= execIptables(V4, "-t", "mangle", "-F", LOCAL_MANGLE_OUTPUT, NULL);
    res |= execIptables(V4, "-t", "nat", "-F", LOCAL_NAT_POSTROUTING, NULL);

    res |= execIptables(V4, "-t", "mangle", "-N", CONNMARK1, NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", CONNMARK1, "-j", "MARK", "--set-mark", DualOnController::MARK1 ,NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", CONNMARK1, "-j", "CONNMARK", "--save-mark", NULL);

    res |= execIptables(V4, "-t", "mangle", "-N", CONNMARK2, NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", CONNMARK2, "-j", "MARK", "--set-mark", DualOnController::MARK2 ,NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", CONNMARK2, "-j", "CONNMARK", "--save-mark", NULL);
    //Remove rule added by createChildChains
    res |= execIptables(V4, "-t", "mangle", "-D", "OUTPUT", "-j", LOCAL_MANGLE_OUTPUT, NULL);

    ALOGE("end setupIptablesHooks: %d", res);
    return res;
}

int DualOnController::setHetCommInterface(const char* iface1, const char* iface2, const char* ratio2) {
    int res = 0;
    ALOGE("setHetCommInterface");
    // flush any existing rules
    removeHetCommInterface();

    res |= execIptables(V4, "-t", "mangle", "-A", "OUTPUT", "-m", "mark", "--mark", "0x0/0xffff", "-j", LOCAL_MANGLE_OUTPUT, NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", "OUTPUT", "-m", "state", "--state", "ESTABLISHED,RELATED",
        "-j", "CONNMARK", "--restore-mark", NULL);
    /*
    res |= execIptables(V4, "-t", "mangle", "-A", LOCAL_MANGLE_OUTPUT, "-m", "state", "--state", "NEW",
        "-m", "statistic", "--mode", "random", "--probability", ratio1, "-j", CONNMARK1, NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", LOCAL_MANGLE_OUTPUT, "-m", "state", "--state", "NEW",
        "-j", CONNMARK2, NULL);
    */
    res |= execIptables(V4, "-t", "mangle", "-A", LOCAL_MANGLE_OUTPUT, "-m", "state", "--state", "NEW",
        "-j", CONNMARK1, NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", LOCAL_MANGLE_OUTPUT, "-m", "state", "--state", "NEW",
        "-m", "statistic", "--mode", "random", "--probability", ratio2, "-j", CONNMARK2, NULL);

    res |= execIptables(V4, "-t", "nat", "-A", "POSTROUTING", "-j", LOCAL_NAT_POSTROUTING, NULL);
    res |= execIptables(V4, "-t", "nat", "-I", LOCAL_NAT_POSTROUTING, "-o", iface1, "-j", "MASQUERADE", NULL);
    res |= execIptables(V4, "-t", "nat", "-I", LOCAL_NAT_POSTROUTING, "-o", iface2, "-j", "MASQUERADE", NULL);
    res |= RouteController::addHetComm( MARK1, iface1);
    res |= RouteController::addHetComm( MARK2, iface2);
    return res;
}

int DualOnController::removeHetCommInterface(void) {
    int res = 0;
    ALOGV("removeHetCommInterface");
    // flush any existing rules
    res |= execIptables(V4, "-t", "mangle", "-F", LOCAL_MANGLE_OUTPUT, NULL);
    res |= execIptables(V4, "-t", "mangle", "-D", "OUTPUT", "-m", "mark", "--mark", "0x0/0xffff", "-j", LOCAL_MANGLE_OUTPUT, NULL);
    res |= execIptables(V4, "-t", "mangle", "-D", "OUTPUT", "-m", "state", "--state", "ESTABLISHED,RELATED",
        "-j", "CONNMARK", "--restore-mark", NULL);
    res |= execIptables(V4, "-t", "mangle", "-Z", CONNMARK1, NULL);
    res |= execIptables(V4, "-t", "mangle", "-Z", CONNMARK2, NULL);
    res |= execIptables(V4, "-t", "nat", "-F", LOCAL_NAT_POSTROUTING, NULL);
    res |= execIptables(V4, "-t", "nat", "-D", "POSTROUTING", "-j", LOCAL_NAT_POSTROUTING, NULL);
    res |= RouteController::removeHetComm( MARK1 );
    res |= RouteController::removeHetComm( MARK2 );
    return res;
}

int DualOnController::setHetCommRatio(const char* ratio2) {
    int res = 0;
    ALOGE("setHetCommRatio");
    // flush any existing rules
    removeHetCommRatio();

    res |= execIptables(V4, "-t", "mangle", "-A", LOCAL_MANGLE_OUTPUT, "-m", "state", "--state", "NEW",
        "-j", CONNMARK1, NULL);
    res |= execIptables(V4, "-t", "mangle", "-A", LOCAL_MANGLE_OUTPUT, "-m", "state", "--state", "NEW",
        "-m", "statistic", "--mode", "random", "--probability", ratio2, "-j", CONNMARK2, NULL);

        return res;
}

int DualOnController::removeHetCommRatio(void) {
    int res = 0;
    ALOGV("removeHetCommInterface");
    // flush any existing rules
    res |= execIptables(V4, "-t", "mangle", "-F", LOCAL_MANGLE_OUTPUT, NULL);
    res |= execIptables(V4, "-t", "mangle", "-Z", CONNMARK1, NULL);
    res |= execIptables(V4, "-t", "mangle", "-Z", CONNMARK2, NULL);
    return res;
}