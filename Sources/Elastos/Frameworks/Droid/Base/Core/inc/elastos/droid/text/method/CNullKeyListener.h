
#ifndef __ELASTOS_DROID_TEXT_METHOD_CNULLKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CNULLKEYLISTENER_H__

#include "_Elastos_Droid_Text_Method_CNullKeyListener.h"
#include "elastos/droid/text/method/TextKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CNullKeyListener)
    , public TextKeyListener::NullKeyListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CNULLKEYLISTENER_H__
