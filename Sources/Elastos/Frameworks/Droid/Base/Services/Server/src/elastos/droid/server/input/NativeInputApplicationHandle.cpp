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

#include "elastos/droid/server/input/NativeInputApplicationHandle.h"
#include "elastos/droid/server/input/InputApplicationHandle.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

NativeInputApplicationHandle::NativeInputApplicationHandle(
    /* [in] */ IWeakReference* obj)
    : mObjWeak(obj)
{}

NativeInputApplicationHandle::~NativeInputApplicationHandle()
{
    mObjWeak = NULL;
}

AutoPtr<Elastos::Droid::Server::Input::InputApplicationHandle> NativeInputApplicationHandle::getInputApplicationHandleObj()
{
    AutoPtr<IInputApplicationHandle> obj;
    mObjWeak->Resolve(EIID_IInputApplicationHandle, (IInterface**)&obj);
    return (Elastos::Droid::Server::Input::InputApplicationHandle*)obj.Get();
}

bool NativeInputApplicationHandle::updateInfo()
{
    AutoPtr<IInputApplicationHandle> obj;
    mObjWeak->Resolve(EIID_IInputApplicationHandle, (IInterface**)&obj);
    if (!obj) {
        releaseInfo();
        return false;
    }

    if (!mInfo) {
        mInfo = new android::InputApplicationInfo();
    }

    Elastos::Droid::Server::Input::InputApplicationHandle* handle =
            (Elastos::Droid::Server::Input::InputApplicationHandle*)obj.Get();
    if (!handle->mName.IsNull()) {
        mInfo->name.setTo(handle->mName.string());
    }
    else {
        mInfo->name.setTo("<null>");
    }

    mInfo->dispatchingTimeout = handle->mDispatchingTimeoutNanos;

    return true;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
