
#ifndef __ELASTOS_DROID_OS_STORAGE_CMOUNTSERVICEBINDERLISTENER_H__
#define __ELASTOS_DROID_OS_STORAGE_CMOUNTSERVICEBINDERLISTENER_H__

#include "_Elastos_Droid_Os_Storage_CMountServiceBinderListener.h"
#include "elastos/droid/os/storage/CStorageManager.h"
#include "Elastos.Droid.Os.h"

#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CMountServiceBinderListener)
    , public Object
    , public IIMountServiceListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IStorageManager* host);

    CARAPI OnUsbMassStorageConnectionChanged(
        /* [in] */ Boolean connected);

    CARAPI OnStorageStateChanged(
        /* [in] */ const String& path,
        /* [in] */ const String& oldState,
        /* [in] */ const String& newState);

private:
    CStorageManager* mHost;
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_CMOUNTSERVICEBINDERLISTENER_H__
