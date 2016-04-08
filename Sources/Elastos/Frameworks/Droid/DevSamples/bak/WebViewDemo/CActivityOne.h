
#ifndef __CWEBVIEWACTIVITY_H__
#define __CWEBVIEWACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Webkit::IWebChromeClient;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Webkit::IWebViewClient;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WebViewDemo {

class CActivityOne
    : public Activity
{
public:
    class ButtonOnClickListener
        : public ElLightRefBase
        , public IViewOnClickListener
    {
    public:
        ButtonOnClickListener(
            /* [in] */ CActivityOne* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        CActivityOne* mOwner;
    };

    class DialogOnClickListener
        : public ElLightRefBase
        , public IDialogInterfaceOnClickListener
    {
    public:
        DialogOnClickListener(
            /* [in] */ CActivityOne* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CActivityOne* mOwner;
    };

    class ViewOnKeyListener
        : public ElLightRefBase
        , public IViewOnKeyListener
    {
    public:
        ViewOnKeyListener(
            /* [in] */ CActivityOne* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL();

        CARAPI OnKey(
            /* [in] */ IView * v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        CActivityOne* mOwner;
    };

public:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI Init();

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI ConfirmExit();

    CARAPI Loadurl(
        /* [in] */ IWebView* view,
        /* [in] */ const String& url);

    virtual CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

private:
    CARAPI HandleMessage(
        /* [in] */ Int32 what);

    CARAPI_(void) SendMessage(
        /* [in] */ Int32 what);

public:
    /** Called when the activity is first created. */
    AutoPtr<IWebView> mWebView;
    AutoPtr<IProgressDialog> mProgressDialog;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IViewOnClickListener> mClickListener;
    AutoPtr<IViewOnKeyListener> mKeyListener;
};

} // namespace WebViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CWEBVIEWACTIVITY_H__
