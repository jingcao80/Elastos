#ifndef __ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGHELPER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGHELPER_H__

#include "_Elastos_Droid_Media_Tv_CTvContentRatingHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CarClass(CTvContentRatingHelper)
    , public Singleton
    , public ITvContentRatingHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates a {@code TvContentRating} object with predefined content rating strings.
     *
     * @param domain The domain string. For example, "com.android.tv".
     * @param ratingSystem The rating system string. For example, "US_TV".
     * @param rating The content rating string. For example, "US_TV_PG".
     * @param subRatings The sub-rating strings. For example, "US_TV_D" and "US_TV_L".
     * @return A {@code TvContentRating} object.
     * @throws IllegalArgumentException If {@code domain}, {@code ratingSystem} or {@code rating} is
     *             {@code null}.
     */
    CARAPI CreateRating(
        /* [in] */ const String& domainStr,
        /* [in] */ const String& ratingSystem,
        /* [in] */ const String& rating,
        /* [in] */ ArrayOf<String> * subRatings,
        /* [out] */ ITvContentRating ** result);

    /**
     * Recovers a {@code TvContentRating} object from the string that was previously created from
     * {@link #flattenToString}.
     *
     * @param ratingString The string returned by {@link #flattenToString}.
     * @return the {@code TvContentRating} object containing the domain, rating system, rating and
     *         sub-ratings information encoded in {@code ratingString}.
     * @see #flattenToString
     */
    CARAPI UnflattenFromString(
        /* [in] */ const String& ratingString,
        /* [out] */ ITvContentRating ** result);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVCONTENTRATINGHELPER_H__
