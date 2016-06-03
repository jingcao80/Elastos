
#include "elastos/droid/internal/telephony/IccProvider.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/telephony/CSubInfoRecord.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CUriMatcher.h"
#include "elastos/droid/database/CMatrixCursor.h"
#include "elastos/droid/database/CMergeCursor.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Database::CMergeCursor;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Telephony::CSubInfoRecord;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String IccProvider::STR_TAG("tag");
const String IccProvider::STR_NUMBER("number");
const String IccProvider::STR_EMAILS("emails");
const String IccProvider::STR_ANRS("anrs");
const String IccProvider::STR_NEW_TAG("newTag");
const String IccProvider::STR_NEW_NUMBER("newNumber");
const String IccProvider::STR_NEW_EMAILS("newEmails");
const String IccProvider::STR_NEW_ANRS("newAnrs");
const String IccProvider::STR_PIN2("pin2");
const Int32 IccProvider::ADN = 1;
const Int32 IccProvider::ADN_SUB = 2;
const Int32 IccProvider::FDN = 3;
const Int32 IccProvider::FDN_SUB = 4;
const Int32 IccProvider::SDN = 5;
const Int32 IccProvider::SDN_SUB = 6;
const Int32 IccProvider::ADN_ALL = 7;
const String IccProvider::TAG("IccProvider");
const Boolean IccProvider::DBG = FALSE;
const AutoPtr<ArrayOf<String> > IccProvider::ADDRESS_BOOK_COLUMN_NAMES = InitNames();
const AutoPtr<IUriMatcher> IccProvider::URL_MATCHER = InitUriMatcher();

AutoPtr<ArrayOf<String> > IccProvider::InitNames()
{
    AutoPtr<ArrayOf<String> > names = ArrayOf<String>::Alloc(5);
    (*names)[0] = String("name");
    (*names)[1] = String("number");
    (*names)[2] = String("emails");
    (*names)[3] = String("anrs");
    (*names)[4] = String("_id");
    return names;
}

AutoPtr<IUriMatcher> IccProvider::InitUriMatcher()
{
    AutoPtr<IUriMatcher> um;
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&um);
    um->AddURI(String("icc"), String("adn"), ADN);
    um->AddURI(String("icc"), String("adn/subId/#"), ADN_SUB);
    um->AddURI(String("icc"), String("fdn"), FDN);
    um->AddURI(String("icc"), String("fdn/subId/#"), FDN_SUB);
    um->AddURI(String("icc"), String("sdn"), SDN);
    um->AddURI(String("icc"), String("sdn/subId/#"), SDN_SUB);
    um->AddURI(String("icc"), String("adn/adn_all"), ADN_ALL);
    return um;
}

// @Override
ECode IccProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

