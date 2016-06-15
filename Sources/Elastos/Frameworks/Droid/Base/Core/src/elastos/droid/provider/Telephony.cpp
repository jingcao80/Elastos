#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Database.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
//TODO #include "elastos/droid/telephony/SmsMessage.h"
//TODO #include "elastos/droid/internal/telephony/SmsApplication.h"
#include "elastos/droid/utility/Patterns.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Character.h"
#include <elastos/utility/logging/Logger.h>

//using Elastos::Droid::Annotation::ISdkConstant;
//using Elastos::Droid::Annotation::SdkConstant::ISdkConstantType;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Telephony::ISmsMessage;
//TODO using Elastos::Droid::Telephony::SmsMessage;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
//using Elastos::Droid::Telephony::IRlog;
//using Elastos::Droid::Utility::IPatterns;
using Elastos::Droid::Utility::Patterns;
//TODO using Elastos::Droid::Database::Sqlite::ISqliteWrapper;

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ISmsApplication;

using Elastos::Core::CoreUtils;
using Elastos::Core::Character;
using Elastos::IO::ICloseable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
//using Elastos::Utility::IArrays;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Provider {

const String Telephony::TAG("Telephony");
CAR_INTERFACE_IMPL(Telephony, Object, ITelephony);


//begin  ===Telephony::Sms============
//CAR_INTERFACE_IMPL_3(Telephony::Sms, Object, ITelephonySms, IBaseColumns, ITelephonyTextBasedSmsColumns);
//CAR_SINGLETON_IMPL(Telephony::Sms);

static AutoPtr<IUri> InitURI(const String& str)
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(str, (IUri**)&uri);
    return uri;
}
AutoPtr<IUri> Telephony::Sms::CONTENT_URI = InitURI(String("content://sms"));


ECode Telephony::Sms::GetDefaultSmsPackage(
    /* [in] */ IContext* context,
    /* [out] */ String* smsPackage)
{
    VALIDATE_NOT_NULL(smsPackage);
    *smsPackage = NULL;
    AutoPtr<IComponentName> component;
    //TODO SmsApplication::GetDefaultSmsApplication(context, FALSE, (IComponentName**)&component);
    if (component != NULL) {
        component->GetPackageName(smsPackage);
    }
    return NOERROR;
}

ECode Telephony::Sms::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    return cr->Query(CONTENT_URI, projection, String(NULL), NULL, ITelephonySms::DEFAULT_SORT_ORDER, cursor);
}

ECode Telephony::Sms::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& where,
    /* [in] */ const String& orderBy,
    /* [out] */ ICursor** cursor)
{
    return cr->Query(CONTENT_URI, projection, where, NULL, orderBy.IsNull() ? ITelephonySms::DEFAULT_SORT_ORDER : orderBy, cursor);
}

ECode Telephony::Sms::AddMessageToUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [in] */ Boolean deliveryReport,
    /* [out] */ IUri** _uri)
{
    Int64 subId;
    SubscriptionManager::GetDefaultSmsSubId(&subId);
    return AddMessageToUri(subId,
            resolver, uri, address, body, subject, date, read, deliveryReport, -1L, _uri);
}

ECode Telephony::Sms::AddMessageToUri(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [in] */ Boolean deliveryReport,
    /* [out] */ IUri** _uri)
{
    return AddMessageToUri(subId, resolver, uri, address, body, subject,
            date, read, deliveryReport, -1L, _uri);
}

ECode Telephony::Sms::AddMessageToUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [in] */ Boolean deliveryReport,
    /* [in] */ Int64 threadId,
    /* [out] */ IUri** _uri)
{
    Int64 subId;
    SubscriptionManager::GetDefaultSmsSubId(&subId);
    return AddMessageToUri(subId,
            resolver, uri, address, body, subject,
            date, read, deliveryReport, threadId, _uri);
}

ECode Telephony::Sms::AddMessageToUri(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [in] */ Boolean deliveryReport,
    /* [in] */ Int64 threadId,
    /* [out] */ IUri** _uri)
{
    return AddMessageToUri(subId, resolver, uri, address, body, subject,
            date, read, deliveryReport, threadId, -1, _uri);
}

