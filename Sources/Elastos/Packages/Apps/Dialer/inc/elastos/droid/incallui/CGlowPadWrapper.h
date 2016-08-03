
#ifndef __ELASTOS_DROID_INCALLUI_CGLOWPADWRAPPER_H__
#define __ELASTOS_DROID_INCALLUI_CGLOWPADWRAPPER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_InCallUI_CGlowPadWrapper.h"
#include "elastos/droid/incallui/widget/multiwaveview/GlowPadView.h"
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::InCallUI::Widget::MultiwaveView::IGlowPadViewOnTriggerListener;
using Elastos::Droid::InCallUI::Widget::MultiwaveView::GlowPadView;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CGlowPadWrapper)
    , public GlowPadView
    , public IGlowPadViewOnTriggerListener
{
private:
    class PingHandler
        : public Handler
    {
    public:
        PingHandler(
            /* [in] */ CGlowPadWrapper* host)
            : Handler(FALSE)
            , mHost(host)
        {}

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CGlowPadWrapper* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CGlowPadWrapper();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) StartPing();

    CARAPI_(void) StopPing();

    // @Override
    CARAPI OnGrabbed(
        /* [in] */ IView* v,
        /* [in] */ Int32 handle);

    // @Override
    CARAPI OnReleased(
        /* [in] */ IView* v,
        /* [in] */ Int32 handle);

    // @Override
    CARAPI OnTrigger(
        /* [in] */ IView* v,
        /* [in] */ Int32 target);

    // @Override
    CARAPI OnGrabbedStateChange(
        /* [in] */ IView* v,
        /* [in] */ Int32 handle);

    CARAPI OnFinishFinalAnimation();

    CARAPI_(void) SetAnswerListener(
        /* [in] */ IAnswerListener* listener);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) TriggerPing();

private:
    // Parameters for the GlowPadView "ping" animation; see triggerPing().
    static const Int32 PING_MESSAGE_WHAT = 101;
    static const Boolean ENABLE_PING_AUTO_REPEAT = TRUE;
    static const Int64 PING_REPEAT_DELAY_MS = 1200;

    AutoPtr<PingHandler> mPingHandler;

    AutoPtr<IAnswerListener> mAnswerListener;
    Boolean mPingEnabled;
    Boolean mTargetTriggered;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CGLOWPADWRAPPER_H__
