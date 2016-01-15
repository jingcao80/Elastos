
#ifndef __ELASTOS_DROID_TEXT_METHOD_CDIGITSKEYLISTENERHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CDIGITSKEYLISTENERHELPER_H__

#include "_Elastos_Droid_Text_Method_CDigitsKeyListenerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CDigitsKeyListenerHelper)
    , public IDigitsKeyListenerHelper
    , public Singleton
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IDigitsKeyListener** ret);

    CARAPI GetInstance(
        /* [in] */ Boolean sign,
        /* [in] */ Boolean decimal,
        /* [out] */ IDigitsKeyListener** ret);

    CARAPI GetInstance(
        /* [in] */ const String& accepted,
        /* [out] */ IDigitsKeyListener** ret);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CDIGITSKEYLISTENERHELPER_H__
