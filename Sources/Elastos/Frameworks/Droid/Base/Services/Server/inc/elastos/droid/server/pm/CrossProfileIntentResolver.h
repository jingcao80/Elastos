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

#ifndef __ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTRESOLVER_H__
#define __ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTRESOLVER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
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

