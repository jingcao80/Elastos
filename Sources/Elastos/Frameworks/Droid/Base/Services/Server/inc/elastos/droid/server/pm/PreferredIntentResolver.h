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

