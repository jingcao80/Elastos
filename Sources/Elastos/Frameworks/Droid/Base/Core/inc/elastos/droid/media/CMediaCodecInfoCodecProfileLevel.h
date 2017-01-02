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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECPROFILELEVEL_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECPROFILELEVEL_H__

#include "_Elastos_Droid_Media_CMediaCodecInfoCodecProfileLevel.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoCodecProfileLevel)
    , public Object
    , public IMediaCodecInfoCodecProfileLevel
{
public:
    CMediaCodecInfoCodecProfileLevel();

    virtual ~CMediaCodecInfoCodecProfileLevel();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

public:
    /**
     * Defined in the OpenMAX IL specs, depending on the type of media
     * this can be OMX_VIDEO_AVCPROFILETYPE, OMX_VIDEO_H263PROFILETYPE,
     * OMX_VIDEO_MPEG4PROFILETYPE or OMX_VIDEO_VP8PROFILETYPE.
     */
    Int32 mProfile;

    /**
     * Defined in the OpenMAX IL specs, depending on the type of media
     * this can be OMX_VIDEO_AVCLEVELTYPE, OMX_VIDEO_H263LEVELTYPE
     * OMX_VIDEO_MPEG4LEVELTYPE or OMX_VIDEO_VP8LEVELTYPE.
     */
    Int32 mLevel;
};


} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECPROFILELEVEL_H__
