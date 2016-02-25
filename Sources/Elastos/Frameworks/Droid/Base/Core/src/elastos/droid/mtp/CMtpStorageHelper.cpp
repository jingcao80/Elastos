#include "elastos/droid/mtp/CMtpStorageHelper.h"
#include "elastos/droid/mtp/MtpStorage.h"

namespace Elastos {
namespace Droid {
namespace Mtp {

CAR_SINGLETON_IMPL(CMtpStorageHelper)

CAR_INTERFACE_IMPL(CMtpStorageHelper, Singleton, IMtpStorageHelper)

ECode CMtpStorageHelper::GetStorageId(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    return MtpStorage::GetStorageId(index, result);
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos