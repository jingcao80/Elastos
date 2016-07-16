
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CEmergencyCarrierArea.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CEmergencyCarrierArea)
    , public AlphaOptimizedLinearLayout
    , public IEmergencyCarrierArea
{
private:
    class MyOnClickListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        TO_STRING_IMPL("CEmergencyButton::MyOnClickListener")

        MyOnClickListener(
            /* [in] */ CEmergencyButton* host)
            : mHost(host)
        {}

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        CEmergencyButton* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetCarrierTextVisible(
        /* [in] */ Boolean visible);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    AutoPtr<ICarrierText> mCarrierText;
    AutoPtr<IEmergencyButton> mEmergencyButton;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__
