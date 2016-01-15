#ifndef __ELASTOS_DROID_SERVER_PM_PREFERREDINTENTRESOLVER_H__
#define __ELASTOS_DROID_SERVER_PM_PREFERREDINTENTRESOLVER_H__

#include "elastos/droid/server/IntentResolver.h"
#include "elastos/droid/server/pm/PreferredActivity.h"

using Elastos::Droid::Content::IIntentFilter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PreferredIntentResolver : public IntentResolver<PreferredActivity, PreferredActivity>
{
protected:
    // @Override
    CARAPI_(AutoPtr<ArrayOf<PreferredActivity*> >) NewArray(
        /* [in] */ Int32 size);

    // @Override
    CARAPI_(Boolean) IsPackageForFilter(
        /* [in] */ const String& packageName,
        /* [in] */ PreferredActivity* filter);

    // @Override
    CARAPI_(void) DumpFilter(
        /* [in] */ IPrintWriter* out,
        /* [in] */ const String& prefix,
        /* [in] */ PreferredActivity* filter);
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PREFERREDINTENTRESOLVER_H__

