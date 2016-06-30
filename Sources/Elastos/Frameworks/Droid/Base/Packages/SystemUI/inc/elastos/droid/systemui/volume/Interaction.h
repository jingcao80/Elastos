
#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_INTERACTION_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_INTERACTION_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnGenericMotionListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

class Interaction
    : public Object
{
private:
    class MyOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnTouchListener(
            /* [in] */ IInteractionCallback* callback);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IInteractionCallback> mCallback;
    };

    class MyOnGenericMotionListener
        : public Object
        , public IViewOnGenericMotionListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnGenericMotionListener(
            /* [in] */ IInteractionCallback* callback);

        CARAPI OnGenericMotion(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IInteractionCallback> mCallback;
    };

public:
    static CARAPI Register(
        /* [in] */ IView* v,
        /* [in] */ IInteractionCallback* callback);
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_VOLUME_INTERACTION_H__