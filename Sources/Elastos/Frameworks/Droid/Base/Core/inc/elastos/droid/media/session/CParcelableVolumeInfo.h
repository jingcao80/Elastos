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

#ifndef __ELASTOS_DROID_MEDIA_SESSION_CPARCELABLEVOLUMEINFO_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CPARCELABLEVOLUMEINFO_H__

#include "_Elastos_Droid_Media_Session_CParcelableVolumeInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Convenience class for passing information about the audio configuration of a
 * session. The public implementation is {@link MediaController.PlaybackInfo}.
 *
 * @hide
 */
CarClass(CParcelableVolumeInfo)
    , public Object
    , public IParcelableVolumeInfo
    , public IParcelable
{
public:
    CParcelableVolumeInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CParcelableVolumeInfo();

    CARAPI constructor();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 volumeType,
        /* [in] */ IAudioAttributes * audioAttrs,
        /* [in] */ Int32 controlType,
        /* [in] */ Int32 maxVolume,
        /* [in] */ Int32 currentVolume);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    Int32 mVolumeType;
    Int32 mControlType;
    Int32 mMaxVolume;
    Int32 mCurrentVolume;
    AutoPtr<IAudioAttributes> mAudioAttrs;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CPARCELABLEVOLUMEINFO_H__
