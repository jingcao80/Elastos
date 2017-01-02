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

#ifndef __ELASTOS_DROID_SERVER_PERMISSIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_PERMISSIONDIALOG_H__

#include "elastos/droid/server/BasePermissionDialog.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {

class PermissionDialog
    : public BasePermissionDialog
    , public IPermissionDialog
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PermissionDialog::MyHandler")

        MyHandler(
            /* [in] */ PermissionDialog* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PermissionDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PermissionDialog();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIAppOpsService* service,
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

private:
    CARAPI_(String) GetAppName(
        /* [in] */ const String& packageName);

private:
    static const String TAG;
    AutoPtr<IIAppOpsService> mService;
    String mPackageName;
    Int32 mCode;
    AutoPtr<IView> mView;
    AutoPtr<ICheckBox> mChoice;
    Int32 mUid;
    AutoPtr<ArrayOf<ICharSequence*> > mOpLabels;
    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;

public:
    // Event 'what' codes
    static const Int32 ACTION_ALLOWED;
    static const Int32 ACTION_IGNORED;
    static const Int32 ACTION_IGNORED_TIMEOUT;

    // 15s timeout, then we automatically dismiss the permission
    // dialog. Otherwise, it may cause watchdog timeout sometimes.
    static const Int64 DISMISS_TIMEOUT;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PERMISSIONDIALOG_H__