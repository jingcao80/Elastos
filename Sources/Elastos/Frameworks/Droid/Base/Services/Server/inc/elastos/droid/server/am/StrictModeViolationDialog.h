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

#ifndef __ELASTOS_DROID_SERVER_AM_STRICTMODEVIOLATIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_STRICTMODEVIOLATIONDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/server/am/AppErrorResult.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/os/Handler.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class StrictModeViolationDialog
    : public BaseErrorDialog
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("StrictModeViolationDialog::MyHandler")

        MyHandler(
            /* [in] */ StrictModeViolationDialog* owner);

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        StrictModeViolationDialog* mOwner;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ CActivityManagerService* service,
        /* [in] */ AppErrorResult* result,
        /* [in] */ ProcessRecord* app);

private:
    friend class MyHandler;

    static const String TAG;

public:
    // Event 'what' codes
    static const Int32 ACTION_OK;
    static const Int32 ACTION_OK_AND_REPORT;

    // 5-minute timeout, then we automatically dismiss the crash dialog
    static const Int64 DISMISS_TIMEOUT;

private:
    CActivityManagerService* mService;
    AutoPtr<AppErrorResult> mResult;
    AutoPtr<ProcessRecord> mProc;

    AutoPtr<IHandler> mHandler;

    Object mProcLock;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_STRICTMODEVIOLATIONDIALOG_H__
