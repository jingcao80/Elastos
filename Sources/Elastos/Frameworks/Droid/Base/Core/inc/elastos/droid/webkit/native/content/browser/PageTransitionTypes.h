
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_PAGETRANSITIONTYPES_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_PAGETRANSITIONTYPES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class PageTransitionTypes
{
public:
    static const Int32 PAGE_TRANSITION_LINK = 0;
    static const Int32 PAGE_TRANSITION_TYPED = 1;
    static const Int32 PAGE_TRANSITION_AUTO_BOOKMARK = 2;
    static const Int32 PAGE_TRANSITION_AUTO_SUBFRAME = 3;
    static const Int32 PAGE_TRANSITION_MANUAL_SUBFRAME = 4;
    static const Int32 PAGE_TRANSITION_GENERATED = 5;
    static const Int32 PAGE_TRANSITION_AUTO_TOPLEVEL = 6;
    static const Int32 PAGE_TRANSITION_FORM_SUBMIT = 7;
    static const Int32 PAGE_TRANSITION_RELOAD = 8;
    static const Int32 PAGE_TRANSITION_KEYWORD = 9;
    static const Int32 PAGE_TRANSITION_KEYWORD_GENERATED = 10;
    static const Int32 PAGE_TRANSITION_LAST_CORE = PAGE_TRANSITION_KEYWORD_GENERATED;
    static const Int32 PAGE_TRANSITION_CORE_MASK = 0xFF;
    static const Int32 PAGE_TRANSITION_BLOCKED = 0x00800000;
    static const Int32 PAGE_TRANSITION_FORWARD_BACK = 0x01000000;
    static const Int32 PAGE_TRANSITION_FROM_ADDRESS_BAR = 0x02000000;
    static const Int32 PAGE_TRANSITION_HOME_PAGE = 0x04000000;
    static const Int32 PAGE_TRANSITION_FROM_API = 0x08000000;
    static const Int32 PAGE_TRANSITION_CHAIN_START = 0x10000000;
    static const Int32 PAGE_TRANSITION_CHAIN_END = 0x20000000;
    static const Int32 PAGE_TRANSITION_CLIENT_REDIRECT = 0x40000000;
    static const Int32 PAGE_TRANSITION_SERVER_REDIRECT = 0x80000000;
    static const Int32 PAGE_TRANSITION_IS_REDIRECT_MASK = 0xC0000000;
    static const Int32 PAGE_TRANSITION_QUALIFIER_MASK = 0xFFFFFF00;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_PAGETRANSITIONTYPES_H__
