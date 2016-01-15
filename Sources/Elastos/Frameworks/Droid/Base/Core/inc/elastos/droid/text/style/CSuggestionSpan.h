
#ifndef __ELASTOS_DROID_TEXT_STYLE_CSUGGESTIONSPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_CSUGGESTIONSPAN_H__

#include "_Elastos_Droid_Text_Style_CSuggestionSpan.h"
#include "elastos/droid/text/style/SuggestionSpan.h"


namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CarClass(CSuggestionSpan)
    , public SuggestionSpan
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_CSUGGESTIONSPAN_H__
