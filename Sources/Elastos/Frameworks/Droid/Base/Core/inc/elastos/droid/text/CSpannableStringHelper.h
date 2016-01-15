#ifndef __ELASTOS_DROID_TEXT_CSPANNABLE_STRING_HELPER_H__
#define __ELASTOS_DROID_TEXT_CSPANNABLE_STRING_HELPER_H__

#include "_Elastos_Droid_Text_CSpannableStringHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CSpannableStringHelper)
    , public Singleton
    , public ISpannableStringHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ValueOf(
        /* [in] */ ICharSequence* source,
        /* [out] */ ISpannableString** spannedString);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CSPANNABLE_STRING_HELPER_H__

