
#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUMEHELPER_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEVOLUMEHELPER_H__

#include "_Elastos_Droid_Os_Storage_CStorageVolumeHelper.h"
#include "elastos/droid/os/storage/CStorageVolume.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::IO::IFile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Storage::IStorageVolume;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageVolumeHelper)
{
public:
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
