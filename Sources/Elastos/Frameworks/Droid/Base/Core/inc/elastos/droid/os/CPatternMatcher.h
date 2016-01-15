#ifndef __ELASTOS_DROID_OS_CPATTERNMATCHER_H__
#define __ELASTOS_DROID_OS_CPATTERNMATCHER_H__

#include "_Elastos_Droid_Os_CPatternMatcher.h"
#include "elastos/droid/os/PatternMatcher.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CPatternMatcher)
    , public PatternMatcher
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CPATTERNMATCHER_H__
