
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_PHONENUMBERLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_PHONENUMBERLISTADAPTER_H__

#include "elastos/droid/contacts/common/list/ContactEntryListAdapter.h"

using Elastos::Droid::Content::ICursorLoader;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Utility::IList;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A cursor adapter for the {@link Phone#CONTENT_ITEM_TYPE} and
 * {@link SipAddress#CONTENT_ITEM_TYPE}.
 *
 * By default this adapter just handles phone numbers. When {@link #setUseCallableUri(boolean)} is
 * called with "true", this adapter starts handling SIP addresses too, by using {@link Callable}
 * API instead of {@link Phone}.
 */
class PhoneNumberListAdapter
    : public ContactEntryListAdapter
    , public IPhoneNumberListAdapter
{
public:
    class PhoneQuery
    {
    public:
        static const AutoPtr<ArrayOf<String> > PROJECTION_PRIMARY;
        static const AutoPtr<ArrayOf<String> > PROJECTION_ALTERNATIVE;

        static const Int32 PHONE_ID                = 0;
        static const Int32 PHONE_TYPE              = 1;
        static const Int32 PHONE_LABEL             = 2;
        static const Int32 PHONE_NUMBER            = 3;
        static const Int32 CONTACT_ID              = 4;
        static const Int32 LOOKUP_KEY              = 5;
        static const Int32 PHOTO_ID                = 6;
        static const Int32 DISPLAY_NAME            = 7;
        static const Int32 PHOTO_URI               = 8;
    };

public:
    CAR_INTERFACE_DECL()

    PhoneNumberListAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI ConfigureLoader(
        /* [in] */ ICursorLoader* loader,
        /* [in] */ Int64 directoryId);

    // @Override
    CARAPI GetContactDisplayName(
        /* [in] */ Int32 position,
        /* [out] */ String* displayName);

    CARAPI GetPhoneNumber(
        /* [in] */ Int32 position,
        /* [out] */ String* displayNumber);

    /**
     * Builds a {@link Data#CONTENT_URI} for the given cursor position.
     *
     * @return Uri for the data. may be null if the cursor is not ready.
     */
    CARAPI GetDataUri(
        /* [in] */ Int32 position,
        /* [out] */ IUri** uri);

    CARAPI GetDataUri(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor,
        /* [out] */ IUri** uri);

    CARAPI SetPhotoPosition(
        /* [in] */ PhotoPosition photoPosition);

    CARAPI GetPhotoPosition(
        /* [out] */ PhotoPosition* photoPosition);

    CARAPI SetUseCallableUri(
        /* [in] */ Boolean useCallableUri);

    CARAPI UsesCallableUri(
        /* [out] */ Boolean* useCallableUri);

    /**
     * Override base implementation to inject extended directories between local & remote
     * directories. This is done in the following steps:
     * 1. Call base implementation to add directories from the cursor.
     * 2. Iterate all base directories and establish the following information:
     *   a. The highest directory id so that we can assign unused id's to the extended directories.
     *   b. The index of the last non-remote directory. This is where we will insert extended
     *      directories.
     * 3. Iterate the extended directories and for each one, assign an ID and insert it in the
     *    proper location.
     */
    // @Override
    CARAPI ChangeDirectories(
        /* [in] */ ICursor* cursor);

protected:
    CARAPI_(AutoPtr<ICharSequence>) GetUnknownNameText();

    CARAPI_(Boolean) IsExtendedDirectory(
        /* [in] */ Int64 directoryId);

    // @Override
    CARAPI_(AutoPtr<IView>) NewView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position,
        /* [in] */ IViewGroup* parent);

    CARAPI_(void) SetHighlight(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor);

    // Override default, which would return number of phone numbers, so we
    // instead return number of contacts.
    // @Override
    CARAPI_(Int32) GetResultCount(
        /* [in] */ ICursor* cursor);

    CARAPI_(void) BindView(
        /* [in] */ IView* itemView,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position);

    CARAPI_(void) BindPhoneNumber(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean displayNumber);

    CARAPI_(void) BindSectionHeaderAndDivider(
        /* [in] */ IContactListItemView* view,
        /* [in] */ Int32 position);

    CARAPI_(void) BindName(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) UnbindName(
        /* [in] */ IContactListItemView* view);

    CARAPI_(void) BindPhoto(
        /* [in] */ IContactListItemView* view,
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor);

    CARAPI_(AutoPtr<IUri>) GetContactUri(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 contactIdColumn,
        /* [in] */ Int32 lookUpKeyColumn);

private:
    CARAPI_(AutoPtr<IDirectoryPartition>) GetExtendedDirectoryFromId(
        /* [in] */ Int64 directoryId);

    /**
     * Configure {@code loader} and {@code uriBuilder} according to {@code directoryId} and {@code
     * filter}.
     */
    CARAPI_(void) ApplyFilter(
        /* [in] */ ICursorLoader* loader,
        /* [in] */ IUriBuilder* uriBuilder,
        /* [in] */ Int64 directoryId,
        /* [in] */ IContactListFilter* filter);

private:
    static const String TAG;
    static const String IGNORE_NUMBER_TOO_LONG_CLAUSE;

    // A list of extended directories to add to the directories from the database
    AutoPtr<IList> mExtendedDirectories;

    // Extended directories will have ID's that are higher than any of the id's from the database.
    // Thi sis so that we can identify them and set them up properly. If no extended directories
    // exist, this will be Long.MAX_VALUE
    Int64 mFirstExtendedDirectoryId;

    AutoPtr<ICharSequence> mUnknownNameText;
    String mCountryIso;

    PhotoPosition mPhotoPosition;

    Boolean mUseCallableUri;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_PHONENUMBERLISTADAPTER_H__
