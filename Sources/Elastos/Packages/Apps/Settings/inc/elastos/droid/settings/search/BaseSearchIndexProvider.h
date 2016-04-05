
#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_BASESEARCHINDEXPROVIDER_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_BASESEARCHINDEXPROVIDER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Settings.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

/**
 * A basic SearchIndexProvider that returns no data to index.
 */
class BaseSearchIndexProvider
    : public Object
    , public IIndexableSearchIndexProvider
{
public:
    CAR_INTERFACE_DECL();

    BaseSearchIndexProvider();

    ~BaseSearchIndexProvider();

    //@Override
    CARAPI GetXmlResourcesToIndex(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled,
        /* [out] */ IList** list); /* List<SearchIndexableResource> */

    //@Override
    CARAPI GetRawDataToIndex(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled,
        /* [out] */ IList** list); /* List<SearchIndexableRaw>  */

    //@Override
    CARAPI GetNonIndexableKeys(
        /* [in] */ IContext* context,
        /* [out] */ IList** list); /* List<String>  */

private:
    // static const List<String> EMPTY_LIST = Collections.<String>EmptyList();
    static const AutoPtr<IList> EMPTY_LIST;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_BASESEARCHINDEXPROVIDER_H__

