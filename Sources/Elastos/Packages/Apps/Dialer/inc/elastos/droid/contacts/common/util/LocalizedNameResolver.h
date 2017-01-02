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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_LOCALIZEDNAMERESOLVER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_LOCALIZEDNAMERESOLVER_H__

#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Content::IContext;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Retrieves localized names per account type. This allows customizing texts like
 * "All Contacts" for certain account types, but e.g. "All Friends" or "All Connections" for others.
 */
class LocalizedNameResolver
{
public:
    /**
     * Returns the name for All Contacts for the specified account type.
     */
    static CARAPI GetAllContactsName(
        /* [in] */ IContext* context,
        /* [in] */ const String& accountType,
        /* [out] */ String* name);

private:
    /**
     * Finds "All Contacts"-Name for the specified account type.
     */
    static CARAPI_(String) ResolveAllContactsName(
        /* [in] */ IContext* context,
        /* [in] */ const String& accountType);

    /**
     * Finds the meta-data XML containing the contacts configuration and
     * reads the picture priority from that file.
     */
    static CARAPI_(String) ResolveAllContactsNameFromMetaData(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName);

    static CARAPI LoadAllContactsNameFromXml(
        /* [in] */ IContext* context,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& packageName,
        /* [out] */ String* name);

private:
    static const String TAG;

    /**
     * Meta-data key for the contacts configuration associated with a sync service.
     */
    static const String METADATA_CONTACTS;

    static const String CONTACTS_DATA_KIND;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_LOCALIZEDNAMERESOLVER_H__
