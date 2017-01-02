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

#ifndef __ELASTOS_DROID_SERVER_AM_APPWAITINGFORDEBUGGERDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_APPWAITINGFORDEBUGGERDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppWaitingForDebuggerDialog : public BaseErrorDialog
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AppWaitingForDebuggerDialog::MyHandler")

        MyHandler(
            /* [in] */ AppWaitingForDebuggerDialog* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    private:
        AppWaitingForDebuggerDialog* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IContext* context,
        /* [in] */ ProcessRecord* app);

    CARAPI OnStop();

public:
    CActivityManagerService* mService;  // weak-ref
    AutoPtr<ProcessRecord> mProc;

private:
    AutoPtr<ICharSequence> mAppName;
    AutoPtr<IHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPWAITINGFORDEBUGGERDIALOG_H__
