
#include "NetworkEventDispatcher.h"
#include "concurrent/CCopyOnWriteArrayList.h"
#include "logging/Logger.h"

using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Libcore {
namespace Net {
namespace Event {

CAR_INTERFACE_IMPL(NetworkEventDispatcher, Object, INetworkEventDispatcher)

AutoPtr<INetworkEventDispatcher> NetworkEventDispatcher::sInstance = new NetworkEventDispatcher();

ECode NetworkEventDispatcher::GetInstance(
    /* [out] */ INetworkEventDispatcher** result)
{
    VALIDATE_NOT_NULL(result)
    *result = sInstance;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

NetworkEventDispatcher::NetworkEventDispatcher()
{
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&mListeners);
}

ECode NetworkEventDispatcher::AddListener(
    /* [in] */ INetworkEventListener* toAdd)
{
    if (toAdd == NULL) {
        // throw new NullPointerException("toAdd == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    return IList::Probe(mListeners)->Add(toAdd);
}

ECode NetworkEventDispatcher::RemoveListener(
    /* [in] */ INetworkEventListener* toRemove)
{
    AutoPtr<IIterator> ator;
    IIterable::Probe(mListeners)->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> listener;
        ator->GetNext((IInterface**)&listener);
        if (INetworkEventListener::Probe(listener) == toRemove) {
            ICollection::Probe(mListeners)->Remove(INetworkEventListener::Probe(listener));
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode NetworkEventDispatcher::OnNetworkConfigurationChanged()
{
    AutoPtr<IIterator> ator;
    IIterable::Probe(mListeners)->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> listener;
        ator->GetNext((IInterface**)&listener);
        if (FAILED(INetworkEventListener::Probe(listener)->OnNetworkConfigurationChanged())) {
            Logger::I("NetworkEventDispatcher", "Exception thrown during network event propagation");
        }
    }
    return NOERROR;
}

} // namespace Event
} // namespace Net
} // namespace Libcore
