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

#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_BASESEARCHINDEXPROVIDER_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_BASESEARCHINDEXPROVIDER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Settings.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

/**
 * A basic SearchIndexProvider that returns no data to index.
 */
class BaseSearchIndexProvider
    : public Object
    , public IIndexableSearchIndexProvider
{
public:
    CAR_INTERFACE_DECL();

    BaseSearchIndexProvider();

    ~BaseSearchIndexProvider();

    //@Override
    CARAPI GetXmlResourcesToIndex(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled,
        /* [out] */ IList** list); /* List<SearchIndexableResource> */

    //@Override
    CARAPI GetRawDataToIndex(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled,
        /* [out] */ IList** list); /* List<SearchIndexableRaw>  */

    //@Override
    CARAPI GetNonIndexableKeys(
        /* [in] */ IContext* context,
        /* [out] */ IList** list); /* List<String>  */

private:
    // static const List<String> EMPTY_LIST = Collections.<String>EmptyList();
    static const AutoPtr<IList> EMPTY_LIST;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_BASESEARCHINDEXPROVIDER_H__

