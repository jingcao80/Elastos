
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_COMMON_RESULTCODES_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_COMMON_RESULTCODES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Common {

class ResultCodes
{
public:
    static const Int32 RESULT_CODE_NORMAL_EXIT = 0;
    static const Int32 RESULT_CODE_KILLED = 1;
    static const Int32 RESULT_CODE_HUNG = 2;
    static const Int32 RESULT_CODE_KILLED_BAD_MESSAGE = 3;
};

} // namespace Common
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_COMMON_RESULTCODES_H__
