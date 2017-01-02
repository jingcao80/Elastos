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

#include "AbstractSelectableChannel.h"
#include "AbstractSelector.h"
#include "AutoLock.h"

using Elastos::Core::IRunnable;
using Elastos::Core::Thread;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

CAR_INTERFACE_IMPL(AbstractSelectableChannel, SelectableChannel, IAbstractSelectableChannel)

AbstractSelectableChannel::AbstractSelectableChannel()
    : mIsBlocking(FALSE)
{}

ECode AbstractSelectableChannel::constructor(
    /* [in] */ ISelectorProvider* provider)
{
    mBlockingLock = new Object();
    mProvider = provider;
    return NOERROR;
}

AbstractSelectableChannel::~AbstractSelectableChannel()
{
}

ECode AbstractSelectableChannel::GetProvider(
    /* [in] */ ISelectorProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode AbstractSelectableChannel::IsRegistered(
    /* [out] */ Boolean* isRegister)
{
    VALIDATE_NOT_NULL(isRegister);
    AutoLock lock(mBlockingLock);
    *isRegister = !mKeyList.IsEmpty();
    return NOERROR;
}

ECode AbstractSelectableChannel::GetKeyFor(
    /* [in] */ ISelector* selector,
    /* [out] */ ISelectionKey** key)
{
    VALIDATE_NOT_NULL(key);
    *key = NULL;

    AutoLock lock(mBlockingLock);
    ISelectionKey* selKey;
    AutoPtr<ISelector> sel;
    List<AutoPtr<ISelectionKey> >::Iterator it = mKeyList.Begin();
    for (; it != mKeyList.End(); ++it) {
        selKey = *it;
        if (selKey) {
            sel = NULL;
            selKey->GetSelector((ISelector**)&sel);
            if (sel.Get() == selector) {
                *key = selKey;
                REFCOUNT_ADD(*key);
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

ECode AbstractSelectableChannel::Register(
    /* [in] */ ISelector* selector,
    /* [in] */ Int32 interestSet,
    /* [in] */ IObject* obj,
    /* [out] */ ISelectionKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(selector);
    VALIDATE_NOT_NULL(obj);

    Boolean isOpen = FALSE;
    IsOpen(&isOpen);
    if (!isOpen) {
        return E_CLOSED_CHANNEL_EXCEPTION;
    }

    Int32 ops = 0;
    GetValidOps(&ops);
    if (!((interestSet & ~ops) == 0)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(mBlockingLock);
    if (mIsBlocking) {
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }

    selector->IsOpen(&isOpen);
    if (!isOpen) {
        if (0 == interestSet) {
            return E_ILLEGAL_SELECTOR_EXCEPTION;
        }
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ISelectionKey> key;
    GetKeyFor(selector, (ISelectionKey**)&key);
    if (NULL == key) {
        AbstractSelector* absSel = (AbstractSelector*)selector;
        absSel->Register(this, interestSet, obj, (ISelectionKey**)&key);
        mKeyList.PushBack(key);
    }
    else {
        Boolean isValid = FALSE;
        key->IsValid(&isValid);
        if (!isValid) {
            return E_CANCELLED_KEY_EXCEPTION;
        }
        key->GetInterestOps(interestSet, NULL);
        key->Attach(obj, NULL);
    }

    *result = key;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AbstractSelectableChannel::IsBlocking(
    /* out*/ Boolean* isBlocking)
{
    AutoLock lock(mBlockingLock);
    *isBlocking = mIsBlocking;
    return NOERROR;
}

ECode AbstractSelectableChannel::GetBlockingLock(
    /* [out] */ IObject** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = (IObject*)mBlockingLock.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode AbstractSelectableChannel::ConfigureBlocking(
    /* [in] */ Boolean blockingMode,
    /* [out] */ ISelectableChannel** channel)
{
    VALIDATE_NOT_NULL(channel);
    *channel = NULL;

    Boolean isOpen = FALSE;
    IsOpen(&isOpen);
    if (!isOpen) {
        return E_CLOSED_CHANNEL_EXCEPTION;
    }

    AutoLock lock(mBlockingLock);

    if (mIsBlocking == blockingMode) {
        *channel = this;
        REFCOUNT_ADD(*channel);
        return NOERROR;
    }

    Boolean contains = ContainsValidKeys();
    if (blockingMode && contains) {
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }

    ImplConfigureBlocking(blockingMode);

    *channel = this;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode AbstractSelectableChannel::Deregister(
   /* [in] */ ISelectionKey* key)
{
    AutoLock lock(mBlockingLock);
    mKeyList.Remove(key);
    return NOERROR;
}

ECode AbstractSelectableChannel::ImplCloseChannel()
{
    AutoLock lock(mBlockingLock);

    ImplCloseSelectableChannel();

    ISelectionKey* key;
    List<AutoPtr<ISelectionKey> >::Iterator it = mKeyList.Begin();
    for (; it != mKeyList.End(); ++it) {
        key = *it;
        if (key != NULL) {
            key->Cancel();
        }
    }
    return NOERROR;
}

Boolean AbstractSelectableChannel::ContainsValidKeys()
{
    AutoLock lock(mBlockingLock);

    Boolean bval;
    ISelectionKey* key;
    List<AutoPtr<ISelectionKey> >::Iterator it = mKeyList.Begin();
    for (; it != mKeyList.End(); ++it) {
        key = *it;
        if (key != NULL && (key->IsValid(&bval), bval)) {
            return TRUE;
        }
    }

    return FALSE;
}

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos
