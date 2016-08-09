#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTADAPTER_H__

#include "elastos/droid/contacts/common/list/IndexerListAdapter.h"

using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * Common base class for various contact-related lists, e.g. contact list, phone number list
 * etc.
 */
class ContactEntryListAdapter
    : public IndexerListAdapter
    , public IContactEntryListAdapter
{
public:
    CAR_INTERFACE_DECL()

    ContactEntryListAdapter();

    virtual ~ContactEntryListAdapter() {}

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Remove all directories after the default directory. This is typically used when contacts
     * list screens are asked to exit the search mode and thus need to remove all remote directory
     * results for the search.
     *
     * This code assumes that the default directory and directories before that should not be
     * deleted (e.g. Join screen has "suggested contacts" directory before the default director,
     * and we should not remove the directory).
     */
    CARAPI RemoveDirectoriesAfterDefault();

    /**
     * Marks all partitions as "loading"
     */
    CARAPI OnDataReload();

    // @Override
    CARAPI ClearPartitions();

    CARAPI IsSearchMode(
        /* [out] */ Boolean* flag);

    CARAPI SetSearchMode(
        /* [in] */ Boolean flag);

    CARAPI GetQueryString(
        /* [out] */ String* queryString);

    CARAPI SetQueryString(
        /* [in] */ String queryString);

    CARAPI GetUpperCaseQueryString(
        /* [out] */ String* result);

    CARAPI GetDirectorySearchMode(
        /* [out] */ Int32* mode);

    CARAPI SetDirectorySearchMode(
        /* [in] */ Int32 mode);

    CARAPI GetDirectoryResultLimit(
        /* [out] */ Int32* limit);

    CARAPI GetDirectoryResultLimit(
        /* [in] */ IDirectoryPartition* directoryPartition,
        /* [out] */ Int32* limit);

    CARAPI SetDirectoryResultLimit(
        /* [in] */ Int32 limit);

    CARAPI GetContactNameDisplayOrder(
        /* [out] */ Int32* displayOrder);

    CARAPI SetContactNameDisplayOrder(
        /* [in] */ Int32 displayOrder);

    CARAPI GetSortOrder(
        /* [out] */ Int32* sortOrder);

    CARAPI SetSortOrder(
        /* [in] */ Int32 sortOrder);

    CARAPI SetPhotoLoader(
        /* [in] */ IContactPhotoManager* photoLoader);

    CARAPI GetDisplayPhotos(
        /* [out] */ Boolean* displayPhotos);

    CARAPI SetDisplayPhotos(
        /* [in] */ Boolean displayPhotos);

    CARAPI GetCircularPhotos(
        /* [out] */ Boolean* circularPhotos);

    CARAPI SetCircularPhotos(
        /* [in] */ Boolean circularPhotos);

    CARAPI IsEmptyListEnabled(
        /* [out] */ Boolean* flag);

    CARAPI SetEmptyListEnabled(
        /* [in] */ Boolean flag);

    CARAPI IsSelectionVisible(
        /* [out] */ Boolean* flag);

    CARAPI SetSelectionVisible(
        /* [in] */ Boolean flag);

    CARAPI IsQuickContactEnabled(
        /* [out] */ Boolean* quickContactEnabled);

    CARAPI SetQuickContactEnabled(
        /* [in] */ Boolean quickContactEnabled);

    CARAPI IsAdjustSelectionBoundsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetAdjustSelectionBoundsEnabled(
        /* [in] */ Boolean enabled);

    CARAPI ShouldIncludeProfile(
        /* [out] */ Boolean* includeProfile);

    CARAPI SetIncludeProfile(
        /* [in] */ Boolean includeProfile);

    CARAPI SetProfileExists(
        /* [in] */ Boolean exists);

    CARAPI HasProfile(
        /* [out] */ Boolean* hasProfile);

    CARAPI SetDarkTheme(
        /* [in] */ Boolean value);

    /**
     * Updates partitions according to the directory meta-data contained in the supplied
     * cursor.
     */
    CARAPI ChangeDirectories(
        /* [in] */ ICursor* cursor);

    // @Override
    CARAPI ChangeCursor(
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor);

    CARAPI ChangeCursor(
        /* [in] */ ICursor* cursor);

    // @Override
    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI_(Int32) GetItemViewType(
        /* [in] */ Int32 partition,
        /* [in] */ Int32 position);

    // @Override
    CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

    CARAPI IsLoading(
        /* [out] */ Boolean* isLoading);

    CARAPI AreAllPartitionsEmpty(
        /* [out] */ Boolean* empty);

    /**
     * Changes visibility parameters for the default directory partition.
     */
    CARAPI ConfigureDefaultPartition(
        /* [in] */ Boolean showIfEmpty,
        /* [in] */ Boolean hasHeader);

    // TODO: fix PluralRules to handle zero correctly and use Resources.getQuantityText directly
    CARAPI GetQuantityText(
        /* [in] */ Int32 count,
        /* [in] */ Int32 zeroResourceId,
        /* [in] */ Int32 pluralResourceId,
        /* [out] */ String* text);

    CARAPI IsPhotoSupported(
        /* [in] */ Int32 partitionIndex,
        /* [out] */ Boolean* supported);

    /**
     * Returns the currently selected filter.
     */
    CARAPI GetFilter(
        /* [out] */ IContactListFilter** filter);

    CARAPI SetFilter(
        /* [in] */ IContactListFilter* filter);

    // @Override
    CARAPI HasStableIds(
        /* [out] */ Boolean* has);

    static CARAPI_(Boolean) IsRemoteDirectory(
        /* [in] */ Int64 directoryId);

    /**
     * Retrieves the lookup key and display name from a cursor, and returns a
     * {@link DefaultImageRequest} containing these contact details
     *
     * @param cursor Contacts cursor positioned at the current row to retrieve contact details for
     * @param displayNameColumn Column index of the display name
     * @param lookupKeyColumn Column index of the lookup key
     * @return {@link DefaultImageRequest} with the displayName and identifier fields set to the
     * display name and lookup key of the contact.
     */
    CARAPI GetDefaultImageRequestFromCursor(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 displayNameColumn,
        /* [in] */ Int32 lookupKeyColumn,
        /* [out] */ IContactPhotoManagerDefaultImageRequest** imageRequest);

protected:
    /**
     * @param fragmentRootView Root view of the fragment. This is used to restrict the scope of
     * image loading requests that get cancelled on cursor changes.
     */
    CARAPI_(void) SetFragmentRootView(
        /* [in] */ IView* fragmentRootView);

    CARAPI_(void) SetDefaultFilterHeaderText(
        /* [in] */ Int32 resourceId);

    // @Override
    CARAPI_(AutoPtr<IView>) NewView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position,
        /* [in] */ IViewGroup* parent);

    // @Override
    CARAPI_(void) BindView(
        /* [in] */ IView* v,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position);

    // @Override
    CARAPI_(AutoPtr<IView>) CreatePinnedSectionHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent);

    // @Override
    CARAPI_(void) SetPinnedSectionTitle(
        /* [in] */ IView* pinnedHeaderView,
        /* [in] */ const String& title);

    CARAPI_(void) AddPartitions();

    CARAPI_(AutoPtr<IDirectoryPartition>) CreateDefaultDirectoryPartition();

    CARAPI_(Int32) GetPartitionByDirectoryId(
        /* [in] */ Int64 id);

    CARAPI_(AutoPtr<IDirectoryPartition>) GetDirectoryById(
        /* [in] */ Int64 id);

    CARAPI_(AutoPtr<IContactPhotoManager>) GetPhotoLoader();

    CARAPI_(Boolean) GetExtraStartingSection();

    // @Override
    CARAPI_(AutoPtr<IView>) NewHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ IViewGroup* parent);

    // @Override
    CARAPI_(void) BindHeaderView(
        /* [in] */ IView* view,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor);

    // Default implementation simply returns number of rows in the cursor.
    // Broken out into its own routine so can be overridden by child classes
    // for eg number of unique contacts for a phone list.
    CARAPI_(Int32) GetResultCount(
        /* [in] */ ICursor* cursor);

    /**
     * Checks whether the contact entry at the given position represents the user's profile.
     */
    CARAPI_(Boolean) IsUserProfile(
        /* [in] */ Int32 position);

    /**
     * Loads the photo for the quick contact view and assigns the contact uri.
     * @param photoIdColumn Index of the photo id column
     * @param photoUriColumn Index of the photo uri column. Optional: Can be -1
     * @param contactIdColumn Index of the contact id column
     * @param lookUpKeyColumn Index of the lookup key column
     * @param displayNameColumn Index of the display name column
     */
    CARAPI_(void) BindQuickContact(
        /* [in] */ IContactListItemView* view,
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 photoIdColumn,
        /* [in] */ Int32 photoUriColumn,
        /* [in] */ Int32 contactIdColumn,
        /* [in] */ Int32 lookUpKeyColumn,
        /* [in] */ Int32 displayNameColumn);

    CARAPI_(void) BindViewId(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 idColumn);

    CARAPI_(AutoPtr<IUri>) GetContactUri(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 contactIdColumn,
        /* [in] */ Int32 lookUpKeyColumn);

