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

#include "UriUtils.h"
#include <elstos.droid.net.Uri.h>

using Elastos::Droid::Net::Uri;

namespace Elastos{
namespace Apps{
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
    return IObject::Probe(uri1)->Equals(uri2);
}

AutoPtr<IUri> UriUtils::ParseUriOrNull(
    /* [in] */ const String& uriString)
{
    if (uriString.IsNUll()) {
        return NULL;
    }
    AutoPtr<IUri> uri;
    Uri::Parse(uriString, &uri);
    return uri;
}

String UriUtils::UriToString(
    /* [in] */ IUri* uri)
{
    String str;
    return uri == NULL ? String(NULL) : uri->ToString(&str), str;
}

Boolean UriUtils::IsEncodedContactUri(
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        return FALSE;
    }
    String lastPathSegment;
    uri->GetLastPathSegment(&lastPathSegment);
    if (lastPathSegment.IsNUll()) {
        return FALSE;
    }
    return lastPathSegment.Equals(IConstants::LOOKUP_URI_ENCODED);
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
