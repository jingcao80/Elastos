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