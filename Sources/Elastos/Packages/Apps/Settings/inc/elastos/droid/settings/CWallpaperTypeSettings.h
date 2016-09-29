#ifndef __ELASTOS_DROID_SETTINGS_CWALLPAPERTYPESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CWALLPAPERTYPESETTINGS_H__

#include "_Elastos_Droid_Settings_CWallpaperTypeSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CWallpaperTypeSettings)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CWallpaperTypeSettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetRawDataToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWallpaperTypeSettings();

    ~CWallpaperTypeSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

private:
    CARAPI_(void) PopulateWallpaperTypes();

private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CWALLPAPERTYPESETTINGS_H__