#include "elastos/droid/media/CRatingHelper.h"
#include "elastos/droid/media/Rating.h"
#include "Elastos.Droid.Media.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CRatingHelper)

CAR_INTERFACE_IMPL(CRatingHelper, Singleton, IRatingHelper)

ECode CRatingHelper::NewUnratedRating(
    /* [in] */ Int32 ratingStyle,
    /* [out] */ IRating** result)
{
    return Rating::NewUnratedRating(ratingStyle, result);
}

ECode CRatingHelper::NewHeartRating(
    /* [in] */ Boolean hasHeart,
    /* [out] */ IRating** result)
{
    return Rating::NewHeartRating(hasHeart, result);
}

ECode CRatingHelper::NewThumbRating(
    /* [in] */ Boolean thumbIsUp,
    /* [out] */ IRating** result)
{
    return Rating::NewThumbRating(thumbIsUp, result);
}

ECode CRatingHelper::NewStarRating(
    /* [in] */ Int32 starRatingStyle,
    /* [in] */ Float starRating,
    /* [out] */ IRating** result)
{
    return Rating::NewStarRating(starRatingStyle, starRating, result);
}

ECode CRatingHelper::NewPercentageRating(
    /* [in] */ Float percent,
    /* [out] */ IRating** result)
{
    return Rating::NewPercentageRating(percent, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos