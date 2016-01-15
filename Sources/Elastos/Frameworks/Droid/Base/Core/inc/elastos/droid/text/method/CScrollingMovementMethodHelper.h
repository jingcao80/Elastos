
#ifndef __ELASTOS_DROID_TEXT_METHOD_CSCROLLINGMOVEMENTMETHODHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CSCROLLINGMOVEMENTMETHODHELPER_H__

#include "_Elastos_Droid_Text_Method_CScrollingMovementMethodHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CScrollingMovementMethodHelper)
    , public Singleton
    , public IScrollingMovementMethodHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IMovementMethod** ret);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CSCROLLINGMOVEMENTMETHODHELPER_H__
