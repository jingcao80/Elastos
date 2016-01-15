
#ifndef _ELASTOS_DROID_WEBKIT_UI_GFX_BITMAPFORMAT_H_
#define _ELASTOS_DROID_WEBKIT_UI_GFX_BITMAPFORMAT_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.gfx;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Gfx {

class BitmapFormat
    : public Object
{
public:
    static const Int32 FORMAT_NO_CONFIG = 0;
    static const Int32 FORMAT_ALPHA_8 = 1;
    static const Int32 FORMAT_ARGB_4444 = 2;
    static const Int32 FORMAT_ARGB_8888 = 3;
    static const Int32 FORMAT_RGB_565 = 4;
};

} // namespace Gfx
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_GFX_BITMAPFORMAT_H_

