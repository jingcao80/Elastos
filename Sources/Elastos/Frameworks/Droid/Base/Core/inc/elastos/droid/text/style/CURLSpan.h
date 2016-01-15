
#ifndef __ELASTOS_DROID_TEXT_STYLE_CURLSPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_CURLSPAN_H__

#include "_Elastos_Droid_Text_Style_CURLSpan.h"
#include "elastos/droid/text/style/URLSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CarClass(CURLSpan)
    , public URLSpan
{
public:
    CAR_OBJECT_DECL()
};


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_CURLSPAN_H__
