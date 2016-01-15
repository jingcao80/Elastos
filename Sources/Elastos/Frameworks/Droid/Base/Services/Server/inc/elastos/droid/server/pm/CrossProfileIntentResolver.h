#ifndef __ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTRESOLVER_H__
#define __ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTRESOLVER_H__

#include "elastos/droid/server/IntentResolver.h"
#include "elastos/droid/server/pm/CrossProfileIntentFilter.h"

using Elastos::Droid::Content::IIntentFilter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

/**
 * Used to find a list of {@link CrossProfileIntentFilter}s that match an intent.
 */
class CrossProfileIntentResolver
    : public IntentResolver<CrossProfileIntentFilter, CrossProfileIntentFilter>
{
protected:
    // @Override
    CARAPI_(AutoPtr<ArrayOf<CrossProfileIntentFilter*> >) NewArray(
        /* [in] */ Int32 size);

    // @Override
    CARAPI_(Boolean) IsPackageForFilter(
        /* [in] */ const String& packageName,
        /* [in] */ CrossProfileIntentFilter* filter);

    // @Override
    CARAPI_(void) SortResults(
        /* [in] */ List<AutoPtr<CrossProfileIntentFilter> >* results);
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTRESOLVER_H__

