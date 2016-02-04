
#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUMEHELPER_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUMEHELPER_H__

#include "_Elastos_Droid_Os_Storage_CStorageVolumeHelper.h"
#include "elastos/droid/os/storage/CStorageVolume.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Content.h"

#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IFile;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageVolumeHelper)
    , public Singleton
    , public IStorageVolumeHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    FromTemplate(
        /* [in] */ IStorageVolume* temp,
        /* [in] */ IFile* path,
        /* [in] */ IUserHandle* owner,
        /* [out] */ IStorageVolume** volume);
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUMEHELPER_H__
