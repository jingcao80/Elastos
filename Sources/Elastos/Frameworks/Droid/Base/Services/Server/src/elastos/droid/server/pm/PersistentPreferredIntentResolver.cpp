//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/server/pm/PersistentPreferredIntentResolver.h"

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
