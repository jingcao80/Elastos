#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSPEOPLE_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSPEOPLE_H__

#include "_Elastos_Droid_Provider_CContactsPeople.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Net::IUri;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsPeople)
    , public Singleton
    , public IContactsPeople
    , public IBaseColumns
    , public ISyncConstValue
    , public IContactsPeopleColumns
    , public IContactsPhonesColumns
    , public IContactsPresenceColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * no public constructor since this is a utility class
     * @deprecated see {@link android.provider.ContactsContract}
     */
    CARAPI constructor();

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for filtering people by name. The filter
     * argument should be passed as an additional path segment after this URI.
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_FILTER_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for the table that holds the deleted
     * contacts.
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetDELETED_CONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for filtering people that have a specific
     * E-mail or IM address. The filter argument should be passed as an
     * additional path segment after this URI. This matches any people with
     * at least one E-mail or IM {@link ContactMethods} that match the
     * filter.
     *
     * Not exposed because we expect significant changes in the contacts
     * schema and do not want to have to support this.
     * @hide
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetWITH_EMAIL_OR_IM_FILTER_URI(
        /* [out] */ IUri** uri);

    CARAPI GetDEFAULTSORTORDER(
        /* [out] */ String* order);

    /**
     * Mark a person as having been contacted.
     *
     * @param resolver the ContentResolver to use
     * @param personId the person who was contacted
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI MarkAsContacted(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int64 personId);

    /**
     * @hide Used in vCard parser code.
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI TryGetMyContactsGroupId(
         /* [in] */ IContentResolver* resolver,
         /* [out] */ Int64* id);

    /**
     * Adds a person to the My Contacts group.
     *
     * @param resolver the resolver to use
     * @param personId the person to add to the group
     * @return the URI of the group membership row
     * @throws IllegalStateException if the My Contacts group can't be found
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI AddToMyContactsGroup(
         /* [in] */ IContentResolver* resolver,
         /* [in] */ Int64 personId,
         /* [out] */ IUri** uri);

    /**
     * Adds a person to a group referred to by name.
     *
     * @param resolver the resolver to use
     * @param personId the person to add to the group
     * @param groupName the name of the group to add the contact to
     * @return the URI of the group membership row
     * @throws IllegalStateException if the group can't be found
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI AddToGroup(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int64 personId,
        /* [in] */ const String& groupName,
        /* [out] */ IUri** uri);

    /**
     * Adds a person to a group.
     *
     * @param resolver the resolver to use
     * @param personId the person to add to the group
     * @param groupId the group to add the person to
     * @return the URI of the group membership row
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI AddToGroup(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int64 personId,
        /* [in] */ Int64 groupId,
        /* [out] */ IUri** uri);

    /**
     * Creates a new contacts and adds it to the "My Contacts" group.
     *
     * @param resolver the ContentResolver to use
     * @param values the values to use when creating the contact
     * @return the URI of the contact, or null if the operation fails
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI CreatePersonInMyContactsGroup(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** uri);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI QueryGroups(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int64 person,
        /* [out] */ ICursor** cursor);

    /**
     * Set the photo for this person. data may be null
     * @param cr the ContentResolver to use
     * @param person the Uri of the person whose photo is to be updated
     * @param data the byte[] that represents the photo
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI SetPhotoData(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* person,
        /* [in] */ ArrayOf<Byte>* data);

    /**
     * Opens an InputStream for the person's photo and returns the photo as a Bitmap.
     * If the person's photo isn't present returns the placeholderImageResource instead.
     * @param person the person whose photo should be used
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI OpenContactPhotoInputStream(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* person,
        /* [out] */ IInputStream** stream);

    /**
     * Opens an InputStream for the person's photo and returns the photo as a Bitmap.
     * If the person's photo isn't present returns the placeholderImageResource instead.
     * @param context the Context
     * @param person the person whose photo should be used
     * @param placeholderImageResource the image resource to use if the person doesn't
     *   have a photo
     * @param options the decoding options, can be set to null
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI LoadContactPhoto(
        /* [in] */ IContext* context,
        /* [in] */ IUri* person,
        /* [in] */ Int32 placeholderImageResource,
        /* [in] */ IBitmapFactoryOptions* options,
        /* [out] */ IBitmap** bitmap);

private:
    static AutoPtr<IBitmap> LoadPlaceholderPhoto(
        /* [in] */ Int32 placeholderImageResource,
        /* [in] */ IContext* context,
        /* [in] */ IBitmapFactoryOptions* options);

private:
    static AutoPtr<ArrayOf<String> > GROUPS_PROJECTION;
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSPEOPLE_H__
