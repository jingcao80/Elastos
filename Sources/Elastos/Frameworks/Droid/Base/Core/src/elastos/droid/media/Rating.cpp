//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/media/Rating.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

const String Rating::TAG(String("Rating"));
const Float Rating::RATING_NOT_RATED = -1.0f;

CAR_INTERFACE_IMPL_2(Rating, Object, IRating, IParcelable)

Rating::Rating()
{}

Rating::~Rating()
{}

ECode Rating::constructor(
    /* [in] */ Int32 ratingStyle,
    /* [in] */ Float rating)
{
    mRatingStyle = ratingStyle;
    mRatingValue = rating;
    return NOERROR;
}

ECode Rating::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String str = mRatingValue < 0.0f ?
            String("unrated") : StringUtils::ToString(mRatingValue);
    *result = String("Rating:style=") + StringUtils::ToString(mRatingStyle) + String(" rating=") + str;
    return NOERROR;
}

ECode Rating::DescribeContents(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRatingStyle;
    return NOERROR;
}

ECode Rating::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    VALIDATE_NOT_NULL(src);
    src->ReadInt32(&mRatingStyle);
    return src->ReadFloat(&mRatingValue);
}

ECode Rating::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInt32(mRatingStyle);
    return dest->WriteFloat(mRatingValue);
}

ECode Rating::NewUnratedRating(
    /* [in] */ Int32 ratingStyle,
    /* [out] */ IRating** result)
{
    VALIDATE_NOT_NULL(result);
    switch(ratingStyle) {
        case RATING_HEART:
        case RATING_THUMB_UP_DOWN:
        case RATING_3_STARS:
        case RATING_4_STARS:
        case RATING_5_STARS:
        case RATING_PERCENTAGE:
        {
            AutoPtr<Rating> r = new Rating();
            r->constructor(ratingStyle, RATING_NOT_RATED);
            *result = r.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        default:
            *result = NULL;
            return NOERROR;
    }
}

ECode Rating::NewHeartRating(
    /* [in] */ Boolean hasHeart,
    /* [out] */ IRating** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<Rating> r = new Rating();
    r->constructor(RATING_HEART, hasHeart ? 1.0f : 0.0f);
    *result = r.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Rating::NewThumbRating(
    /* [in] */ Boolean thumbIsUp,
    /* [out] */ IRating** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<Rating> r = new Rating();
    r->constructor(RATING_THUMB_UP_DOWN, thumbIsUp ? 1.0f : 0.0f);
    *result = r.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Rating::NewStarRating(
    /* [in] */ Int32 starRatingStyle,
    /* [in] */ Float starRating,
    /* [out] */ IRating** result)
{
    VALIDATE_NOT_NULL(result);
    Float maxRating = -1.0f;
    switch(starRatingStyle) {
        case RATING_3_STARS:
        {
            maxRating = 3.0f;
            break;
        }
        case RATING_4_STARS:
        {
            maxRating = 4.0f;
            break;
        }
        case RATING_5_STARS:
        {
            maxRating = 5.0f;
            break;
        }
        default:
        {
            Slogger::E(TAG, "Invalid rating style (\" %d \") for a star rating", starRatingStyle);
            *result = NULL;
            return NOERROR;
        }
    }

    if ((starRating < 0.0f) || (starRating > maxRating)) {
        Slogger::E(TAG, "Trying to set out of range star-based rating");
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<Rating> r = new Rating();
    r->constructor(starRatingStyle, starRating);
    *result = r.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Rating::NewPercentageRating(
    /* [in] */ Float percent,
    /* [out] */ IRating** result)
{
    VALIDATE_NOT_NULL(result);
    if ((percent < 0.0f) || (percent > 100.0f)) {
        Slogger::E(TAG, "Invalid percentage-based rating value");
        *result = NULL;
        return NOERROR;
    } else {
        AutoPtr<Rating> r = new Rating();
        r->constructor(RATING_PERCENTAGE, percent);
        *result = r.Get();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
}

ECode Rating::IsRated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRatingValue >= 0.0f;
    return NOERROR;
}

ECode Rating::GetRatingStyle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRatingStyle;
    return NOERROR;
}

ECode Rating::HasHeart(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mRatingStyle != RATING_HEART) {
        *result = FALSE;
    } else {
        *result = (mRatingValue == 1.0f);
    }
    return NOERROR;
}

ECode Rating::IsThumbUp(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mRatingStyle != RATING_THUMB_UP_DOWN) {
        *result = FALSE;
    } else {
        *result = (mRatingValue == 1.0f);
    }
    return NOERROR;
}

ECode Rating::GetStarRating(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    switch (mRatingStyle) {
        case RATING_3_STARS:
        case RATING_4_STARS:
        case RATING_5_STARS:
        {
            Boolean flag = FALSE;
            IsRated(&flag);
            if (flag) {
                *result = mRatingValue;
                return NOERROR;
            }
        }

        default:
        {
            *result = -1.0f;
            return NOERROR;
        }

    }
}

ECode Rating::GetPercentRating(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    if ((mRatingStyle != RATING_PERCENTAGE) || !(IsRated(&flag), flag)) {
        *result = -1.0f;
    } else {
        *result = mRatingValue;
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos