
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
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::Webkit::IWebChromeClient;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Webkit::IWebViewClient;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WebViewDemo {

class CActivityOne
    : public Activity
    , public IObjectFactory
    , public ILogger
{
public:
    class ButtonOnClickListener
        : public ElLightRefBase
        , public IViewOnClickListener
    {
        public:
            ButtonOnClickListener(
                /* [in] */ CActivityOne* host)
                : mHost(host)
            {}

            CAR_INTERFACE_DECL()

            CARAPI OnClick(
                /* [in] */ IView* v);
        private:
            CActivityOne* mHost;
    };

    class DialogOnClickListener
        : public ElLightRefBase
        , public IDialogInterfaceOnClickListener
    {
        public:
            DialogOnClickListener(
                /* [in] */ CActivityOne* host)
                : mHost(host)
            {}

            CAR_INTERFACE_DECL()

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);

        private:
            CActivityOne* mHost;
    };

    class ViewOnKeyListener
        : public ElLightRefBase
        , public IViewOnKeyListener
    {
        public:
            ViewOnKeyListener(
                /* [in] */ CActivityOne* host)
                : mHost(host)
            {}

            CAR_INTERFACE_DECL()

            CARAPI OnKey(
                /* [in] */ IView* v,
                /* [in] */ Int32 keyCode,
                /* [in] */ IKeyEvent* event,
                /* [out] */ Boolean* result);

        private:
            CActivityOne* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI ConfirmExit();

    virtual CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI CreateInstance(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** object);

    CARAPI Log(
        /* [in] */ const String& message);

private:
    CARAPI HandleMessage(
        /* [in] */ Int32 what);

    CARAPI_(void) SendMessage(
        /* [in] */ Int32 what);

public:
    /** Called when the activity is first created. */
    AutoPtr<IWebView> mWebView;
    AutoPtr<IProgressDialog> mDialog;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IViewOnClickListener> mClickListener;
    AutoPtr<IViewOnKeyListener> mKeyListener;

    AutoPtr<IInterface> mJSTestObject;
};

} // namespace WebViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CWEBVIEWACTIVITY_H__
