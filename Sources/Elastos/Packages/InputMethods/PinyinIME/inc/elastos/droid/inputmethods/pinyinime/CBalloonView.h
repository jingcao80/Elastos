
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CBALLOONVIEW_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CBALLOONVIEW_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CBalloonView.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

CarClass(CBalloonView)
    , public Elastos::Droid::View::View
    , public IBalloonView
{
public:
    CBalloonView();

    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI SetTextConfig(
        /* [in] */ const String& label,
        /* [in] */ Float fontSize,
        /* [in] */ Boolean textBold,
        /* [in] */ Int32 textColor);

protected:
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(String) GetLimitedLabelForDrawing(
        /* [in] */ const String& rawLabel,
        /* [in] */ Float widthToDraw);

private:
    /**
     * Suspension points used to display long items.
     */
    static String SUSPENSION_POINTS;

    /**
     * The icon to be shown. If it is not NULL, {@link #mLabel} will be
     * ignored.
     */
    AutoPtr<IDrawable> mIcon;

    /**
     * The label to be shown. It is enabled only if {@link #mIcon} is NULL.
     */
    String mLabel;

    Int32 mLabeColor;
    AutoPtr<IPaint> mPaintLabel;
    AutoPtr<IPaintFontMetricsInt> mFmi;

    /**
     * The width to show suspension points.
     */
    Float mSuspensionPointsWidth;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODS_PINYINIME_CBALLOONVIEW_H__
