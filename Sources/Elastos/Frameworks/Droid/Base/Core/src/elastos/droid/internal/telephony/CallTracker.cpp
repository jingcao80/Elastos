
#include "elastos/droid/internal/telephony/CallTracker.h"
#include "elastos/droid/internal/telephony/Call.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Int32 CallTracker::POLL_DELAY_MSEC = 250;
const Int32 CallTracker::EVENT_POLL_CALLS_RESULT;
const Int32 CallTracker::EVENT_CALL_STATE_CHANGE;
const Int32 CallTracker::EVENT_REPOLL_AFTER_DELAY;
const Int32 CallTracker::EVENT_OPERATION_COMPLETE;
const Int32 CallTracker::EVENT_GET_LAST_CALL_FAIL_CAUSE;

const Int32 CallTracker::EVENT_SWITCH_RESULT;
const Int32 CallTracker::EVENT_RADIO_AVAILABLE;
const Int32 CallTracker::EVENT_RADIO_NOT_AVAILABLE;
const Int32 CallTracker::EVENT_CONFERENCE_RESULT;
const Int32 CallTracker::EVENT_SEPARATE_RESULT;
const Int32 CallTracker::EVENT_ECT_RESULT;
const Int32 CallTracker::EVENT_EXIT_ECM_RESPONSE_CDMA;
const Int32 CallTracker::EVENT_CALL_WAITING_INFO_CDMA;
const Int32 CallTracker::EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA;
const Int32 CallTracker::EVENT_THREE_WAY_DIAL_BLANK_FLASH;
const Boolean CallTracker::DBG_POLL = FALSE;
CallTracker::CallTracker()
    : mPendingOperations(0)
    , mNeedsPoll(FALSE)
    , mNumberConverted(FALSE)
    , VALID_COMPARE_LENGTH(3)
{
    CArrayList::New((IArrayList**)&mHandoverConnections);
}

CallTracker::~CallTracker()
{}

void CallTracker::PollCallsWhenSafe()
{
    mNeedsPoll = TRUE;

    if (CheckNoOperationsPending()) {
        ObtainMessage(EVENT_POLL_CALLS_RESULT, (IMessage**)&mLastRelevantPoll);
        mCi->GetCurrentCalls(mLastRelevantPoll);
    }
}

void CallTracker::PollCallsAfterDelay()
{
    AutoPtr<IMessage> msg;
    ObtainMessage((IMessage**)&msg);

    msg->SetWhat(EVENT_REPOLL_AFTER_DELAY);
    Boolean tmp = FALSE;
    SendMessageDelayed(msg, POLL_DELAY_MSEC, &tmp);
}

Boolean CallTracker::IsCommandExceptionRadioNotAvailable(
    /* [in] */ IThrowable* e)
{
    assert(0 && "TODO");
    // return e != NULL && e instanceof CommandException
    //         && ((CommandException)e).getCommandError()
    //                 == CommandException.Error.RADIO_NOT_AVAILABLE;
    return FALSE;
}

AutoPtr<IConnection> CallTracker::GetHoConnection(
    /* [in] */ DriverCall* dc)
{
    AutoPtr<IIterator> ator;
    mHandoverConnections->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    String v, str;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnection> hoConn = IConnection::Probe(obj);

        IObject::Probe(hoConn)->ToString(&str);
        Log(String("getHoConnection - compare number: hoConn= ") + str);
        if ((hoConn->GetAddress(&v), v) != NULL && v.Contains(dc->mNumber)) {
            Log(String("getHoConnection: Handover connection match found = ") + str);
            return hoConn;
        }
    }

    ator = NULL;
    mHandoverConnections->GetIterator((IIterator**)&ator);
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnection> hoConn = IConnection::Probe(obj);

        IObject::Probe(hoConn)->ToString(&str);
        Log(String("getHoConnection: compare state hoConn= ") + str);
        ICallState cs;
        if ((hoConn->GetStateBeforeHandover(&cs), cs) == Call::StateFromDCState(dc->mState)) {
            Log(String("getHoConnection: Handover connection match found = ") + str);
            return hoConn;
        }
    }

    return NULL;
}

void CallTracker::NotifySrvccState(
    /* [in] */ ICallSrvccState state,
    /* [in] */ IArrayList* c)
{
    if (state == ICallSrvccState_STARTED && c != NULL) {
        // SRVCC started. Prepare handover connections list
        mHandoverConnections->AddAll(ICollection::Probe(c));
    }
    else if (state != ICallSrvccState_COMPLETED) {
        // SRVCC FAILED/CANCELED. Clear the handover connections list
        // Individual connections will be removed from the list in handlePollCalls()
        mHandoverConnections->Clear();
    }
    String s;
    IObject::Probe(mHandoverConnections)->ToString(&s);
    Log(String("notifySrvccState: mHandoverConnections= ") + s);
}

void CallTracker::HandleRadioAvailable()
{
    PollCallsWhenSafe();
}

AutoPtr<IMessage> CallTracker::ObtainNoPollCompleteMessage(
    /* [in] */ Int32 what)
{
    mPendingOperations++;
    mLastRelevantPoll = NULL;
    AutoPtr<IMessage> msg;
    ObtainMessage(what, (IMessage**)&msg);
    return msg;
}

