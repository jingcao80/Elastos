#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTATTRIBUTE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTATTRIBUTE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for representing text attributes for SIM Toolkit.
  *
  * {@hide}
  */
class TextAttribute
    : public Object
    , public ITextAttribute
{
public:
    CAR_INTERFACE_DECL();

    TextAttribute(
        /* [in] */ Int32 start,
        /* [in] */ Int32 length,
        /* [in] */ TextAlignment align,
        /* [in] */ FontSize size,
        /* [in] */ Boolean bold,
        /* [in] */ Boolean italic,
        /* [in] */ Boolean underlined,
        /* [in] */ Boolean strikeThrough,
        /* [in] */ TextColor color);

public:
    Int32 start;
    Int32 length;
    TextAlignment align;
    FontSize size;
    Boolean bold;
    Boolean italic;
    Boolean underlined;
    Boolean strikeThrough;
    TextColor color;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTATTRIBUTE_H__

