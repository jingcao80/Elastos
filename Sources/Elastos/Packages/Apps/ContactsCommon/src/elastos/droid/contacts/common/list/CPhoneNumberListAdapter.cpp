
#include "CPhoneNumberListAdapter.h"
#include <StringUtils.h>

using Elastos::Droid::Net::Uri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCallable;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsCallable;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;

using Elastos::Core::StringUtils;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

const String CPhoneNumberListAdapter::PhoneQuery PROJECTION_PRIMARY[] = {
    IContactsContractCommonDataKindsPhone::_ID,                          // 0
    IContactsContractCommonDataKindsPhone::TYPE,                         // 1
    IContactsContractCommonDataKindsPhone::LABEL,                        // 2
    IContactsContractCommonDataKindsPhone::NUMBER,                       // 3
    IContactsContractCommonDataKindsPhone::CONTACT_ID,                   // 4
    IContactsContractCommonDataKindsPhone::LOOKUP_KEY,                   // 5
    IContactsContractCommonDataKindsPhone::PHOTO_ID,                     // 6
    IContactsContractCommonDataKindsPhone::DISPLAY_NAME_PRIMARY,         // 7
    IContactsContractCommonDataKindsPhone::PHOTO_THUMBNAIL_URI,          // 8
}

const String CPhoneNumberListAdapter::PhoneQuery PROJECTION_ALTERNATIVE[] = {
    IContactsContractCommonDataKindsPhone::_ID,                          // 0
    IContactsContractCommonDataKindsPhone::TYPE,                         // 1
    IContactsContractCommonDataKindsPhone::LABEL,                        // 2
    IContactsContractCommonDataKindsPhone::NUMBER,                       // 3
    IContactsContractCommonDataKindsPhone::CONTACT_ID,                   // 4
    IContactsContractCommonDataKindsPhone::LOOKUP_KEY,                   // 5
    IContactsContractCommonDataKindsPhone::PHOTO_ID,                     // 6
    IContactsContractCommonDataKindsPhone::DISPLAY_NAME_ALTERNATIVE,     // 7
    IContactsContractCommonDataKindsPhone::PHOTO_THUMBNAIL_URI,          // 8
}

String createTOO_LONG_CLAUSE()
{
    String str("length(");
    str += IContactsContractCommonDataKindsPhone::NUMBER + ") < 1000";
}

const String CPhoneNumberListAdapter::TAG("CPhoneNumberListAdapter");
const String CPhoneNumberListAdapterIGNORE_NUMBER_TOO_LONG_CLAUSE = createTOO_LONG_CLAUSE();

CAR_INTERFACE_IMPL(CPhoneNumberListAdapter, ContactEntryListAdapter, IPhoneNumberListAdapter)

CAR_OBJECT_IMPL(CPhoneNumberListAdapter)

CPhoneNumberListAdapter::CPhoneNumberListAdapter()
    : mFirstExtendedDirectoryId(0x7FFFFFFFFFFFFFFFL)
{}

CPhoneNumberListAdapter::~CPhoneNumberListAdapter()
{}

ECode CPhoneNumberListAdapter::CPhoneNumberListAdapter::constructor(
    /* [in] */ IContext* context)
{
    ContactEntryListAdapter::constructor(context);
    SetDefaultFilterHeaderText(R::string::list_filter_phones);
    context->GetText(Elastos::R::string::unknownName, (ICharSequence**)&mUnknownNameText);
    mCountryIso = GeoUtil::GetCurrentCountryIso(context);

    assert(0 && "TODO");
    // final ExtendedPhoneDirectoriesManager manager
    //         = ExtensionsFactory.getExtendedPhoneDirectoriesManager();
    // if (manager != null) {
    //     mExtendedDirectories = manager.getExtendedDirectories(mContext);
    // } else {
    //     // Empty list to avoid sticky NPE's
    //     mExtendedDirectories = new ArrayList<DirectoryPartition>();
    // }

    return NOERROR;
}

AutoPtr<ICharSequence> CPhoneNumberListAdapter::GetUnknownNameText()
{
    return mUnknownNameText;
}

