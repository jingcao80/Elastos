
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"

using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

static AutoPtr<IList> InitEMPTY_LIST()
{
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    AutoPtr<IList> list;
    coll->GetEmptyList((IList**)&list);
    return list;
}

const AutoPtr<IList> BaseSearchIndexProvider::EMPTY_LIST = InitEMPTY_LIST();

CAR_INTERFACE_IMPL(BaseSearchIndexProvider, Object, IIndexableSearchIndexProvider);

BaseSearchIndexProvider::BaseSearchIndexProvider()
{}

BaseSearchIndexProvider::~BaseSearchIndexProvider()
{}

ECode BaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) /* List<SearchIndexableResource> */
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;
    return NOERROR;
}

ECode BaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) /* List<SearchIndexableRaw>  */
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;
    return NOERROR;
}

ECode BaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** list) /* List<String>  */
{
    VALIDATE_NOT_NULL(list);
    *list = EMPTY_LIST;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos
