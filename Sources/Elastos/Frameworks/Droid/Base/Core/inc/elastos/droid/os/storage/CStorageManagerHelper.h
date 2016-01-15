
#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__

#include "_Elastos_Droid_Os_Storage_CStorageManagerHelper.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageManagerHelper)
{
public:
    CARAPI From(
        /* [in] */IContext* context,
        /* [out] */ IStorageManager** manager);

    CARAPI GetPrimaryVolume(
        /* [in] */ ArrayOf<IStorageVolume*>* volumes,
        /* [out] */ IStorageVolume** result );
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGERHELPER_H__
