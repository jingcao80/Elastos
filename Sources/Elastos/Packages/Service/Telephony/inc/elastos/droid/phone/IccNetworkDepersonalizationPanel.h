#ifndef  __ELASTOS_DROID_PHONE_ICCNETWORKDEPERSONALIZATIONPANEL_H__
#define  __ELASTOS_DROID_PHONE_ICCNETWORKDEPERSONALIZATIONPANEL_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * "SIM network unlock" PIN entry screen.
 *
 * @see PhoneGlobals.EVENT_SIM_NETWORK_LOCKED
 *
 * TODO: This UI should be part of the lock screen, not the
 * phone app (see bug 1804111).
 */
class IccNetworkDepersonalizationPanel
    : public IccPanel
{
private:
    class MyTextWatcher
        : public Object
        , public ITextWatcher
        , public INoCopySpan
    {
    public:
        TO_STRING_IMPL("IccNetworkDepersonalizationPanel::MyTextWatcher")

        MyTextWatcher(
            /* [in] */ IccNetworkDepersonalizationPanel* host)
            : mHost(host)
        {}

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* buffer,
            /* [in] */ Int32 start,
            /* [in] */ Int32 olen,
            /* [in] */ Int32 nlen)
        {}

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* buffer,
            /* [in] */ Int32 start,
            /* [in] */ Int32 olen,
            /* [in] */ Int32 nlen)
        {}

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* buffer);

    private:
        IccNetworkDepersonalizationPanel* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("IccNetworkDepersonalizationPanel::MyRunnable")

        MyRunnable(
            /* [in] */ IccNetworkDepersonalizationPanel* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        IccNetworkDepersonalizationPanel* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("IccNetworkDepersonalizationPanel::MyRunnable2")

        MyRunnable2(
            /* [in] */ IccNetworkDepersonalizationPanel* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        IccNetworkDepersonalizationPanel* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("IccNetworkDepersonalizationPanel::MyHandler")

        MyHandler(
            /* [in] */ IccNetworkDepersonalizationPanel* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        IccNetworkDepersonalizationPanel* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("IccNetworkDepersonalizationPanel::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ IccNetworkDepersonalizationPanel* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        IccNetworkDepersonalizationPanel* mHost;
    };

    class MyViewOnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("IccNetworkDepersonalizationPanel::MyViewOnClickListener2")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener2(
            /* [in] */ IccNetworkDepersonalizationPanel* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        IccNetworkDepersonalizationPanel* mHost;
    };


public:
    TO_STRING_IMPL("IccNetworkDepersonalizationPanel")

    //constructor
    IccNetworkDepersonalizationPanel(
        /* [in] */ IContext* context);

    //Mirrors IccPinUnlockPanel.onKeyDown().
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnStart();

private:
    CARAPI_(void) IndicateBusy();

    CARAPI_(void) IndicateError();

    CARAPI_(void) IndicateSuccess();

    CARAPI_(void) HideAlert();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    //debug constants
    static const Boolean DBG;

    //events
    static const Int32 EVENT_ICC_NTWRK_DEPERSONALIZATION_RESULT;

    AutoPtr<IPhone> mPhone;

    //UI elements
    AutoPtr<IEditText>     mPinEntry;
    AutoPtr<ILinearLayout> mEntryPanel;
    AutoPtr<ILinearLayout> mStatusPanel;
    AutoPtr<ITextView>     mStatusText;

    AutoPtr<IButton>       mUnlockButton;
    AutoPtr<IButton>       mDismissButton;

    //private textwatcher to control text entry.
    AutoPtr<ITextWatcher> mPinEntryWatcher;

    //handler for unlock function results
    AutoPtr<IHandler> mHandler;

    AutoPtr<IViewOnClickListener> mUnlockListener;

    AutoPtr<IViewOnClickListener> mDismissListener;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_ICCNETWORKDEPERSONALIZATIONPANEL_H__