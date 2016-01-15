#ifndef __ELASTOS_DROID_TEXT_STYLE_RasterizerSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_RasterizerSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

using Elastos::Droid::Graphics::IRasterizer;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class RasterizerSpan
    : public CharacterStyle
    , public IRasterizerSpan
    , public IUpdateAppearance
{
public:
    CAR_INTERFACE_DECL()

    RasterizerSpan();

    virtual ~RasterizerSpan();

    CARAPI constructor(
        /* [in] */ IRasterizer* r);

    CARAPI GetRasterizer(
        /* [out] */ IRasterizer** rastersizer);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

private:
    AutoPtr<IRasterizer> mRasterizer;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_RasterizerSpan_H__
