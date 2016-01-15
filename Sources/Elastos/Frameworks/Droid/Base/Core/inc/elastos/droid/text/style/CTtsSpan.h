
#ifndef __ELASTOS_DROID_TEXT_STYLE_CTTSSPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_CTTSSPAN_H__

#include "_Elastos_Droid_Text_Style_CTtsSpan.h"
#include "elastos/droid/text/style/TtsSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CarClass(CTtsSpan)
    , public TtsSpan
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_CTTSSPAN_H__
