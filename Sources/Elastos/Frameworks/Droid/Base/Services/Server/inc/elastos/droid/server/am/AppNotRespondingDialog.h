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

#ifndef __ELASTOS_DROID_SERVER_AM_APPNOTRESPONDINGDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_APPNOTRESPONDINGDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/server/am/ProcessRecord.h"

using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppNotRespondingDialog : public BaseErrorDialog
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AppNotRespondingDialog::MyHandler")

        MyHandler(
            /* [in] */ AppNotRespondingDialog* host);

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AppNotRespondingDialog* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IContext* context,
        /* [in] */ ProcessRecord* app,
        /* [in] */ ActivityRecord* activity,
        /* [in] */ Boolean aboveSystem);

    CARAPI OnStop();

private:
    static const String TAG;

    AutoPtr<IHandler> mHandler;

public:
    // Event 'what' codes
    static const Int32 FORCE_CLOSE;
    static const Int32 WAIT;
    static const Int32 WAIT_AND_REPORT;

private:
    AutoPtr<CActivityManagerService> mService;
    AutoPtr<ProcessRecord> mProc;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPNOTRESPONDINGDIALOG_H__
