
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYCARRIERAREA_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CEmergencyCarrierArea.h"
#include "elastos/droid/systemui/keyguard/AlphaOptimizedLinearLayout.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::Object;

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
        TO_STRING_IMPL("CEmergencyCarrierArea::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CEmergencyCarrierArea* host)
            : mHost(host)
        {}

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        CEmergencyCarrierArea* mHost;
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
