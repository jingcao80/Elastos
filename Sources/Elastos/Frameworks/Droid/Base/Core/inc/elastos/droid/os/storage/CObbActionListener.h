
#ifndef __ELASTOS_DROID_OS_STORAGE_COBBACTIONLISTENER_H__
#define __ELASTOS_DROID_OS_STORAGE_COBBACTIONLISTENER_H__

#include "_Elastos_Droid_Os_Storage_CObbActionListener.h"
#include "elastos/droid/os/storage/CStorageManager.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CObbActionListener)
{
public:
    CARAPI constructor(
        /* [in] */ IStorageManager* host);

    CARAPI OnObbResult(
        /* [in] */ const String& filename,
        /* [in] */ Int32 nonce,
        /* [in] */ Int32 status);

    Int32 AddListener(
        /* [in] */ IOnObbStateChangeListener* listener);

private:
    CStorageManager* mHost;

    HashMap<Int32, AutoPtr<CStorageManager::ObbListenerDelegate> > mListeners;

    Object mListenersLock;
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_COBBACTIONLISTENER_H__
