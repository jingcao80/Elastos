
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CNUMPADKEY_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CNUMPADKEY_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CNumPadKey.h"
#include <elastos/droid/view/ViewGroup.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CNumPadKey)
    , public ViewGroup
{
private:
    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CNumPadKey::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CNumPadKey* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* thisView);

    private:
        CNumPadKey* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CNumPadKey();

    CARAPI UserActivity();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI OnDetachedFromWindow();

    CARAPI SetDigit(
        /* [in] */ Int32 digit);

    //@Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

    // Cause a VIRTUAL_KEY vibration
    CARAPI DoHapticKeyClick();

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    CARAPI_(void) UpdateText();

private:
    // list of "ABC", etc per digit, starting with '0'
    static AutoPtr<ArrayOf<String> > sKlondike;

    Int32 mDigit;
    Int32 mTextViewResId;
    AutoPtr<IPasswordTextView> mTextView;
    AutoPtr<ITextView> mDigitText;
    AutoPtr<ITextView> mKlondikeText;
    Boolean mEnableHaptics;
    AutoPtr<IPowerManager> mPM;

    AutoPtr<IViewOnClickListener> mListener;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CNUMPADKEY_H__
