
#include "elastos/droid/phone/CallLogger.h"

namespace Elastos {
namespace Droid {
namespace Phone {

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 value;
    helper->GetInt32(String("ro.debuggable"), 0, &value);
    Boolean result = (PhoneGlobals::DBG_LEVEL >= 1) && (value == 1);
    return result;
}

const CallLogger::String LOG_TAG("CallLogger");// = CallLogger.class.getSimpleName();
Boolean CallLogger::DBG = initDBG();
Boolean CallLogger::VDBG = (PhoneGlobals::DBG_LEVEL >= 2);

CallLogger::CallLogger(
    /* [in] */ IPhoneGlobals* application,
    /* [in] */ CallLogAsync* callLogAsync)
    : mApplication(application)
    , mCallLog(callLogAsync)
{
}

ECode CallLogger::LogCall(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 callLogType)
{
    String number;
    c->GetAddress(&number);
    Int64 date;
    c->GetCreateTime(&date);
    Int64 duration;
    c->GetDurationMillis(&duration);
    AutoPtr<ICall> call;
    c->GetCall((ICall**)&call);
    AutoPtr<IPhone> phone;
    call->GetPhone((IPhone**)&phone);

    AutoPtr<ICallerInfo> ci = GetCallerInfoFromConnection(c);  // May be null.
    const String logNumber = GetLogNumber(c, ci);

    if (DBG) {
        StringBuilder sb;
        sb += "- onDisconnect(): logNumber set to:";
        sb += PhoneUtils::ToLogSafePhoneNumber(logNumber);
        sb += ", number set to: "l;
        sb += PhoneUtils::ToLogSafePhoneNumber(number);
        Log(sb.ToString());
    }

    // TODO: In getLogNumber we use the presentation from
    // the connection for the CNAP. Should we use the one
    // below instead? (comes from caller info)

    // For international calls, 011 needs to be logged as +
    const Int32 presentation = GetPresentation(c, ci);

    const Boolean isOtaspNumber = TelephonyCapabilities::SupportsOtasp(phone)
            && phone::IsOtaSpNumber(number);

    // Don't log OTASP calls.
    if (!isOtaspNumber) {
        LogCall(ci, logNumber, presentation, callLogType, date, duration);
    }
    return NOERROR;
}

ECode CallLogger::LogCall(
    /* [in] */ IConnection* c)
{
    Int32 cause;
    c->GetDisconnectCause(&cause);

    // Set the "type" to be displayed in the call log (see constants in CallLog.Calls)
    Int32 callLogType;

    Boolean result;
    if (c->IsIncoming(&result),result) {
        callLogType = (cause == IDisconnectCause::INCOMING_MISSED ?
                       ICalls::MISSED_TYPE : ICalls::INCOMING_TYPE);
    }
    else {
        callLogType = ICalls::OUTGOING_TYPE;
    }
    if (VDBG) {
        StringBuilder sb;
        sb += "- callLogType: ";
        sb += callLogType;
        sb += ", UserData: ";
        AutoPtr<IInterface> obj;
        c->GetUserData((IInterface**)&obj);
        sb += TO_CSTR(obj);
        Log(sb.ToString());
    }

    return LogCall(c, callLogType);
}

ECode CallLogger::LogCall(
    /* [in] */ ICallerInfo* ci,
    /* [in] */ const String& number,
    /* [in] */ Int32 presentation,
    /* [in] */ Int32 callType,
    /* [in] */ Int64 start,
    /* [in] */ Int64 duration)
{
    // no-op
    return NOERROR;
}

AutoPtr<ICallerInfo> CallLogger::GetCallerInfoFromConnection(
    /* [in] */ IConnection* conn)
{
    AutoPtr<ICallerInfo> ci;
    AutoPtr<IInterface> o;
    conn->GetUserData((IInterface**)&o);

    if ((o == NULL) || (ICallerInfo::Probe(o) != NULL)) {
        ci = ICallerInfo::Probe(o);
    }
    else if (IUri::Probe(o) != NULL) {
        AutoPtr<IContext> context;
        mApplication->GetApplicationContext((IContext**)&context);
        CallerInfo->GetCallerInfo(context, IUri::Probe(o), (ICallerInfo**)&ci);
    }
    else {
        ci = ((PhoneUtils::CallerInfoToken)IObject::Probe(o))->mCurrentInfo;
    }
    return ci;
}


String CallLogger::GetLogNumber(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallerInfo* callerInfo)
{
    String number(NULL);

    Boolean result;
    if (conn->IsIncoming(&result),result) {
        conn->GetAddress(&number);
    }
    else {
        // For emergency and voicemail calls,
        // CallerInfo.phoneNumber does *not* contain a valid phone
        // number.  Instead it contains an I18N'd string such as
        // "Emergency Number" or "Voice Mail" so we get the number
        // from the connection.
        String pnumber;
        if (callerInfo != NULL) {
            callerInfo->GetPhoneNumber(&pnumber);
        }
        Boolean res1,res2;
        if (NULL == callerInfo || TextUtils::IsEmpty(pnumber) ||
                (callerInfo->IsEmergencyNumber(&res1), res1) ||
                (callerInfo->IsVoiceMailNumber(&res2), res2) {
            AutoPtr<ICall> call;
            conn->GetCall((ICall**)&call);
            AutoPtr<IPhone> phone;
            call->GetPhone((IPhone**)&phone);
            Int32 type;
            phone->GetPhoneType(&type);
            if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
                // In cdma getAddress() is not always equals to getOrigDialString().
                conn->GetOrigDialString(&number);
            }
            else {
                conn->GetAddress(&number);
            }
        }
        else {
            number = pnumber;
        }
    }

    if (number.IsNull()) {
        return String(NULL);
    }
    else {
        Int32 presentation;
        conn->GetNumberPresentation(&presentation);

        // Do final CNAP modifications.
        String newNumber = PhoneUtils::ModifyForSpecialCnapCases(mApplication, callerInfo,
                                                      number, presentation);

        Boolean res;
        if (PhoneNumberUtils->IsUriNumber(number, &res), !res) {
            number = PhoneNumberUtils::StripSeparators(number);
        }
        if (VDBG) {
            StringBuilder sb;
            sb += "getLogNumber: ";
            sb += number;
            Log(sb.ToString());
        }
        return number;
    }
    return number;
}

Int32 CallLogger::GetPresentation(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallerInfo* callerInfo)
{
    Int32 presentation;

    if (NULL == callerInfo) {
        conn->GetNumberPresentation(&presentation);
    }
    else {
        callerInfo->GetNumberPresentation(&presentation);
        if (DBG) {
            StringBuilder sb;
            sb += "- getPresentation(): ignoring connection's presentation: ";
            Int32 tmp;
            conn->GetNumberPresentation(&tmp);
            sb += tmp;
            Log(sb.ToString());
        }
    }
    if (DBG) {
        StringBuilder sb;
        sb += "- getPresentation: presentation: ";
        sb += presentation;
        Log(sb.ToString());
    }
    return presentation;
}

void CallLogger::Log(
        /* [in] */ const String& msg)
{
    Logger::D(LOG_TAG, msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos