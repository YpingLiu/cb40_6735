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

#ifndef _DUAL_ON_CONTROLLER_H
#define _DUAL_ON_CONTROLLER_H

#include <linux/in.h>
#include <list>
#include <string>

class DualOnController {
public:
    DualOnController();
    virtual ~DualOnController();

    int setupIptablesHooks();

    static const char* LOCAL_NAT_POSTROUTING;
    static const char* LOCAL_MANGLE_OUTPUT;

    static const char* CONNMARK1;
    static const char* CONNMARK2;
    static const char* MARK1;
    static const char* MARK2;

    int setHetCommInterface(const char* iface1, const char* iface2, const char* ratio1);
    int setHetCommRatio(const char* ratio1);
    int removeHetCommInterface(void);
    int removeHetCommRatio(void);
};
#endif
