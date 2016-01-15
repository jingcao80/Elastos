#ifndef __ELASTOS_DROID_TEXT_STYLE_BulletSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_BulletSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class BulletSpan
    : public Object
    , public IBulletSpan
    , public ILeadingMarginSpan
    , public IParagraphStyle
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    BulletSpan();

    virtual ~BulletSpan();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 gapWidth);

    CARAPI constructor(
        /* [in] */ Int32 gapWidth,
        /* [in] */ Int32 color);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

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
        /* [in] */ ILayout* l);

private:
    Int32 mGapWidth;
    Boolean mWantColor;
    Int32 mColor;

    static const Int32 BULLET_RADIUS;// = 3;
    static AutoPtr<IPath> sBulletPath;// = NULL;
};


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_BulletSpan_H__
