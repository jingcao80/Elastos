
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/SurfaceWrapper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Common {

SurfaceWrapper::SurfaceWrapper(
    /* [in] */ ISurface* surface)
    : mSurface(surface)
{
}

AutoPtr<ISurface> SurfaceWrapper::GetSurface()
{
    return mSurface;
}

// //@Override
ECode SurfaceWrapper::WriteToParcel(
    /* [in] */ IParcel* out)
{
    // Ignore flags so that the Surface won't call release()
    return IParcelable::Probe(mSurface)->WriteToParcel(out);
}

ECode SurfaceWrapper::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return IParcelable::Probe(mSurface)->ReadFromParcel(source);
}

} // namespace Common
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
