
#ifndef __ELASTOS_DROID_MTP_CMTPSTORAGE_H__
#define __ELASTOS_DROID_MTP_CMTPSTORAGE_H__

#include "_Elastos_Droid_Mtp_CMtpStorage.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Storage::IStorageVolume;

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpStorage)
    , public Object
    , public IMtpStorage
{
public:
    CMtpStorage();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IStorageVolume* volume,
        /* [in] */ IContext* context);

    CARAPI GetStorageId(
        /* [out] */ Int32* result);

    CARAPI GetStorageId(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI GetPath(
        /* [out] */ String* result);

    CARAPI GetDescription(
        /* [out] */ String* result);

    CARAPI GetReserveSpace(
        /* [out] */ Int64* result);

    CARAPI IsRemovable(
        /* [out] */ Boolean* result);

    CARAPI GetMaxFileSize(
        /* [out] */ Int64* result);

private:
    Int32 mStorageId;
    String mPath;
    String mDescription;
    Int64 mReserveSpace;
    Boolean mRemovable;
    Int64 mMaxFileSize;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MTP_CMTPSTORAGE_H__