Boolean CallTracker::CheckNoOperationsPending()
{
    if (DBG_POLL) Log(String("checkNoOperationsPending: pendingOperations=")
            + StringUtils::ToString(mPendingOperations));
    return mPendingOperations == 0;
}

String CallTracker::CheckForTestEmergencyNumber(
    /* [in] */ const String& _dialString)
{
    String testEn, dialString = _dialString;
    SystemProperties::Get(String("ril.test.emergencynumber"), &testEn);
    if (DBG_POLL) {
        Log(String("checkForTestEmergencyNumber: dialString=") + dialString +
            String(" testEn=") + testEn);
    }
    if (!TextUtils::IsEmpty(testEn)) {
        AutoPtr<ArrayOf<String> > values;
        StringUtils::Split(testEn, String(":"), (ArrayOf<String>**)&values);
        Log(String("checkForTestEmergencyNumber: values.length=") + StringUtils::ToString(values->GetLength()));
        if (values->GetLength() == 2) {
            String ss;
            PhoneNumberUtils::StripSeparators(dialString, &ss);
            if ((*values)[0].Equals(ss)) {
                mCi->TestingEmergencyCall();
                Log(String("checkForTestEmergencyNumber: remap ") +
                        dialString + String(" to ") + (*values)[1]);
                dialString = (*values)[1];
            }
        }
    }
    return dialString;
}

String CallTracker::ConvertNumberIfNecessary(
    /* [in] */ IPhoneBase* phoneBase,
    /* [in] */ const String& dialNumber)
{
    if (dialNumber == NULL) {
        return dialNumber;
    }
    AutoPtr<IContext> ctx;
    IPhone::Probe(phoneBase)->GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > convertMaps;
    res->GetStringArray(Elastos::Droid::R::array::dial_string_replace,
            (ArrayOf<String>**)&convertMaps);
    Log(String("convertNumberIfNecessary Roaming")
        + String(" convertMaps.length ")
        + StringUtils::ToString(convertMaps->GetLength())
        + String(" dialNumber.length() ")
        + StringUtils::ToString(dialNumber.GetLength()));

    if (convertMaps->GetLength() < 1 || dialNumber.GetLength() < VALID_COMPARE_LENGTH) {
        return dialNumber;
    }

    AutoPtr<ArrayOf<String> > entry;
    AutoPtr<ArrayOf<String> > tmpArray;
    String outNumber("");
    for(Int32 i = 0; i < convertMaps->GetLength(); i++) {
        String convertMap = (*convertMaps)[i];
        Log(String("convertNumberIfNecessary: ") + convertMap);
        entry = NULL;
        StringUtils::Split(convertMap, String(":"), (ArrayOf<String>**)&entry);
        if (entry->GetLength() > 1) {
            tmpArray = NULL;
            StringUtils::Split((*entry)[1], String(","), (ArrayOf<String>**)&tmpArray);
            if (!TextUtils::IsEmpty((*entry)[0]) && dialNumber.Equals((*entry)[0])) {
                if (tmpArray->GetLength() >= 2 && !TextUtils::IsEmpty((*tmpArray)[1])) {
                    if (CompareGid1(phoneBase, (*tmpArray)[1])) {
                        mNumberConverted = TRUE;
                    }
                }
                else if (outNumber.IsEmpty()) {
                    mNumberConverted = TRUE;
                }
                if (mNumberConverted) {
                    if(!TextUtils::IsEmpty((*tmpArray)[0]) && (*tmpArray)[0].EndWith("MDN")) {
                        String prefix = (*tmpArray)[0].Substring(0, (*tmpArray)[0].GetLength() -3);
                        String v;
                        IPhone::Probe(phoneBase)->GetLine1Number(&v);
                        outNumber = prefix + v;
                    }
                    else {
                        outNumber = (*tmpArray)[0];
                    }
                }
            }
        }
    }

    if (mNumberConverted) {
        Log(String("convertNumberIfNecessary: convert service number"));
        return outNumber;
    }

    return dialNumber;
}

Boolean CallTracker::CompareGid1(
    /* [in] */ IPhoneBase* phoneBase,
    /* [in] */ const String& serviceGid1)
{
    String gid1;
    IPhone::Probe(phoneBase)->GetGroupIdLevel1(&gid1);
    Int32 gid_length = serviceGid1.GetLength();
    Boolean ret = TRUE;

    if (serviceGid1 == NULL || serviceGid1.Equals("")) {
        Log(String("compareGid1 serviceGid is empty, return ") + StringUtils::ToString(ret));
        return ret;
    }
    // Check if gid1 match service GID1
    if (!((gid1 != NULL) && (gid1.GetLength() >= gid_length) &&
            gid1.Substring(0, gid_length).EqualsIgnoreCase(serviceGid1))) {
        Log(String(" gid1 ") + gid1 + String(" serviceGid1 ") + serviceGid1);
        ret = FALSE;
    }
    Log(String("compareGid1 is ") + String((ret ? "Same" : "Different")));
    return ret;
}

ECode CallTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("CallTracker:"));
    pw->Println(String(" mPendingOperations=") + StringUtils::ToString(mPendingOperations));
    pw->Println(String(" mNeedsPoll=") + StringUtils::ToString(mNeedsPoll));
    pw->Println(String(" mLastRelevantPoll=") + TO_CSTR(mLastRelevantPoll));
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
