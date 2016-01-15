
#ifndef __ELASTOS_DROID_TEXT_METHOD_CDATEKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CDATEKEYLISTENER_H__

#include "_Elastos_Droid_Text_Method_CDateKeyListener.h"
#include "elastos/droid/text/method/DateKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CDateKeyListener)
    , public DateKeyListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CDATEKEYLISTENER_H__
