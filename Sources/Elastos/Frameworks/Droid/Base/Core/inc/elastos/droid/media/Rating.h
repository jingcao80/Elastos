#ifndef __ELASTOS_DROID_MEDIA_RATING_H__
#define __ELASTOS_DROID_MEDIA_RATING_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

class Rating
    : public Object
    , public IRating
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~Rating();

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI DescribeContents(
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* src);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    static CARAPI NewUnratedRating(
        /* [in] */ Int32 ratingStyle,
        /* [out] */ IRating** result);

    static CARAPI NewHeartRating(
        /* [in] */ Boolean hasHeart,
        /* [out] */ IRating** result);

    static CARAPI NewThumbRating(
        /* [in] */ Boolean thumbIsUp,
        /* [out] */ IRating** result);

    static CARAPI NewStarRating(
        /* [in] */ Int32 starRatingStyle,
        /* [in] */ Float starRating,
        /* [out] */ IRating** result);

    static CARAPI NewPercentageRating(
        /* [in] */ Float percent,
        /* [out] */ IRating** result);

    CARAPI IsRated(
        /* [out] */ Boolean* result);

    CARAPI GetRatingStyle(
        /* [out] */ Int32* result);

    CARAPI HasHeart(
        /* [out] */ Boolean* result);

    CARAPI IsThumbUp(
        /* [out] */ Boolean* result);

    CARAPI GetStarRating(
        /* [out] */ Float* result);

    CARAPI GetPercentRating(
        /* [out] */ Float* result);

private:
    Rating();

    CARAPI constructor(
        /* [in] */ Int32 ratingStyle,
        /* [in] */ Float rating);

private:
    const static String TAG;
    const static Float RATING_NOT_RATED;
    Int32 mRatingStyle;
    Float mRatingValue;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_RATING_H__