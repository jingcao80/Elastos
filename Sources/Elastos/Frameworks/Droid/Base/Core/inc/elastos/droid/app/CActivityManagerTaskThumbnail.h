
#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERTASKTHUMBNAIL_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERTASKTHUMBNAIL_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CActivityManagerTaskThumbnail.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace App {

/** @hide */
CarClass(CActivityManagerTaskThumbnail)
    , public Object
    , public IActivityManagerTaskThumbnail
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerTaskThumbnail();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetMainThumbnail(
        /* [out] */ IBitmap** thumbnail);

    CARAPI SetMainThumbnail(
        /* [in] */ IBitmap* thumbnail);

    CARAPI GetThumbnailFileDescriptor(
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI SetThumbnailFileDescriptor(
        /* [in] */ IParcelFileDescriptor* pfd);

public:
    AutoPtr<IBitmap> mMainThumbnail;

    AutoPtr<IParcelFileDescriptor> mThumbnailFileDescriptor;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERTASKTHUMBNAIL_H__
