
#ifndef __ELASTOS_DROID_TEXT_CSPANNABLEFACTORY_H__
#define __ELASTOS_DROID_TEXT_CSPANNABLEFACTORY_H__

#include "_Elastos_Droid_Text_CSpannableFactory.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CSpannableFactory)
    , public Singleton
    , public ISpannableFactory
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewSpannable(
        /* [in] */ ICharSequence* source,
        /* [out] */ ISpannable** spannable);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CSPANNABLEFACTORY_H__
