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

#include "elastos/droid/server/wm/CWindowId.h"
#include "elastos/droid/server/wm/WindowState.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"

using Elastos::Droid::View::EIID_IIWindowId;
using Elastos::Droid::View::EIID_IWindowState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CAR_INTERFACE_IMPL(CWindowId, Object, IIWindowId)

CAR_OBJECT_IMPL(CWindowId)

ECode CWindowId::RegisterFocusObserver(
    /* [in] */ IIWindowFocusObserver* observer)
{
    AutoPtr<IWindowState> iowner;
    mWeakOwner->Resolve(EIID_IWindowState, (IInterface**)&iowner);
    if (iowner == NULL) {
        return NOERROR;
    }
    ((WindowState*)iowner.Get())->RegisterFocusObserver(observer);
    return NOERROR;
}

ECode CWindowId::UnregisterFocusObserver(
    /* [in] */ IIWindowFocusObserver* observer)
{
    AutoPtr<IWindowState> iowner;
    mWeakOwner->Resolve(EIID_IWindowState, (IInterface**)&iowner);
    if (iowner == NULL) {
        return NOERROR;
    }
    ((WindowState*)iowner.Get())->UnregisterFocusObserver(observer);
    return NOERROR;
}

ECode CWindowId::IsFocused(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWindowState> iowner;
    mWeakOwner->Resolve(EIID_IWindowState, (IInterface**)&iowner);
    if (iowner == NULL) {
        return NOERROR;
    }
    *result = ((WindowState*)iowner.Get())->IsFocused();
    return NOERROR;
}

ECode CWindowId::constructor(
    /* [in] */ IWindowState* owner)
{
    IWeakReferenceSource::Probe(owner)->GetWeakReference((IWeakReference**)&mWeakOwner);
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
