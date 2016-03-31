
#ifndef __ELASTOS_DROID_SERVER_WIFI_SUPPLICANTSTATETRACKER_H__
#define __ELASTOS_DROID_SERVER_WIFI_SUPPLICANTSTATETRACKER_H__

#include "elastos/droid/ext/frameworkdef.h"
#ifdef DROID_CORE
#include "elastos/droid/net/wifi/CStateChangeResult.h"
#endif
#include "elastos/droid/net/wifi/WifiConfigStore.h"
#include "elastos/droid/utility/State.h"
#include "elastos/droid/utility/StateMachine.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiStateMachine;

/**
 * Tracks the state changes in supplicant and provides functionality
 * that is based on these state changes:
 * - detect a failed WPA handshake that loops indefinitely
 * - authentication failure handling
 */
class SupplicantStateTracker : public StateMachine
{
private:
    /********************************************************
     * HSM states
     *******************************************************/
    class DefaultState : public State
    {
    public:
        DefaultState(
            /* [in] */ SupplicantStateTracker* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        SupplicantStateTracker* mOwner;
    };

    /*
     * This indicates that the supplicant state as seen
     * by the framework is not initialized yet. We are
     * in this state right after establishing a control
     * channel connection before any supplicant events
     * or after we have lost the control channel
     * connection to the supplicant
     */
    class UninitializedState : public State
    {
    public:
        UninitializedState(
            /* [in] */ SupplicantStateTracker* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

    private:
        SupplicantStateTracker* mOwner;
    };

    class InactiveState : public State
    {
    public:
        InactiveState(
            /* [in] */ SupplicantStateTracker* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

    private:
        SupplicantStateTracker* mOwner;
    };

    class DisconnectedState : public State
    {
    public:
        DisconnectedState(
            /* [in] */ SupplicantStateTracker* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

    private:
        SupplicantStateTracker* mOwner;
    };

    class ScanState : public State
    {
    public:
        ScanState(
            /* [in] */ SupplicantStateTracker* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

    private:
        SupplicantStateTracker* mOwner;
    };

    class HandshakeState : public State
    {
    public:
        HandshakeState(
            /* [in] */ SupplicantStateTracker* owner)
            : mLoopDetectIndex(0)
            , mLoopDetectCount(0)
            , mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        /**
         * The max number of the WPA supplicant loop iterations before we
         * decide that the loop should be terminated:
         */
        static const Int32 MAX_SUPPLICANT_LOOP_ITERATIONS = 4;
        Int32 mLoopDetectIndex;
        Int32 mLoopDetectCount;
        SupplicantStateTracker* mOwner;
    };

    class CompletedState : public State
    {
    public:
        CompletedState(
            /* [in] */ SupplicantStateTracker* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        SupplicantStateTracker* mOwner;
    };

    //TODO: remove after getting rid of the state in supplicant
    class DormantState : public State
    {
    public:
        DormantState(
            /* [in] */ SupplicantStateTracker* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

    private:
        SupplicantStateTracker* mOwner;
    };

public:
    SupplicantStateTracker(
        /* [in] */ IContext* c,
        /* [in] */ WifiStateMachine* wsm,
        /* [in] */ WifiConfigStore* wcs,
        /* [in] */ ILooper* l);

    CARAPI_(void) EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    CARAPI_(String) GetSupplicantStateName();

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args)
private:
    CARAPI_(void) HandleNetworkConnectionFailure(
        /* [in] */ Int32 netId,
        /* [in] */ Int32 disableReason);

    CARAPI_(void) TransitionOnSupplicantStateChange(
        /* [in] */ IStateChangeResult* stateChangeResult);

    CARAPI_(void) SendSupplicantStateChangedBroadcast(
        /* [in] */ SupplicantState state,
        /* [in] */ Boolean failedAuth);

private:
    static const String TAG;
    static const Boolean DBG;

    WifiStateMachine* mWifiStateMachine;
    AutoPtr<WifiConfigStore> mWifiConfigStore;
    AutoPtr<IIBatteryStats> mBatteryStats;
    Int32 mAuthenticationFailuresCount;
    Int32 mAssociationRejectCount;
    /* Indicates authentication failure in supplicant broadcast.
     * TODO: enhance auth failure reporting to include notification
     * for all type of failures: EAP, WPS & WPA networks */
    Boolean mAuthFailureInSupplicantBroadcast;

    /* Maximum retries on a authentication failure notification */
    static const Int32 MAX_RETRIES_ON_AUTHENTICATION_FAILURE = 2;

    /* Maximum retries on assoc rejection events */
    static const Int32 MAX_RETRIES_ON_ASSOCIATION_REJECT = 16;

    /* Tracks if networks have been disabled during a connection */
    Boolean mNetworksDisabledDuringConnect;

    AutoPtr<IContext> mContext;

    AutoPtr<State> mUninitializedState;
    AutoPtr<State> mDefaultState;
    AutoPtr<State> mInactiveState;
    AutoPtr<State> mDisconnectState;
    AutoPtr<State> mScanState;
    AutoPtr<State> mHandshakeState;
    AutoPtr<State> mCompletedState;
    AutoPtr<State> mDormantState;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_SERVER_WIFI_SUPPLICANTSTATETRACKER_H__
