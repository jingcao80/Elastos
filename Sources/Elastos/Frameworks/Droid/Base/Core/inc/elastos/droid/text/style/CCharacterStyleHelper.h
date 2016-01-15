#ifndef __ELASTOS_DROID_TEXT_STYLE_CCHARACTERSTYLEHELPER_H__
#define __ELASTOS_DROID_TEXT_STYLE_CCHARACTERSTYLEHELPER_H__

#include "_Elastos_Droid_Text_Style_CCharacterStyleHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CarClass(CCharacterStyleHelper)
    , public Singleton
    , public ICharacterStyleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Wrap(
        /* [in] */ ICharacterStyle* cs,
        /* [out] */ ICharacterStyle** style);
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_STYLE_CCHARACTERSTYLEHELPER_H__