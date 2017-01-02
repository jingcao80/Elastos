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

#include "elastos/droid/database/DataSetObservable.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(DataSetObservable, Observable, IDataSetObservable)

ECode DataSetObservable::NotifyChanged()
{
    AutoLock syncLock(mObserversLock);
    List< AutoPtr<IInterface> >::ReverseIterator rit = mObservers.RBegin();
    List< AutoPtr<IInterface> >::ReverseIterator end = mObservers.REnd();
    IDataSetObserver* observer;
    for (; rit != end; ++rit) {
        observer = IDataSetObserver::Probe(*rit);
        observer->OnChanged();
    }

    return NOERROR;
}

ECode DataSetObservable::NotifyInvalidated()
{
    AutoLock syncLock(mObserversLock);
    List< AutoPtr<IInterface> >::ReverseIterator rit = mObservers.RBegin();
    List< AutoPtr<IInterface> >::ReverseIterator end = mObservers.REnd();
    IDataSetObserver* observer;
    for (; rit != end; ++rit) {
        observer = IDataSetObserver::Probe(*rit);
        observer->OnInvalidated();
    }

    return NOERROR;
}

} //Database
} //Droid
} //Elastos