ECode Telephony::Sms::AddMessageToUri(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [in] */ Boolean deliveryReport,
    /* [in] */ Int64 threadId,
    /* [in] */ Int32 priority,
    /* [out] */ IUri** _uri)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(8, (IContentValues**)&values);
    Logger::V(TAG,"Telephony addMessageToUri sub id: %d", subId);

    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    values->Put(ITelephonyTextBasedSmsColumns::PHONE_ID, phoneId);
    values->Put(ITelephonyTextBasedSmsColumns::ADDRESS, address);
    //if (date != NULL) {
    values->Put(ITelephonyTextBasedSmsColumns::DATE, date);
    //}
    values->Put(ITelephonyTextBasedSmsColumns::READ, read ? 1 : 0);
    values->Put(ITelephonyTextBasedSmsColumns::SUBJECT, subject);
    values->Put(ITelephonyTextBasedSmsColumns::BODY, body);
    values->Put(ITelephonyTextBasedSmsColumns::PRIORITY, priority);
    if (deliveryReport) {
        values->Put(ITelephonyTextBasedSmsColumns::STATUS, ITelephonyTextBasedSmsColumns::STATUS_PENDING);
    }
    if (threadId != -1L) {
        values->Put(ITelephonyTextBasedSmsColumns::THREAD_ID, threadId);
    }
    return resolver->Insert(uri, values, _uri);
}

ECode Telephony::Sms::MoveMessageToFolder(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 folder,
    /* [in] */ Int32 error,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (uri == NULL) {
        return NOERROR;
    }

    Boolean markAsUnread = FALSE;
    Boolean markAsRead = FALSE;
    switch(folder) {
        case ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_INBOX:
        case ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT:
            break;
        case ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX:
        case ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_SENT:
            markAsRead = TRUE;
            break;
        case ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED:
        case ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_QUEUED:
            markAsUnread = TRUE;
            break;
        default:
            return NOERROR;
    }

    AutoPtr<IContentValues> values;
    CContentValues::New(3, (IContentValues**)&values);

    values->Put(ITelephonyTextBasedSmsColumns::TYPE, folder);
    if (markAsUnread) {
        values->Put(ITelephonyTextBasedSmsColumns::READ, 0);
    } else if (markAsRead) {
        values->Put(ITelephonyTextBasedSmsColumns::READ, 1);
    }
    values->Put(ITelephonyTextBasedSmsColumns::ERROR_CODE, error);

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    //TODO *result = 1 == ISqliteWrapper::Update(context, cr, uri, values, String(NULL), NULL);
    return NOERROR;
}

ECode Telephony::Sms::IsOutgoingFolder(
    /* [in] */ Int32 messageType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (messageType == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED)
        || (messageType == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX)
        || (messageType == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_SENT)
        || (messageType == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_QUEUED);
    return NOERROR;
}

//begin ===========Telephony::Sms::Inbox========================
//CAR_INTERFACE_IMPL_3(Telephony::Sms::Inbox, Object, ITelephonySms, IBaseColumns, ITelephonyTextBasedSmsColumns);
//CAR_SINGLETON_IMPL(Telephony::Sms::Inbox);

AutoPtr<IUri> Telephony::Sms::Inbox::CONTENT_URI = InitURI(String("content://sms/inbox"));

Telephony::Sms::Inbox::Inbox()
{
    //CTelephonySms::AcquireSingleton((ITelephonySms**)&mOwner);
}

ECode Telephony::Sms::Inbox::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [out] */ IUri** result)
{
    Int64 subId;
    SubscriptionManager::GetDefaultSmsSubId(&subId);
    return Telephony::Sms::AddMessageToUri(subId,
            resolver, CONTENT_URI, address, body, subject, date, read, FALSE, result);
}

ECode Telephony::Sms::Inbox::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
            subject, date, read, FALSE, uri);
}
//end===========Telephony::Sms::Inbox========================
//begin =============Telephony::Sms::Sent===============
//CAR_INTERFACE_IMPL_3(Telephony::Sms::Sent, Object, ITelephonySmsSent, IBaseColumns, ITelephonyTextBasedSmsColumns);
//CAR_SINGLETON_IMPL(Telephony::Sms::Sent);
AutoPtr<IUri> Telephony::Sms::Sent::CONTENT_URI = InitURI(String("content://sms/sent"));

Telephony::Sms::Sent::Sent()
{
    //CTelephonySms::AcquireSingleton((ITelephonySms**)&mOwner);
}

ECode Telephony::Sms::Sent::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    Int64 subId;
    SubscriptionManager::GetDefaultSmsSubId(&subId);
    return Telephony::Sms::AddMessageToUri(subId,
            resolver, CONTENT_URI, address, body, subject, date, TRUE, FALSE, uri);
}

