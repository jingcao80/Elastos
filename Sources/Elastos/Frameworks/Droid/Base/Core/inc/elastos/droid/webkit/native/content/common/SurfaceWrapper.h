
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_COMMON_SURFACEWRAPPER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_COMMON_SURFACEWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// TODO using Elastos::Droid::Os::IParcel;
// TODO using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::View::ISurface;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Common {

/**
 * A wrapper for marshalling a Surface without self-destruction.
 */
class SurfaceWrapper
    : public Object
    // TODO , public IParcelable
{
public:
    SurfaceWrapper(
        /* [in] */ ISurface* surface);

    AutoPtr<ISurface> GetSurface();

    //@Override
    // TODO
    // CARAPI WriteToParcel(
    //     /* [in] */ IParcel* out,
    //     /* [in] */ Int32 flags);

public:
    // TODO static final Parcelable.Creator<SurfaceWrapper> CREATOR;

private:
    AutoPtr<ISurface> mSurface;
};

} // namespace Common
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_COMMON_SURFACEWRAPPER_H__
