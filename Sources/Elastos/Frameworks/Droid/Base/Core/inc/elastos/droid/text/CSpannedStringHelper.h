#ifndef __ELASTOS_DROID_TEXT_CSPANNEDSTRINGHELPER_H__
#define __ELASTOS_DROID_TEXT_CSPANNEDSTRINGHELPER_H__

#include "_Elastos_Droid_Text_CSpannedStringHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CSpannedStringHelper)
    , public Singleton
    , public ISpannedStringHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ValueOf(
        /* [in] */ ICharSequence* source,
        /* [out] */ ISpannedString** spannedString);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CSPANNEDSTRINGHELPER_H__