ECode Telephony::Sms::Sent::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
            subject, date, TRUE, FALSE, uri);
}
//end =============Telephony::Sms::Sent===============


//begin =============Telephony::Sms::Draft===============
//CAR_INTERFACE_IMPL_3(Telephony::Sms::Draft, Object, ITelephonySmsDraft, IBaseColumns, ITelephonyTextBasedSmsColumns);
//CAR_SINGLETON_IMPL(Telephony::Sms::Draft);

AutoPtr<IUri> Telephony::Sms::Draft::CONTENT_URI = InitURI(String("content://sms/draft"));

Telephony::Sms::Draft::Draft()
{
    //CTelephonySms::AcquireSingleton((ITelephonySms**)&mOwner);
}

ECode Telephony::Sms::Draft::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    Int64 subId;
    SubscriptionManager::GetDefaultSmsSubId(&subId);
    return Telephony::Sms::AddMessageToUri(subId,
            resolver, CONTENT_URI, address, body, subject, date, TRUE, FALSE, uri);
}

ECode Telephony::Sms::Draft::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
            subject, date, TRUE, FALSE, uri);
}

//end =============Telephony::Sms::Draft===============

//begin =============Telephony::Sms::Outbox===============
//CAR_INTERFACE_IMPL_3(Telephony::Sms::Outbox, Object, ITelephonySmsOutbox, IBaseColumns, ITelephonyTextBasedSmsColumns);
//CAR_SINGLETON_DECL(Telephony::Sms::Outbox);

AutoPtr<IUri> Telephony::Sms::Outbox::CONTENT_URI = InitURI(String("content://sms/outbox"));

Telephony::Sms::Outbox::Outbox()
{
    //CTelephonySms::AcquireSingleton((ITelephonySms**)&mOwner);
}

ECode Telephony::Sms::Outbox::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean deliveryReport,
    /* [in] */ Int64 threadId,
    /* [out] */ IUri** uri)
{
    Int64 subId;
    SubscriptionManager::GetDefaultSmsSubId(&subId);
    return Telephony::Sms::AddMessageToUri(subId,
            resolver, CONTENT_URI, address, body, subject, date,
            TRUE, deliveryReport, threadId, uri);
}

ECode Telephony::Sms::Outbox::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean deliveryReport,
    /* [in] */ Int64 threadId,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
            subject, date, TRUE, deliveryReport, threadId, uri);
}

//end =============Telephony::Sms::Outbox===============

//begin =============Telephony::Sms::Conversations===============
//CAR_INTERFACE_IMPL_3(Telephony::Sms::Conversations, Object, ITelephonySmsConversations, IBaseColumns, ITelephonyTextBasedSmsColumns);

AutoPtr<IUri> Telephony::Sms::Conversations::CONTENT_URI = InitURI(String("content://sms/conversations"));

//end =============Telephony::Sms::Conversations===============

