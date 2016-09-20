#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONINTERSTITIAL_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONINTERSTITIAL_H__

#include "_Elastos_Droid_Settings_Notification_CRedactionInterstitial.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Settings::SettingsActivity;
using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IRadioButton;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CRedactionInterstitial)
    , public SettingsActivity
{
public:
    class RedactionInterstitialFragment
        : public SettingsPreferenceFragment
    {
    public:
        class InnerListener
            : public Object
            , public IViewOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            TO_STRING_IMPL("CRedactionInterstitial::RedactionInterstitialFragment::InnerListener")

            InnerListener(
                /* [in] */ RedactionInterstitialFragment* host);

            //@Override
            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            RedactionInterstitialFragment* mHost;
        };

    public:
        TO_STRING_IMPL("CRedactionInterstitial::RedactionInterstitialFragment")

        RedactionInterstitialFragment();

        ~RedactionInterstitialFragment();

        CARAPI constructor();

        //@Override
        CARAPI OnCreateView(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* container,
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IView** result);

        //@Override
        CARAPI OnResume();

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CARAPI_(void) LoadFromSettings();

    private:
        AutoPtr<IRadioButton> mShowAllButton;
        AutoPtr<IRadioButton> mRedactSensitiveButton;
        AutoPtr<IRadioButton> mHideAllButton;
    };

public:
    CAR_OBJECT_DECL()

    CRedactionInterstitial();

    ~CRedactionInterstitial();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    static CARAPI_(AutoPtr<IIntent>) CreateStartIntent(
        /* [in] */ IContext* ctx);

protected:
    //@Override
    CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);
};

} // namespace NOtification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONINTERSTITIAL_H__