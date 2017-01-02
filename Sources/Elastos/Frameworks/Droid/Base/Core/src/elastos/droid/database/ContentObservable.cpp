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

#include "elastos/droid/database/ContentObservable.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(ContentObservable, Observable, IContentObservable)

ECode ContentObservable::RegisterObserver(
    /* [in] */ IInterface* observer)
{
    return Observable::RegisterObserver(observer);
}

ECode ContentObservable::DispatchChange(
    /* [in] */ Boolean selfChange)
{
    return DispatchChange(selfChange, NULL);
}

ECode ContentObservable::DispatchChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    {    AutoLock syncLock(mObserversLock);
        List< AutoPtr<IInterface> >::Iterator iter;
        for (iter = mObservers.Begin(); iter != mObservers.End(); ++iter) {
            AutoPtr<IContentObserver> observer = IContentObserver::Probe(*iter);
            Boolean result;
            if (!selfChange || (observer->DeliverSelfNotifications(&result), result)) {
                observer->DispatchChange(selfChange);
            }
        }
    }
    return NOERROR;
}

ECode ContentObservable::NotifyChange(
    /* [in] */ Boolean selfChange)
{
    {    AutoLock syncLock(mObserversLock);
        List< AutoPtr<IInterface> >::Iterator iter;
        for (iter = mObservers.Begin(); iter != mObservers.End(); ++iter) {
            AutoPtr<IContentObserver> observer = IContentObserver::Probe(*iter);
            observer->OnChange(selfChange);
        }
    }
    return NOERROR;
}

} //Database
} //Droid
} //Elastos