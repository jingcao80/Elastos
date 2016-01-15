
#ifndef __ELASTOS_DROID_TEXT_METHOD_CMULTITAPKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CMULTITAPKEYLISTENER_H__

#include "_Elastos_Droid_Text_Method_CMultiTapKeyListener.h"
#include "elastos/droid/text/method/MultiTapKeyListener.h"

using Elastos::Droid::Text::Method::Capitalize;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CMultiTapKeyListener)
    , public MultiTapKeyListener
{
    CAR_OBJECT_DECL()
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CMULTITAPKEYLISTENER_H__
