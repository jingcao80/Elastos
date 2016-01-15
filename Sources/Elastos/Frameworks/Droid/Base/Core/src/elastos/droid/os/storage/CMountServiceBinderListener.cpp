
#include "elastos/droid/os/storage/CMountServiceBinderListener.h"

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

ECode CMountServiceBinderListener::constructor(
    /* [in] */ IStorageManager* host)
{
    mHost = (CStorageManager*)host;
    return NOERROR;
}

ECode CMountServiceBinderListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean available)
{
    List<AutoPtr<CStorageManager::ListenerDelegate> >::Iterator it;
    for (it = mHost->mListeners.Begin(); it != mHost->mListeners.Begin(); ++it) {
        (*it)->SendShareAvailabilityChanged(available);
    }
    return NOERROR;
}

CARAPI CMountServiceBinderListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    List<AutoPtr<CStorageManager::ListenerDelegate> >::Iterator it;
    for (it = mHost->mListeners.Begin(); it != mHost->mListeners.Begin(); ++it) {
        (*it)->SendStorageStateChanged(path, oldState, newState);
    }
    return NOERROR;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
