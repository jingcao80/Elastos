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

#include "Observable.h"
#include "CArrayList.h"
#include "AutoLock.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(Observable, Object, IObservable)

Observable::Observable()
    : mChanged(FALSE)
{}

Observable::~Observable()
{
}

ECode Observable::constructor()
{
    AutoPtr<IArrayList> outlist;
    FAIL_RETURN(CArrayList::New((IArrayList**)&outlist));
    mObservers = IList::Probe(outlist);
    mChanged = FALSE;
    return NOERROR;
}

ECode Observable::AddObserver(
    /* [in] */ IObserver* observer)
{
    if (observer == NULL) {
        // throw new NullPointerException("observer == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    {
        AutoLock lock(this);
        Boolean isflag = FALSE;
        if (!(mObservers->Contains(observer, &isflag), isflag))
            mObservers->Add(observer, &isflag);
    }
    return NOERROR;
}

ECode Observable::CountObservers(
    /* [out] */ Int32* value)
{
    return mObservers->GetSize(value);
}

ECode Observable::DeleteObserver(
    /* [in] */ IObserver* observer)
{
    AutoLock lock(this);
    return mObservers->Remove(observer);
}

ECode Observable::DeleteObservers()
{
    AutoLock lock(this);
    return mObservers->Clear();
}

ECode Observable::HasChanged(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mChanged;
    return NOERROR;
}

ECode Observable::NotifyObservers()
{
    return NotifyObservers(NULL);
}

ECode Observable::NotifyObservers(
    /* [in] */ IInterface* data)
{
    Int32 size = 0;
    AutoPtr< ArrayOf<IInterface*> > arrays;
    {
        AutoLock lock(this);
        Boolean isflag = FALSE;
        if (HasChanged(&isflag), isflag) {
            ClearChanged();
            mObservers->GetSize(&size);
            mObservers->ToArray((ArrayOf<IInterface*>**)&arrays);
        }
    }
    if (arrays != NULL) {
        for (Int32 i = 0; i < arrays->GetLength(); i++) {
            AutoPtr<IObserver> observer = IObserver::Probe((*arrays)[i]);
            observer->Update(this, data);
        }
    }
    return NOERROR;
}

ECode Observable::ClearChanged()
{
    mChanged = FALSE;
    return NOERROR;
}

ECode Observable::SetChanged()
{
    mChanged = TRUE;
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
