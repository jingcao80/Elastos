
#include "elastos/droid/dialer/list/RegularSearchListAdapter.h"
#include "elastos/droid/dialer/list/DialerPhoneNumberListAdapter.h"
#include "elastos/droid/contacts/common/CallUtil.h"
// #include "elastos/droid/dialer/calllog/CContactInfo.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::Contacts::Common::List::IDirectoryPartition;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Text::TextUtils;
// using Elastos::Droid::Dialer::CallLog::IContactInfo;
// using Elastos::Apps::Dialer::CallLog::CContactInfo;
// using Elastos::Droid::Dialer::CallLog::IPhoneQuery;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CAR_INTERFACE_IMPL(RegularSearchListAdapter, DialerPhoneNumberListAdapter, IRegularSearchListAdapter);

ECode RegularSearchListAdapter::constructor(
    /* [in] */ IContext* context)
{
    return DialerPhoneNumberListAdapter::constructor(context);
}

ECode RegularSearchListAdapter::GetContactInfo(
    /* [in] */ ICachedNumberLookupService* lookupService,
    /* [in] */ Int32 position,
    /* [out] */ ICachedContactInfo** info)
{
    VALIDATE_NOT_NULL(info);
assert(0);
    // AutoPtr<IContactInfo> contatctInfo;
    // CContactInfo::NewByFriend((IContactInfo**)&contatctInfo);
    // AutoPtr<ICachedContactInfo> cacheInfo;
    // lookupService->BuildCachedContactInfo(info, (ICachedContactInfo**)&cacheInfo);

    // AutoPtr<IInterface> temp;
    // GetItem(position, (IInterface**)&temp);
    // ICursor* item = ICursor::Probe(temp);
    // if (item != NULL) {
    //     item->GetString(IPhoneQuery::DISPLAY_NAME, &contatctInfo->mName);
    //     item->GetInt(IPhoneQuery::PHONE_TYPE, &contatctInfo->mType);
    //     item->GetString(IPhoneQuery::PHONE_LABEL, &contatctInfo->mLabel);
    //     item->GetString(IPhoneQuery::PHONE_NUMBER, &contatctInfo->mNumber);
    //     String photoUriStr;
    //     item->GetString(IPhoneQuery::PHOTO_URI, &photoUriStr);
    //     AutoPtr<IUriHelper> helper;
    //     CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    //     if (photoUriStr.IsNull()) {
    //         contatctInfo->mPhotoUri = NULL;
    //     }
    //     else {
    //         helper->Parse(photoUriStr, (IUri**)&contatctInfo->mPhotoUri);
    //     }

    //     String lookupKey;
    //     item->GetString(IPhoneQuery::LOOKUP_KEY, &lookupKey);
    //     cacheInfo->SetLookupKey(lookupKey);

    //     Int32 partitionIndex;
    //     GetPartitionForPosition(position, &partitionIndex);
    //     AutoPtr<ICompositeCursorAdapterPartition> temp;
    //     GetPartition(partitionIndex, (ICompositeCursorAdapterPartition**)&temp);
    //     AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
    //     Int64 directoryId;
    //     partition->GetDirectoryId(&directoryId);
    //     String sourceName;
    //     partition->GetLabel(&sourceName);
    //     if (IsExtendedDirectory(directoryId)) {
    //         cacheInfo->SetExtendedSource(sourceName, directoryId);
    //     }
    //     else {
    //         cacheInfo->SetDirectorySource(sourceName, directoryId);
    //     }
    // }
    // *info = ICachedContactInfo::Probe(cacheInfo);
    // REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode RegularSearchListAdapter::SetQueryString(
    /* [in] */ const String& queryString)
{
    String str;
    GetFormattedQueryString(&str);
    Boolean showNumberShortcuts = !TextUtils::IsEmpty(str);
    Boolean changed = FALSE;
    Boolean result;
    SetShortcutEnabled(SHORTCUT_DIRECT_CALL, showNumberShortcuts, &result);
    changed |= result;
    // Either one of the add contacts options should be enabled. If the user entered
    // a dialable number, then clicking add to contact should add it as a number.
    // Otherwise, it should add it to a new contact as a name.
    SetShortcutEnabled(SHORTCUT_ADD_NUMBER_TO_CONTACTS, showNumberShortcuts, &result);
    changed |= result;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    SetShortcutEnabled(SHORTCUT_MAKE_VIDEO_CALL,
            showNumberShortcuts && CallUtil::IsVideoEnabled(context), &result);
    changed |= result;
    if (changed) {
        NotifyDataSetChanged();
    }
    return DialerPhoneNumberListAdapter::SetQueryString(queryString);
}

} // List
} // Dialer
} // Droid
} // Elastos
