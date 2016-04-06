
#include "elastos/droid/database/DataSetObservable.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(DataSetObservable, Observable, IDataSetObservable)

ECode DataSetObservable::NotifyChanged()
{
    synchronized(mObserversLock) {
        List< AutoPtr<IInterface> >::ReverseIterator rit = mObservers.RBegin();
        for (; rit != mObservers.REnd(); ++rit) {
            AutoPtr<IDataSetObserver> observer = IDataSetObserver::Probe(*rit);
            observer->OnChanged();
        }
    }

    return NOERROR;
}

ECode DataSetObservable::NotifyInvalidated()
{
    synchronized(mObserversLock) {
        List< AutoPtr<IInterface> >::ReverseIterator rit = mObservers.RBegin();
        for (; rit != mObservers.REnd(); ++rit) {
            AutoPtr<IDataSetObserver> observer = IDataSetObserver::Probe(*rit);
            observer->OnInvalidated();
        }
    }

    return NOERROR;
}

} //Database
} //Droid
} //Elastos
