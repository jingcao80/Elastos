#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWPICTURE_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWPICTURE_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/common/CleanupReference.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPicture;
using Elastos::Droid::Webkit::Content::Common::CleanupReference;
using Elastos::Core::IRunnable;
using Elastos::IO::IOutputStream;


// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

// A simple wrapper around a SkPicture, that allows final rendering to be performed using the
// chromium skia library.
//@JNINamespace("android_webview")
class AwPicture
    : public Object
    , public IPicture
{
public:
    CAR_INTERFACE_DECL();
private:
    // There is no explicit destroy method on Picture base-class, so cleanup is always
    // handled via the CleanupReference.
    class DestroyRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();
        //@Override
        CARAPI Run();

    public:
        DestroyRunnable(
            /* [in] */ Int64 nativeAwPicture);

        Int64 mNativeAwPicture;
    };

public:
    /**
     * @param nativeAwPicture is an instance of the AwPicture native class. Ownership is
     *                        taken by this java instance.
     */
    AwPicture(
        /* [in] */ Int64 nativeAwPicture);

    //@Override
    CARAPI BeginRecording(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ ICanvas** canvas);

    //@Override
    CARAPI EndRecording();

    //@Override
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //override
    CARAPI GetNativePicture(
        /* [out] */ Handle32* handle);
    //@Override
    //@SuppressWarnings("deprecation")
    CARAPI WriteToStream(
        /* [in] */ IOutputStream* stream);

private:
    CARAPI_(void) UnsupportedOperation();

    static CARAPI_(void) NativeDestroy(
        /* [in] */ Int64 nativeAwPicture);

    CARAPI_(Int32) NativeGetWidth(
        /* [in] */ Int64 nativeAwPicture);

    CARAPI_(Int32) NativeGetHeight(
        /* [in] */ Int64 nativeAwPicture);

    CARAPI_(void) NativeDraw(
        /* [in] */ Int64 nativeAwPicture, ICanvas* canvas);

private:
    Int64 mNativeAwPicture;
    AutoPtr<CleanupReference> mCleanupReference;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWPICTURE_H__
