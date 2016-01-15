#ifndef __ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGSYSTEMINFOHELPER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGSYSTEMINFOHELPER_H__

#include "_Elastos_Droid_Media_Tv_CTvContentRatingSystemInfoHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CarClass(CTvContentRatingSystemInfoHelper)
    , public Singleton
    , public ITvContentRatingSystemInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates a TvContentRatingSystemInfo object with given resource ID and receiver info.
     *
     * @param xmlResourceId The ID of an XML resource whose root element is
     *            <code> &lt;rating-system-definitions&gt;</code>
     * @param applicationInfo Information about the application that provides the TV content rating
     *            system definition.
     */
    CARAPI CreateTvContentRatingSystemInfo(
        /* [in] */ Int32 xmlResourceId,
        /* [in] */ IApplicationInfo * applicationInfo,
        /* [out] */ ITvContentRatingSystemInfo ** result);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGSYSTEMINFOHELPER_H__
