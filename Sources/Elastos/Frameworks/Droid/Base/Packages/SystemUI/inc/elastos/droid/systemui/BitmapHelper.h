
#ifndef  __ELASTOS_DROID_SYSTEMUI_CBITMAPHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CBITMAPHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class BitmapHelper
{
public:
    static AutoPtr<IBitmap> CreateCircularClip(
        /* [in] */ IBitmap* input,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CBITMAPHELPER_H__
