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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_URIUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_URIUTILS_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Utility methods for dealing with URIs.
 */
class UriUtils
{
public:
    /** Checks whether two URI are equal, taking care of the case where either is null. */
    static CARAPI_(Boolean) AreEqual(
        /* [in] */ IUri* uri1,
        /* [in] */ IUri* uri2);

    /** Parses a string into a URI and returns null if the given string is null. */
    static CARAPI_(AutoPtr<IUri>) ParseUriOrNull(
        /* [in] */ const String& uriString);

    /** Converts a URI into a string, returns null if the given URI is null. */
    static CARAPI_(String) UriToString(
        /* [in] */ IUri* uri);

    static CARAPI_(Boolean) IsEncodedContactUri(
        /* [in] */ IUri* uri);

private:
    /** Static helper, not instantiable. */
    UriUtils() {}
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_URIUTILS_H__
