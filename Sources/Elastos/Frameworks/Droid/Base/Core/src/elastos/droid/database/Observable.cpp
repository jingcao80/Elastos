
#include "elastos/droid/database/Observable.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(Observable, Object, IObservable);

ECode Observable::RegisterObserver(
    /* [in] */ IInterface* observer)
{
    if (observer == NULL) {
        Slogger::E("Observable", "The observer is NULL.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {
        AutoLock syncLock(mObserversLock);
        AutoPtr<IInterface> obj = observer;
        List< AutoPtr<IInterface> >::Iterator it = Find(mObservers.Begin(), mObservers.End(), obj);
        if (it != mObservers.End()) {
            Slogger::E(String("Observable"), "Observer %s is already registered.", TO_CSTR(observer));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mObservers.PushBack(observer);
    }

    return NOERROR;
}

ECode Observable::UnregisterObserver(
    /* [in] */ IInterface* observer)
{
    if (observer == NULL) {
        Slogger::E("Observable", "The observer is NULL.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {
        AutoLock syncLock(mObserversLock);
        AutoPtr<IInterface> obj = observer;
        List< AutoPtr<IInterface> >::Iterator it = Find(mObservers.Begin(), mObservers.End(), obj);
        if (it == mObservers.End()) {
            Slogger::E(String("Observable"), "Observer %s was not registered.", TO_CSTR(observer));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mObservers.Erase(it);
    }

    return NOERROR;
}

ECode Observable::UnregisterAll()
{
    {
        AutoLock syncLock(mObserversLock);
        mObservers.Clear();
    }
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
