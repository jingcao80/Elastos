#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_SEARCHINDEXABLERESOURCES_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_SEARCHINDEXABLERESOURCES_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

class SearchIndexableResources
{
public:
    static CARAPI_(Int32) GetSize();

    static CARAPI_(AutoPtr<ISearchIndexableResource>) GetResourceByName(
        /* [in] */ const String& className);

    static CARAPI_(AutoPtr<ICollection>) GetValues();

private:
    SearchIndexableResources()
    {}

public:
    static Int32 NO_DATA_RES_ID;

private:
    // static HashMap<String, SearchIndexableResource> sResMap =
    //         new HashMap<String, SearchIndexableResource>();
    static AutoPtr<IHashMap> sResMap;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_SEARCHINDEXABLERESOURCES_H__

