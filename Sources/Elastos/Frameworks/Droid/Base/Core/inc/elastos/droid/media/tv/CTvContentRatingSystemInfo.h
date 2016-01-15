#ifndef __ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGSYSTEMINFO_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGSYSTEMINFO_H__

#include "_Elastos_Droid_Media_Tv_CTvContentRatingSystemInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * TvContentRatingSystemInfo class provides information about a specific TV content rating system
 * defined either by a system app or by a third-party app.
 *
 * @hide
 */
// @SystemApi
CarClass(CTvContentRatingSystemInfo)
    , public Object
    , public ITvContentRatingSystemInfo
    , public IParcelable
{
public:
    CTvContentRatingSystemInfo();

    virtual ~CTvContentRatingSystemInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IUri * xmlUri,
        /* [in] */ IApplicationInfo * applicationInfo);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns {@code true} if the TV content rating system is defined by a system app,
     * {@code false} otherwise.
     */
    CARAPI IsSystemDefined(
        /* [out] */ Boolean * result);

    /**
     * Returns the URI to the XML resource that defines the TV content rating system.
     *
     * TODO: Remove. Instead, parse the XML resource and provide an interface to directly access
     * parsed information.
     */
    CARAPI GetXmlUri(
        /* [out] */ IUri ** result);

    /**
     * Creates a TvContentRatingSystemInfo object with given resource ID and receiver info.
     *
     * @param xmlResourceId The ID of an XML resource whose root element is
     *            <code> &lt;rating-system-definitions&gt;</code>
     * @param applicationInfo Information about the application that provides the TV content rating
     *            system definition.
     */
    static CARAPI CreateTvContentRatingSystemInfo(
        /* [in] */ Int32 xmlResourceId,
        /* [in] */ IApplicationInfo * applicationInfo,
        /* [out] */ ITvContentRatingSystemInfo ** result);

public:
    AutoPtr<IUri> mXmlUri;

    AutoPtr<IApplicationInfo> mApplicationInfo;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGSYSTEMINFO_H__