// @Override
ECode IccProvider::Query(
    /* [in] */ IUri* url,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sort,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Log(String("query"));

    Int64 iv = 0;
    SubscriptionManager::GetDefaultSubId(&iv);

    Int32 value = 0;
    URL_MATCHER->Match(url, &value);
    switch (value) {
        case ADN: {
            *result = ICursor::Probe(LoadFromEf(IIccConstants::EF_ADN, iv));
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case ADN_SUB: {
            *result = ICursor::Probe(LoadFromEf(IIccConstants::EF_ADN, GetRequestSubId(url)));
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case FDN: {
            *result = ICursor::Probe(LoadFromEf(IIccConstants::EF_FDN, iv));
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case FDN_SUB: {
            *result = ICursor::Probe(LoadFromEf(IIccConstants::EF_FDN, GetRequestSubId(url)));
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case SDN: {
            *result = ICursor::Probe(LoadFromEf(IIccConstants::EF_SDN, iv));
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case SDN_SUB: {
            *result = ICursor::Probe(LoadFromEf(IIccConstants::EF_SDN, GetRequestSubId(url)));
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case ADN_ALL:{
            *result = LoadAllSimContacts(IIccConstants::EF_ADN);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        // default:
        //     throw new IllegalArgumentException("Unknown URL " + url);
    }
    *result = NULL;
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

AutoPtr<ICursor> IccProvider::LoadAllSimContacts(
    /* [in] */ Int32 efType)
{
    AutoPtr<ArrayOf<ICursor*> > result;
    AutoPtr<IList/*<SubInfoRecord*/> subInfoList;
    SubscriptionManager::GetActiveSubInfoList((IList**)&subInfoList);

    Int32 size = 0;
    if ((subInfoList == NULL) || ((subInfoList->GetSize(&size), size) == 0)) {
        result = ArrayOf<ICursor*>::Alloc(0);
    }
    else {
        Int32 subIdCount = size;
        result = ArrayOf<ICursor*>::Alloc(subIdCount);
        Int64 subId = 0;

        CSubInfoRecord* sr = NULL;
        for (Int32 i = 0; i < subIdCount; i++) {
            AutoPtr<IInterface> obj;
            subInfoList->Get(i, (IInterface**)&obj);
            sr = (CSubInfoRecord*)ISubInfoRecord::Probe(obj);
            subId = sr->mSubId;
            result->Set(i, ICursor::Probe(LoadFromEf(efType, subId)));
            Logger::I(TAG, "ADN Records loaded for Subscription ::%lld", subId);
        }
    }

    AutoPtr<ICursor> cursor;
    CMergeCursor::New(result, (ICursor**)&cursor);
    return cursor;
}

// @Override
ECode IccProvider::GetType(
    /* [in] */ IUri* url,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 match = 0;
    switch (URL_MATCHER->Match(url, &match), match) {
        case ADN:
        case ADN_SUB:
        case FDN:
        case FDN_SUB:
        case SDN:
        case SDN_SUB:
        case ADN_ALL: {
            *result = String("vnd.android.cursor.dir/sim-contact");
            return NOERROR;
        }

        // default:
        //     throw new IllegalArgumentException("Unknown URL " + url);
    }
    *result = String(NULL);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

// @Override
ECode IccProvider::Insert(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUri> resultUri;
    Int32 efType = 0;
    String pin2;
    Int64 subId = 0;

    if (DBG) Log(String("insert"));

    Int32 match = 0;
    URL_MATCHER->Match(url, &match);
    switch (match) {
        case ADN:
            efType = IIccConstants::EF_ADN;
            SubscriptionManager::GetDefaultSubId(&subId);
            break;

        case ADN_SUB:
            efType = IIccConstants::EF_ADN;
            subId = GetRequestSubId(url);
            break;

        case FDN:
            efType = IIccConstants::EF_FDN;
            SubscriptionManager::GetDefaultSubId(&subId);
            initialValues->GetAsString(String("pin2"), &pin2);
            break;

        case FDN_SUB:
            efType = IIccConstants::EF_FDN;
            subId = GetRequestSubId(url);
            initialValues->GetAsString(String("pin2"), &pin2);
            break;

        default: {
            // throw new UnsupportedOperationException(
            //         "Cannot insert into URL: " + url);
            *result = NULL;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    String tag;
    initialValues->GetAsString(String("tag"), &tag);
    String number;
    initialValues->GetAsString(String("number"), &number);
    String emails;
    initialValues->GetAsString(String("emails"), &emails);
    String anrs;
    initialValues->GetAsString(String("anrs"), &anrs);

    // TODO(): Read email instead of sending NULL.
    AutoPtr<IContentValues> mValues;
    CContentValues::New((IContentValues**)&mValues);
    mValues->Put(STR_TAG, String(""));
    mValues->Put(STR_NUMBER, String(""));
    mValues->Put(STR_EMAILS, String(""));
    mValues->Put(STR_ANRS, String(""));
    mValues->Put(STR_NEW_TAG, tag);
    mValues->Put(STR_NEW_NUMBER, number);
    mValues->Put(STR_NEW_EMAILS, emails);
    mValues->Put(STR_NEW_ANRS, anrs);
    Boolean success = UpdateIccRecordInEf(efType, mValues, pin2, subId);

    if (!success) {
        *result = NULL;
        return NOERROR;
    }

    StringBuilder buf("content://icc/");
    switch (match) {
        case ADN:
            buf.Append("adn/");
            break;

        case ADN_SUB:
            buf.Append("adn/subId/");
            break;

        case FDN:
            buf.Append("fdn/");
            break;

        case FDN_SUB:
            buf.Append("fdn/subId/");
            break;
    }

    // TODO: we need to find out the rowId for the newly added record
    buf.Append(0);

    Uri::Parse(buf.ToString(), (IUri**)&resultUri);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cr->NotifyChange(url, NULL);
    /*
    // notify interested parties that an insertion happened
    getContext().getContentResolver().notifyInsert(
            resultUri, rowID, NULL);
    */

    *result = resultUri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

String IccProvider::NormalizeValue(
    /* [in] */ const String& inVal)
{
    Int32 len = inVal.GetLength();
    // If name is empty in contact return NULL to avoid crash.
    if (len == 0) {
        if (DBG) Log(String("len of input String is 0"));
        return inVal;
    }
    String retVal = inVal;

    if (inVal.GetChar(0) == '\'' && inVal.GetChar(len - 1) == '\'') {
        retVal = inVal.Substring(1, len - 1);
    }

    return retVal;
}

// @Override
ECode IccProvider::Delete(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 efType = 0;
    Int64 subId = 0;

    if (DBG) Log(String("delete"));
    Int32 match = 0;
    URL_MATCHER->Match(url, &match);
    switch (match) {
        case ADN:
            efType = IIccConstants::EF_ADN;
            SubscriptionManager::GetDefaultSubId(&subId);
            break;

        case ADN_SUB:
            efType = IIccConstants::EF_ADN;
            subId = GetRequestSubId(url);
            break;

        case FDN:
            efType = IIccConstants::EF_FDN;
            SubscriptionManager::GetDefaultSubId(&subId);
            break;

        case FDN_SUB:
            efType = IIccConstants::EF_FDN;
            subId = GetRequestSubId(url);
            break;

        default: {
            // throw new UnsupportedOperationException(
            //         "Cannot insert into URL: " + url);
            *result = 0;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    // parse where clause
    String tag;
    String number;
    String emails;
    String anrs;
    String pin2;

    AutoPtr<ArrayOf<String> > tokens;
    StringUtils::Split(where, String("AND"), (ArrayOf<String>**)&tokens);
    Int32 n = tokens->GetLength();

    while (--n >= 0) {
        String param = (*tokens)[n];
        if (DBG) Log(String("parsing '") + param + "'");

        AutoPtr<ArrayOf<String> > pair;
        StringUtils::Split(param, String("="), 2, (ArrayOf<String>**)&pair);

        if (pair->GetLength() != 2) {
            Logger::E(TAG, "resolve: bad whereClause parameter: %s", param.string());
            continue;
        }
        String key = (*pair)[0].Trim();
        String val = (*pair)[1].Trim();

        if (STR_TAG.Equals(key)) {
            tag = NormalizeValue(val);
        }
        else if (STR_NUMBER.Equals(key)) {
            number = NormalizeValue(val);
        }
        else if (STR_EMAILS.Equals(key)) {
            emails = NormalizeValue(val);
        }
        else if (STR_ANRS.Equals(key)) {
            anrs = NormalizeValue(val);
        }
        else if (STR_PIN2.Equals(key)) {
            pin2 = NormalizeValue(val);
        }
    }

    AutoPtr<IContentValues> mValues;
    CContentValues::New((IContentValues**)&mValues);
    mValues->Put(STR_TAG, tag);
    mValues->Put(STR_NUMBER, number);
    mValues->Put(STR_EMAILS, emails);
    mValues->Put(STR_ANRS, anrs);
    mValues->Put(STR_NEW_TAG, String(""));
    mValues->Put(STR_NEW_NUMBER, String(""));
    mValues->Put(STR_NEW_EMAILS, String(""));
    mValues->Put(STR_NEW_ANRS, String(""));
    if ((efType == FDN) && TextUtils::IsEmpty(pin2)) {
        *result = 0;
        return NOERROR;
    }

    if (DBG) Log(String("delete mvalues= ") + TO_CSTR(mValues));
    Boolean success = UpdateIccRecordInEf(efType, mValues, pin2, subId);
    if (!success) {
        *result = 0;
        return NOERROR;
    }

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cr->NotifyChange(url, NULL);
    *result = 1;
    return NOERROR;
}

// @Override
ECode IccProvider::Update(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    String pin2;
    Int32 efType = 0;
    Int64 subId = 0;

    if (DBG) Log(String("update"));

    Int32 match = 0;
    URL_MATCHER->Match(url, &match);
    switch (match) {
        case ADN:
            efType = IIccConstants::EF_ADN;
            SubscriptionManager::GetDefaultSubId(&subId);
            break;

        case ADN_SUB:
            efType = IIccConstants::EF_ADN;
            subId = GetRequestSubId(url);
            break;

        case FDN:
            efType = IIccConstants::EF_FDN;
            SubscriptionManager::GetDefaultSubId(&subId);
            values->GetAsString(String("pin2"), &pin2);
            break;

        case FDN_SUB:
            efType = IIccConstants::EF_FDN;
            subId = GetRequestSubId(url);
            values->GetAsString(String("pin2"), &pin2);
            break;

        default: {
            // throw new UnsupportedOperationException(
            //         "Cannot insert into URL: " + url);
            *result = 0;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    String tag;
    values->GetAsString(String("tag"), &tag);
    String number;
    values->GetAsString(String("number"), &number);
    // AutoPtr<ArrayOf<String> > emails;
    String newTag;
    values->GetAsString(String("newTag"), &newTag);
    String newNumber;
    values->GetAsString(String("newNumber"), &newNumber);
    // AutoPtr<ArrayOf<String> > newEmails;
    // TODO(): Update for email.
    Boolean success = UpdateIccRecordInEf(efType, values, pin2, subId);

    if (!success) {
        *result = 0;
        return NOERROR;
    }

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cr->NotifyChange(url, NULL);
    *result = 1;
    return NOERROR;
}

AutoPtr<IMatrixCursor> IccProvider::LoadFromEf(
    /* [in] */ Int32 efType,
    /* [in] */ Int64 subId)
{
    if (DBG) Log(String("loadFromEf: efType=") + StringUtils::ToString(efType)
        + ", subscription=" + StringUtils::ToString(subId));

    AutoPtr<IList/*<AdnRecord*/> adnRecords;
    // try {
    AutoPtr<IIIccPhoneBook> iccIpb = IIIccPhoneBook::Probe(
            ServiceManager::GetService(String("simphonebook")));
    if (iccIpb != NULL) {
        iccIpb->GetAdnRecordsInEfForSubscriber(subId, efType, (IList**)&adnRecords);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // } catch (SecurityException ex) {
    //     if (DBG) Log(ex.toString());
    // }

    AutoPtr<IMatrixCursor> cursor;
    if (adnRecords != NULL) {
        // Load the results
        Int32 N = 0;
        adnRecords->GetSize(&N);
        CMatrixCursor::New(ADDRESS_BOOK_COLUMN_NAMES, N, (IMatrixCursor**)&cursor);
        Log(String("adnRecords.size=") + StringUtils::ToString(N));
        for (Int32 i = 0; i < N ; i++) {
            AutoPtr<IInterface> obj;
            adnRecords->Get(i, (IInterface**)&obj);
            LoadRecord(IAdnRecord::Probe(obj), cursor, i);
        }
        return cursor;
    }

    // No results to load
    Logger::W(TAG, "Cannot load ADN records");
    CMatrixCursor::New(ADDRESS_BOOK_COLUMN_NAMES, (IMatrixCursor**)&cursor);
    return cursor;
}

Boolean IccProvider::UpdateIccRecordInEf(
    /* [in] */ Int32 efType,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& pin2,
    /* [in] */ Int64 subId)
{
    Boolean success = FALSE;

    if (DBG) Log(String("updateIccRecordInEf: efType=") + StringUtils::ToString(efType) +
                ", values: [ "+ TO_CSTR(values) + " ], subId:" + StringUtils::ToString(subId));

    // try {
    AutoPtr<IIIccPhoneBook> iccIpb = IIIccPhoneBook::Probe(
            ServiceManager::GetService(String("simphonebook")));
    if (iccIpb != NULL) {
        iccIpb->UpdateAdnRecordsWithContentValuesInEfBySearchUsingSubId(
                subId, efType, values, pin2, &success);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // } catch (SecurityException ex) {
    //     if (DBG) Log(ex.toString());
    // }
    if (DBG) Log(String("updateIccRecordInEf: ") + StringUtils::ToString(success));
    return success;
}

/**
 * Loads an AdnRecord into a MatrixCursor. Must be called with mLock held.
 *
 * @param record the ADN record to load from
 * @param cursor the cursor to receive the results
 */
void IccProvider::LoadRecord(
    /* [in] */ IAdnRecord* record,
    /* [in] */ IMatrixCursor* cursor,
    /* [in] */ Int32 id)
{
    Boolean tmp = FALSE;
    if (record->IsEmpty(&tmp), !tmp) {
        AutoPtr<ArrayOf<IInterface*> > contact = ArrayOf<IInterface*>::Alloc(5);
        String alphaTag;
        record->GetAlphaTag(&alphaTag);
        String number;
        record->GetNumber(&number);
        AutoPtr<ArrayOf<String> > anrs;
        record->GetAdditionalNumbers((ArrayOf<String>**)&anrs);
        if (DBG) Log(String("loadRecord: ") + alphaTag + ", " + number + ",");
        contact->Set(0, CoreUtils::Convert(alphaTag));
        contact->Set(1, CoreUtils::Convert(number));

        AutoPtr<ArrayOf<String> > emails;
        record->GetEmails((ArrayOf<String>**)&emails);
        if (emails != NULL) {
            StringBuilder emailString;
            Int32 len = emails->GetLength();
            for (Int32 i = 0; i < len; i++) {
                String email = (*emails)[i];
                if (DBG) Log(String("Adding email:") + email);
                emailString.Append(email);
                emailString.Append(",");
            }
            contact->Set(2, CoreUtils::Convert(emailString.ToString()));
        }

        if (anrs != NULL) {
            StringBuilder anrString;
            Int32 len = anrs->GetLength();
            for (Int32 i = 0; i < len; i++) {
                String anr = (*anrs)[i];
                if (DBG) Log(String("Adding anr:") + anr);
                anrString.Append(anr);
                anrString.Append(",");
            }
            contact->Set(3, CoreUtils::Convert(anrString.ToString()));
        }

        contact->Set(4, CoreUtils::Convert(id));
        cursor->AddRow(*contact);
    }
}

void IccProvider::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, "[IccProvider] %s", msg.string());
}

Int64 IccProvider::GetRequestSubId(
    /* [in] */ IUri* url)
{
    if (DBG) Log(String("getRequestSubId url: ") + TO_CSTR(url));

    String v;
    url->GetLastPathSegment(&v);
    Int64 lv = 0;
    if (FAILED(StringUtils::Parse(v, &lv))) {
        // throw new IllegalArgumentException("Unknown URL " + url);
        assert(0);
    }
    return lv;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
