
#include "elastos/apps/dialer/list/SmartDialNumberListAdapter.h"
#include "elastos/apps/dialer/list/DialerPhoneNumberListAdapter.h"
#include "elastos/apps/dialer/dialpad/CSmartDialNameMatcher.h"
#include "elastos/apps/dialer/dialpad/SmartDialPrefix.h"
#include "elastos/utility/logging/Logger.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

const String SmartDialNumberListAdapter::TAG("SmartDialNumberListAdapter");
const Boolean SmartDialNumberListAdapter::DEBUG = FALSE;

CAR_INTERFACE_IMPL(SmartDialNumberListAdapter, DialerPhoneNumberListAdapter, ISmartDialNumberListAdapter);

ECode SmartDialNumberListAdapter::constructor(
    /* [in] */ IContext* context)
{
    DialerPhoneNumberListAdapter::constructor(context);
    CSmartDialNameMatcher::New(String(""), SmartDialPrefix::GetMap(),
            (ISmartDialNameMatcher**)&mNameMatcher);

    if (DEBUG) {
        Logger::V(TAG, "Constructing List Adapter");
    }

    return NOERROR;
}

ECode SmartDialNumberListAdapter::ConfigureLoader(
    /* [in] */ ISmartDialCursorLoader* loader)
{
    String str;
    GetQueryString(&str);
    if (DEBUG) {
        Logger::V(TAG, "Configure Loader with query %s", str.string());
    }

    if (str.IsNull()) {
        loader->ConfigureQuery(String(""));
        mNameMatcher->SetQuery(String(""));
    }
    else {
        loader->ConfigureQuery(str);
        mNameMatcher->SetQuery(PhoneNumberUtils::NormalizeNumber(str));
    }
    return NOERROR;
}

// TODO:
// ECode SmartDialNumberListAdapter::SetHighlight(
//     /* [in] */ IContactListItemView* view,
//     /* [in] */ ICursor* cursor)
// {
//     view->ClearHighlightSequences();

//     String name;
//     cursor->GetString(IPhoneNumberListAdapterPhoneQuery::DISPLAY_NAME, &name);
//     Boolean result;
//     if (mNameMatcher->Matches(name, &result), result) {
//         AutoPtr<IArrayList> nameMatches;
//         mNameMatcher->GetMatchPositions((IArrayList**)&nameMatches);

//         AutoPtr<IIterator> it;
//         nameMatches->GetIterator((IIterator**)&it);
//         Boolean hasNext;
//         while(it->HasNext(&hasNext), hasNext) {
//             AutoPtr<IInterface> item;
//             it->GetNext((IInterface**)&item);
//             CSmartDialMatchPosition* match = (CSmartDialMatchPosition*)item;
//             view->AddNameHighlightSequence(match->mStart, match->mEnd);
//             if (DEBUG) {
//                 String query;
//                 mNameMatcher->GetQuery(&query);
//                 Logger::V(TAG, "%s %s %d", str.string(), query.string(), match->mStart);
//             }
//         }
//     }

//     String number;
//     cursor->GetString(IPhoneNumberListAdapterPhoneQuery::PHONE_NUMBER, &number);
//     AutoPtr<ISmartDialMatchPosition> numberMatch;
//     mNameMatcher->MatchesNumber(number, (ISmartDialMatchPosition**)&numberMatch);
//     if (numberMatch != NULL) {
//         view->AddNumberHighlightSequence(
//                 ((CSmartDialMatchPosition*)numberMatch)->mStart,
//                 ((CSmartDialMatchPosition*)numberMatch)->mEnd);
//     }
//     return NOERROR;
// }

ECode SmartDialNumberListAdapter::GetDataUri(
    /* [in] */ Int32 position,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IInterface> cursor;
    GetItem(position, (IInterface**)&cursor)
    if (cursor != NULL) {
        Int64 id;
        ICursor::Probe(cursor)->GetInt64(
                IPhoneNumberListAdapterPhoneQuery::PHONE_ID, &id);

        AutoPtr<IContactsContractData> data;
        CContactsContractData::AcquireSingleton((IContactsContractData**)&data);
        AutoPtr<IUri> contentUri;
        data->GetCONTENT_URI((IUri**)&contentUri);
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        return contentUris->WithAppendedId(contentUri, id, uri);
    }
    else {
        Logger::W(TAG, "Cursor was null in getDataUri() call. Returning null instead.");
        *uri = NULL;
        return NOERROR;
    }
}

ECode SmartDialNumberListAdapter::SetQueryString(
    /* [in] */ const String& queryString)
{
    String str;
    GetFormattedQueryString(&str);
    const Boolean showNumberShortcuts = !TextUtils::IsEmpty(str);
    Boolean changed = FALSE;
    Boolean result;
    SetShortcutEnabled(SHORTCUT_ADD_NUMBER_TO_CONTACTS, showNumberShortcuts, &result);
    changed |= result;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    assert(0 && "TODO");
    // SetShortcutEnabled(SHORTCUT_MAKE_VIDEO_CALL,
    //         showNumberShortcuts && CallUtil::IsVideoEnabled(context), &result);
    changed |= result
    if (changed) {
        NotifyDataSetChanged();
    }
    return DialerPhoneNumberListAdapter::SetQueryString(queryString);
}

} // List
} // Dialer
} // Apps
} // Elastos
