
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_CANONICALAXISINDEX_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_CANONICALAXISINDEX_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

class CanonicalAxisIndex
{
public:
    static const Int32 AXIS_LEFT_STICK_X = 0;
    static const Int32 AXIS_LEFT_STICK_Y = 1;
    static const Int32 AXIS_RIGHT_STICK_X = 2;
    static const Int32 AXIS_RIGHT_STICK_Y = 3;
    static const Int32 NUM_CANONICAL_AXES = 4;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_CANONICALAXISINDEX_H__
