
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCSWITCHSTATE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCSWITCHSTATE_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/internal/utility/StateMachine.h>

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

class DcSwitchState
    : public StateMachine
    , public IDcSwitchState
{
private:
    class IdleState
        : public State
    {
    public:
        IdleState(
            /* [in] */ DcSwitchState* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DcSwitchState* mHost;
    };

    class ActingState
        : public State
    {
    public:
        ActingState(
            /* [in] */ DcSwitchState* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DcSwitchState* mHost;
    };

    class ActedState
        : public State
    {
    public:
        ActedState(
            /* [in] */ DcSwitchState* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DcSwitchState* mHost;
    };

    class DeactingState
        : public State
    {
    public:
        DeactingState(
            /* [in] */ DcSwitchState* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DcSwitchState* mHost;
    };

    class DefaultState
        : public State
    {
    public:
        DefaultState(
            /* [in] */ DcSwitchState* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DcSwitchState* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DcSwitchState();

    CARAPI constructor(
        /* [in] */ IPhone* phone,
        /* [in] */ const String& name,
        /* [in] */ Int32 id);

    CARAPI NotifyDataConnection(
        /* [in] */ Int32 phoneId,
        /* [in] */ const String& state,
        /* [in] */ const String& reason,
        /* [in] */ const String& apnName,
        /* [in] */ const String& apnType,
        /* [in] */ Boolean unavailable);

    CARAPI CleanupAllConnection();

    CARAPI RegisterForIdle(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForIdle(
        /* [in] */ IHandler* h);

    CARAPI TransitToIdleState();

    CARAPI TransitToActingState();

    CARAPI Log(
        /* [in] */ const char *fmt, ...);

private:
    CARAPI SetupConnection(
        /* [in] */ const String& type,
        /* [out] */ Int32* result);

    CARAPI TeardownConnection(
        /* [in] */ const String& type,
        /* [out] */ Int32* result);

    CARAPI RequestDataIdle();

private:
    static const Boolean DBG;

    static const Boolean VDBG;

    static const String LOG__TAG;

    // ***** Event codes for driving the state machine
    static const Int32 BASE;

    static const Int32 EVENT_CONNECT;

    static const Int32 EVENT_DISCONNECT;

    static const Int32 EVENT_CLEANUP_ALL;

    static const Int32 EVENT_CONNECTED;

    static const Int32 EVENT_DETACH_DONE;

    static const Int32 EVENT_TO_IDLE_DIRECTLY;

    static const Int32 EVENT_TO_ACTING_DIRECTLY;

    Int32 mId;

    AutoPtr<IPhone> mPhone;

    AutoPtr<IAsyncChannel> mAc;

    AutoPtr<IRegistrantList> mIdleRegistrants;

    AutoPtr<IHashSet> mApnTypes;

    AutoPtr<IdleState> mIdleState;

    AutoPtr<ActingState> mActingState;

    AutoPtr<ActedState> mActedState;

    AutoPtr<DeactingState> mDeactingState;

    AutoPtr<DefaultState> mDefaultState;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCSWITCHSTATE_H__
