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
