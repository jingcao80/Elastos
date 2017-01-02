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

#include "elastos/droid/os/storage/CMountServiceBinderListener.h"

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CAR_INTERFACE_IMPL(CMountServiceBinderListener, Object, IIMountServiceListener)

CAR_OBJECT_IMPL(CMountServiceBinderListener)

ECode CMountServiceBinderListener::constructor(
    /* [in] */ IStorageManager* host)
{
    mHost = (CStorageManager*)host;
    return NOERROR;
}

ECode CMountServiceBinderListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean available)
{
    List<AutoPtr<CStorageManager::ListenerDelegate> >::Iterator it;
    for (it = mHost->mListeners.Begin(); it != mHost->mListeners.Begin(); ++it) {
        (*it)->SendShareAvailabilityChanged(available);
    }
    return NOERROR;
}

CARAPI CMountServiceBinderListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    List<AutoPtr<CStorageManager::ListenerDelegate> >::Iterator it;
    for (it = mHost->mListeners.Begin(); it != mHost->mListeners.Begin(); ++it) {
        (*it)->SendStorageStateChanged(path, oldState, newState);
    }
    return NOERROR;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
