
#ifndef __ELASTOS_DROID_TEXT_STYLE_CQUOTESPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_CQUOTESPAN_H__

#include "_Elastos_Droid_Text_Style_CQuoteSpan.h"
#include "elastos/droid/text/style/QuoteSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CarClass(CQuoteSpan)
    , public QuoteSpan
{
public:
    CAR_OBJECT_DECL()
};


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_CQUOTESPAN_H__
