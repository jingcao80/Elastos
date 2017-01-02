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

#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLERPLAYBACKINFO_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLERPLAYBACKINFO_H__

#include "_Elastos_Droid_Media_Session_CMediaControllerPlaybackInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Media::IAudioAttributes;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Holds information about the current playback and how audio is handled for
 * this session.
 */
CarClass(CMediaControllerPlaybackInfo)
    , public Object
    , public IMediaControllerPlaybackInfo
{
public:
    CMediaControllerPlaybackInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaControllerPlaybackInfo();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ IAudioAttributes * attrs,
        /* [in] */ Int32 control,
        /* [in] */ Int32 max,
        /* [in] */ Int32 current);

    /**
     * Get the type of playback which affects volume handling. One of:
     * <ul>
     * <li>{@link #PLAYBACK_TYPE_LOCAL}</li>
     * <li>{@link #PLAYBACK_TYPE_REMOTE}</li>
     * </ul>
     *
     * @return The type of playback this session is using.
     */
    CARAPI GetPlaybackType(
        /* [out] */ Int32 * result);

    /**
     * Get the audio attributes for this session. The attributes will affect
     * volume handling for the session. When the volume type is
     * {@link PlaybackInfo#PLAYBACK_TYPE_REMOTE} these may be ignored by the
     * remote volume handler.
     *
     * @return The attributes for this session.
     */
    CARAPI GetAudioAttributes(
        /* [out] */ IAudioAttributes ** result);

    /**
     * Get the type of volume control that can be used. One of:
     * <ul>
     * <li>{@link VolumeProvider#VOLUME_CONTROL_ABSOLUTE}</li>
     * <li>{@link VolumeProvider#VOLUME_CONTROL_RELATIVE}</li>
     * <li>{@link VolumeProvider#VOLUME_CONTROL_FIXED}</li>
     * </ul>
     *
     * @return The type of volume control that may be used with this
     *         session.
     */
    CARAPI GetVolumeControl(
        /* [out] */ Int32 * result);

    /**
     * Get the maximum volume that may be set for this session.
     *
     * @return The maximum allowed volume where this session is playing.
     */
    CARAPI GetMaxVolume(
        /* [out] */ Int32 * result);

    /**
     * Get the current volume for this session.
     *
     * @return The current volume where this session is playing.
     */
    CARAPI GetCurrentVolume(
        /* [out] */ Int32 * result);

public:
    Int32 mVolumeType;
    Int32 mVolumeControl;
    Int32 mMaxVolume;
    Int32 mCurrentVolume;
    AutoPtr<IAudioAttributes> mAudioAttrs;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLERPLAYBACKINFO_H__
