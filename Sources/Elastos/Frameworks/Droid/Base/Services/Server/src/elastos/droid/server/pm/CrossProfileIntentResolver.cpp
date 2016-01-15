
#include "pm/CrossProfileIntentResolver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

AutoPtr<ArrayOf<CrossProfileIntentFilter*> > CrossProfileIntentResolver::NewArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<CrossProfileIntentFilter*> > filters = ArrayOf<CrossProfileIntentFilter*>::Alloc(size);
    return filters;
}

Boolean CrossProfileIntentResolver::IsPackageForFilter(
    /* [in] */ const String& packageName,
    /* [in] */ CrossProfileIntentFilter* filter)
{
    return FALSE;
}

void CrossProfileIntentResolver::SortResults(
    /* [in] */ List<AutoPtr<CrossProfileIntentFilter> >* results)
{
    //We don't sort the results
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
