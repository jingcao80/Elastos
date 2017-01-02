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

#include "elastos/droid/provider/CSearchIndexablesContractNonIndexableKey.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSearchIndexablesContractNonIndexableKey)

CAR_INTERFACE_IMPL(CSearchIndexablesContractNonIndexableKey, Singleton, ISearchIndexablesContractNonIndexableKey)

ECode CSearchIndexablesContractNonIndexableKey::GetNonIndexableKeyMIME_TYPE(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchIndexablesContract::NonIndexableKey::MIME_TYPE;
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos

