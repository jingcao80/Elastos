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

#include "Elastos.Droid.Net.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/provider/ContactsContractContacts.h"
#include "elastos/droid/provider/CContactsContractContactsAggregationSuggestionsBuilder.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/coredef.h>

using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_OBJECT_IMPL(CContactsContractContactsAggregationSuggestionsBuilder)

CAR_INTERFACE_IMPL(CContactsContractContactsAggregationSuggestionsBuilder, Object, IContactsContractContactsAggregationSuggestionsBuilder)


ECode CContactsContractContactsAggregationSuggestionsBuilder::SetContactId(
    /* [in] */ Int64 contactId)
{
    mContactId = contactId;
    return NOERROR;
}

ECode CContactsContractContactsAggregationSuggestionsBuilder::AddParameter(
    /* [in] */ const String& kind,
    /* [in] */ const String& value)
{
    if (!TextUtils::IsEmpty(value)) {
        mKinds->Add(CoreUtils::Convert(kind));
        mValues->Add(CoreUtils::Convert(value));
    }
    return NOERROR;
}

ECode CContactsContractContactsAggregationSuggestionsBuilder::SetLimit(
    /* [in] */ Int32 limit)
{
    mLimit = limit;
    return NOERROR;
}

ECode CContactsContractContactsAggregationSuggestionsBuilder::Build(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    AutoPtr<IUriBuilder> builder;
    ContactsContractContacts::CONTENT_URI->BuildUpon((IUriBuilder**)&builder);
    builder->AppendEncodedPath(StringUtils::ToString(mContactId));
    builder->AppendPath(IContactsContractContactsAggregationSuggestions::CONTENT_DIRECTORY);
    if (mLimit != 0) {
        FAIL_RETURN(builder->AppendQueryParameter(String("limit"), StringUtils::ToString(mLimit)))
    }

    Int32 count;
    mKinds->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> inter;
        mKinds->Get(i, (IInterface**)&inter);
        String str;
        IObject::Probe(inter)->ToString(&str);
        AutoPtr<IInterface> inter_;
        mValues->Get(i, (IInterface**)&inter_);
        String str_;
        IObject::Probe(inter)->ToString(&str_);
        FAIL_RETURN(builder->AppendQueryParameter(String("query"), str + str_))
    }

    return builder->Build(uri);
}

} //Provider
} //Droid
} //Elastos