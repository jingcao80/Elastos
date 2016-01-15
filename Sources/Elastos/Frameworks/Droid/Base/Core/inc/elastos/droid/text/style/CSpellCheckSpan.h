
#ifndef __ELASTOS_DROID_TEXT_STYLE_CSPELLCHECKSPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_CSPELLCHECKSPAN_H__

#include "_Elastos_Droid_Text_Style_CSpellCheckSpan.h"
#include "elastos/droid/text/style/SpellCheckSpan.h"


namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CarClass(CSpellCheckSpan)
    , public SpellCheckSpan
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_CSPELLCHECKSPAN_H__
