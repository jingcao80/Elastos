
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/TextAttribute.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                            TextAttribute
//=====================================================================
CAR_INTERFACE_IMPL(TextAttribute, Object, ITextAttribute);

TextAttribute::TextAttribute()
{
}

ECode TextAttribute::constructor(
    /* [in] */ Int32 start,
    /* [in] */ Int32 length,
    /* [in] */ TextAlignment align,
    /* [in] */ FontSize size,
    /* [in] */ Boolean bold,
    /* [in] */ Boolean italic,
    /* [in] */ Boolean underlined,
    /* [in] */ Boolean strikeThrough,
    /* [in] */ TextColor color)
{
    // ==================before translated======================
    // this.start = start;
    // this.length = length;
    // this.align = align;
    // this.size = size;
    // this.bold = bold;
    // this.italic = italic;
    // this.underlined = underlined;
    // this.strikeThrough = strikeThrough;
    // this.color = color;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
