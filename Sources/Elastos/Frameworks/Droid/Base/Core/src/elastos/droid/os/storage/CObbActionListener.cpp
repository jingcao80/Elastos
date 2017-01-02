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

#include "elastos/droid/os/storage/CObbActionListener.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CAR_INTERFACE_IMPL(CObbActionListener, Object, IIObbActionListener)

CAR_OBJECT_IMPL(CObbActionListener)

ECode CObbActionListener::constructor(
    /* [in] */ IStorageManager* host)
{
    mHost = (CStorageManager*)host;
    return NOERROR;
}

ECode CObbActionListener::OnObbResult(
    /* [in] */ const String& filename,
    /* [in] */ Int32 nonce,
    /* [in] */ Int32 status)
{
    AutoPtr<CStorageManager::ObbListenerDelegate> delegate;
    {
        AutoLock lock(mListenersLock);
        HashMap<Int32, AutoPtr<CStorageManager::ObbListenerDelegate> >::Iterator it = mListeners.Find(nonce);
        if (it != mListeners.End()) delegate = it->mSecond;
        if (delegate != NULL) {
            mListeners.Erase(nonce);
        }
    }

    if (delegate != NULL) {
        delegate->SendObbStateChanged(filename, status);
    }
    return NOERROR;
}

Int32 CObbActionListener::AddListener(
    /* [in] */ IOnObbStateChangeListener* listener)
{
    AutoPtr<CStorageManager::ObbListenerDelegate> delegate = new CStorageManager::ObbListenerDelegate(listener, mHost);

    {
        AutoLock lock(mListenersLock);
        mListeners[delegate->mNonce] = delegate;
    }

    return delegate->mNonce;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