//begin =============Telephony::Sms::Intents===============
ECode Telephony::Sms::Intents::GetMessagesFromIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ ArrayOf<ISmsMessage*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    //Object[] messages = (Object[]) intent->GetSerializableExtra("pdus");
    AutoPtr<ISerializable> serializable;
    intent->GetSerializableExtra(String("pdus"), (ISerializable**)&serializable);
    AutoPtr<ArrayOf<IObject*> > messages;
    assert(0);
    //TODO message = serializable;

    String format;
    intent->GetStringExtra(String("format"), &format);
    Int64 defaultSubId;
    SubscriptionManager::GetDefaultSmsSubId(&defaultSubId);
    Int64 subId;
    intent->GetInt64Extra(IPhoneConstants::SUBSCRIPTION_KEY, defaultSubId, &subId);

    Logger::V(TAG, " getMessagesFromIntent sub_id : %ld", subId);

    Int32 pduCount = messages->GetLength();
    AutoPtr<ArrayOf<ISmsMessage*> > msgs = ArrayOf<ISmsMessage*>::Alloc(pduCount);

    for (Int32 i = 0; i < pduCount; i++) {
        //Byte[] pdu = (Byte[]) messages[i];
        AutoPtr<ArrayOf<Byte> > pdu;
        assert(0);
        //TODO pdu = (Byte[]) messages[i];
        AutoPtr<ISmsMessage> smsMessage;//TODO = SmsMessage::CreateFromPdu(pdu, format);
        msgs->Set(i, smsMessage);
        smsMessage->SetSubId(subId);
    }

    *result = msgs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Telephony::Sms::Intents::GetNormalizedAddressesFromPdus(
    /* [in] */ ArrayOf<ArrayOf<Byte>*>* pdus,
    /* [in] */ const String& format,
    /* [out] */ IList** result)//String
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 pduCount = pdus->GetLength();
    AutoPtr<ArrayOf<ISmsMessage*> > msgs = ArrayOf<ISmsMessage*>::Alloc(pduCount);
    AutoPtr<IList> addresses;
    CArrayList::New((IList**)&addresses);

    for (Int32 i = 0; i < pduCount; i++) {
        AutoPtr<ArrayOf<Byte> > pdu = (*pdus)[i];
        AutoPtr<ISmsMessage> smsMessage;//TODO = SmsMessage::CreateFromPdu(pdu, format);
        msgs->Set(i, smsMessage);;
        // If the originating address is NULL on our message
        // then the format for SmsMessage createFromPdu is likely
        // incorrect. SmsMessage CreateFromPdu(the new method)
        // takes in a format parameter that it gets from the Tracker
        // however, on some of our legacy devices using a legacy ril,
        // since that format is derived by getting voice tech,
        // we can get a bad format and no valid members.
        // Thus we introduce a hack that utilizes the deprecated
        // SmsMessage.createFromPdu if we get a NULL originating address.
        String oriAddress;
        smsMessage->GetOriginatingAddress(&oriAddress);
        if (oriAddress.IsNull()) {
            //TODO smsMessage = SmsMessage::CreateFromPdu(pdu);
            msgs->Set(i, smsMessage);
        }
        String originatingAddress;
        smsMessage->GetOriginatingAddress(&originatingAddress);
        if (!TextUtils::IsEmpty(originatingAddress)) {
            String normalized = NormalizeDigitsOnly(originatingAddress);
            addresses->Add(CoreUtils::Convert(normalized));
        }
    }
    *result = addresses;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

String Telephony::Sms::Intents::NormalizeDigitsOnly(
    /* [in] */ const String& number)
{
    return NormalizeDigits(number, FALSE /* strip non-digits */)->ToString();
}

AutoPtr<StringBuilder> Telephony::Sms::Intents::NormalizeDigits(
    /* [in] */ const String& number,
    /* [in] */ Boolean keepNonDigits)
{
    AutoPtr<StringBuilder> normalizedDigits = new StringBuilder(number.GetLength());
    AutoPtr<ArrayOf<Char32> > array = number.GetChars();
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        Char32 c = (*array)[i];
        Int32 digit = Character::ToDigit(c, 10);
        if (digit != -1) {
            normalizedDigits->Append(digit);
        }
        else if (keepNonDigits) {
            normalizedDigits->Append((Int32)c);//TODO check the cast
        }
    }
    return normalizedDigits;
}

//end =============Telephony::Sms::Intents===============
//end =============Telephony::Sms===============


//begin =============Telephony::Mms===============
static AutoPtr<IUri> Init_Uri_WithAppendedPath(
    /* [in] */ IUri* path,
    /* [in] */ const String& str)
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->WithAppendedPath(path, str, (IUri**)&uri);
    return uri;
}

//CAR_INTERFACE_IMPL_2(Telephony::Mms, Object, ITelephonyMms, ITelephonyBaseMmsColumns);

AutoPtr<IUri> Telephony::Mms::CONTENT_URI = InitURI(String("content://mms"));
AutoPtr<IUri> Telephony::Mms::REPORT_REQUEST_URI = Init_Uri_WithAppendedPath(CONTENT_URI, String("report-request"));

AutoPtr<IUri> Telephony::Mms::REPORT_STATUS_URI = Init_Uri_WithAppendedPath(CONTENT_URI, String("report-status"));

static AutoPtr<IPattern> Init_Pattern(
    /* [in] */ const String& str)
{
    AutoPtr<IPattern> pattern;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(str, (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> Telephony::Mms::NAME_ADDR_EMAIL_PATTERN = Init_Pattern(String("\\s*(\"[^\"]*\"|[^<>\"]+)\\s*<([^<>]+)>\\s*"));

ECode Telephony::Mms::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    return cr->Query(CONTENT_URI, projection, String(NULL), NULL, ITelephonyMms::DEFAULT_SORT_ORDER, cursor);
}

ECode Telephony::Mms::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& where,
    /* [in] */ const String& orderBy,
    /* [out] */ ICursor** cursor)
{
    return cr->Query(CONTENT_URI, projection,
            where, NULL, orderBy.IsNull() ? ITelephonyMms::DEFAULT_SORT_ORDER : orderBy, cursor);
}

ECode Telephony::Mms::ExtractAddrSpec(
    /* [in] */ const String& address,
    /* [out] */ String* addrSpec)
{
    VALIDATE_NOT_NULL(addrSpec);
    AutoPtr<IMatcher> match;
    NAME_ADDR_EMAIL_PATTERN->Matcher(address, (IMatcher**)&match);

    Boolean matched;
    if (match->Matches(&matched), matched) {
        return IMatchResult::Probe(match)->Group(2, addrSpec);
    }
    *addrSpec = address;
    return NOERROR;
}

ECode Telephony::Mms::IsEmailAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* isEmailAddress)
{
    VALIDATE_NOT_NULL(isEmailAddress);
    isEmailAddress = FALSE;
    if (TextUtils::IsEmpty(address)) {
        return NOERROR;
    }

    String s;
    ExtractAddrSpec(address, &s);
    AutoPtr<IMatcher> match;
    Patterns::EMAIL_ADDRESS->Matcher(s, (IMatcher**)&match);
    return match->Matches(isEmailAddress);
}

ECode Telephony::Mms::IsPhoneNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* isPhoneNumber)
{
    VALIDATE_NOT_NULL(isPhoneNumber);
    isPhoneNumber = FALSE;
    if (TextUtils::IsEmpty(number)) {
        return NOERROR;
    }

    AutoPtr<IMatcher> match;
    Patterns::PHONE->Matcher(number, (IMatcher**)&match);
    return match->Matches(isPhoneNumber);
}

//== begin Telephony::Mms::Inbox ============
//CAR_INTERFACE_IMPL_2(Telephony::Mms::Inbox, Object, ITelephonyMmsInbox, ITelephonyBaseMmsColumns);
AutoPtr<IUri> Telephony::Mms::Inbox::CONTENT_URI = InitURI(String("content://mms/inbox"));

//== end Telephony::Mms::Inbox ============

//== begin Telephony::Mms::Sent ============
//CAR_INTERFACE_IMPL_2(Telephony::Mms::Sent, Object, ITelephonyMmsSent, ITelephonyBaseMmsColumns);
AutoPtr<IUri> Telephony::Mms::Sent::CONTENT_URI = InitURI(String("content://mms/sent"));

//== end Telephony::Mms::Sent ============

//== begin Telephony::Mms::Draft============

//CAR_INTERFACE_IMPL_2(Telephony::Mms::Draft, Object, ITelephonyMmsDraft, ITelephonyBaseMmsColumns);
AutoPtr<IUri> Telephony::Mms::Draft::CONTENT_URI = InitURI(String("content://mms/drafts"));

//== end Telephony::Mms::Draft============

//== begin Telephony::Mms::Outbox ============
//CAR_INTERFACE_IMPL_2(Telephony::Mms::Outbox, Object, ITelephonyMmsOutbox, ITelephonyBaseMmsColumns);
AutoPtr<IUri> Telephony::Mms::Outbox::CONTENT_URI = InitURI(String("content://mms/outbox"));

//== end Telephony::Mms::Outbox ============

//== begin Telephony::Mms::Addr============
//CAR_INTERFACE_IMPL_2(Telephony::Mms::Addr, Object, ITelephonyMmsAddr, IBaseColumns);

//== end Telephony::Mms::Addr============

//== begin Telephony::Mms::Part============
//CAR_INTERFACE_IMPL_2(Telephony::Mms::Part, Object, ITelephonyMmsPart, IBaseColumns);

//== end Telephony::Mms::Part============

//== begin Telephony::Mms::Rate============
//CAR_INTERFACE_IMPL(Telephony::Mms::Rate, Object, ITelephonyMmsRate);

AutoPtr<IUri> Telephony::Mms::Rate::CONTENT_URI = Init_Uri_WithAppendedPath(Mms::CONTENT_URI, String("rate"));

//== end Telephony::Mms::Rate============

//== begin Telephony::Mms::Intents============
//CAR_INTERFACE_IMPL(Telephony::Mms::Intents, Object, ITelephonyMmsIntents);

