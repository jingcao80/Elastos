#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RILMESSAGEDECODER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RILMESSAGEDECODER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/telephony/cat/RilMessage.h"
#include "elastos/droid/internal/telephony/cat/CommandParamsFactory.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class used for queuing raw ril messages, decoding them into CommanParams
  * objects and sending the result back to the CAT Service.
  */
class RilMessageDecoder
    : public StateMachine
{
private:
    class StateStart
        : public State
    {
    public:
        StateStart(
            /* [in] */ RilMessageDecoder* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    public:
        RilMessageDecoder* mHost;
    };

    class StateCmdParamsReady
        : public State
    {
    public:
        StateCmdParamsReady(
            /* [in] */ RilMessageDecoder* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    public:
        RilMessageDecoder* mHost;
    };

public:
    /**
      * Get the singleton instance, constructing if necessary.
      *
      * @param caller
      * @param fh
      * @return RilMesssageDecoder
      */
    // synchronized
    static CARAPI_(AutoPtr<RilMessageDecoder>) GetInstance(
        /* [in] */ IHandler* caller,
        /* [in] */ IIccFileHandler* fh,
        /* [in] */ Int32 slotId);

    /**
      * Start decoding the message parameters,
      * when complete MSG_ID_RIL_MSG_DECODED will be returned to caller.
      *
      * @param rilMsg
      */
    virtual CARAPI SendStartDecodingMessageParams(
        /* [in] */ RilMessage* rilMsg);

    /**
      * The command parameters have been decoded.
      *
      * @param resCode
      * @param cmdParams
      */
    virtual CARAPI SendMsgParamsDecoded(
        /* [in] */ ResultCode resCode,
        /* [in] */ CommandParams* cmdParams);

    virtual CARAPI Dispose(
        /* [in] */ Int32 slotId);

private:
    RilMessageDecoder(
        /* [in] */ IHandler* caller,
        /* [in] */ IIccFileHandler* fh);

    RilMessageDecoder();

    CARAPI_(void) SendCmdForExecution(
        /* [in] */ RilMessage* rilMsg);

    CARAPI_(Boolean) DecodeMessageParams(
        /* [in] */ RilMessage* rilMsg);

private:
    // constants
    static const Int32 CMD_START = 1;
    static const Int32 CMD_PARAMS_READY = 2;
    // members
    AutoPtr<CommandParamsFactory> mCmdParamsFactory;
    AutoPtr<RilMessage> mCurrentRilMessage;
    AutoPtr<IHandler> mCaller;
    static Int32 mSimCount;// = 0;
    static AutoPtr<ArrayOf<RilMessageDecoder*> > mInstance;
    // States
    AutoPtr<StateStart> mStateStart;
    AutoPtr<StateCmdParamsReady> mStateCmdParamsReady;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RILMESSAGEDECODER_H__

