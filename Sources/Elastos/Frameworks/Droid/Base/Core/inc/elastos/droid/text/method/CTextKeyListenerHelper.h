
#ifndef __ELASTOS_DROID_TEXT_METHOD_CTEXTKEYLISTENERHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CTEXTKEYLISTENERHELPER_H__

#include "_Elastos_Droid_Text_Method_CTextKeyListenerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CTextKeyListenerHelper)
    , public Singleton
    , public ITextKeyListenerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Clear(
        /* [in] */ IEditable* e);

    CARAPI GetInstance(
        /* [out] */ ITextKeyListener** ret);

    CARAPI GetInstance(
        /* [in] */ Boolean autotext,
        /* [in] */ Capitalize cap,
        /* [out] */ ITextKeyListener** ret);

    CARAPI ShouldCap(
        /* [in] */ Capitalize cap,
        /* [in] */ ICharSequence* cs,
        /* [in] */ Int32 off,
        /* [out] */ Boolean* ret);
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CTEXTKEYLISTENERHELPER_H__
