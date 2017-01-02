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

#ifndef __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTCHANNELS_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTCHANNELS_H__

#include "_Elastos_Droid_Media_Tv_CTvContractChannels.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/** Column definitions for the TV channels table. */
CarClass(CTvContractChannels)
    , public Singleton
    , public ITvContractChannels
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Returns the video resolution (definition) for a given video format.
     *
     * @param videoFormat The video format defined in {@link Channels}.
     * @return the corresponding video resolution string. {@code null} if the resolution string
     *         is not defined for the given video format.
     * @see #COLUMN_VIDEO_FORMAT
     */
    CARAPI GetVideoResolution(
        /* [in] */ const String& videoFormat,
        /* [out] */ String* result);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVCONTRACTCHANNELS_H__
