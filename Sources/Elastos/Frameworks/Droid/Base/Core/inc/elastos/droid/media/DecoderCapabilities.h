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

#ifndef __ELASTOS_DROID_MEDIA_DECODERCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_DECODERCAPABILITIES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

class DecoderCapabilities
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    /**
     * Returns the list of video decoder types
     * @see android.media.DecoderCapabilities.VideoDecoder
     */
    static CARAPI GetVideoDecoders(
        /* [out, callee] */ ArrayOf<VideoDecoder>** result);

    /**
     * Returns the list of audio decoder types
     * @see android.media.DecoderCapabilities.AudioDecoder
     */
    static CARAPI GetAudioDecoders(
        /* [out, callee] */ ArrayOf<AudioDecoder>** result);

private:
    // Implemented by JNI
    static CARAPI_(void) Native_init();

    static CARAPI_(Int32) Native_get_num_video_decoders();

    static CARAPI_(Int32) Native_get_video_decoder_type(
        /* [in] */ Int32 index);

    static CARAPI_(Int32) Native_get_num_audio_decoders();

    static CARAPI_(Int32) Native_get_audio_decoder_type(
        /* [in] */ Int32 index);

private:
    static const StaticInitializer sInitializer;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_DECODERCAPABILITIES_H__
