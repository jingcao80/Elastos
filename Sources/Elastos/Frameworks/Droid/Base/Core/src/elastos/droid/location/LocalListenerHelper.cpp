
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/location/LocalListenerHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::AutoLock;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(LocalListenerHelper, Object, ILocalListenerHelper)

LocalListenerHelper::LocalListenerHelper(
    /* [in] */ const String& name)
    : mTag(name)
{
    Preconditions::CheckNotNull(name);
    CHashSet::New((IHashSet**)&mListeners);
}

ECode LocalListenerHelper::Add(
    /* [in] */ IInterface* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    FAIL_RETURN(Preconditions::CheckNotNull(listener));
    {    AutoLock syncLock(this);
        // we need to register with the service first, because we need to find out if the
        // service will actually support the request before we attempt anything
        Boolean isEmpty = FALSE;
        mListeners->IsEmpty(&isEmpty);
        if (isEmpty) {
            Boolean registeredWithService = FALSE;
            ECode ec = RegisterWithServer(&registeredWithService);
            if (FAILED(ec)) {
                Logger::E(mTag, "Error handling first listener.");
                *result = FALSE;
                return E_REMOTE_EXCEPTION;
            }
            if (!registeredWithService) {
                Logger::E(mTag, "Unable to register listener transport.");
                *result = FALSE;
                return NOERROR;
            }
        }

        Boolean isContains = FALSE;
        mListeners->Contains(listener, &isContains);
        if (isContains) {
            *result = TRUE;
            return NOERROR;
        }
        mListeners->Add(listener);
    }
    *result = TRUE;
    return NOERROR;
}

ECode LocalListenerHelper::Remove(
    /* [in] */ IInterface* listener)
{
    FAIL_RETURN(Preconditions::CheckNotNull(listener));
    {    AutoLock syncLock(this);
        Boolean removed = FALSE;
        mListeners->Remove(listener, &removed);
        Boolean isEmpty = FALSE;
        mListeners->IsEmpty(&isEmpty);
        Boolean isLastRemoved = removed && isEmpty;
        if (isLastRemoved) {
            ECode ec = UnregisterFromServer();
            if (FAILED(ec)) {
                return E_REMOTE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode LocalListenerHelper::Foreach(
    /* [in] */ ILocalListenerHelperListenerOperation* operation)
{
    AutoPtr<ICollection> listeners;
    {    AutoLock syncLock(this);
        AutoPtr<ICollection> coll = ICollection::Probe(mListeners);
        AutoPtr<IArrayList> al;
        CArrayList::New(coll, (IArrayList**)&al);
        listeners = ICollection::Probe(al);
    }

    AutoPtr<IIterator> it;
    listeners->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        ECode ec = operation->Execute(next.Get());
        if (FAILED(ec)) {
            Logger::E(mTag, "Error in monitored listener.");
            // don't return, give a fair chance to all listeners to receive the event
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
