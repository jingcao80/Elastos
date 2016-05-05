
#ifndef __Elastos_DevSamples_DialogFragmentDemo_CActivityOne_H__
#define __Elastos_DevSamples_DialogFragmentDemo_CActivityOne_H__

#include <elastos/droid/app/Activity.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include "_Elastos_DevSamples_DialogFragmentDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

CarClass(CActivityOne)
    , public Activity
    , public ILoginInputListenerHolder
{
public:
    class LoginButtonOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        LoginButtonOnClickListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 id);

    private:
        CActivityOne* mHost;
    };

    class LoginInputListener
        : public Object
        , public ILoginInputListener
    {
    public:
        CAR_INTERFACE_DECL()

        LoginInputListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnLoginInputComplete(
            /* [in] */ const String& username,
            /* [in] */ const String& password);

    private:
        CActivityOne* mHost;
    };

    class ViewClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewClickListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetLoginInputListener(
        /* [out] */ ILoginInputListener** listener);

protected:

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    void ShowConfimDialog();

    void ShowEditDialog();

    void ShowLoginDialog();

    void ShowDialogInDifferentScreen();

    void ShowLoginDialogWithoutFragment();

private:
    AutoPtr<ILoginInputListener> mLoginInputListener;
};

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DialogFragmentDemo_CActivityOne_H__
