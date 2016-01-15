#ifndef __ELASTOS_DROID_TEXT_STYLE_DrawableMarginSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_DrawableMarginSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class DrawableMarginSpan
    : public Object
    , public IDrawableMarginSpan
    , public ILineHeightSpan
    , public IParagraphStyle
    , public IWrapTogetherSpan
    , public ILeadingMarginSpan
{
public:
    CAR_INTERFACE_DECL()

    DrawableMarginSpan();

    virtual ~DrawableMarginSpan();

    CARAPI constructor(
        /* [in] */ IDrawable* b);

    CARAPI constructor(
        /* [in] */ IDrawable* b,
        /* [in] */ Int32 pad);

    CARAPI GetLeadingMargin(
        /* [in] */ Boolean first,
        /* [out] */ Int32* id);

    CARAPI DrawLeadingMargin(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p,
        /* [in] */ Int32 x,
        /* [in] */ Int32 dir,
        /* [in] */ Int32 top,
        /* [in] */ Int32 baseline,
        /* [in] */ Int32 bottom,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Boolean first,
        /* [in] */ ILayout* layout);

    CARAPI ChooseHeight(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 istartv,
        /* [in] */ Int32 v,
        /* [in] */ IPaintFontMetricsInt* fm);

private:
    AutoPtr<IDrawable> mDrawable;// = NULL;
    Int32 mPad;// = 0;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_DrawableMarginSpan_H__