private:
    /**
     * Updates the indexer, which is used to produce section headers.
     */
    CARAPI_(void) UpdateIndexer(
        /* [in] */ ICursor* cursor);

private:
    static const String TAG;

    Int32 mDisplayOrder;
    Int32 mSortOrder;

    Boolean mDisplayPhotos;
    Boolean mCircularPhotos;
    Boolean mQuickContactEnabled;
    Boolean mAdjustSelectionBoundsEnabled;

    /**
     * indicates if contact queries include profile
     */
    Boolean mIncludeProfile;

    /**
     * indicates if query results includes a profile
     */
    Boolean mProfileExists;

    /**
     * The root view of the fragment that this adapter is associated with.
     */
    AutoPtr<IView> mFragmentRootView;

    AutoPtr<IContactPhotoManager> mPhotoLoader;

    String mQueryString;
    String mUpperCaseQueryString;
    Boolean mSearchMode;
    Int32 mDirectorySearchMode;
    Int32 mDirectoryResultLimit;

    Boolean mEmptyListEnabled;

    Boolean mSelectionVisible;

    AutoPtr<IContactListFilter> mFilter;
    Boolean mDarkTheme;

    /** Resource used to provide header-text for default filter. */
    AutoPtr<ICharSequence> mDefaultFilterHeaderText;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTADAPTER_H__
