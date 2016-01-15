
#ifndef __ELASTOS_DROID_TEXT_METHOD_CTEXTKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CTEXTKEYLISTENER_H__

#include "_Elastos_Droid_Text_Method_CTextKeyListener.h"
#include "elastos/droid/text/method/TextKeyListener.h"



namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CTextKeyListener)
    , public TextKeyListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CTEXTKEYLISTENER_H__
