#ifndef __ELASTOS_DROID_TEXT_METHOD_CSCROLLINGMOVEMENTMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_CSCROLLINGMOVEMENTMETHOD_H__

#include "_Elastos_Droid_Text_Method_CScrollingMovementMethod.h"
#include "elastos/droid/text/method/ScrollingMovementMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CScrollingMovementMethod)
    , public ScrollingMovementMethod
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CSCROLLINGMOVEMENTMETHOD_H__
