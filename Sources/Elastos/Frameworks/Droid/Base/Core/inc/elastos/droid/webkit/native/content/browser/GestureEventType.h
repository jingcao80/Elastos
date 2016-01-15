
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_GESTUREEVENTTYPE_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_GESTUREEVENTTYPE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class GestureEventType
{
public:
    static const Int32 SHOW_PRESS = 0;
    static const Int32 DOUBLE_TAP = 1;
    static const Int32 SINGLE_TAP_UP = 2;
    static const Int32 SINGLE_TAP_CONFIRMED = 3;
    static const Int32 SINGLE_TAP_UNCONFIRMED = 4;
    static const Int32 LONG_PRESS = 5;
    static const Int32 SCROLL_START = 6;
    static const Int32 SCROLL_BY = 7;
    static const Int32 SCROLL_END = 8;
    static const Int32 FLING_START = 9;
    static const Int32 FLING_CANCEL = 10;
    static const Int32 FLING_END = 11;
    static const Int32 PINCH_BEGIN = 12;
    static const Int32 PINCH_BY = 13;
    static const Int32 PINCH_END = 14;
    static const Int32 TAP_CANCEL = 15;
    static const Int32 LONG_TAP = 16;
    static const Int32 TAP_DOWN = 17;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_GESTUREEVENTTYPE_H__
