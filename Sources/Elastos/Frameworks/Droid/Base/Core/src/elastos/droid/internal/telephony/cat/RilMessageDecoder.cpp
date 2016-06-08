
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/RilMessageDecoder.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                    RilMessageDecoder::StateStart
//=====================================================================
ECode RilMessageDecoder::StateStart::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (msg.what == CMD_START) {
    //     if (decodeMessageParams((RilMessage)msg.obj)) {
    //         transitionTo(mStateCmdParamsReady);
    //     }
    // } else {
    //     CatLog.d(this, "StateStart unexpected expecting START=" +
    //              CMD_START + " got " + msg.what);
    // }
    // return true;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                RilMessageDecoder::StateCmdParamsReady
//=====================================================================
ECode RilMessageDecoder::StateCmdParamsReady::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (msg.what == CMD_PARAMS_READY) {
    //     mCurrentRilMessage.mResCode = ResultCode.fromInt(msg.arg1);
    //     mCurrentRilMessage.mData = msg.obj;
    //     sendCmdForExecution(mCurrentRilMessage);
    //     transitionTo(mStateStart);
    // } else {
    //     CatLog.d(this, "StateCmdParamsReady expecting CMD_PARAMS_READY="
    //              + CMD_PARAMS_READY + " got " + msg.what);
    //     deferMessage(msg);
    // }
    // return true;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                          RilMessageDecoder
//=====================================================================
const Int32 RilMessageDecoder::CMD_START;
const Int32 RilMessageDecoder::CMD_PARAMS_READY;
Int32 RilMessageDecoder::mSimCount = 0;
AutoPtr<ArrayOf<RilMessageDecoder*> > RilMessageDecoder::mInstance = NULL;

// synchronized
AutoPtr<RilMessageDecoder> RilMessageDecoder::GetInstance(
    /* [in] */ IHandler* caller,
    /* [in] */ IIccFileHandler* fh,
    /* [in] */ Int32 slotId)
{
    // ==================before translated======================
    // if (null == mInstance) {
    //     mSimCount = TelephonyManager.getDefault().getSimCount();
    //     mInstance = new RilMessageDecoder[mSimCount];
    //     for (int i = 0; i < mSimCount; i++) {
    //         mInstance[i] = null;
    //     }
    // }
    //
    // if (slotId != SubscriptionManager.INVALID_SLOT_ID && slotId < mSimCount) {
    //     if (null == mInstance[slotId]) {
    //         mInstance[slotId] = new RilMessageDecoder(caller, fh);
    //     }
    // } else {
    //     CatLog.d("RilMessageDecoder", "invaild slot id: " + slotId);
    //     return null;
    // }
    //
    // return mInstance[slotId];
    assert(0);
    AutoPtr<RilMessageDecoder> empty;
    return empty;
}

ECode RilMessageDecoder::SendStartDecodingMessageParams(
    /* [in] */ RilMessage* rilMsg)
{
    // ==================before translated======================
    // Message msg = obtainMessage(CMD_START);
    // msg.obj = rilMsg;
    // sendMessage(msg);
    assert(0);
    return NOERROR;
}

ECode RilMessageDecoder::SendMsgParamsDecoded(
    /* [in] */ ResultCode resCode,
    /* [in] */ CommandParams* cmdParams)
{
    // ==================before translated======================
    // Message msg = obtainMessage(RilMessageDecoder.CMD_PARAMS_READY);
    // msg.arg1 = resCode.value();
    // msg.obj = cmdParams;
    // sendMessage(msg);
    assert(0);
    return NOERROR;
}

ECode RilMessageDecoder::Dispose(
    /* [in] */ Int32 slotId)
{
    // ==================before translated======================
    // quitNow();
    // mStateStart = null;
    // mStateCmdParamsReady = null;
    // mCmdParamsFactory.dispose();
    // mCmdParamsFactory = null;
    // mCurrentRilMessage = null;
    // mCaller = null;
    // mInstance[slotId] = null;
    assert(0);
    return NOERROR;
}

RilMessageDecoder::RilMessageDecoder(
    /* [in] */ IHandler* caller,
    /* [in] */ IIccFileHandler* fh)
{
    // ==================before translated======================
    // super("RilMessageDecoder");
    //
    // addState(mStateStart);
    // addState(mStateCmdParamsReady);
    // setInitialState(mStateStart);
    //
    // mCaller = caller;
    // mCmdParamsFactory = CommandParamsFactory.getInstance(this, fh);
}

RilMessageDecoder::RilMessageDecoder()
{
    // ==================before translated======================
    // super("RilMessageDecoder");
}

void RilMessageDecoder::SendCmdForExecution(
    /* [in] */ RilMessage* rilMsg)
{
    // ==================before translated======================
    // Message msg = mCaller.obtainMessage(CatService.MSG_ID_RIL_MSG_DECODED,
    //         new RilMessage(rilMsg));
    // msg.sendToTarget();
    assert(0);
}

Boolean RilMessageDecoder::DecodeMessageParams(
    /* [in] */ RilMessage* rilMsg)
{
    // ==================before translated======================
    // boolean decodingStarted;
    //
    // mCurrentRilMessage = rilMsg;
    // switch(rilMsg.mId) {
    // case CatService.MSG_ID_SESSION_END:
    // case CatService.MSG_ID_CALL_SETUP:
    //     mCurrentRilMessage.mResCode = ResultCode.OK;
    //     sendCmdForExecution(mCurrentRilMessage);
    //     decodingStarted = false;
    //     break;
    // case CatService.MSG_ID_PROACTIVE_COMMAND:
    // case CatService.MSG_ID_EVENT_NOTIFY:
    // case CatService.MSG_ID_REFRESH:
    //     byte[] rawData = null;
    //     try {
    //         rawData = IccUtils.hexStringToBytes((String) rilMsg.mData);
    //     } catch (Exception e) {
    //         // zombie messages are dropped
    //         CatLog.d(this, "decodeMessageParams dropping zombie messages");
    //         decodingStarted = false;
    //         break;
    //     }
    //     try {
    //         // Start asynch parsing of the command parameters.
    //         mCmdParamsFactory.make(BerTlv.decode(rawData));
    //         decodingStarted = true;
    //     } catch (ResultException e) {
    //         // send to Service for proper RIL communication.
    //         CatLog.d(this, "decodeMessageParams: caught ResultException e=" + e);
    //         mCurrentRilMessage.mResCode = e.result();
    //         sendCmdForExecution(mCurrentRilMessage);
    //         decodingStarted = false;
    //     }
    //     break;
    // default:
    //     decodingStarted = false;
    //     break;
    // }
    // return decodingStarted;
    assert(0);
    return FALSE;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
