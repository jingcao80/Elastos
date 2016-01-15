#ifndef __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTPROFILE_H__
#define __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTPROFILE_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

class ContactsContractProfile
{
public:
    /**
     * The content:// style URI for this table, which requests the contact entry
     * representing the user's personal profile data.
     */
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * {@link Uri} for referencing the user's profile {@link Contacts} entry,
     * Provides {@link OpenableColumns} columns when queried, or returns the
     * user's profile contact formatted as a vCard when opened through
     * {@link ContentResolver#openAssetFileDescriptor(Uri, String)}.
     */
    static CARAPI GetCONTENT_VCARD_URI(
        /* [out] */ IUri** uri);

    /**
     * {@link Uri} for referencing the raw contacts that make up the user's profile
     * {@link Contacts} entry.  An individual raw contact entry within the profile
     * can be addressed by appending the raw contact ID.  The entities or data within
     * that specific raw contact can be requested by appending the entity or data
     * path as well.
     */
    static CARAPI GetCONTENTRAWCONTACTSURI(
        /* [out] */ IUri** uri);

    /**
     * The minimum ID for any entity that belongs to the profile.  This essentially
     * defines an ID-space in which profile data is stored, and is used by the provider
     * to determine whether a request via a non-profile-specific URI should be directed
     * to the profile data rather than general contacts data, along with all the special
     * permission checks that entails.
     *
     * Callers may use {@link #isProfileId} to check whether a specific ID falls into
     * the set of data intended for the profile.
     */
    static CARAPI GetMINID(
        /* [out] */ Int64* id);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTPROFILE_H__