//== begin Telephony::Mms::Intents============
//end =============Telephony::Mms===============

//begin =============Telephony::MmsSms===============
//CAR_INTERFACE_IMPL_2(Telephony::MmsSms, Object, ITelephonyMmsSms, IBaseColumns);

AutoPtr<IUri> Telephony::MmsSms::CONTENT_URI = InitURI(String("content://mms-sms/"));
AutoPtr<IUri> Telephony::MmsSms::CONTENT_CONVERSATIONS_URI = InitURI(String("content://mms-sms/conversations"));
AutoPtr<IUri> Telephony::MmsSms::CONTENT_FILTER_BYPHONE_URI = InitURI(String("content://mms-sms/messages/byphone"));
AutoPtr<IUri> Telephony::MmsSms::CONTENT_UNDELIVERED_URI = InitURI(String("content://mms-sms/undelivered"));
AutoPtr<IUri> Telephony::MmsSms::CONTENT_DRAFT_URI = InitURI(String("content://mms-sms/draft"));
AutoPtr<IUri> Telephony::MmsSms::CONTENT_LOCKED_URI = InitURI(String("content://mms-sms/locked"));
AutoPtr<IUri> Telephony::MmsSms::SEARCH_URI = InitURI(String("content://mms-sms/search"));

//begin =============Telephony::MmsSms::PendingMessages===============
//CAR_INTERFACE_IMPL_2(Telephony::MmsSms::PendingMessages, Object, ITelephonyMmsSmsPendingMessages, IBaseColumns);

AutoPtr<IUri> Telephony::MmsSms::PendingMessages::CONTENT_URI = Init_Uri_WithAppendedPath(MmsSms::CONTENT_URI, String("pending"));

//end =============Telephony::MmsSms::PendingMessages===============

//begin =============Telephony::MmsSms::WordsTable===============
//CAR_INTERFACE_IMPL(Telephony::MmsSms::WordsTable, Object, ITelephonyMmsSmsWordsTable);

//end =============Telephony::MmsSms::WordsTable===============
//end =============Telephony::MmsSms===============

//begin =============Telephony::Threads===============
//CAR_INTERFACE_IMPL_2(Telephony::Threads, Object, ITelephonyThreads, ITelephonyThreadsColumns);

static AutoPtr<ArrayOf<String> > InitID_PROJECTION()
{
    AutoPtr<ArrayOf<String> > res = ArrayOf<String>::Alloc(1);
    res->Set(0, IBaseColumns::ID);
    return res;
}
AutoPtr<ArrayOf<String> > Telephony::Threads::ID_PROJECTION = InitID_PROJECTION();
AutoPtr<IUri> Telephony::Threads::THREAD_ID_CONTENT_URI = InitURI(String("content://mms-sms/threadID"));

AutoPtr<IUri> Telephony::Threads::CONTENT_URI = Init_Uri_WithAppendedPath(MmsSms::CONTENT_URI, String("conversations"));
AutoPtr<IUri> Telephony::Threads::OBSOLETE_THREADS_URI = Init_Uri_WithAppendedPath(CONTENT_URI, String("obsolete"));

ECode Telephony::Threads::GetOrCreateThreadId(
    /* [in] */ IContext* context,
    /* [in] */ const String& recipient,
    /* [out] */ Int64* threadId)
{
    AutoPtr<ISet> recipients;
    CHashSet::New((ISet**)&recipients);

    recipients->Add(CoreUtils::Convert(recipient));
    return GetOrCreateThreadId(context, recipients, threadId);
}

ECode Telephony::Threads::GetOrCreateThreadId(
    /* [in] */ IContext* context,
    /* [in] */ ISet* recipients,//String
    /* [out] */ Int64* threaId)
{
    AutoPtr<IUriBuilder> uriBuilder;
    THREAD_ID_CONTENT_URI->BuildUpon((IUriBuilder**)&uriBuilder);

    AutoPtr<ArrayOf<IInterface*> > array;
    recipients->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        String recipient;
        ICharSequence::Probe((*array)[i])->ToString(&recipient);
        if (FALSE/*TODO Mms::IsEmailAddress(recipient)*/) {
            String newRecipient;
            //TODO Mms::ExtractAddrSpec(recipient, &newRecipient);
            recipient = newRecipient;
        }

        uriBuilder->AppendQueryParameter(String("recipient"), recipient);
    }

    AutoPtr<IUri> uri;
    uriBuilder->Build((IUri**)&uri);
    //if (DEBUG) Logger::V(TAG, "getOrCreateThreadId uri: " + uri);

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> cursor;
    //TODO ISqliteWrapper::Query(context, cr, uri, ID_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        //try {
            Boolean first;
            if (cursor->MoveToFirst(&first), first) {
                return cursor->GetInt64(0, threaId);
            } else {
                Logger::E(TAG, "getOrCreateThreadId returned no rows!");
            }
        //} finally {
            ICloseable::Probe(cursor)->Close();
        //}
    }

    String str;
    IObject::Probe(uri)->ToString(&str);
    Logger::E(TAG, "getOrCreateThreadId failed with uri %s", str.string());
    //throw new IllegalArgumentException("Unable to find or allocate a thread ID.");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}