ECode CPhoneNumberListAdapter::CPhoneNumberListAdapter::ConfigureLoader(
    /* [in] */ ICursorLoader* loader,
    /* [in] */ Int64 directoryId)
{
    String query;
    GetQueryString(&query);
    if (query.IsNull()) {
        query = "";
    }
    Boolean extended;
    if (IsExtendedDirectory(directoryId, &extended), extended) {
        AutoPtr<IDirectoryPartition> directory = GetExtendedDirectoryFromId(directoryId);
        String contentUri;
        directory->GetContentUri(&contentUri);
        if (contentUri.IsNull()) {
            String str;
            directory->ToString(&str);
            Logger::E(TAG, "Extended directory must have a content URL: %s", str);
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Extended directory must have a content URL: "
            //         + directory);
        }
        AutoPtr<IUri> uri;
        Uri::Parse(contentUri, (IUri**)&uri);
        AutoPtr<IUriBuilder> builder;
        uri->BuildUpon((IUriBuilder**)&builder);
        builder->AppendPath(query);

        Int32 limit;
        GetDirectoryResultLimit(director, &limit);
        builder->AppendQueryParameter(IContactsContract::LIMIT_PARAM_KEY,
                StringUtils::ParseInt32(limit));

        uri = NULL;
        builder->Build((IUri**)&uri);
        loader->SetUri(uri);
        loader->SetProjection(PhoneQuery::PROJECTION_PRIMARY);
    }
    else {
        Boolean isRemoteDirectoryQuery = IsRemoteDirectory(directoryId);
        AutoPtr<IUriBuilder> builder;
        Boolean searchMode;
        if (IsSearchMode(&searchMode), searchMode) {
            AutoPtr<IUri> baseUri;
            if (isRemoteDirectoryQuery) {
                AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                CContactsContractCommonDataKindsPhone::AcquireSingleton(
                        (IContactsContractCommonDataKindsPhone**)&phone);
                phone->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            else if (mUseCallableUri) {
                AutoPtr<IContactsContractCommonDataKindsCallable> callable;
                CContactsContractCommonDataKindsCallable::AcquireSingleton(
                        (IContactsContractCommonDataKindsCallable**)&callable);
                callable->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            else {
                 AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                CContactsContractCommonDataKindsPhone::AcquireSingleton(
                        (IContactsContractCommonDataKindsPhone**)&phone);
                phone->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            baseUri->BuildUpon((IUriBuilder**)&builder);
            builder->AppendPath(query);      // Builder will encode the query
            builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
                    StringUtils::ParseInt32(directoryId));
            if (isRemoteDirectoryQuery) {
                AutoPtr<IDirectoryPartition> partition = GetDirectoryById(directoryId);
                Int32 limit;
                GetDirectoryResultLimit(partition, &limit);
                builder->AppendQueryParameter(IContactsContract::LIMIT_PARAM_KEY,
                        StringUtils::ParseInt32(limit));
            }
        }
        else {
            AutoPtr<IUri> baseUri;
            if (mUseCallableUri) {
                AutoPtr<IContactsContractCommonDataKindsCallable> callable;
                CContactsContractCommonDataKindsCallable::AcquireSingleton(
                        (IContactsContractCommonDataKindsCallable**)&callable);
                callable->GetCONTENT_URI((IUri**)&baseUri);
            }
            else {
                 AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                CContactsContractCommonDataKindsPhone::AcquireSingleton(
                        (IContactsContractCommonDataKindsPhone**)&phone);
                phone->GetCONTENT_URI((IUri**)&baseUri);
            }
            baseUri->BuildUpon((IUriBuilder**)&builder);
            builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
                     StringUtils::ParseInt32(IContactsContractDirectory::DEFAULT));
            Boolean enabled;
            if (IsSectionHeaderDisplayEnabled(&enabled), enabled) {
                builder->AppendQueryParameter(
                    IContactsContractCommonDataKindsPhone::EXTRA_ADDRESS_BOOK_INDEX, String("true"));
            }

            AutoPtr<IContactListFilter> filter;
            GetFilter((IContactListFilter**)&filter);
            ApplyFilter(loader, builder, directoryId, filter);
        }

        // Ignore invalid phone numbers that are too long. These can potentially cause freezes
        // in the UI and there is no reason to display them.
        String prevSelection;
        loader->GetSelection(&prevSelection);
        String newSelection;
        if (!TextUtils::IsEmpty(prevSelection)) {
            newSelection = prevSelection + " AND " + IGNORE_NUMBER_TOO_LONG_CLAUSE;
        }
        else {
            newSelection = IGNORE_NUMBER_TOO_LONG_CLAUSE;
        }
        loader->SetSelection(newSelection);

        // Remove duplicates when it is possible.
        builder->AppendQueryParameter(IContactsContract::REMOVE_DUPLICATE_ENTRIES, String("true"));
        AutoPtr<IUri> uri;
        builder->Build(&uri);
        loader->SetUri(uri);

        // TODO a projection that includes the search snippet
        Int32 order;
        GetContactNameDisplayOrder(&order);
        assert(0 && "TODO");
        // if (order == IContactsPreferences::DISPLAY_ORDER_PRIMARY) {
        //     loader->setProjection(PhoneQuery::PROJECTION_PRIMARY);
        // }
        // else {
        //     loader->setProjection(PhoneQuery::PROJECTION_ALTERNATIVE);
        // }

        // GetSortOrder(&order);
        // if (order == IContactsPreferences.SORT_ORDER_PRIMARY) {
        //     loader->SetSortOrder(IContactsContractCommonDataKindsPhone::SORT_KEY_PRIMARY);
        // }
        // else {
        //     loader->SetSortOrder(IContactsContractCommonDataKindsPhone::SORT_KEY_ALTERNATIVE);
        // }
    }

    return NOERROR;
}

void CPhoneNumberListAdapter::ApplyFilter(
    /* [in] */ ICursorLoader* loader,
    /* [in] */ IUriBuilder* uriBuilder,
    /* [in] */ Int64 directoryId,
    /* [in] */ IContactListFilter* filter)
{
    if (filter == NULL || directoryId != IContactsContractDirectory::DEFAULT) {
        return;
    }

    AutoPtr<StringBuilder> selection = new StringBuilder();
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    IList* selectionArgs = IList::Probe(list);

    assert(0 && "TODO");
    // switch (filter.filterType) {
    //     case IContactListFilter::FILTER_TYPE_CUSTOM: {
    //         selection.append(Contacts.IN_VISIBLE_GROUP + "=1");
    //         selection.append(" AND " + Contacts.HAS_PHONE_NUMBER + "=1");
    //         break;
    //     }
    //     case IContactListFilter::FILTER_TYPE_ACCOUNT: {
    //         filter.addAccountQueryParameterToUrl(uriBuilder);
    //         break;
    //     }
    //     case IContactListFilter::FILTER_TYPE_ALL_ACCOUNTS:
    //     case IContactListFilter::FILTER_TYPE_DEFAULT:
    //         break; // No selection needed.
    //     case IContactListFilter::FILTER_TYPE_WITH_PHONE_NUMBERS_ONLY:
    //         break; // This adapter is always "phone only", so no selection needed either.
    //     default:
    //         Log.w(TAG, "Unsupported filter type came " +
    //                 "(type: " + filter.filterType + ", toString: " + filter + ")" +
    //                 " showing all contacts.");
    //         // No selection.
    //         break;
    // }
    // loader.setSelection(selection.toString());
    // loader.setSelectionArgs(selectionArgs.toArray(new String[0]));
}

ECode CPhoneNumberListAdapter::GetContactDisplayName(
    /* [in] */ Int32 position,
    /* [out] */ String* displayName)
{
    VALUE_NOT_NULL(displayName);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberListAdapter::GetPhoneNumber(
    /* [in] */ Int32 position,
    /* [out] */ String* displayName)
{
    VALUE_NOT_NULL(displayName);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberListAdapter::GetDataUri(
    /* [in] */ Int32 position,
    /* [out] */ IUri* uri)
{
    VALUE_NOT_NULL(uri);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberListAdapter::GetDataUri(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor,
    /* [out] */ IUri* uri)
{
    VALUE_NOT_NULL(uri);
    return E_NOT_IMPLEMENTED;
}

AutoPtr<IView> CPhoneNumberListAdapter::NewView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int position,
    /* [in] */ IViewGroup* parent)
{
    assert(0 && "TODO");
    return NULL;
}

void CPhoneNumberListAdapter::SetHighlight(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor)
{
    assert(0 && "TODO");
}

Int32 CPhoneNumberListAdapter::GetResultCount(
    /* [in] */ ICursor* cursor)
{
    assert(0 && "TODO");
}

void CPhoneNumberListAdapter::BindView(
    /* [in] */ IView* itemView,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position)
{
    assert(0 && "TODO");
}

void CPhoneNumberListAdapter::BindPhoneNumber(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean displayNumber)
{
    assert(0 && "TODO");
}

void CPhoneNumberListAdapter::BindSectionHeaderAndDivider(
    /* [in] */ IContactListItemView* view,
    /* [in] */ Int32 position)
{
    assert(0 && "TODO");
}

void CPhoneNumberListAdapter::CPhoneNumberListAdapter::BindName(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor)
{
    assert(0 && "TODO");
}

void CPhoneNumberListAdapter::UnbindName(
    /* [in] */ IContactListItemView* view)
{
    assert(0 && "TODO");
}

void CPhoneNumberListAdapter::BindPhoto(
    /* [in] */ IContactListItemView* view,
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor)
{
    assert(0 && "TODO");
}

ECode CPhoneNumberListAdapter::SetPhotoPosition(
    /* [in] */ IContactListItemView::PhotoPosition photoPosition)
{
    VALUE_NOT_NULL(uri);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberListAdapter::GetPhotoPosition(
    /* [out] */ IContactListItemView::PhotoPosition* photoPosition)
{
    VALUE_NOT_NULL(uri);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberListAdapter::SetUseCallableUri(
    /* [in] */ Boolean useCallableUri)
{
    VALUE_NOT_NULL(uri);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberListAdapter::UsesCallableUri(
    /* [out] */ Boolean* useCallableUri)
{
    VALUE_NOT_NULL(uri);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberListAdapter::ChangeDirectories(
    /* [in] */ ICursor* cursor)
{
    VALUE_NOT_NULL(uri);
    return E_NOT_IMPLEMENTED;
}

AutoPtr<IUri> CPhoneNumberListAdapter::GetContactUri(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 contactIdColumn,
    /* [in] */ Int32 lookUpKeyColumn)
{
    assert(0 && "TODO");
}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
