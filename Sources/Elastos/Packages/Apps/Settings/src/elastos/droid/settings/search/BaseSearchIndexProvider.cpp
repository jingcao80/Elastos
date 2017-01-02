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

#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"

using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

static AutoPtr<IList> InitEMPTY_LIST()
{
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    AutoPtr<IList> list;
    coll->GetEmptyList((IList**)&list);
    return list;
}

const AutoPtr<IList> BaseSearchIndexProvider::EMPTY_LIST = InitEMPTY_LIST();

CAR_INTERFACE_IMPL(BaseSearchIndexProvider, Object, IIndexableSearchIndexProvider);

BaseSearchIndexProvider::BaseSearchIndexProvider()
{}

BaseSearchIndexProvider::~BaseSearchIndexProvider()
{}

ECode BaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) /* List<SearchIndexableResource> */
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;
    return NOERROR;
}

ECode BaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) /* List<SearchIndexableRaw>  */
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;
    return NOERROR;
}

ECode BaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** list) /* List<String>  */
{
    VALIDATE_NOT_NULL(list);
    *list = EMPTY_LIST;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos
