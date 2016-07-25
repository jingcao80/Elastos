
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CCalls.h"
#include "elastos/droid/provider/CContactsContractDataUsageFeedback.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsCallable.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsPhone.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include "elastos/droid/text/TextUtils.h"
// #include "elastos/droid/telephony/CPhoneNumberUtils.h"
#include <elastos/coredef.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Internal.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ICountryDetector;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsCallable;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractDataUsageFeedback;
using Elastos::Droid::Provider::IContactsContractDataUsageFeedback;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCallable;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
// using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::IO::ICloseable;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;


namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CCalls)

CAR_INTERFACE_IMPL(CCalls, Singleton, ICalls)

const Int32 CCalls::MIN_DURATION_FOR_NORMALIZED_NUMBER_UPDATE_MS = 1000 * 10;

static AutoPtr<IUri> initCONTENTURI()
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(String("content://call_log/calls"), (IUri**)&uri);
    return uri;
}

static AutoPtr<IUri> initCONTENTFILTERURI()
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(String("content://call_log/calls/filter"), (IUri**)&uri);
    return uri;
}

static AutoPtr<IUri> initCONTENTURIWITHVOICEMAIL()
{
    AutoPtr<IUri> uri = initCONTENTURI();
    AutoPtr<IUriBuilder> result;
    uri->BuildUpon((IUriBuilder**)&result);
    result->AppendQueryParameter(ICalls::ALLOW_VOICEMAILS_PARAM_KEY, String("true"));
    AutoPtr<IUri> _uri;
    result->Build((IUri**)&_uri);
    return _uri;
}

AutoPtr<IUri> CCalls::CONTENT_URI = initCONTENTURI();
AutoPtr<IUri> CCalls::CONTENT_FILTER_URI = initCONTENTFILTERURI();
AutoPtr<IUri> CCalls::CONTENT_URI_WITH_VOICEMAIL = initCONTENTURIWITHVOICEMAIL();

