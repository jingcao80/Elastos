#ifndef __ELASTOS_DROID_TEXT_METHOD_CPASSWORDTRANSFORMATIONMETHODHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CPASSWORDTRANSFORMATIONMETHODHELPER_H__

#include "_Elastos_Droid_Text_Method_CPasswordTransformationMethodHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CPasswordTransformationMethodHelper)
    , public Singleton
    , public IPasswordTransformationMethodHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IPasswordTransformationMethod** ret);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CPASSWORDTRANSFORMATIONMETHODHELPER_H__
