
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CBITMAPHELPER_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CBITMAPHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

class BitmapHelper
{
public:
    AutoPtr<IBitmap> CreateCircularClip(
        /* [in] */ IBitmap* input,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);
};

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_CBITMAPHELPER_H__
