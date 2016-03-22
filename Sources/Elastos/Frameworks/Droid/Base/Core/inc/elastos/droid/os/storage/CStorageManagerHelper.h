
#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__

#include "_Elastos_Droid_Os_Storage_CStorageManagerHelper.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageManagerHelper)
    , public Singleton
    , public IStorageManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI From(
        /* [in] */IContext* context,
        /* [out] */ IStorageManager** manager);

    CARAPI GetPrimaryVolume(
        /* [in] */ ArrayOf<IStorageVolume*>* volumes,
        /* [out] */ IStorageVolume** result );

    CARAPI GetNoEmulatedVolume(
        /* [in] */ ArrayOf<IStorageVolume*>* volumes,
        /* [out] */ IStorageVolume** result);

    CARAPI GetPhysicalExternalVolume(
        /* [in] */ ArrayOf<IStorageVolume*>* volumesphy,
        /* [out] */ IArrayList** result);
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__
