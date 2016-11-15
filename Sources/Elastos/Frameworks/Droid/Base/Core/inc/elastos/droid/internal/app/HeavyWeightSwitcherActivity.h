
#ifndef __ELASTOS_DROID_INTERNAL_APP_HEAVYWEIGHTSWITCHERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_HEAVYWEIGHTSWITCHERACTIVITY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class HeavyWeightSwitcherActivity
    : public Activity
    , public IHeavyWeightSwitcherActivity
{
private:
    class SwitchOldListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        SwitchOldListener(
            /* [in] */ HeavyWeightSwitcherActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        HeavyWeightSwitcherActivity* mHost;
    };

    class SwitchNewListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        SwitchNewListener(
            /* [in] */ HeavyWeightSwitcherActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        HeavyWeightSwitcherActivity* mHost;
    };

    class CancelListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CancelListener(
            HeavyWeightSwitcherActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        HeavyWeightSwitcherActivity* mHost;
    };

public:
    HeavyWeightSwitcherActivity();

    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI SetText(
        /* [in] */ Int32 id,
        /* [in] */ ICharSequence* text);

    CARAPI SetDrawable(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* dr);

    CARAPI SetIconAndText(
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 actionId,
        /* [in] */ Int32 descriptionId,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 actionStr,
        /* [in] */ Int32 descriptionStr);

public:
    AutoPtr<IIntentSender> mStartIntent;
    Boolean mHasResult;
    String mCurApp;
    Int32 mCurTask;
    String mNewApp;
    AutoPtr<SwitchOldListener> mSwitchOldListener;
    AutoPtr<SwitchNewListener> mSwitchNewListener;
    AutoPtr<CancelListener> mCancelListener;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_HEAVYWEIGHTSWITCHERACTIVITY_H__
