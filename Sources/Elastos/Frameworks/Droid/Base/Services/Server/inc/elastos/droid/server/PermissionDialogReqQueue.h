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

#ifndef __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__
#define __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__

#include <_Elastos.Droid.Server.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {

class PermissionDialogReqQueue
    : public Object
{
public:
    class PermissionDialogReq
        : public Object
    {
    public:
        TO_STRING_IMPL("PermissionDialogReqQueue::PermissionDialogReq")

        PermissionDialogReq();

        CARAPI_(void) Set(
            /* [in] */ Int32 res);

        CARAPI_(Int32) Get();

    private:
        Boolean mHasResult;
        Int32 mResult;
    };

public:
    TO_STRING_IMPL("PermissionDialogReqQueue")

    PermissionDialogReqQueue();

    CARAPI_(AutoPtr<IPermissionDialog>) GetDialog();

    using Object::NotifyAll;

    CARAPI_(void) NotifyAll(
        /* [in] */ Int32 mode);

    CARAPI_(void) Register(
        /* [in] */ PermissionDialogReq* res);

    CARAPI_(void) SetDialog(
        /* [in] */ IPermissionDialog* dialog);

private:
    AutoPtr<IPermissionDialog> mDialog;
    List<AutoPtr<PermissionDialogReq> > mResultList;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__