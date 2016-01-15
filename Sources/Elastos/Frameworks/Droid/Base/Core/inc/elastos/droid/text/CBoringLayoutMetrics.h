
#ifndef __ELASTOS_DROID_TEXT_CBORINGLAYOUTMETRICS_H__
#define __ELASTOS_DROID_TEXT_CBORINGLAYOUTMETRICS_H__

#include "_Elastos_Droid_Text_CBoringLayoutMetrics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPaintFontMetricsInt;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBoringLayoutMetrics)
    , public Object
    , public IBoringLayoutMetrics
    , public IPaintFontMetricsInt
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBoringLayoutMetrics();

    CARAPI constructor();

    CARAPI GetTop(
        /* [out] */ Int32* top);

    CARAPI SetTop(
        /* [in] */ Int32 top);

    CARAPI GetAscent(
        /* [out] */ Int32* ascent);

    CARAPI SetAscent(
        /* [in] */ Int32 ascent);

    CARAPI GetDescent(
        /* [out] */ Int32* descent);

    CARAPI SetDescent(
        /* [in] */ Int32 descent);

    CARAPI GetBottom(
        /* [out] */ Int32* bottom);

    CARAPI SetBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetLeading(
        /* [out] */ Int32* leading);

    CARAPI SetLeading(
        /* [in] */ Int32 leading);

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI SetWidth(
        /* [in] */ Int32 width);

private:
    Int32 mTop;
    Int32 mAscent;
    Int32 mDescent;
    Int32 mBottom;
    Int32 mLeading;
    Int32 mWidth;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CBORINGLAYOUTMETRICS_H__
