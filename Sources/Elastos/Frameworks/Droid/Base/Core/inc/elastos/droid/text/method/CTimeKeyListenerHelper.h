
#ifndef __ELASTOS_DROID_TEXT_METHOD_CTIMEKEYLISTENERHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CTIMEKEYLISTENERHELPER_H__

#include "_Elastos_Droid_Text_Method_CTimeKeyListenerHelper.h"
#include "elastos/droid/text/method/TimeKeyListener.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CTimeKeyListenerHelper)
    , public Singleton
    , public ITimeKeyListenerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ ITimeKeyListener** ret);

    CARAPI GetCHARACTERS(
        /* [out, callee] */ ArrayOf<Char32>** ret);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CTIMEKEYLISTENERHELPER_H__
