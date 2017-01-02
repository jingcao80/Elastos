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

#ifndef __ELASTOS_DROID_SERVER_AM_APPERRORDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_APPERRORDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/AppErrorResult.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/ProcessRecord.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppErrorDialog : public BaseErrorDialog
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AppErrorDialog::MyHandler")

        MyHandler(
            /* [in] */ AppErrorDialog* host);

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    AppErrorDialog() {}

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ CActivityManagerService* service,
        /* [in] */ AppErrorResult* result,
        /* [in] */ ProcessRecord* app);

private:
    AutoPtr<CActivityManagerService> mService;

public:
    // Event 'what' codes
    static const Int32 FORCE_QUIT;
    static const Int32 FORCE_QUIT_AND_REPORT;

    // 5-minute timeout, then we automatically dismiss the crash dialog
    static const Int64 DISMISS_TIMEOUT;

private:
    AutoPtr<AppErrorResult> mResult;
    AutoPtr<ProcessRecord> mProc;
    AutoPtr<IHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPERRORDIALOG_H__
