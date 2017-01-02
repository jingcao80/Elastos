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

#include "elastos/droid/provider/CContactsContractContactsAggregationSuggestions.h"
#include "elastos/droid/provider/CContactsContractContactsAggregationSuggestionsBuilder.h"
#include <elastos/coredef.h>

using Elastos::Droid::Provider::IContactsContractContactsAggregationSuggestionsBuilder;
using Elastos::Droid::Provider::CContactsContractContactsAggregationSuggestionsBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractContactsAggregationSuggestions)

CAR_INTERFACE_IMPL(CContactsContractContactsAggregationSuggestions, Singleton, IContactsContractContactsAggregationSuggestions)

ECode CContactsContractContactsAggregationSuggestions::Builder(
    /* [out] */ IContactsContractContactsAggregationSuggestionsBuilder** build)
{
    AutoPtr<IContactsContractContactsAggregationSuggestionsBuilder> builder;
    FAIL_RETURN(CContactsContractContactsAggregationSuggestionsBuilder::New((IContactsContractContactsAggregationSuggestionsBuilder**)&builder))
    *build = builder;
    REFCOUNT_ADD(*build);

    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
