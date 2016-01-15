#ifndef __ELASTOS_DROID_TEXT_METHOD_CHIDERETURNSTRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_CHIDERETURNSTRANSFORMATIONMETHOD_H__

#include "_Elastos_Droid_Text_Method_CHideReturnsTransformationMethod.h"
#include "elastos/droid/text/method/HideReturnsTransformationMethod.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::Graphics::IRect;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CHideReturnsTransformationMethod)
    , public HideReturnsTransformationMethod
{
public:
    CAR_OBJECT_DECL()
};

} //namespace Method
} //namespace Text
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CHIDERETURNSTRANSFORMATIONMETHOD_H__
