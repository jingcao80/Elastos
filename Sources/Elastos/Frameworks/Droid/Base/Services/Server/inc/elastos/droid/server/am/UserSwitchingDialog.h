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

#ifndef __ELASTOS_DROID_SERVER_AM_USERSWITCHINGDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_USERSWITCHINGDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

/**
  * Dialog to show when a user switch it about to happen. The intent is to snapshot the screen
  * immediately after the dialog shows so that the user is informed that something is happening
  * in the background rather than just freeze the screen and not know if the user-switch affordance
  * was being handled.
  */
class UserSwitchingDialog
    : public AlertDialog
{
private:
    class InnerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("UserSwitchingDialog::InnerHandler")

        InnerHandler(
            /* [in] */ UserSwitchingDialog* owner);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        UserSwitchingDialog* mOwner;
    };

public:
    UserSwitchingDialog(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId,
        /* [in] */ const String& userName,
        /* [in] */ Boolean aboveSystem);

    // @Override
    CARAPI Show();

private:
    static const String TAG;
    static const Int32 MSG_START_USER = 1;
    AutoPtr<CActivityManagerService> mService;
    Int32 mUserId;
    AutoPtr<IHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_USERSWITCHINGDIALOG_H__

