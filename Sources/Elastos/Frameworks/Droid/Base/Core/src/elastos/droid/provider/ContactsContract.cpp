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
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/ContactsContractProfile.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/regex/Pattern.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::IContactsContractProfile;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::Pattern;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

static AutoPtr<IUri> initContactsContractAUTHORITY_URI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + IContactsContract::AUTHORITY, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> ContactsContract::AUTHORITY_URI = initContactsContractAUTHORITY_URI();

ECode ContactsContract::IsProfileId(
    /* [in] */ Int64 id,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int64 _id;
    FAIL_RETURN(ContactsContractProfile::GetMINID(&_id))
    *res =  id >= _id;
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos