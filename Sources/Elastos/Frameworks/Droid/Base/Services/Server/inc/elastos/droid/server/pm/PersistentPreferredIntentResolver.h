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

#ifndef __ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDINTENTRESOLVER_H__
#define __ELASTOS_DROID_SERVER_PM_PERSISTENTPREFERREDINTENTRESOLVER_H__

#include <Elastos.Droid.Content.h>
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

