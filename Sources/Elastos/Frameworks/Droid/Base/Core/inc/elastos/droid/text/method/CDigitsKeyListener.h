#ifndef __ELASTOS_DROID_TEXT_METHOD_CDIGITSKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CDIGITSKEYLISTENER_H__

#include "_Elastos_Droid_Text_Method_CDigitsKeyListener.h"
#include "elastos/droid/text/method/DigitsKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CDigitsKeyListener)
    , public DigitsKeyListener
{
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CDIGITSKEYLISTENER_H__
