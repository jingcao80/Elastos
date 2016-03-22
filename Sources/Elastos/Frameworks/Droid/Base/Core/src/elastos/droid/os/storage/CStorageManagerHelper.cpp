
#include "elastos/droid/os/storage/CStorageManagerHelper.h"
#include "elastos/droid/os/storage/CStorageManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {
CAR_INTERFACE_IMPL(CStorageManagerHelper, Singleton, IStorageManagerHelper)

CAR_SINGLETON_IMPL(CStorageManagerHelper)

/** {@hide} */
ECode CStorageManagerHelper::From(
    /* [in] */ IContext* context,
    /* [out] */ IStorageManager** manager)
{
    VALIDATE_NOT_NULL(manager);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&service);
    *manager = IStorageManager::Probe(service);
    REFCOUNT_ADD(*manager);

    return NOERROR;
}

/** {@hide} */
ECode CStorageManagerHelper::GetPrimaryVolume(
    /* [in] */ ArrayOf<IStorageVolume*>* volumes,
    /* [out] */ IStorageVolume** result )
{
    VALIDATE_NOT_NULL(result);

    for (Int32 i = 0; i < volumes->GetLength(); i++) {
        AutoPtr<IStorageVolume> volume = (*volumes)[i];
        Boolean primary = FALSE;
        if (volume->IsPrimary(&primary), primary) {
            *result = volume;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    Logger::W(CStorageManager::TAG, "No primary storage defined");
    *result = NULL;
    return NOERROR;
}


/** {@hide} */
ECode CStorageManagerHelper::GetNoEmulatedVolume(
    /* [in] */ ArrayOf<IStorageVolume*>* volumes,
    /* [out] */ IStorageVolume** result)
{
   VALIDATE_NOT_NULL(result);

    for (Int32 i = 0; i < volumes->GetLength(); i++) {
        AutoPtr<IStorageVolume> volume = (*volumes)[i];
        Boolean isEmulated = FALSE;
        if (volume->IsEmulated(&isEmulated), !isEmulated) {
            *result = volume;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    } 

    *result = NULL;
    return NOERROR;
}

/**
* Return the list of physical external storages
*
* @hide
*/
ECode CStorageManagerHelper::GetPhysicalExternalVolume(
    /* [in] */ ArrayOf<IStorageVolume*>* volumesphy,
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count = volumesphy->GetLength();
    AutoPtr<IArrayList> volumes;
    CArrayList::New((IArrayList**)&volumes);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IStorageVolume> volume = (*volumesphy)[i];
        Boolean isEmulated = FALSE;
        if (volume->IsEmulated(&isEmulated), !isEmulated) {
            volumes->Add(volume);
        }
    }
    *result = volumes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos