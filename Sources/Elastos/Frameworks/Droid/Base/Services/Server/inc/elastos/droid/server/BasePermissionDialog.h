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

#ifndef __ELASTOS_DROID_SERVER_BASEPERMISSIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_BASEPERMISSIONDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Server {

class BasePermissionDialog
    : public AlertDialog
    , public IBasePermissionDialog
{
public:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CBasePermissionDialog::MyHandler")

        MyHandler(
            /* [in] */ BasePermissionDialog* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        BasePermissionDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BasePermissionDialog();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnStart();

    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

private:
    AutoPtr<IHandler> mHandler;

    Boolean mConsuming;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_SERVER_BASEPERMISSIONDIALOG_H__
