
#ifndef __ELASTOS_DROID_TEXT_METHOD_CTIMEKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CTIMEKEYLISTENER_H__

#include "_Elastos_Droid_Text_Method_CTimeKeyListener.h"
#include "elastos/droid/text/method/TimeKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CTimeKeyListener)
    , public TimeKeyListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CTIMEKEYLISTENER_H__
