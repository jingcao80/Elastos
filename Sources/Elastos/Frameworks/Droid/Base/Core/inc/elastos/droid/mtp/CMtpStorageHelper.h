#ifndef __ELASTOS_DROID_MTP_CMTPSTORAGEHELPER_H__
#define __ELASTOS_DROID_MTP_CMTPSTORAGEHELPER_H__

#include "_Elastos_Droid_Mtp_CMtpStorageHelper.h"
#include <elastos/core/Singleton.h>
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpStorageHelper)
    , public Singleton
    , public IMtpStorageHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetStorageId(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MTP_CMTPSTORAGEHELPER_H__

