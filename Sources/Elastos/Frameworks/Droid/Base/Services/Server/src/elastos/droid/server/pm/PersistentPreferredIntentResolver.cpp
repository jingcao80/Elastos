
#include "pm/PersistentPreferredIntentResolver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

AutoPtr<ArrayOf<PersistentPreferredActivity*> > PersistentPreferredIntentResolver::NewArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<PersistentPreferredActivity*> > filters = ArrayOf<PersistentPreferredActivity*>::Alloc(size);
    return filters;
}

Boolean PersistentPreferredIntentResolver::IsPackageForFilter(
    /* [in] */ const String& packageName,
    /* [in] */ PersistentPreferredActivity* filter)
{
    String pkgName;
    filter->mComponent->GetPackageName(&pkgName);
    return packageName.Equals(pkgName);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
