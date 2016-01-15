
#ifndef __ELASTOS_DROID_TEXT_METHOD_CQWERTYKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CQWERTYKEYLISTENER_H__

#include "_Elastos_Droid_Text_Method_CQwertyKeyListener.h"
#include "elastos/droid/text/method/QwertyKeyListener.h"



namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CQwertyKeyListener)
    , public QwertyKeyListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CQWERTYKEYLISTENER_H__
