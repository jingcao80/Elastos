//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "ServiceManager.h"
#include <sys/types.h>
#include <unistd.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <stdio.h>

int main(int argv, char**argc)
{
    android::sp<android::ProcessState> proc(android::ProcessState::self());
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    int res = sm->addService(android::String16(ELASTOS_SERVICEMGR_NAME), new ServiceManager());
    if (res != 0) {
        ALOGE("<%s, %d> add service '%s' failed, res=%d\n",
            __FILE__, __LINE__, ELASTOS_SERVICEMGR_NAME, res);
        return -1;
    }
    else {
        ALOGE("<%s, %d> add service '%s' succeeded.\n",
            __FILE__, __LINE__, ELASTOS_SERVICEMGR_NAME);
    }

    android::IPCThreadState::self()->joinThreadPool();
    return 0;
}
