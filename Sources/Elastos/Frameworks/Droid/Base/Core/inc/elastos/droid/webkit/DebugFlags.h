
#ifndef __ELASTOS_DROID_WEBKIT_DEBUGFLAGS_H__
#define __ELASTOS_DROID_WEBKIT_DEBUGFLAGS_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * This class is a container for all of the debug flags used in the Java
 * components of webkit.  These flags must be final in order to ensure that
 * the compiler optimizes the code that uses them out of the final executable.
 *
 * The name of each flags maps directly to the name of the class in which that
 * flag is used.
 *
 * @hide Only used by WebView implementations.
 */
class DebugFlags
{
public:
    static const Boolean COOKIE_SYNC_MANAGER = FALSE;
    static const Boolean TRACE_API = FALSE;
    static const Boolean TRACE_CALLBACK;
    static const Boolean URL_UTIL;
    static const Boolean WEB_SYNC_MANAGER;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_DEBUGFLAGS_H__
