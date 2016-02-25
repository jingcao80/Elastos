
#ifndef __ELASTOS_DROID_MTP_CMTPSTORAGE_H__
#define __ELASTOS_DROID_MTP_CMTPSTORAGE_H__

#include "_Elastos_Droid_Mtp_CMtpStorage.h"
#include "elastos/droid/mtp/MtpStorage.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Storage::IStorageVolume;

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpStorage)
    , public MtpStorage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MTP_CMTPSTORAGE_H__
