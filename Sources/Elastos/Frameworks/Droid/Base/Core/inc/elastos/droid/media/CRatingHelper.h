#ifndef __ELASTOS_DROID_MEDIA_CRATINGHELPER_H__
#define __ELASTOS_DROID_MEDIA_CRATINGHELPER_H__

#include "_Elastos_Droid_Media_CRatingHelper.h"
#include "Elastos.Droid.Media.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRatingHelper)
    , public Singleton
    , public IRatingHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NewUnratedRating(
        /* [in] */ Int32 ratingStyle,
        /* [out] */ IRating** result);

    CARAPI NewHeartRating(
        /* [in] */ Boolean hasHeart,
        /* [out] */ IRating** result);

    CARAPI NewThumbRating(
        /* [in] */ Boolean thumbIsUp,
        /* [out] */ IRating** result);

    CARAPI NewStarRating(
        /* [in] */ Int32 starRatingStyle,
        /* [in] */ Float starRating,
        /* [out] */ IRating** result);

    CARAPI NewPercentageRating(
        /* [in] */ Float percent,
        /* [out] */ IRating** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CRATINGHELPER_H__