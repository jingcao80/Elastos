
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_SURFACEWRAPPER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_SURFACEWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::ISurface;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Common {

/**
 * A wrapper for marshalling a Surface without self-destruction.
 */
class SurfaceWrapper
    : public Object
    , public IParcelable
{
public:
    SurfaceWrapper(
        /* [in] */ ISurface* surface);

    AutoPtr<ISurface> GetSurface();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<ISurface> mSurface;
};

} // namespace Common
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_SURFACEWRAPPER_H__
