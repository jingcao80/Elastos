
#ifndef __ELASTOS_DROID_TEXT_METHOD_CSINGLELINETRANSFORMATIONMETHODHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CSINGLELINETRANSFORMATIONMETHODHELPER_H__

#include "_Elastos_Droid_Text_Method_CSingleLineTransformationMethodHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CSingleLineTransformationMethodHelper)
    , public Singleton
    , public ISingleLineTransformationMethodHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ ISingleLineTransformationMethod** ret);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CSINGLELINETRANSFORMATIONMETHODHELPER_H__
