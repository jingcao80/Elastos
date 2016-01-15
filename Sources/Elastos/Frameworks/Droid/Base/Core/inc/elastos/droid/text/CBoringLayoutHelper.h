
#ifndef __ELASTOS_DROID_TEXT_CBORINGLAYOUTHELPER_H__
#define __ELASTOS_DROID_TEXT_CBORINGLAYOUTHELPER_H__

#include "_Elastos_Droid_Text_CBoringLayoutHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBoringLayoutHelper)
    , public Singleton
    , public IBoringLayoutHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Make(
        /* [in] */ ICharSequence* source,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 outerwidth,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ IBoringLayoutMetrics* metrics,
        /* [in] */ Boolean includepad,
        /* [out] */ IBoringLayout** ret);

    CARAPI Make(
        /* [in] */ ICharSequence* source,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 outerwidth,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ IBoringLayoutMetrics* metrics,
        /* [in] */ Boolean includepad,
        /* [in] */ TextUtilsTruncateAt ellipsize,
        /* [in] */ Int32 ellipsizedWidth,
        /* [out] */ IBoringLayout** ret);

    CARAPI IsBoring(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* paint,
        /* [out] */ IBoringLayoutMetrics** ret);

    CARAPI IsBoring(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* paint,
        /* [in] */ ITextDirectionHeuristic* textDir,
        /* [out] */ IBoringLayoutMetrics** ret);

    CARAPI IsBoring(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* paint,
        /* [in] */ IBoringLayoutMetrics* metrics,
        /* [out] */ IBoringLayoutMetrics** ret);

    CARAPI IsBoring(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* paint,
        /* [in] */ ITextDirectionHeuristic* textDir,
        /* [in] */ IBoringLayoutMetrics* metrics,
        /* [out] */ IBoringLayoutMetrics** ret);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_TEXT_CBORINGLAYOUTHELPER_H__
