
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_COMMON_SCREENORIENTATIONVALUES_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_COMMON_SCREENORIENTATIONVALUES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Common {

class ScreenOrientationValues
{
public:
    static const Int32 DEFAULT = 0;
    static const Int32 PORTRAIT_PRIMARY = 1;
    static const Int32 PORTRAIT_SECONDARY = 2;
    static const Int32 LANDSCAPE_PRIMARY = 3;
    static const Int32 LANDSCAPE_SECONDARY = 4;
    static const Int32 ANY = 5;
    static const Int32 LANDSCAPE = 6;
    static const Int32 PORTRAIT = 7;
};

} // namespace Common
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_COMMON_SCREENORIENTATIONVALUES_H__