#ifndef __ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDINTENTRESOLVER_H__
#define __ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDINTENTRESOLVER_H__

#include "elastos/droid/server/IntentResolver.h"
#include "elastos/droid/server/pm/PersistentPreferredActivity.h"

using Elastos::Droid::Server::IntentResolver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PersistentPreferredIntentResolver
    : public IntentResolver<PersistentPreferredActivity, PersistentPreferredActivity>
{
protected:
    // @Override
    CARAPI_(AutoPtr<ArrayOf<PersistentPreferredActivity*> >) NewArray(
        /* [in] */ Int32 size);

    // @Override
    CARAPI_(Boolean) IsPackageForFilter(
        /* [in] */ const String& packageName,
        /* [in] */ PersistentPreferredActivity* filter);
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDINTENTRESOLVER_H__

