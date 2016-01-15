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
