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
        /* [in] */ Handle64 nativeMediaCodecBridge,
        /* [in] */ Int32 inputFD,
        /* [in] */ Int64 dataSize);

    static CARAPI_(void) NativeOnChunkDecoded(
        /* [in] */ Handle64 nativeWebAudioMediaCodecBridge,
        /* [in] */ IByteBuffer* buf,
        /* [in] */ Int32 size,
        /* [in] */ Int32 inputChannelCount,
        /* [in] */ Int32 outputChannelCount);

    static CARAPI_(void) NativeInitializeDestination(
        /* [in] */ Handle64 nativeWebAudioMediaCodecBridge,
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
        /* [in] */ Handle64 nativeMediaCodecBridge,
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