ECode CCalls::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CCalls::GetCONTENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = CONTENT_FILTER_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CCalls::GetCONTENT_URI_WITH_VOICEMAIL(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = CONTENT_URI_WITH_VOICEMAIL;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CCalls::AddCall(
    /* [in] */ ICallerInfo* ci,
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 presentation,
    /* [in] */ Int32 callType,
    /* [in] */ Int32 features,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int64 start,
    /* [in] */ Int32 duration,
    /* [in] */ Int64 dataUsage,
    /* [out] */ IUri** uri)
{
    return AddCall(ci, context, number, presentation, callType, features, accountHandle,
                    start, duration, dataUsage, FALSE, DURATION_TYPE_ACTIVE, uri);
}

ECode CCalls::AddCall(
    /* [in] */ ICallerInfo* ci,
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 presentation,
    /* [in] */ Int32 callType,
    /* [in] */ Int32 features,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int64 start,
    /* [in] */ Int32 duration,
    /* [in] */ Int64 dataUsage,
    /* [in] */ Boolean addForAllUsers,
    /* [out] */ IUri** uri)
{
    return AddCall(ci, context, number, presentation, callType, features, accountHandle,
                    start, duration, dataUsage, addForAllUsers, DURATION_TYPE_ACTIVE, uri);
}

ECode CCalls::AddCall(
    /* [in] */ ICallerInfo* ci,
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 presentation,
    /* [in] */ Int32 callType,
    /* [in] */ Int32 features,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int64 start,
    /* [in] */ Int32 duration,
    /* [in] */ Int64 dataUsage,
    /* [in] */ Boolean addForAllUsers,
    /* [in] */ Int32 durationType,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 numberPresentation = ICalls::PRESENTATION_ALLOWED;

    // Remap network specified number presentation types
    // PhoneConstants.PRESENTATION_xxx to calllog number presentation types
    // Calls.PRESENTATION_xxx, in order to insulate the persistent calllog
    // from any future radio changes.
    // If the number field is empty set the presentation type to Unknown.
    String _number = number;
    if (presentation == IPhoneConstants::PRESENTATION_RESTRICTED) {
        numberPresentation = ICalls::PRESENTATION_RESTRICTED;
    } else if (presentation == IPhoneConstants::PRESENTATION_PAYPHONE) {
        numberPresentation = ICalls::PRESENTATION_PAYPHONE;
    } else if (TextUtils::IsEmpty(number)
            || presentation == IPhoneConstants::PRESENTATION_UNKNOWN) {
        numberPresentation = ICalls::PRESENTATION_UNKNOWN;
    }
    if (numberPresentation != ICalls::PRESENTATION_ALLOWED) {
        if (ci != NULL) {
            ci->SetName(String(""));
        }
    }

    // accountHandle information
    String accountComponentString;
    String accountId;
    if (accountHandle != NULL) {
        AutoPtr<IComponentName> icn;
        accountHandle->GetComponentName((IComponentName**)&icn);
        icn->FlattenToString(&accountComponentString);
        accountHandle->GetId(&accountId);
    }

    AutoPtr<IContentValues> values;
    CContentValues::New(6, (IContentValues**)&values);
    values->Put(NUMBER, number);
    values->Put(NUMBER_PRESENTATION, StringUtils::ToString(numberPresentation));
    values->Put(TYPE, StringUtils::ToString(callType));
    values->Put(FEATURES, features);
    values->Put(DATE, StringUtils::ToString(start));
    values->Put(DURATION, StringUtils::ToString(duration));
    values->Put(DURATION_TYPE, StringUtils::ToString(durationType));
    if (dataUsage != -1 )
        values->Put(DATA_USAGE, dataUsage);
    values->Put(PHONE_ACCOUNT_COMPONENT_NAME, accountComponentString);
    values->Put(PHONE_ACCOUNT_ID, accountId);
    values->Put(NEW, StringUtils::ToString(1));

    if (callType == ICalls::MISSED_TYPE) {
        values->Put(ICalls::IS_READ, StringUtils::ToString(0));
    }
    if (ci != NULL) {
        String name;
        ci->GetName(&name);
        AutoPtr<ICharSequence> csqName;
        values->Put(ICalls::CACHED_NAME, csqName);

        Int32 type;
        ci->GetNumberType(&type);
        values->Put(ICalls::CACHED_NUMBER_TYPE, type);

        String lable;
        ci->GetNumberLabel(&lable);
        values->Put(ICalls::CACHED_NUMBER_LABEL, lable);
    }

    Int64 pId;
    if (ci != NULL)
        ci->GetContactIdOrZero(&pId);
    if ((ci != NULL) && (pId > 0)) {
        // Update usage information for the number associated with the contact ID.
        // We need to use both the number and the ID for obtaining a data ID since other
        // contacts may have the same number.

        AutoPtr<ICursor> cursor;

        // We should prefer normalized one (probably coming from
        // Phone.NORMALIZED_NUMBER column) first. If it isn't available try others.
        String normalNum;
        ci->GetNormalizedNumber(&normalNum);
        if (!normalNum.IsNull()) {
            const String normalizedPhoneNumber = normalNum;
            AutoPtr<IUri> uri;
            AutoPtr<IContactsContractCommonDataKindsPhone> helper;
            FAIL_RETURN(CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&helper))
            FAIL_RETURN(helper->GetCONTENT_URI((IUri**)&uri))
            AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
            (*projection)[0] = IBaseColumns::ID;
            AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(2);
            (*selectionArgs)[0] = StringUtils::ToString(pId);
            (*selectionArgs)[1] = normalizedPhoneNumber;

            StringBuilder builder;
            builder += IContactsContractRawContactsColumns::CONTACT_ID;
            builder += " =? AND ";
            builder += IContactsContractCommonDataKindsPhone::NORMALIZED_NUMBER;
            builder += " =?";
            String selection = builder.ToString();
            FAIL_RETURN(resolver->Query(uri, projection, selection, selectionArgs, String(NULL), (ICursor**)&cursor))
        } else {
            String pNumber;
            FAIL_RETURN(ci->GetPhoneNumber(&pNumber))
            const String phoneNumber = !pNumber.IsNull() ? pNumber : _number;
            AutoPtr<IUri> _uri, uri;
            AutoPtr<IContactsContractCommonDataKindsCallable> helper;
            FAIL_RETURN(CContactsContractCommonDataKindsCallable::AcquireSingleton((IContactsContractCommonDataKindsCallable**)&helper))
            FAIL_RETURN(helper->GetCONTENT_URI((IUri**)&_uri))
            String encoded;
            FAIL_RETURN(Uri::Encode(phoneNumber, &encoded))
            FAIL_RETURN(Uri::WithAppendedPath(_uri, encoded, (IUri**)&uri))
            AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
            (*projection)[0] = IBaseColumns::ID;
            AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
            (*selectionArgs)[0] = StringUtils::ToString(pId);
            FAIL_RETURN(resolver->Query(uri, projection, IContactsContractRawContactsColumns::CONTACT_ID + String(" =?"), selectionArgs, String(NULL), (ICursor**)&cursor))
        }

        if (cursor != NULL) {
            //try {
            Int32 count;
            FAIL_GOTO(cursor->GetCount(&count), EXIT)
            Boolean isFirst;
            FAIL_GOTO(cursor->MoveToFirst(&isFirst), EXIT)
            if (count > 0 && isFirst) {
                String dataId;
                cursor->GetString(0, &dataId);
                String normalizedNum;
                ci->GetNormalizedNumber(&normalizedNum);

                if (duration >= CCalls::MIN_DURATION_FOR_NORMALIZED_NUMBER_UPDATE_MS
                        && callType == OUTGOING_TYPE
                        && TextUtils::IsEmpty(normalizedNum)) {
                    UpdateNormalizedNumber(context, resolver, dataId, number);
                }
            }
            //} finally {
    EXIT:
            FAIL_RETURN(ICloseable::Probe(cursor)->Close())
            //}
        }
    }

    AutoPtr<IUri> result;
     if (addForAllUsers) {
        // Insert the entry for all currently running users, in order to trigger any
        // ContentObservers currently set on the call log.
        AutoPtr<IUserManager> userManager;
        AutoPtr<IInterface> interUserManager;
        context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&interUserManager);
        userManager = IUserManager::Probe(interUserManager);
        AutoPtr<IList> users;
        userManager->GetUsers(TRUE, (IList**)&users);
        Int32 currentUserId;
        userManager->GetUserHandle(&currentUserId);
        Int32 count;
        users->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            users->Get(i, (IInterface**)&obj);
            AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);

            AutoPtr<IUserHandle> userHandle;
            user->GetUserHandle((IUserHandle**)&userHandle);

            Boolean flag = FALSE;
            userManager->IsUserRunning(userHandle, &flag);

            Boolean bHasUserRes = FALSE;
            userManager->HasUserRestriction(IUserManager::DISALLOW_OUTGOING_CALLS, userHandle.Get(), &bHasUserRes);

            Boolean isManagedProfile = FALSE;
            user->IsManagedProfile(&isManagedProfile);
            if (flag && !bHasUserRes && !isManagedProfile) {
                Int32 id;
                user->GetId(&id);
                AutoPtr<IUri> iur;
                AddEntryAndRemoveExpiredEntries(context,
                        ContentProvider::MaybeAddUserId(CONTENT_URI.Get(), id), values, (IUri**)&iur);
                if (id == currentUserId) {
                    result = iur;
                }
            }
        }
    } else {
        AddEntryAndRemoveExpiredEntries(context, CONTENT_URI.Get(), values, (IUri**)&result);
    }

    *uri = result;
    REFCOUNT_ADD(*uri);
    return NOERROR;

}

