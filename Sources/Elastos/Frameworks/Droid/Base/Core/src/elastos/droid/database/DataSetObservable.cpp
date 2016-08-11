
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
