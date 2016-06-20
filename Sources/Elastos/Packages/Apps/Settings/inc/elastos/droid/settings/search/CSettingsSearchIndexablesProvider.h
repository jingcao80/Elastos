#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_CSETTINGSSEARCHINDEXABLESPROVIDER_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_CSETTINGSSEARCHINDEXABLESPROVIDER_H__

#include "_Elastos_Droid_Settings_Search_CSettingsSearchIndexablesProvider.h"
#include "elastos/droid/provider/SearchIndexablesProvider.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::SearchIndexablesProvider;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

CarClass(CSettingsSearchIndexablesProvider)
    , public SearchIndexablesProvider
{
public:
    CAR_OBJECT_DECL()

    CSettingsSearchIndexablesProvider();

    ~CSettingsSearchIndexablesProvider();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    //@Override
    virtual CARAPI QueryXmlResources(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    //@Override
    virtual CARAPI QueryRawData(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    //@Override
    virtual CARAPI QueryNonIndexableKeys(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

private:
    static const String TAG;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_CSETTINGSSEARCHINDEXABLESPROVIDER_H__
