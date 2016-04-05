
#include "elastos/droid/settings/search/SearchIndexableRaw.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Provider::ISearchIndexableData;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

CAR_INTERFACE_IMPL(SearchIndexableRaw, SearchIndexableData, ISearchIndexableRaw);

SearchIndexableRaw::SearchIndexableRaw()
{}

SearchIndexableRaw::~SearchIndexableRaw()
{}

ECode SearchIndexableRaw::constructor(
    /* [in] */ IContext* context)
{
    return SearchIndexableData::constructor(context);
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos
