
#include "elastos/droid/contacts/common/list/ContactEntryListAdapter.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "elastos/droid/contacts/common/list/ContactListPinnedHeaderView.h"
#include "elastos/droid/contacts/common/list/DirectoryPartition.h"
#include "elastos/droid/contacts/common/list/ContactsSectionIndexer.h"
#include "elastos/droid/contacts/common/list/DirectoryListLoader.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/util/SearchUtil.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Contacts::Common::List::EIID_IContactEntryListAdapter;
using Elastos::Droid::Contacts::Common::Util::SearchUtil;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::IContactsContractContactCounts;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IQuickContactBadge;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

const String ContactEntryListAdapter::TAG("ContactEntryListAdapter");

CAR_INTERFACE_DECL(ContactEntryListAdapter, IndexerListAdapter, IContactEntryListAdapter)

ContactEntryListAdapter::ContactEntryListAdapter()
    : mDisplayOrder(0)
    , mSortOrder(0)
    , mDisplayPhotos(FALSE)
    , mCircularPhotos(TRUE)
    , mQuickContactEnabled(FALSE)
    , mAdjustSelectionBoundsEnabled(FALSE)
    , mIncludeProfile(FALSE)
    , mProfileExists(FALSE)
    , mSearchMode(FALSE)
    , mDirectorySearchMode(0)
    , mDirectoryResultLimit(Elastos::Core::Math::INT32_MAX_VALUE)
    , mEmptyListEnabled(TRUE)
    , mSelectionVisible(FALSE)
    , mDarkTheme(FALSE)
{}

ECode ContactEntryListAdapter::constructor(
    /* [in] */ IContext* context)
{
    IndexerListAdapter::constructor(context);
    SetDefaultFilterHeaderText(Elastos::Droid::Dialer::R::string::local_search_label);
    AddPartitions();
    return NOERROR;
}

ECode ContactEntryListAdapter::SetFragmentRootView(
    /* [in] */ IView* fragmentRootView)
{
    mFragmentRootView = fragmentRootView;
    return NOERROR;
}

void ContactEntryListAdapter::SetDefaultFilterHeaderText(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ICharSequence> text;
    res->GetText(resourceId, (ICharSequence**)&text);
    mDefaultFilterHeaderText = text;
}

AutoPtr<IView> ContactEntryListAdapter::NewView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<ContactListItemView> view = new ContactListItemView(context, NULL);
    Boolean enabled;
    IsSectionHeaderDisplayEnabled(&enabled);
    view->SetIsSectionHeaderEnabled(enabled);
    IsAdjustSelectionBoundsEnabled(&enabled);
    view->SetAdjustSelectionBoundsEnabled(enabled);
    return IView::Probe(view);
}

void ContactEntryListAdapter::BindView(
    /* [in] */ IView* v,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position)
{
    AutoPtr<ContactListItemView> view = (ContactListItemView*)IContactListItemView::Probe(itemView);
    Boolean enabled;
    IsSectionHeaderDisplayEnabled(&enabled);
    view->SetIsSectionHeaderEnabled(enabled);
}

AutoPtr<IView> ContactEntryListAdapter::CreatePinnedSectionHeaderView(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<ContactListPinnedHeaderView> view = new ContactListPinnedHeaderView(context, NULL, parent);
    return IView::Probe(view);
}

void ContactEntryListAdapter::SetPinnedSectionTitle(
    /* [in] */ IView* pinnedHeaderView,
    /* [in] */ const String& title)
{
    AutoPtr<ContactListItemView> view = (ContactListItemView*)IContactListItemView::Probe(pinnedHeaderView);
    view->SetSectionHeaderTitle(title);
}

void ContactEntryListAdapter::AddPartitions()
{
    AutoPtr<IDirectoryPartition> partition = CreateDefaultDirectoryPartition()
    AddPartition(ICompositeCursorAdapterPartition::Probe(partition));
}

AutoPtr<IDirectoryPartition> ContactEntryListAdapter::CreateDefaultDirectoryPartition()
{
    AutoPtr<DirectoryPartition> partition = new DirectoryPartition(TRUE, TRUE);
    partition->SetDirectoryId(IContactsContractDirectory::DEFAULT);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String list;
    context->GetString(Elastos::Droid::Dialer::R::string::contactsList, &list);
    partition->SetDirectoryType(list);
    partition->SetPriorityDirectory(TRUE);
    partition->SetPhotoSupported(TRUE);
    String str;
    mDefaultFilterHeaderText->ToString(&str);
    partition->SetLabel(str);
    return IDirectoryPartition::Probe(partition);
}

ECode ContactEntryListAdapter::RemoveDirectoriesAfterDefault()
{
    Int32 partitionCount;
    GetPartitionCount(&partitionCount);
    for (Int32 i = partitionCount - 1; i >= 0; i--) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL
                && ((DirectoryPartition*)partition.Get())->GetDirectoryId() == IContactsContractDirectory::DEFAULT) {
            break;
        }
        else {
            RemovePartition(i);
        }
    }
    return NOERROR;
}

Int32 ContactEntryListAdapter::GetPartitionByDirectoryId(
    /* [in] */ Int64 id)
{
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL) {
            if ((DirectoryPartition*)partition.Get())->GetDirectoryId() == id) {
                return i;
            }
        }
    }
    return -1;
}

AutoPtr<IDirectoryPartition> ContactEntryListAdapter::GetDirectoryById(
    /* [in] */ Int64 id)
{
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL) {
            AutoPtr<DirectoryPartition> directoryPartition = (DirectoryPartition*)partition.Get();
            if (directoryPartition->GetDirectoryId() == id) {
                return partition;
            }
        }
    }
    return NULL;
}

ECode ContactEntryListAdapter::OnDataReload()
{
    Boolean notify = FALSE;
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL) {
            AutoPtr<DirectoryPartition> directoryPartition = (DirectoryPartition*)partition.Get();
            if (!directoryPartition->IsLoading()) {
                notify = TRUE;
            }
            directoryPartition->SetStatus(IDirectoryPartition::STATUS_NOT_LOADED);
        }
    }
    if (notify) {
        NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode ContactEntryListAdapter::ClearPartitions()
{
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL) {
            AutoPtr<DirectoryPartition> directoryPartition = (DirectoryPartition*)partition.Get();
            directoryPartition->SetStatus(IDirectoryPartition::STATUS_NOT_LOADED);
        }
    }
    return IndexerListAdapter::ClearPartitions();
}

ECode ContactEntryListAdapter::IsSearchMode(
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag)
    *flag = mSearchMode;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetSearchMode(
    /* [in] */ Boolean flag)
{
    mSearchMode = flag;
    return NOERROR;
}

ECode ContactEntryListAdapter::GetQueryString(
    /* [out] */ String* queryString)
{
    VALIDATE_NOT_NULL(queryString)
    *queryString = mQueryString;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetQueryString(
    /* [in] */ const String& queryString)
{
    mQueryString = queryString;
    if (TextUtils::IsEmpty(queryString)) {
        mUpperCaseQueryString = NULL;
    }
    else {
        String str = SearchUtil::CleanStartAndEndOfSearchQuery(queryString.ToUpperCase());
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        mUpperCaseQueryString = cs;
    }
    return NOERROR;
}

ECode ContactEntryListAdapter::GetUpperCaseQueryString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mUpperCaseQueryString->ToString(result);
}

ECode ContactEntryListAdapter::GetDirectorySearchMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = mDirectorySearchMode;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetDirectorySearchMode(
    /* [in] */ Int32 mode)
{
    mDirectorySearchMode = mode;
    return NOERROR;
}

ECode ContactEntryListAdapter::GetDirectoryResultLimit(
    /* [out] */ Int32* limit)
{
    VALIDATE_NOT_NULL(limit)
    *limit = mDirectoryResultLimit;
    return NOERROR;
}

ECode ContactEntryListAdapter::GetDirectoryResultLimit(
    /* [in] */ IDirectoryPartition* directoryPartition,
    /* [out] */ Int32* limit)
{
    VALIDATE_NOT_NULL(limit)
    Int32 lim = ((DirectoryPartition*)directoryPartition)->GetResultLimit();
    *limit = lim == IDirectoryPartition::RESULT_LIMIT_DEFAULT ? mDirectoryResultLimit : limit;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetDirectoryResultLimit(
    /* [in] */ Int32 limit)
{
    mDirectoryResultLimit = limit;
    return NOERROR;
}

ECode ContactEntryListAdapter::GetContactNameDisplayOrder(
    /* [out] */ Int32* displayOrder)
{
    VALIDATE_NOT_NULL(displayOrder)
    *displayOrder = mDisplayOrder;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetContactNameDisplayOrder(
    /* [in] */ Int32 displayOrder)
{
    mDisplayOrder = displayOrder;
    return NOERROR;
}

ECode ContactEntryListAdapter::GetSortOrder(
    /* [out] */ Int32* sortOrder)
{
    VALIDATE_NOT_NULL(sortOrder)
    *sortOrder = mSortOrder;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetSortOrder(
    /* [in] */ Int32 sortOrder)
{
    mSortOrder = sortOrder;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetPhotoLoader(
    /* [in] */ IContactPhotoManager* photoLoader)
{
    mPhotoLoader = photoLoader;
    return NOERROR;
}

AutoPtr<IContactPhotoManager> ContactEntryListAdapter::GetPhotoLoader()
{
    return mPhotoLoader;
}

ECode ContactEntryListAdapter::GetDisplayPhotos(
    /* [out] */ Boolean* displayPhotos)
{
    VALIDATE_NOT_NULL(displayPhotos)
    *displayPhotos = mDisplayPhotos;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetDisplayPhotos(
    /* [in] */ Boolean displayPhotos)
{
    mDisplayPhotos = displayPhotos;
    return NOERROR;
}

ECode ContactEntryListAdapter::GetCircularPhotos(
    /* [out] */ Boolean* circularPhotos)
{
    VALIDATE_NOT_NULL(circularPhotos)
    *circularPhotos = mCircularPhotos;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetCircularPhotos(
    /* [in] */ Boolean circularPhotos)
{
    mCircularPhotos = circularPhotos;
    return NOERROR;
}

ECode ContactEntryListAdapter::IsEmptyListEnabled(
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag)
    *flag = mEmptyListEnabled;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetEmptyListEnabled(
    /* [in] */ Boolean flag)
{
    mEmptyListEnabled = flag;
    return NOERROR;
}

ECode ContactEntryListAdapter::IsSelectionVisible(
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag)
    *flag = mSelectionVisible;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetSelectionVisible(
    /* [in] */ Boolean flag)
{
    mSelectionVisible = flag;
    return NOERROR;
}

ECode ContactEntryListAdapter::IsQuickContactEnabled(
    /* [out] */ Boolean* quickContactEnabled)
{
    VALIDATE_NOT_NULL(quickContactEnabled)
    *quickContactEnabled = mQuickContactEnabled;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetQuickContactEnabled(
    /* [in] */ Boolean quickContactEnabled)
{
    mQuickContactEnabled = quickContactEnabled;
    return NOERROR;
}

ECode ContactEntryListAdapter::IsAdjustSelectionBoundsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = mAdjustSelectionBoundsEnabled;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetAdjustSelectionBoundsEnabled(
    /* [in] */ Boolean enabled)
{
    mAdjustSelectionBoundsEnabled = enabled;
    return NOERROR;
}

ECode ContactEntryListAdapter::ShouldIncludeProfile(
    /* [out] */ Boolean* includeProfile)
{
    VALIDATE_NOT_NULL(includeProfile)
    *includeProfile = mIncludeProfile;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetIncludeProfile(
    /* [in] */ Boolean includeProfile)
{
    mIncludeProfile = includeProfile;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetProfileExists(
    /* [in] */ Boolean exists)
{
    mProfileExists = exists;
    // Stick the "ME" header for the profile
    if (exists) {
        AutoPtr<ISectionIndexer> indexer;
        GetIndexer((ISectionIndexer**)&indexer);
        if (indexer != NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            String header;
            context->GetString(Elastos::Droid::Dialer::R::string::user_profile_contacts_list_header, &header);
            ((ContactsSectionIndexer*)indexer)->SetProfileHeader(header);
        }
    }
    return NOERROR;
}

ECode ContactEntryListAdapter::HasProfile(
    /* [out] */ Boolean* hasProfile)
{
    VALIDATE_NOT_NULL(hasProfile)
    *hasProfile = mProfileExists;
    return NOERROR;
}

ECode ContactEntryListAdapter::SetDarkTheme(
    /* [in] */ Boolean value)
{
    mDarkTheme = value;
    return NOERROR;
}

ECode ContactEntryListAdapter::ChangeDirectories(
    /* [in] */ ICursor* cursor)
{
    Int32 count;
    if (cursor->GetCount(&count), count == 0) {
        // Directory table must have at least local directory, without which this adapter will
        // enter very weird state.
        Logger::E(TAG, "Directory search loader returned an empty cursor, which implies we have no directory entries.");
        return E_RUNTIME_EXCEPTION;
    }
    HashSet<Int64> directoryIds;

    Int32 idColumnIndex;
    cursor->GetColumnIndex(IContactsContractDirectory::ID, &idColumnIndex);
    Int32 directoryTypeColumnIndex;
    cursor->GetColumnIndex(DirectoryListLoader::DIRECTORY_TYPE, &directoryTypeColumnIndex);
    Int32 displayNameColumnIndex;
    cursor->GetColumnIndex(IContactsContractDirectory::DISPLAY_NAME, &displayNameColumnIndex);
    Int32 photoSupportColumnIndex;
    cursor->GetColumnIndex(IContactsContractDirectory::PHOTO_SUPPORT, &photoSupportColumnIndex);

    // TODO preserve the order of partition to match those of the cursor
    // Phase I: add new directories
    Boolean result;
    cursor->MoveToPosition(-1, &result);
    Boolean hasNext;
    while (cursor->MoveToNext(&hasNext), hasNext) {
        Int64 id;
        cursor->GetInt64(idColumnIndex, &id);
        directoryIds.Insert(id);
        Int32 partition;
        if (GetPartitionByDirectoryId(id, &partition), partition == -1) {
            AutoPtr<DirectoryPartition> partition = new DirectoryPartition(FALSE, TRUE);
            partition->SetDirectoryId(id);
            if (IsRemoteDirectory(id)) {
                String label;
                mContext->GetString(Elastos::Droid::Dialer::R::string::directory_search_label, &label);
                partition->SetLabel(label);
            }
            else {
                String str;
                mDefaultFilterHeaderText->ToString(&str);
                partition->SetLabel(str);
            }
            String str;
            cursor->GetString(directoryTypeColumnIndex, &str);
            partition->SetDirectoryType(str);
            cursor->GetString(displayNameColumnIndex, &str);
            partition->SetDisplayName(str);
            Int32 photoSupport;
            cursor->GetInt32(photoSupportColumnIndex, &photoSupport);
            partition->SetPhotoSupported(photoSupport == IContactsContractDirectory::PHOTO_SUPPORT_THUMBNAIL_ONLY
                    || photoSupport == IContactsContractDirectory::PHOTO_SUPPORT_FULL);
            AddPartition(ICompositeCursorAdapterPartition::Probe(partition));
        }
    }

    // Phase II: remove deleted directories
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = count; --i >= 0; ) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL) {
            Int64 id = ((DirectoryPartition*)partition.Get())->GetDirectoryId();
            if (directoryIds.Find(id) == directories.End()) {
                RemovePartition(i);
            }
        }
    }

    Invalidate();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode ContactEntryListAdapter::ChangeCursor(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor)
{
    Int32 count;
    if (GetPartitionCount(&count), partitionIndex >= count) {
        // There is no partition for this data
        return NOERROR;
    }

    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(partitionIndex, (ICompositeCursorAdapterPartition**)&temp);
    AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
    if (partition != NULL) {
        ((DirectoryPartition*)partition.Get())->SetStatus(IDirectoryPartition::STATUS_LOADED);
    }

    Boolean isSupported;
    if (mDisplayPhotos && mPhotoLoader != NULL && (IsPhotoSupported(partitionIndex, &isSupported), isSupported)) {
        mPhotoLoader->RefreshCache();
    }

    FAIL_RETURN(IndexerListAdapter::ChangeCursor(partitionIndex, cursor))

    Boolean isEnabled;
    Int32 value;
    if ((IsSectionHeaderDisplayEnabled(&isEnabled), isEnabled) &&
            (GetIndexedPartition(&value), partitionIndex == value)) {
        UpdateIndexer(cursor);
    }

    // When the cursor changes, cancel any pending asynchronous photo loads.
    mPhotoLoader->CancelPendingRequests(mFragmentRootView);
    return NOERROR;
}

ECode ContactEntryListAdapter::ChangeCursor(
    /* [in] */ ICursor* cursor)
{
    return ChangeCursor(0, cursor);
}

void ContactEntryListAdapter::UpdateIndexer(
    /* [in] */ ICursor* cursor)
{
    if (cursor == NULL) {
        SetIndexer(NULL);
        return;
    }

    AutoPtr<IBundle> bundle;
    cursor->GetExtras((IBundle**)&bundle);
    Boolean containsT, containsC;
    if ((bundle->ContainsKey(IContactsContractContactCounts::EXTRA_ADDRESS_BOOK_INDEX_TITLES, &contains), contains) &&
            (bundle->ContainsKey(IContactsContractContactCounts::EXTRA_ADDRESS_BOOK_INDEX_COUNTS, &containsC), containsC)) {
        AutoPtr<ArrayOf<String> > sections;
        bundle->GetStringArray(IContactsContractContactCounts::EXTRA_ADDRESS_BOOK_INDEX_TITLES,
                (ArrayOf<String>**)&sections);
        AutoPtr<ArrayOf<Int32> > counts;
        bundle->GetInt32Array(IContactsContractContactCounts::EXTRA_ADDRESS_BOOK_INDEX_COUNTS,
                (ArrayOf<Int32>**)&counts);

        if (GetExtraStartingSection()) {
            // Insert an additional unnamed section at the top of the list.
            AutoPtr<ArrayOf<String> > allSections = ArrayOf<String>::Alloc(sections->GetLength() + 1);
            AutoPtr<ArrayOf<Int32> > allCounts = ArrayOf<Int32>::Alloc(counts->GetLength() + 1);
            for (Int32 i = 0; i < sections->GetLength(); i++) {
                (*allSections)[i + 1] = (*sections)[i];
                (*allCounts)[i + 1] = (*counts)[i];
            }
            (*allCounts)[0] = 1;
            (*allSections)[0] = String("");
            AutoPtr<ISectionIndexer> indexer = (ISectionIndexer*)new ContactsSectionIndexer(allSections, allCounts);
            SetIndexer(indexer);
        }
        else {
            AutoPtr<ISectionIndexer> indexer = (ISectionIndexer*)new ContactsSectionIndexer(sections, counts);
            SetIndexer(indexer);
        }
    }
    else {
        SetIndexer(NULL);
    }
}

Boolean ContactEntryListAdapter::GetExtraStartingSection()
{
    return FALSE;
}

ECode ContactEntryListAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    // We need a separate view type for each item type, plus another one for
    // each type with header, plus one for "other".
    Int32 c;
    GetItemViewTypeCount(&c);
    *count = c * 2 + 1;
    return NOERROR;
}

Int32 ContactEntryListAdapter::GetItemViewType(
    /* [in] */ Int32 partition,
    /* [in] */ Int32 position)
{
    Int32 type = IndexerListAdapter::GetItemViewType(partitionIndex, position);
    Boolean isEnabled;
    Int32 value;
    if (!IsUserProfile(position)
            && (IsSectionHeaderDisplayEnabled(&isEnabled), isEnabled)
            && (GetIndexedPartition(&value), partitionIndex == value)) {
        AutoPtr<IIndexerListAdapterPlacement> temp;
        GetItemPlacementInSection(position, (IIndexerListAdapterPlacement**)&temp);
        AutoPtr<Placement> placement = (Placement*)temp.Get();
        Int32 c;
        return placement->mFirstInSection ? type : (GetItemViewTypeCount(&c), c + type);
    }
    else {
        return type;
    }
}

ECode ContactEntryListAdapter::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty)
    // TODO
//        if (contactsListActivity.mProviderStatus != ProviderStatus.STATUS_NORMAL) {
//            return true;
//        }

    Boolean isSearchMode;
    if (!mEmptyListEnabled) {
        *empty = FALSE;
        return NOERROR;
    }
    else if (IsSearchMode(&isSearchMode), isSearchMode) {
        String str;
        GetQueryString(&str);
        *empty = TextUtils::IsEmpty(str);
        return NOERROR;
    }
    else {
        return IndexerListAdapter::IsEmpty(empty);
    }
}

ECode ContactEntryListAdapter::IsLoading(
    /* [out] */ Boolean* isLoading)
{
    VALIDATE_NOT_NULL(isLoading)
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL
                && ((DirectoryPartition*)partition.Get())->IsLoading()) {
            *isLoading = TRUE;
            return NOERROR;
        }
    }
    *isLoading = FALSE;
    return NOERROR;
}

ECode ContactEntryListAdapter::AreAllPartitionsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty)
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = 0; i < count; i++) {
        Boolean isEmpty;
        if (IsPartitionEmpty(i, &isEmpty), !isEmpty) {
            *empty = FALSE;
            return NOERROR;
        }
    }
    *empty = TRUE;
    return NOERROR;
}

ECode ContactEntryListAdapter::ConfigureDefaultPartition(
    /* [in] */ Boolean showIfEmpty,
    /* [in] */ Boolean hasHeader)
{
    Int32 defaultPartitionIndex = -1;
    Int32 count;
    GetPartitionCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
        if (partition != NULL &&
                ((DirectoryPartition*)partition.Get())->GetDirectoryId() == IContactsContractDirectory::DEFAULT) {
            defaultPartitionIndex = i;
            break;
        }
    }
    if (defaultPartitionIndex != -1) {
        SetShowIfEmpty(defaultPartitionIndex, showIfEmpty);
        SetHasHeader(defaultPartitionIndex, hasHeader);
    }
    return NOERROR;
}

AutoPtr<IView> ContactEntryListAdapter::NewHeaderView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::directory_header, parent, FALSE, (IView**)&view);
    Boolean enabled;
    if (GetPinnedPartitionHeadersEnabled(&enabled), !enabled) {
        // If the headers are unpinned, there is no need for their background
        // color to be non-transparent. Setting this transparent reduces maintenance for
        // non-pinned headers. We don't need to bother synchronizing the activity's
        // background color with the header background color.
        view->SetBackground(NULL);
    }
    return view;
}

void ContactEntryListAdapter::BindHeaderView(
    /* [in] */ IView* view,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(partitionIndex, (ICompositeCursorAdapterPartition**)&temp);
    AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
    if (partition == NULL) {
        return;
    }

    AutoPtr<DirectoryPartition> directoryPartition = (DirectoryPartition*)partition.Get();
    Int64 directoryId = directoryPartition->GetDirectoryId();
    AutoPtr<IView> label;
    view->FindViewById(Elastos::Droid::Dialer::R::id::label, (IView**)&label);
    AutoPtr<ITextView> labelTextView = ITextView::Probe(label);
    AutoPtr<IView> dn;
    view->FindViewById(Elastos::Droid::Dialer::R::id::display_name, (IView**)&dn);
    AutoPtr<ITextView> displayNameTextView = ITextView::Probe(dn);
    AutoPtr<ICharSequence> labelCS;
    CString::New(directoryPartition->GetLabel(), (ICharSequence**)&labelCS);
    labelTextView->SetText(labelCS);
    if (!IsRemoteDirectory(directoryId)) {
        displayNameTextView->SetText(NULL);
    }
    else {
        String directoryName = directoryPartition->GetDisplayName();
        String displayName = !TextUtils::IsEmpty(directoryName)
                ? directoryName
                : directoryPartition->GetDirectoryType();
        AutoPtr<ICharSequence> dnCS;
        CString::New(displayName, (ICharSequence**)&dnCS);
        displayNameTextView->SetText(dnCS);
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ICompositeCursorAdapterPartition> temp1;
    GetPartition(0, (ICompositeCursorAdapterPartition**)&temp1);
    AutoPtr<DirectoryPartition> dp = (DirectoryPartition*)IDirectoryPartition::Probe(temp1);
    Int32 offset;
    Int32 headerPaddingTop = partitionIndex == 1 && dp->IsEmpty()?
            0 : (res->GetDimensionPixelOffset(Elastos::Droid::Dialer::R::dimen::directory_header_extra_top_padding, &offset), offset);
    // There should be no extra padding at the top of the first directory header
    Int32 start, end, bottom;
    view->GetPaddingStart(&start);
    view->GetPaddingEnd(&end);
    view->GetPaddingBottom(&bottom);
    view->SetPaddingRelative(start, headerPaddingTop, end, bottom);
}

Int32 ContactEntryListAdapter::GetResultCount(
    /* [in] */ ICursor* cursor)
{
    Int32 count;
    return cursor == NULL ? 0 : (cursor->GetCount(&count), count);
}

Boolean ContactEntryListAdapter::IsUserProfile(
    /* [in] */ Int32 position)
{
    // The profile only ever appears in the first position if it is present.  So if the position
    // is anything beyond 0, it can't be the profile.
    Boolean isUserProfile = FALSE;
    if (position == 0) {
        Int32 partition;
        GetPartitionForPosition(position, &partition);
        if (partition >= 0) {
            // Save the old cursor position - the call to getItem() may modify the cursor
            // position.
            AutoPtr<ICursor> cursor;
            GetCursor(partition, (ICursor**)&cursor);
            Int32 offset;
            cursor->GetPosition(&offset);
            AutoPtr<IInterface> temp;
            GetItem(position, (IInterface**)&temp)
            cursor = ICursor::Probe(temp);
            if (cursor != NULL) {
                Int32 profileColumnIndex;
                cursor->GetColumnIndex(IContactsContractContactsColumns::IS_USER_PROFILE, &profileColumnIndex);
                if (profileColumnIndex != -1) {
                    Int32 value;
                    cursor->GetInt32(profileColumnIndex, &value);
                    isUserProfile = value == 1;
                }
                // Restore the old cursor position.
                Boolean result;
                cursor->MoveToPosition(offset, &result);
            }
        }
    }
    return isUserProfile;
}

ECode ContactEntryListAdapter::GetQuantityText(
    /* [in] */ Int32 count,
    /* [in] */ Int32 zeroResourceId,
    /* [in] */ Int32 pluralResourceId,
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text)
    if (count == 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        return context->GetString(zeroResourceId, text);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<ICharSequence> cs;
        res->GetQuantityText(pluralResourceId, count, (ICharSequence**)&cs);
        String format;
        cs->ToString(&format);
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<IInteger32> integer;
        CInteger32::New(count, (IInteger32**)&integer);
        args->Set(0, integer);
        *text = StringUtils::Format(format, args);
        return NOERROR;
    }
}

ECode ContactEntryListAdapter::IsPhotoSupported(
    /* [in] */ Int32 partitionIndex,
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(partitionIndex, (ICompositeCursorAdapterPartition**)&temp);
    AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
    if (partition == NULL) {
        *supported = ((DirectoryPartition*)partition.Get())->IsPhotoSupported();
        return NOERROR;
    }
    *supported = TRUE;
    return NOERROR;
}

ECode ContactEntryListAdapter::GetFilter(
    /* [out] */ IContactListFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mFilter;
    REFCOUNT_ADD(*filter)
    return NOERROR;
}

ECode ContactEntryListAdapter::SetFilter(
    /* [in] */ IContactListFilter* filter)
{
    mFilter = filter;
    return NOERROR;
}

void ContactEntryListAdapter::BindQuickContact(
    /* [in] */ IContactListItemView* _view,
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 photoIdColumn,
    /* [in] */ Int32 photoUriColumn,
    /* [in] */ Int32 contactIdColumn,
    /* [in] */ Int32 lookUpKeyColumn,
    /* [in] */ Int32 displayNameColumn)
{
    AutoPtr<ContactListItemView> view = (ContactListItemView*)_view;
    Int64 photoId = 0;
    Boolean isNull;
    if (cursor->IsNull(photoIdColumn, &isNull), !isNull) {
        cursor->GetInt64(photoIdColumn, &photoId);
    }

    AutoPtr<IQuickContactBadge> quickContact;
    view->GetQuickContact((IQuickContactBadge**)&quickContact);
    assert(quickContact == NULL);
    AutoPtr<IUri> uri = GetContactUri(partitionIndex, cursor, contactIdColumn, lookUpKeyColumn);
    quickContact->AssignContactUri(uri);

    if (photoId != 0 || photoUriColumn == -1) {
        GetPhotoLoader()->LoadThumbnail(quickContact, photoId, mDarkTheme, mCircularPhotos, NULL);
    }
    else {
        String photoUriString;
        cursor->GetString(photoUriColumn, &photoUriString);
        AutoPtr<IUri> photoUri;
        if (!photoUriString.IsNull()) {
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            helper->Parse(photoUriString, (IUri**)&photoUri);
        }
        AutoPtr<IContactPhotoManagerDefaultImageRequest> request;
        if (photoUri == NULL) {
            GetDefaultImageRequestFromCursor(cursor, displayNameColumn,
                    lookUpKeyColumn, (IContactPhotoManagerDefaultImageRequest**)&request);
        }
        GetPhotoLoader()->LoadPhoto(quickContact, photoUri, -1, mDarkTheme, mCircularPhotos,
                request);
    }
}

ECode ContactEntryListAdapter::HasStableIds(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    // Whenever bindViewId() is called, the values passed into setId() are stable or
    // stable-ish. For example, when one contact is modified we don't expect a second
    // contact's Contact._ID values to change.
    *has = TRUE;
    return NOERROR;
}

void ContactEntryListAdapter::BindViewId(
    /* [in] */ IContactListItemView* _view,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 idColumn)
{
    // Set a semi-stable id, so that talkback won't get confused when the list gets
    // refreshed. There is little harm in inserting the same ID twice.
    AutoPtr<ContactListItemView> view = (ContactListItemView*)_view;
    Int64 contactId;
    cursor->GetInt64(idColumn, &contactId);
    view->SetId((Int32)(contactId % Elastos::Core::Math::INT32_MAX_VALUE));
}

AutoPtr<IUri> ContactEntryListAdapter::GetContactUri(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 contactIdColumn,
    /* [in] */ Int32 lookUpKeyColumn)
{
    Int64 contactId;
    cursor->GetInt64(contactIdColumn, &contactId);
    String lookupKey;
    cursor->GetString(lookUpKeyColumn, &lookupKey);
    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(partitionIndex, (ICompositeCursorAdapterPartition**)&temp);
    AutoPtr<DirectoryPartition> partition = (DirectoryPartition*)IDirectoryPartition::Probe(temp);
    Int64 directoryId = partition->GetDirectoryId();
    // Remote directories must have a lookup key or we don't have
    // a working contact URI
    if (TextUtils::IsEmpty(lookupKey) && IsRemoteDirectory(directoryId)) {
        return NULL;
    }
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> uri;
    contacts->GetLookupUri(contactId, lookupKey, (IUri**)&uri);
    if (directoryId != IContactsContractDirectory::DEFAULT) {
        AutoPtr<IUriBuilder> builder;
        uri->BuildUpon((IUriBuilder**)&builder);
        builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY, StringUtils::ToString(directoryId));
        builder->Build((IUri**)&uri);
    }
    return uri;
}

Boolean ContactEntryListAdapter::IsRemoteDirectory(
    /* [in] */ Int64 directoryId)
{
    return directoryId != IContactsContractDirectory::DEFAULT
                && directoryId != IContactsContractDirectory::LOCAL_INVISIBLE;
}

ECode ContactEntryListAdapter::GetDefaultImageRequestFromCursor(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 displayNameColumn,
    /* [in] */ Int32 lookupKeyColumn,
    /* [out] */ IContactPhotoManagerDefaultImageRequest** imageRequest)
{
    VALIDATE_NOT_NULL(imageRequest)
    String displayName;
    cursor->GetString(displayNameColumn, &displayName);
    String lookupKey;
    cursor->GetString(lookupKeyColumn, &lookupKey);
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request
            = new ContactPhotoManager::DefaultImageRequest(displayName, lookupKey, mCircularPhotos);
    *imageRequest = IContactPhotoManagerDefaultImageRequest::Probe(request);
    REFCOUNT_ADD(*imageRequest)
    return NOERROR;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
