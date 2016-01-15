
#ifndef __ELASTOS_DROID_WEBKIT_MUSTOVERRIDEEXCEPTION_H__
#define __ELASTOS_DROID_WEBKIT_MUSTOVERRIDEEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

// TODO: Remove MustOverrideException and make all methods throwing it abstract instead;
// needs API file update.
class MustOverrideException : public RuntimeException
{
    MustOverrideException() : RuntimeException("abstract function called: must be overriden!") {
    }
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_MUSTOVERRIDEEXCEPTION_H__