//end =============Telephony::Threads===============

//begin =============Telephony::Carriers===============
//CAR_INTERFACE_IMPL_2(Telephony::Carriers, Object, ITelephonyCarriers, IBaseColumns);

AutoPtr<IUri> Telephony::Carriers::CONTENT_URI = InitURI(String("content://telephony/carriers"));

//end =============Telephony::Carriers===============

//begin =============Telephony::CellBroadcasts===============
//CAR_INTERFACE_IMPL_2(Telephony::CellBroadcasts, Object, ITelephonyCellBroadcasts, IBaseColumns);

AutoPtr<IUri> Telephony::CellBroadcasts::CONTENT_URI = InitURI(String("content://cellbroadcasts"));

static AutoPtr<ArrayOf<String> > Init_QUERY_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(20);
    array->Set(0, IBaseColumns::ID);
    array->Set(1, ITelephonyCellBroadcasts::GEOGRAPHICAL_SCOPE);
    array->Set(2, ITelephonyCellBroadcasts::PLMN);
    array->Set(3, ITelephonyCellBroadcasts::LAC);
    array->Set(4, ITelephonyCellBroadcasts::CID);
    array->Set(5, ITelephonyCellBroadcasts::SERIAL_NUMBER);
    array->Set(6, ITelephonyCellBroadcasts::SERVICE_CATEGORY);
    array->Set(7, ITelephonyCellBroadcasts::LANGUAGE_CODE);
    array->Set(8, ITelephonyCellBroadcasts::MESSAGE_BODY);
    array->Set(9, ITelephonyCellBroadcasts::DELIVERY_TIME);
    array->Set(10, ITelephonyCellBroadcasts::MESSAGE_READ);
    array->Set(11, ITelephonyCellBroadcasts::MESSAGE_FORMAT);
    array->Set(12, ITelephonyCellBroadcasts::MESSAGE_PRIORITY);
    array->Set(13, ITelephonyCellBroadcasts::ETWS_WARNING_TYPE);
    array->Set(14, ITelephonyCellBroadcasts::CMAS_MESSAGE_CLASS);
    array->Set(15, ITelephonyCellBroadcasts::CMAS_CATEGORY);
    array->Set(16, ITelephonyCellBroadcasts::CMAS_RESPONSE_TYPE);
    array->Set(17, ITelephonyCellBroadcasts::CMAS_SEVERITY);
    array->Set(18, ITelephonyCellBroadcasts::CMAS_URGENCY);
    array->Set(19, ITelephonyCellBroadcasts::CMAS_CERTAINTY);
    return array;
}
AutoPtr<ArrayOf<String> >  Telephony::CellBroadcasts::QUERY_COLUMNS = Init_QUERY_COLUMNS();
//end =============Telephony::CellBroadcasts===============

//begin =============Telephony::Blacklist===============
//CAR_INTERFACE_IMPL_2(Telephony::Blacklist, Object, ITelephonyBlacklist, IBaseColumns);

AutoPtr<IUri> Telephony::Blacklist::CONTENT_URI = InitURI(String("content://blacklist"));

AutoPtr<IUri> Telephony::Blacklist::CONTENT_FILTER_BYNUMBER_URI = InitURI(String("content://blacklist/bynumber"));

AutoPtr<IUri> Telephony::Blacklist::CONTENT_PHONE_URI = InitURI(String("content://blacklist/phone"));

AutoPtr<IUri> Telephony::Blacklist::CONTENT_MESSAGE_URI = InitURI(String("content://blacklist/message"));

//end =============Telephony::Blacklist===============

} // namespace Provider
} // namespace Droid
} // namespace Elastos

