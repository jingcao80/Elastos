
#include "elastos/droid/teleservice/phone/CCLIRListPreference.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const Int32 CCLIRListPreference::MyHandler::MESSAGE_GET_CLIR = 0;
const Int32 CCLIRListPreference::MyHandler::MESSAGE_SET_CLIR = 1;

CCLIRListPreference::MyHandler::MyHandler(
    /* [in] */ CCLIRListPreference* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCLIRListPreference::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_GET_CLIR:
            HandleGetCLIRResponse(msg);
            break;
        case MESSAGE_SET_CLIR:
            HandleSetCLIRResponse(msg);
            break;
    }
    return NOERROR;
}


void CCLIRListPreference::MyHandler::HandleGetCLIRResponse(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);

    Int32 arg2;
    msg->GetArg2(&arg2);
    if (arg2 == MESSAGE_SET_CLIR) {
        mTcpListener->OnFinished(mHost, FALSE);
    }
    else {
        mTcpListener->OnFinished(mHost, TRUE);
    }
    mHost->mClirArray = NULL;
    if (ar->mException != NULL) {
        if (DBG) {
            StringBuilder sb;
            sb += "handleGetCLIRResponse: ar.exception=";
            sb += TO_CSTR(ar->mException);
            Logger::D(TAG, sb.ToString());
        }
        mTcpListener->OnException(mHost, ICommandException::Probe(ar->mException)));
    }
    else if (IThrowable::Probe(ar->mUserObj) != NULL) {
        mTcpListener->OnError(mHost, RESPONSE_ERROR);
    }
    else {
        AutoPtr<IArrayOf> clirArray = IArrayOf::Probe(ar->mResult);
        if (clirArray->GetLength() != 2) {
            mTcpListener->OnError(mHost, RESPONSE_ERROR);
        }
        else {
            if (DBG) {
                StringBuilder sb;
                sb += "handleGetCLIRResponse: CLIR successfully queried,";
                sb += " clirArray[0]=";
                AutoPtr<IInterface> obj1;
                clirArray->Get(0, (IInterface**)&obj1);
                AutoPtr<IInteger32> intObj1 = IInteger32::Probe(obj1);
                Int32 value1;
                intObj1->GetValue(&value1);
                sb += value1;
                sb += ", clirArray[1]=";
                AutoPtr<IInterface> obj2;
                clirArray->Get(1, (IInterface**)&obj2);
                AutoPtr<IInteger32> intObj2 = IInteger32::Probe(obj2);
                Int32 value2;
                intObj2->GetValue(&value2);
                sb += value2;
                Logger::D(TAG, sb.ToString());
            }
            HandleGetCLIRResult(clirArray);
        }
    }
}

void CCLIRListPreference::MyHandler::HandleSetCLIRResponse(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);

    if (ar->mException != NULL) {
        if (DBG) {
            StringBuilder sb;
            sb += "handleSetCallWaitingResponse: ar.exception=";
            sb += TO_CSTR(ar->mException);
            Logger::D(TAG, sb.ToString());
        }
        //setEnabled(false);
    }
    if (DBG) Logger::D(TAG, String("handleSetCallWaitingResponse: re get"));

    AutoPtr<IMessage> msg;
    ObtainMessage(MESSAGE_GET_CLIR,
            MESSAGE_SET_CLIR, MESSAGE_SET_CLIR, ar->mException, (IMessage**)&msg);
    mPhone->GetOutgoingCallerIdDisplay(msg);
}

const String CCLIRListPreference::TAG("CLIRListPreference");

CAR_INTERFACE_IMPL(CCLIRListPreference, ListPreference, ICLIRListPreference)

CAR_OBJECT_IMPL(CCLIRListPreference)

CCLIRListPreference::CCLIRListPreference()
{
    DBG = PhoneGlobals::DBG_LEVEL >= 2;
    mHandler = new MyHandler(this);
}

ECode CCLIRListPreference::constructor()
{
    assert(0)
}


} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos