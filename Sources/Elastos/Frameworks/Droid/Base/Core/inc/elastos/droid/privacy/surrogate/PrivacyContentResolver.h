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

#ifndef __ELASTOS_DROID_PRIVACY_SURROGATE_PRIVACYCONTENTRESOLVER_H__
#define __ELASTOS_DROID_PRIVACY_SURROGATE_PRIVACYCONTENTRESOLVER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Core_server.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Privacy {
namespace Surrogate {

class PrivacyContentResolver : public ElRefBase
{
private:
    PrivacyContentResolver();

public:
    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

public:
    /**
     * Returns a dummy database cursor if access is restricted by privacy settings
     * @param uri
     * @param context
     * @param realCursor
     */
    static CARAPI EnforcePrivacyPermission(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* realCursor,
        /* [out] */ ICursor** outCusor);

    /**
     * This method is especially for faking android_id if google wants to read it in their privacy database
     * @author CollegeDev
     * @param uri
     * @param projection
     * @param context
     * @param realCursor
     */
    static CARAPI EnforcePrivacyPermission(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* realCursor,
        /* [in] */ Boolean google_access,
        /* [out] */ ICursor** outCusor);

private:
    static  CARAPI_(String) ArrayToString(
        /* [in] */ ArrayOf<String>* array);

    /**
     * Contacts.ContractContacts.CONTENT_URI.toString()
     */
    static CARAPI_(String) GetContactsContractContactsContentUriString();

    /**
     * IBrowser::BOOKMARKS_URI::getAuthority()
     */
    static CARAPI_(String) GetBrowserBookmarksUriAuthority();

private:
    const static String TAG;
    const static String SMS_CONTENT_URI_AUTHORITY;
    const static String MMS_CONTENT_URI_AUTHORITY;
    const static String MMS_SMS_CONTENT_URI_AUTHORITY;

    static AutoPtr<IPrivacySettingsManager> mPrivacySetMan;
};

} // namespace Surrogate
} // namespace Privacy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PRIVACY_SURROGATE_PRIVACYCONTENTRESOLVER_H__
