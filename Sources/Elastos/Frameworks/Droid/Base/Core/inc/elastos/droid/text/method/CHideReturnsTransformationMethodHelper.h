#ifndef __ELASTOS_DROID_TEXT_METHOD_CHIDERETURNSTRANSFORMATIONMETHODHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CHIDERETURNSTRANSFORMATIONMETHODHELPER_H__

#include "_Elastos_Droid_Text_Method_CHideReturnsTransformationMethodHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CHideReturnsTransformationMethodHelper)
    , public Singleton
    , public IHideReturnsTransformationMethodHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IHideReturnsTransformationMethod** ret);
};

} //namespace Method
} //namespace Text
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CHIDERETURNSTRANSFORMATIONMETHODHELPER_H__