ECode CCalls::GetLastOutgoingCall(
    /* [in] */ IContext* context,
    /* [out] */ String* call)
{
    VALIDATE_NOT_NULL(call);
    AutoPtr<IContentResolver> resolver;
    FAIL_RETURN(context->GetContentResolver((IContentResolver**)&resolver))
    AutoPtr<ICursor> c = NULL;

    //try {
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = ICalls::NUMBER;

    StringBuilder builder;
    builder += TYPE;
    builder += " = ";
    builder += ICalls::OUTGOING_TYPE;
    String selection = builder.ToString();

    FAIL_GOTO(resolver->Query(CONTENT_URI, projection, selection, NULL,
            String(ICalls::DEFAULT_SORT_ORDER) + String(" LIMIT 1"), (ICursor**)&c), EXIT)
    Boolean isFIrst;
    if (c == NULL || (c->MoveToFirst(&isFIrst), !isFIrst)) {
        *call = String("");
    }
    else {
        FAIL_GOTO(c->GetString(0, call), EXIT)
    }
    //} finally {
EXIT:
    if (c != NULL) {
        FAIL_RETURN(ICloseable::Probe(c)->Close())
    }
    //}
    return NOERROR;
}

ECode CCalls::AddEntryAndRemoveExpiredEntries(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> result;
    resolver->Insert(uri, values, (IUri**)&result);
    Int32 num;

    StringBuilder builder;
    builder += "_id IN ";
    builder += "(SELECT _id FROM calls ORDER BY ";
    builder += ICalls::DEFAULT_SORT_ORDER;
    builder += " LIMIT -1 OFFSET 500)";
    String where = builder.ToString();

    resolver->Delete(CCalls::CONTENT_URI.Get(), where, NULL, &num);
    *ret = result;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

void CCalls::UpdateDataUsageStatForData(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& dataId)
{
    AutoPtr<IContactsContractDataUsageFeedback> ccduf;
    CContactsContractDataUsageFeedback::AcquireSingleton((IContactsContractDataUsageFeedback**)&ccduf);
    AutoPtr<IUri> uri;
    ccduf->GetFEEDBACKURI((IUri**)&uri);
    AutoPtr<IUriBuilder> iub;
    uri->BuildUpon((IUriBuilder**)&iub);
    iub->AppendPath(dataId);
    iub->AppendQueryParameter(IContactsContractDataUsageFeedback::USAGE_TYPE,
        IContactsContractDataUsageFeedback::USAGE_TYPE_CALL);
    AutoPtr<IUri> feedbackUri;
    iub->Build((IUri**)&feedbackUri);

    Int32 out;
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    resolver->Update(feedbackUri.Get(), cv.Get(), String(NULL), NULL, &out);
}

void CCalls::UpdateNormalizedNumber(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& dataId,
    /* [in] */ const String& number)
{
    if (TextUtils::IsEmpty(number) || TextUtils::IsEmpty(dataId)) {
        return;
    }

    String countryIso = GetCurrentCountryIso(context);
    if (TextUtils::IsEmpty(countryIso)) {
        return;
    }

    String currentCountry = GetCurrentCountryIso(context);

    String normalizedNumber;
    // CPhoneNumberUtils::FormatNumberToE164(number, currentCountry, &normalizedNumber);
    if (TextUtils::IsEmpty(normalizedNumber)) {
        return;
    }

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IContactsContractCommonDataKindsPhone::NORMALIZED_NUMBER, normalizedNumber);
    AutoPtr<IUri> uri;
    ContactsContractData::GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ArrayOf<String> > dstr = ArrayOf<String>::Alloc(1);
    (*dstr)[0] = dataId;
    Int32 ret;
    resolver->Update(uri.Get(), values.Get(), IBaseColumns::ID + String("=?"), dstr.Get(), &ret);
}

String CCalls::GetCurrentCountryIso(
    /* [in] */ IContext* context)
{
    String countryIso;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::COUNTRY_DETECTOR, (IInterface**)&obj);
    AutoPtr<ICountryDetector> detector = ICountryDetector::Probe(obj);
    if (detector != NULL) {
        AutoPtr<ICountry> country;
        detector->DetectCountry((ICountry**)&country);
        if (country != NULL) {
            country->GetCountryIso(&countryIso);
        }
    }
    return countryIso;
}

} //Provider
} //Droid
} //Elastos
