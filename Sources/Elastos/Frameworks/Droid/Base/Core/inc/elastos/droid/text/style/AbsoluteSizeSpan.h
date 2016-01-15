#ifndef __ELASTOS_DROID_TEXT_STYLE_AbsoluteSizeSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_AbsoluteSizeSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/text/style/MetricAffectingSpan.h"

using Elastos::Droid::Text::ITextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class AbsoluteSizeSpan
    : public MetricAffectingSpan
    , public IAbsoluteSizeSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    AbsoluteSizeSpan();

    virtual ~AbsoluteSizeSpan();

    CARAPI constructor();

    /**
     * Set the text size to <code>size</code> physical pixels.
     */
    CARAPI constructor(
        /* [in] */ Int32 size);

    /**
     * Set the text size to <code>size</code> physical pixels,
     * or to <code>size</code> device-independent pixels if
     * <code>dip</code> is true.
     */
    CARAPI constructor(
        /* [in] */ Int32 size,
        /* [in] */ Boolean dip);


    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI GetDip(
        /* [out] */ Boolean* dip);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* ds);

private:
    /*const*/ Int32 mSize;
    Boolean mDip;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_AbsoluteSizeSpan_H__
