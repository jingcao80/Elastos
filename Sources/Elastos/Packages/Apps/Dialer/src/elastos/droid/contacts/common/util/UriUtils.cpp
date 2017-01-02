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

#include "elastos/droid/contacts/common/util/UriUtils.h"
#include "elastos/droid/contacts/common/util/Constants.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

Boolean UriUtils::AreEqual(
    /* [in] */ IUri* uri1,
    /* [in] */ IUri* uri2)
{
    if (uri1 == NULL && uri2 == NULL) {
        return TRUE;
    }
    if (uri1 == NULL || uri2 == NULL) {
        return FALSE;
    }
    Boolean equals;
    IObject::Probe(uri1)->Equals(uri2, &equals);
    return equals;
}

AutoPtr<IUri> UriUtils::ParseUriOrNull(
    /* [in] */ const String& uriString)
{
    if (uriString.IsNull()) {
        return NULL;
    }
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(uriString, (IUri**)&uri);
    return uri;
}

String UriUtils::UriToString(
    /* [in] */ IUri* uri)
{
    String str;
    return uri == NULL ? String(NULL) : (IObject::Probe(uri)->ToString(&str), str);
}

Boolean UriUtils::IsEncodedContactUri(
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        return FALSE;
    }
    String lastPathSegment;
    uri->GetLastPathSegment(&lastPathSegment);
    if (lastPathSegment.IsNull()) {
        return FALSE;
    }
    return lastPathSegment.Equals(Constants::LOOKUP_URI_ENCODED);
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
