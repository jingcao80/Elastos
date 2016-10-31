#ifndef __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRM_H__
#define __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRM_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Settings_CCryptKeeperConfirm.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CCryptKeeperConfirm)
    , public Fragment
{
public:
    class Blank
        : public Activity
    {
    private:
        class MyRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("CCryptKeeperConfirm::Blank::MyRunnable")

            MyRunnable(
                /* [in] */ Blank* host);

            CARAPI Run();

        private:
            Blank* mHost;
        };

    public:
        TO_STRING_IMPL("CCryptKeeperConfirm::Blank");

        Blank();

        virtual ~Blank();

        CARAPI constructor();

        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

    private:
        AutoPtr<IHandler> mHandler;
    };

private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CCryptKeeperConfirm::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CCryptKeeperConfirm* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCryptKeeperConfirm* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CCryptKeeperConfirm();

    ~CCryptKeeperConfirm();

    CARAPI constructor();

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

private:
    CARAPI_(void) EstablishFinalConfirmationState();

private:
    static const String TAG;// = "CCryptKeeperConfirm";

    AutoPtr<IView> mContentView;
    AutoPtr<IButton> mFinalButton;
    AutoPtr<IViewOnClickListener> mFinalClickListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRM_H__