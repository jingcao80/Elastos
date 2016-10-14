#ifndef __ELASTOS_DROID_SETTINGS_CSCREENPINNINGSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CSCREENPINNINGSETTINGS_H__

#include "_Elastos_Droid_Settings_CScreenPinningSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"

using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ISwitch;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Screen pinning settings.
 */
CarClass(CScreenPinningSettings)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    class InnerListener
        : public Object
        , public ISwitchBarOnSwitchChangeListener
    {
    public:
        TO_STRING_IMPL("CScreenPinningSettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CScreenPinningSettings* host);

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

    private:
        CScreenPinningSettings* mHost;
    };

    /**
     * For search
     */
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CScreenPinningSettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetRawDataToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list); //List<SearchIndexableRaw>
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CScreenPinningSettings();

    ~CScreenPinningSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    /**
     * Listens to the state change of the lock-to-app master switch.
     */
    //@Override
    CARAPI OnSwitchChanged(
        /* [in] */ ISwitch* switchView,
        /* [in] */ Boolean isChecked);

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

private:
    CARAPI_(Boolean) IsLockToAppEnabled();

    CARAPI_(void) SetLockToAppEnabled(
        /* [in] */ Boolean isEnabled);
private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    AutoPtr<ISwitchBar> mSwitchBar;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSCREENPINNINGSETTINGS_H__