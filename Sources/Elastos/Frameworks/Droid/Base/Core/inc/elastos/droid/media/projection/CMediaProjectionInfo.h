#ifndef __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONINFO_H__
#define __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONINFO_H__

#include "_Elastos_Droid_Media_Projection_CMediaProjectionInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

/** @hide */
CarClass(CMediaProjectionInfo)
    , public Object
    , public IMediaProjectionInfo
    , public IParcelable
{
public:
    CMediaProjectionInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaProjectionInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle * handle);

    CARAPI GetPackageName(
        /* [out] */ String * result);

    CARAPI GetUserHandle(
        /* [out] */ IUserHandle ** result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean * result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI ToString(
        /* [out] */ String* s);

private:
    String mPackageName;
    AutoPtr<IUserHandle> mUserHandle;

};

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONINFO_H__
