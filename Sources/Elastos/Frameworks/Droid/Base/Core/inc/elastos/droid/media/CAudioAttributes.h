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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTES_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTES_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CAudioAttributes.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashSet;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * A class to encapsulate a collection of attributes describing information about an audio
 * stream.
 * <p><code>AudioAttributes</code> supersede the notion of stream types (see for instance
 * {@link AudioManager#STREAM_MUSIC} or {@link AudioManager#STREAM_ALARM}) for defining the
 * behavior of audio playback. Attributes allow an application to specify more information than is
 * conveyed in a stream type by allowing the application to define:
 * <ul>
 * <li>usage: "why" you are playing a sound, what is this sound used for. This is achieved with
 *     the "usage" information. Examples of usage are {@link #USAGE_MEDIA} and {@link #USAGE_ALARM}.
 *     These two examples are the closest to stream types, but more detailed use cases are
 *     available. Usage information is more expressive than a stream type, and allows certain
 *     platforms or routing policies to use this information for more refined volume or routing
 *     decisions. Usage is the most important information to supply in <code>AudioAttributes</code>
 *     and it is recommended to build any instance with this information supplied, see
 *     {@link AudioAttributes.Builder} for exceptions.</li>
 * <li>content type: "what" you are playing. The content type expresses the general category of
 *     the content. This information is optional. But in case it is known (for instance
 *     {@link #CONTENT_TYPE_MOVIE} for a movie streaming service or {@link #CONTENT_TYPE_MUSIC} for
 *     a music playback application) this information might be used by the audio framework to
 *     selectively configure some audio post-processing blocks.</li>
 * <li>flags: "how" is playback to be affected, see the flag definitions for the specific playback
 *     behaviors they control. </li>
 * </ul>
 * <p><code>AudioAttributes</code> are used for example in one of the {@link AudioTrack}
 * constructors (see {@link AudioTrack#AudioTrack(AudioAttributes, AudioFormat, int, int, int)}),
 * to configure a {@link MediaPlayer}
 * (see {@link MediaPlayer#setAudioAttributes(AudioAttributes)} or a
 * {@link android.app.Notification} (see {@link android.app.Notification#audioAttributes}). An
 * <code>AudioAttributes</code> instance is built through its builder,
 * {@link AudioAttributes.Builder}.
 */
CarClass(CAudioAttributes)
    , public Object
    , public IAudioAttributes
    , public IParcelable
{
public:
    friend class CAudioAttributesBuilder;

    CAudioAttributes();

    virtual ~CAudioAttributes();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Return the content type.
     * @return one of the values that can be set in {@link Builder#setContentType(int)}
     */
    CARAPI GetContentType(
        /* [out] */ Int32* result);

    /**
     * Return the usage.
     * @return one of the values that can be set in {@link Builder#setUsage(int)}
     */
    CARAPI GetUsage(
        /* [out] */ Int32* result);

    /**
     * @hide
     * CANDIDATE FOR PUBLIC API
     * Return the capture preset.
     * @return one of the values that can be set in {@link Builder#setCapturePreset(int)} or a
     *    negative value if none has been set.
     */
    CARAPI GetCapturePreset(
        /* [out] */ Int32* result);

    /**
     * Return the flags.
     * @return a combined mask of all flags
     */
    CARAPI GetFlags(
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return all the flags, even the non-public ones.
     * Internal use only
     * @return a combined mask of all flags
     */
    CARAPI GetAllFlags(
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return the set of tags.
     * @return a read-only set of all tags stored as strings.
     */
    CARAPI GetTags(
        /* [out] */ ISet** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equal);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* result);

    /** @hide */
    CARAPI UsageToString(
        /* [out] */ String* result);

    /** @hide */
    static CARAPI UsageToString(
        /* [in] */ Int32 usage,
        /* [out] */ String* result);

    /** @hide */
    static CARAPI UsageForLegacyStreamType(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI ToLegacyStreamType(
        /* [in] */ IAudioAttributes* aa,
        /* [out] */ Int32* result);

public:
    Int32 mUsage;
    Int32 mContentType;
    Int32 mSource;
    Int32 mFlags;
    String mFormattedTags;

private:
    static String TAG;

    static Int32 FLAG_ALL;
    static Int32 FLAG_ALL_PUBLIC;

    /**
     * When adding tags for writeToParcel(Parcel, int), add them in the list of flags (| NEW_FLAG)
     */
    static Int32 ALL_PARCEL_FLAGS;

    AutoPtr<IHashSet> mTags;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTES_H__
