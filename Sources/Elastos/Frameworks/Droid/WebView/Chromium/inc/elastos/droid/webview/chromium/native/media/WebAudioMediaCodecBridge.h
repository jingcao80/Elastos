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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_WEBAUDIOMEDIACODECBRIDGE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_WEBAUDIOMEDIACODECBRIDGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

//@JNINamespace("media")
class WebAudioMediaCodecBridge
    : public Object
{
public:
    static const String TAG;
    // TODO(rtoy): What is the correct timeout value for reading
    // from a file in memory?
    static const Int64 TIMEOUT_MICROSECONDS = 500;

private:
    //@CalledByNative
    static CARAPI_(String) CreateTempFile(
        /* [in] */ IContext* ctx);

    //@CalledByNative
    static CARAPI_(Boolean) DecodeAudioFile(
        /* [in] */ IContext* ctx,
        /* [in] */ HANDLE nativeMediaCodecBridge,
        /* [in] */ Int32 inputFD,
        /* [in] */ Int64 dataSize);

    static CARAPI_(void) NativeOnChunkDecoded(
        /* [in] */ HANDLE nativeWebAudioMediaCodecBridge,
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Int32 size,
        /* [in] */ Int32 inputChannelCount,
        /* [in] */ Int32 outputChannelCount);

    static CARAPI_(void) NativeInitializeDestination(
        /* [in] */ HANDLE nativeWebAudioMediaCodecBridge,
        /* [in] */ Int32 inputChannelCount,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int64 durationMicroseconds);
//callback function declaration
public:
    static CARAPI_(void*) ElaWebAudioMediaCodecBridgeCallback_Init();

private:
    static CARAPI_(String) CreateTempFile(
        /* [in] */ IInterface* ctx);

    static CARAPI_(Boolean) DecodeAudioFile(
        /* [in] */ IInterface* ctx,
        /* [in] */ HANDLE nativeMediaCodecBridge,
        /* [in] */ Int32 inputFD,
        /* [in] */ Int64 dataSize);
};

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_WEBAUDIOMEDIACODECBRIDGE_H__
