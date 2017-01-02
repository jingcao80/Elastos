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

#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/server/location/RemoteListenerHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//===============================
//RemoteListenerHelper::LinkedListener
//===============================

CAR_INTERFACE_IMPL(RemoteListenerHelper::LinkedListener, Object, IProxyDeathRecipient)

RemoteListenerHelper::LinkedListener::LinkedListener(
    /* [in] */ IInterface* listener,
    /* [in] */ RemoteListenerHelper* host)
    : mListener(listener)
    , mHost(host)
{}

ECode RemoteListenerHelper::LinkedListener::GetUnderlyingListener(
    /* [out] */ IInterface** listener)
{
    VALIDATE_NOT_NULL(listener)
    *listener = mListener;
    REFCOUNT_ADD(*listener)
    return NOERROR;
}

ECode RemoteListenerHelper::LinkedListener::ProxyDied()
{
    String str;
    IObject::Probe(mListener)->ToString(&str);
    Logger::D(mHost->mTag, "Remote Listener died: %s", str.string());
    Boolean result;
    mHost->RemoveListener(mListener.Get(), &result);
    return NOERROR;
}

//===============================
//RemoteListenerHelper
//===============================

CAR_INTERFACE_IMPL(RemoteListenerHelper, Object, IRemoteListenerHelper)

ECode RemoteListenerHelper::AddListener(
    /* [in] */ IInterface* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    preconditions->CheckNotNull(listener, String("Attempted to register a 'null' listener."));
    Boolean isSupported;
    if (IsSupported(&isSupported), !isSupported) {
        Logger::E(mTag, "Refused to add listener, the feature is not supported.");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IBinder> binder = IBinder::Probe(listener);
    AutoPtr<LinkedListener> deathListener = new LinkedListener(listener, this);
    {    AutoLock syncLock(mListenerMap);
        Boolean isContained;
        if (mListenerMap->ContainsKey(binder, &isContained), isContained) {
            // listener already added
            *result = TRUE;
            return NOERROR;
        }

        AutoPtr<IProxy> proxy = (IProxy*)(binder->Probe(EIID_IProxy));
        if (proxy != NULL) {
            ECode ec = proxy->LinkToDeath((IProxyDeathRecipient*)deathListener.Get(), 0 /* flags */);
            if (FAILED(ec)) {
                // if the remote process registering the listener is already death, just swallow the
                // exception and continue
                Logger::E(mTag, "Remote listener already died.%08x", ec);
                *result = FALSE;
                return E_REMOTE_EXCEPTION;
            }
        }

        mListenerMap->Put(binder, (IProxyDeathRecipient*)deathListener.Get());
        Int32 size;
        mListenerMap->GetSize(&size);
        if (size == 1) {
            Boolean isRegistered;
            if (RegisterWithService(&isRegistered), !isRegistered) {
                Logger::E(mTag, "RegisterWithService failed, listener will be removed.");
                Boolean b;
                RemoveListener(listener, &b);
                *result = FALSE;
                return NOERROR;
            }
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode RemoteListenerHelper::RemoveListener(
    /* [in] */ IInterface* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    preconditions->CheckNotNull(listener, String("Attempted to remove a 'null' listener."));
    Boolean isSupported;
    if (IsSupported(&isSupported), !isSupported) {
        Logger::E(mTag, "Refused to remove listener, the feature is not supported.");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IBinder> binder = IBinder::Probe(listener);
    AutoPtr<LinkedListener> linkedListener;
    {    AutoLock syncLock(mListenerMap);
        AutoPtr<IInterface> value;
        mListenerMap->Remove(binder, (IInterface**)&value);
        linkedListener = (LinkedListener*)(IObject::Probe(value));
        Boolean isEmpty;
        if (mListenerMap->IsEmpty(&isEmpty), isEmpty && linkedListener != NULL) {
            UnregisterFromService();
        }
    }

    if (linkedListener != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)(binder->Probe(EIID_IProxy));
        Boolean result;
        proxy->UnlinkToDeath(linkedListener, 0 /* flags */, &result);
    }
    *result = TRUE;
    return NOERROR;
}

RemoteListenerHelper::RemoteListenerHelper(
    /* [in] */ const String& name)
{
    CHashMap::New((IHashMap**)&mListenerMap);
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    preconditions->CheckNotNull(cs.Get());
    mTag = name;
}

ECode RemoteListenerHelper::Foreach(
    /* [in] */ IListenerOperation* operation)
{
    AutoPtr<ICollection> linkedListeners;
    {    AutoLock syncLock(mListenerMap);
        AutoPtr<ICollection> values;
        mListenerMap->GetValues((ICollection**)&values);
        CArrayList::New(values.Get(), (ICollection**)&linkedListeners);
    }

    AutoPtr<IIterator> iter;
    linkedListeners->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        iter->GetNext((IInterface**)&next);
        AutoPtr<LinkedListener> linkedListener = (LinkedListener*)(IObject::Probe(next));
        AutoPtr<IInterface> listener;
        linkedListener->GetUnderlyingListener((IInterface**)&listener);
        ECode ec = operation->Execute(listener);
        if (FAILED(ec)) {
            Logger::E(mTag, "Error in monitored listener.%08x", ec);
            Boolean result;
            RemoveListener(listener, &result);
        }
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
