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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTESBUILDER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTESBUILDER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CAudioAttributesBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Builder class for {@link AudioAttributes} objects.
 * <p> Here is an example where <code>Builder</code> is used to define the
 * {@link AudioAttributes} to be used by a new <code>AudioTrack</code> instance:
 *
 * <pre class="prettyprint">
 * AudioTrack myTrack = new AudioTrack(
 *         new AudioAttributes.Builder()
 *             .setUsage(AudioAttributes.USAGE_MEDIA)
 *             .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
 *             .build(),
 *         myFormat, myBuffSize, AudioTrack.MODE_STREAM, mySession);
 * </pre>
 *
 * <p>By default all types of information (usage, content type, flags) conveyed by an
 * <code>AudioAttributes</code> instance are set to "unknown". Unknown information will be
 * interpreted as a default value that is dependent on the context of use, for instance a
 * {@link MediaPlayer} will use a default usage of {@link AudioAttributes#USAGE_MEDIA}.
 */
CarClass(CAudioAttributesBuilder)
    , public Object
    , public IAudioAttributesBuilder
{
public:
    CAudioAttributesBuilder();

    virtual ~CAudioAttributesBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IAudioAttributes* aa);

    /**
     * Combines all of the attributes that have been set and return a new
     * {@link AudioAttributes} object.
     * @return a new {@link AudioAttributes} object
     */
    // @SuppressWarnings("unchecked") // for cloning of mTags
    CARAPI Build(
        /* [out] */ IAudioAttributes** result);

    /**
     * Sets the attribute describing what is the intended use of the the audio signal,
     * such as alarm or ringtone.
     * @param usage one of {@link AudioAttributes#USAGE_UNKNOWN},
     *     {@link AudioAttributes#USAGE_MEDIA},
     *     {@link AudioAttributes#USAGE_VOICE_COMMUNICATION},
     *     {@link AudioAttributes#USAGE_VOICE_COMMUNICATION_SIGNALLING},
     *     {@link AudioAttributes#USAGE_ALARM}, {@link AudioAttributes#USAGE_NOTIFICATION},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_RINGTONE},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_COMMUNICATION_REQUEST},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_COMMUNICATION_INSTANT},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_COMMUNICATION_DELAYED},
     *     {@link AudioAttributes#USAGE_NOTIFICATION_EVENT},
     *     {@link AudioAttributes#USAGE_ASSISTANCE_ACCESSIBILITY},
     *     {@link AudioAttributes#USAGE_ASSISTANCE_NAVIGATION_GUIDANCE},
     *     {@link AudioAttributes#USAGE_ASSISTANCE_SONIFICATION},
     *     {@link AudioAttributes#USAGE_GAME}.
     * @return the same Builder instance.
     */
    CARAPI SetUsage(
        /* [in] */ Int32 usage);

    /**
     * Sets the attribute describing the content type of the audio signal, such as speech,
     * or music.
     * @param contentType the content type values, one of
     *     {@link AudioAttributes#CONTENT_TYPE_MOVIE},
     *     {@link AudioAttributes#CONTENT_TYPE_MUSIC},
     *     {@link AudioAttributes#CONTENT_TYPE_SONIFICATION},
     *     {@link AudioAttributes#CONTENT_TYPE_SPEECH},
     *     {@link AudioAttributes#CONTENT_TYPE_UNKNOWN}.
     * @return the same Builder instance.
     */
    CARAPI SetContentType(
        /* [in] */ Int32 contentType);

    /**
     * Sets the combination of flags.
     * @param flags the {@link AudioAttributes#FLAG_AUDIBILITY_ENFORCED} flag.
     * @return the same Builder instance.
     */
    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    /**
     * @hide
     * Add a custom tag stored as a string
     * @param tag
     * @return the same Builder instance.
     */
    CARAPI AddTag(
        /* [in] */ const String& tag);

    /**
     * Sets attributes as inferred from the legacy stream types.
     * Use this method when building an {@link AudioAttributes} instance to initialize some of
     * the attributes by information derived from a legacy stream type.
     * @param streamType one of {@link AudioManager#STREAM_VOICE_CALL},
     *   {@link AudioManager#STREAM_SYSTEM}, {@link AudioManager#STREAM_RING},
     *   {@link AudioManager#STREAM_MUSIC}, {@link AudioManager#STREAM_ALARM},
     *    or {@link AudioManager#STREAM_NOTIFICATION}.
     * @return the same Builder instance.
     */
    CARAPI SetLegacyStreamType(
        /* [in] */ Int32 streamType);

    /**
     * @hide
     * For internal framework use only, enables building from hidden stream types.
     * @param streamType
     * @return the same Builder instance.
     */
    CARAPI SetInternalLegacyStreamType(
        /* [in] */ Int32 streamType);

    /**
     * @hide
     * Sets the capture preset.
     * Use this audio attributes configuration method when building an {@link AudioRecord}
     * instance with {@link AudioRecord#AudioRecord(AudioAttributes, AudioFormat, int)}.
     * @param preset one of {@link MediaRecorder.AudioSource#DEFAULT},
     *     {@link MediaRecorder.AudioSource#MIC}, {@link MediaRecorder.AudioSource#CAMCORDER},
     *     {@link MediaRecorder.AudioSource#VOICE_RECOGNITION} or
     *     {@link MediaRecorder.AudioSource#VOICE_COMMUNICATION}.
     * @return the same Builder instance.
     */
    CARAPI SetCapturePreset(
        /* [in] */ Int32 preset);

    /**
     * @hide
     * Same as {@link #setCapturePreset(int)} but authorizes the use of HOTWORD and
     * REMOTE_SUBMIX.
     * @param preset
     * @return the same Builder instance.
     */
    CARAPI SetInternalCapturePreset(
        /* [in] */ Int32 preset);

private:
    Int32 mUsage;
    Int32 mContentType;
    Int32 mSource;
    Int32 mFlags;
    AutoPtr<IHashSet> mTags;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTESBUILDER_H__
