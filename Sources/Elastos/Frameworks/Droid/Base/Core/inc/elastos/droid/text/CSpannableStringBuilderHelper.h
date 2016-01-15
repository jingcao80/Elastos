
#ifndef __ELASTOS_DROID_TEXT_CSPANNABLESTRINGBUILDER_HELPER_H__
#define __ELASTOS_DROID_TEXT_CSPANNABLESTRINGBUILDER_HELPER_H__

#include "_Elastos_Droid_Text_CSpannableStringBuilderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CSpannableStringBuilderHelper)
    , public Singleton
    , public ISpannableStringBuilderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ValueOf(
        /* [in] */ ICharSequence* source,
        /* [out] */ ISpannableStringBuilder** builder);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CSPANNABLESTRINGBUILDER_HELPER_H__
