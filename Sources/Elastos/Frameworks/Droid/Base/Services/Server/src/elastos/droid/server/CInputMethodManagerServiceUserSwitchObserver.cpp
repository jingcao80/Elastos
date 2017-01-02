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

#include "elastos/droid/server/CInputMethodManagerServiceUserSwitchObserver.h"
#include "elastos/droid/server/CInputMethodManagerService.h"
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::App::EIID_IIUserSwitchObserver;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_INTERFACE_IMPL_2(CInputMethodManagerServiceUserSwitchObserver, Object, IIUserSwitchObserver, IBinder)

CAR_OBJECT_IMPL(CInputMethodManagerServiceUserSwitchObserver)

ECode CInputMethodManagerServiceUserSwitchObserver::constructor(
    /* [in] */ IIInputMethodManager* inputMethodManagerService)
{
    mHost = (CInputMethodManagerService*)inputMethodManagerService;
    return NOERROR;
}

ECode CInputMethodManagerServiceUserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    {
        AutoLock lock(mHost->mMethodMapLock);
        mHost->SwitchUserLocked(newUserId);
    }

    if (reply != NULL) {
        // try {
            reply->SendResult(NULL);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CInputMethodManagerServiceUserSwitchObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    return NOERROR;
}

ECode CInputMethodManagerServiceUserSwitchObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Elastos;
} // namespace Droid;
} // namespace Server;