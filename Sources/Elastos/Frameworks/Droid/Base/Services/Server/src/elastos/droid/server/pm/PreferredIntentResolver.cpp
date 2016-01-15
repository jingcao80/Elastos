
#include "pm/PreferredIntentResolver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

AutoPtr<ArrayOf<PreferredActivity*> > PreferredIntentResolver::NewArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<PreferredActivity*> > filters = ArrayOf<PreferredActivity*>::Alloc(size);
    return filters;
}

Boolean PreferredIntentResolver::IsPackageForFilter(
    /* [in] */ const String& packageName,
    /* [in] */ PreferredActivity* filter)
{
    String pkgName;
    filter->mPref->mComponent->GetPackageName(&pkgName);
    return packageName.Equals(pkgName);
}

void PreferredIntentResolver::DumpFilter(
    /* [in] */ IPrintWriter* out,
    /* [in] */ const String& prefix,
    /* [in] */ PreferredActivity* filter)
{
    filter->mPref->Dump(out, prefix, (IIntentFilter*)filter);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
