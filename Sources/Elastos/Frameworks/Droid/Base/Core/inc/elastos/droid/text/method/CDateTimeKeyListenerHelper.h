
#ifndef __ELASTOS_DROID_TEXT_METHOD_CDATETIMEKEYLISTENERHELPER_H__
#define __ELASTOS_DROID_TEXT_METHOD_CDATETIMEKEYLISTENERHELPER_H__

#include "_Elastos_Droid_Text_Method_CDateTimeKeyListenerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CarClass(CDateTimeKeyListenerHelper)
    , public Singleton
    , public IDateTimeKeyListenerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCHARACTERS(
        /* [out, callee] */ ArrayOf<Char32>** ret);

    CARAPI GetInstance(
        /* [out] */ IDateTimeKeyListener** ret);

};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_CDATETIMEKEYLISTENERHELPER_H__
