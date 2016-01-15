#ifndef __ELASTOS_DROID_TEXT_STYLE_MaskFilterSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_MaskFilterSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

using Elastos::Droid::Graphics::IMaskFilter;
using Elastos::Droid::Text::ITextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class MaskFilterSpan
    : public CharacterStyle
    , public IMaskFilterSpan
    , public IUpdateAppearance
{
public:
    CAR_INTERFACE_DECL()

    MaskFilterSpan();

    virtual ~MaskFilterSpan();

    CARAPI constructor(
        /* [in] */ IMaskFilter* filter);

    CARAPI GetMaskFilter(
        /* [out] */ IMaskFilter** filter);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

private:
    AutoPtr<IMaskFilter> mFilter;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_MaskFilterSpan_H__
