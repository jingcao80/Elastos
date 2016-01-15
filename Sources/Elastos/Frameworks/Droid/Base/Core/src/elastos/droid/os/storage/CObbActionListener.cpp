
#include "elastos/droid/os/storage/CObbActionListener.h"

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

ECode CObbActionListener::constructor(
    /* [in] */ IStorageManager* host)
{
    mHost = (CStorageManager*)host;
    return NOERROR;
}

ECode CObbActionListener::OnObbResult(
    /* [in] */ const String& filename,
    /* [in] */ Int32 nonce,
    /* [in] */ Int32 status)
{
    AutoPtr<CStorageManager::ObbListenerDelegate> delegate;
    {
        AutoLock lock(mListenersLock);
        HashMap<Int32, AutoPtr<CStorageManager::ObbListenerDelegate> >::Iterator it = mListeners.Find(nonce);
        if (it != mListeners.End()) delegate = it->mSecond;
        if (delegate != NULL) {
            mListeners.Erase(nonce);
        }
    }

    if (delegate != NULL) {
        delegate->SendObbStateChanged(filename, status);
    }
    return NOERROR;
}

Int32 CObbActionListener::AddListener(
    /* [in] */ IOnObbStateChangeListener* listener)
{
    AutoPtr<CStorageManager::ObbListenerDelegate> delegate = new CStorageManager::ObbListenerDelegate(listener, mHost);

    {
        AutoLock lock(mListenersLock);
        mListeners[delegate->mNonce] = delegate;
    }

    return delegate->mNonce;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
