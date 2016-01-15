#ifndef __ELASTOS_DROID_TEXT_METHOD_CDIALERKEYLISTENERHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CDIALERKEYLISTENERHELPER_H__

#include "_Elastos_Droid_Text_Method_CDialerKeyListenerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CDialerKeyListenerHelper)
    , public Singleton
    , public IDialerKeyListenerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCHARACTERS(
        /* [out, callee] */ ArrayOf<Char32>** ret);

    CARAPI GetInstance(
        /* [out] */ IDialerKeyListener** ret);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CDIALERKEYLISTENERHELPER_H__
