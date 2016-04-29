#ifndef __ELASTOS_DROID_SERVER_WIFIP2PSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_WIFIP2PSERVICEIMPL_H__

#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/wifi/WifiNative.h"
#include "elastos/droid/server/wifi/WifiMonitor.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnKeyListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IDhcpStateMachine;
using Elastos::Net::IInetAddress;;
using Elastos::Droid::Server::Wifi::WifiNative;
using Elastos::Droid::Server::Wifi::WifiMonitor;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::IIWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroupList;
using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::IWifiP2pWfdInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroupListGroupDeleteListener;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceInfo;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceRequest;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceResponse;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {
namespace P2p {

class WifiP2pServiceImpl
    : public Object
    , public IIWifiP2pManager
    , public IBinder
{
public:
    /**
     * Error code definition.
     * see the Table.8 in the WiFi Direct specification for the detail.
     */
    enum P2pStatus {
        /* Success. */
        P2pStatus_SUCCESS,

        /* The target device is currently unavailable. */
        P2pStatus_INFORMATION_IS_CURRENTLY_UNAVAILABLE,

        /* Protocol error. */
        P2pStatus_INCOMPATIBLE_PARAMETERS,

        /* The target device reached the limit of the number of the connectable device.
         * For example, device limit or group limit is set. */
        P2pStatus_LIMIT_REACHED,

        /* Protocol error. */
        P2pStatus_INVALID_PARAMETER,

        /* Unable to accommodate request. */
        P2pStatus_UNABLE_TO_ACCOMMODATE_REQUEST,

        /* Previous protocol error, or disruptive behavior. */
        P2pStatus_PREVIOUS_PROTOCOL_ERROR,

        /* There is no common channels the both devices can use. */
        P2pStatus_NO_COMMON_CHANNEL,

        /* Unknown p2p group. For example, Device A tries to invoke the previous persistent group,
         *  but device B has removed the specified credential already. */
        P2pStatus_UNKNOWN_P2P_GROUP,

        /* Both p2p devices indicated an intent of 15 in group owner negotiation. */
        P2pStatus_BOTH_GO_INTENT_15,

        /* Incompatible provisioning method. */
        P2pStatus_INCOMPATIBLE_PROVISIONING_METHOD,

        /* Rejected by user */
        P2pStatus_REJECTED_BY_USER,

        /* Unknown error */
        P2pStatus_UNKNOWN
    };
    /**
     * Information about a particular client and we track the service discovery requests
     * and the local services registered by the client.
     */
    class ClientInfo
        : public Object
    {
    public:
        ClientInfo(
            /* [in] */ IMessenger* m,
            /* [in] */ WifiP2pServiceImpl* host);

        CARAPI GetMessenger(
            /* [out] */ IMessenger** messenger);

        CARAPI SetMessenger(
            /* [in] */ IMessenger* messenger);

    public:
        /*
         * A reference to WifiP2pManager.Channel handler.
         * The response of this request is notified to WifiP2pManager.Channel handler
         */
        AutoPtr<IMessenger> mMessenger;

        /*
         * A service discovery request list.
         */
        HashMap<Int32, AutoPtr<IWifiP2pServiceRequest> > mReqList;

        /*
         * A local service information list.
         */
        List<AutoPtr<IWifiP2pServiceInfo> > mServList;

        WifiP2pServiceImpl* mHost;
    };

    class P2pStateMachine
        : public StateMachine
    {
    public:
        class DefaultState : public State
        {
        public:
            DefaultState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("DefaultState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class P2pNotSupportedState : public State
        {
        public:
            P2pNotSupportedState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("P2pNotSupportedState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class P2pDisablingState : public State
        {
        public:
            P2pDisablingState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            // @Override
            CARAPI Exit();

            CARAPI_(String) GetName()
            {
                return String("P2pDisablingState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class P2pDisabledState : public State
        {
        public:
            P2pDisabledState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("P2pDisabledState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class P2pEnablingState : public State
        {
        public:
            P2pEnablingState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("P2pEnablingState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class P2pEnabledState : public State
        {
        public:
            P2pEnabledState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            // @Override
            CARAPI Exit();

            CARAPI_(String) GetName()
            {
                return String("P2pEnabledState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class InactiveState : public State
        {
        public:
            InactiveState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("InactiveState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class GroupCreatingState : public State
        {
        public:
            GroupCreatingState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("GroupCreatingState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class UserAuthorizingNegotiationRequestState : public State
        {
        public:
            UserAuthorizingNegotiationRequestState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            // @Override
            CARAPI Exit();

            CARAPI_(String) GetName()
            {
                return String("UserAuthorizingNegotiationRequestState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class UserAuthorizingInviteRequestState : public State
        {
        public:
            UserAuthorizingInviteRequestState (
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            // @Override
            CARAPI Exit();

            CARAPI_(String) GetName()
            {
                return String("UserAuthorizingInviteRequestState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class ProvisionDiscoveryState : public State
        {
        public:
            ProvisionDiscoveryState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("ProvisionDiscoveryState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class GroupNegotiationState : public State
        {
        public:
            GroupNegotiationState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("GroupNegotiationState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class FrequencyConflictState : public State
        {
        public:
            FrequencyConflictState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            CARAPI NotifyFrequencyConflict();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI Exit();

            CARAPI_(String) GetName()
            {
                return String("FrequencyConflictState");
            }
        private:
            P2pStateMachine* mHost;
            AutoPtr<IAlertDialog> mFrequencyConflictDialog;
        };

        class GroupCreatedState : public State
        {
        public:
            GroupCreatedState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI Exit();

            CARAPI_(String) GetName()
            {
                return String("GroupCreatedState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class UserAuthorizingJoinState : public State
        {
        public:
            UserAuthorizingJoinState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            // @Override
            CARAPI Exit();

            CARAPI_(String) GetName()
            {
                return String("UserAuthorizingJoinState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class OngoingGroupRemovalState : public State
        {
        public:
            OngoingGroupRemovalState(
                /* [in] */ P2pStateMachine* host);

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* message,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("OngoingGroupRemovalState");
            }
        private:
            P2pStateMachine* mHost;
        };

        class GroupDeleteListener
            : public Object
            , public IWifiP2pGroupListGroupDeleteListener
        {
        public:
            CAR_INTERFACE_DECL()

            GroupDeleteListener(
                /* [in] */ P2pStateMachine* host);

            CARAPI OnDeleteGroup(
                /* [in] */ Int32 netId);

        private:
            P2pStateMachine* mHost;
        };

        class PositiveButtonListener
            : public Object
            , public IDialogInterfaceOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            PositiveButtonListener(
                /* [in] */ P2pStateMachine* host);

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);
        private:
            P2pStateMachine* mHost;
        };

        class NegativeButtonListener
            : public Object
            , public IDialogInterfaceOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            NegativeButtonListener(
                /* [in] */ P2pStateMachine* host);

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);
        private:
            P2pStateMachine* mHost;
        };

        class DialogCancelListener
            : public Object
            , public IDialogInterfaceOnCancelListener
        {
        public:
            CAR_INTERFACE_DECL()

            DialogCancelListener(
                /* [in] */ P2pStateMachine* host);

            CARAPI OnCancel(
                /* [in] */ IDialogInterface* dialog);
        private:
            P2pStateMachine* mHost;
        };

        class NotifyP2pProvDiscShowPinRequestOnClickListener
            : public Object
            , public IDialogInterfaceOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            NotifyP2pProvDiscShowPinRequestOnClickListener(
                /* [in] */ P2pStateMachine* host,
                /* [in] */ const String& address,
                /* [in] */ const String& pin);

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);
        private:
            P2pStateMachine* mHost;
            String mAddress;
            String mPin;
        };

        class NotifyInvitationReceivedPositiveButtonListener
            : public Object
            , public IDialogInterfaceOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            NotifyInvitationReceivedPositiveButtonListener(
                /* [in] */ P2pStateMachine* host,
                /* [in] */ IWpsInfo* wpsInfo,
                /* [in] */ IEditText* pin);

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);
        private:
            P2pStateMachine* mHost;
            AutoPtr<IWpsInfo> mWpsInfo;
            AutoPtr<IEditText> mPin;
        };

        class NotifyInvitationReceivedNegativeButtonListener
            : public Object
            , public IDialogInterfaceOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            NotifyInvitationReceivedNegativeButtonListener(
                /* [in] */ P2pStateMachine* host);

            CARAPI OnClick(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 which);
        private:
            P2pStateMachine* mHost;
        };

        class NotifyInvitationReceivedDialogCancelListener
            : public Object
            , public IDialogInterfaceOnCancelListener
        {
        public:
            CAR_INTERFACE_DECL()

            NotifyInvitationReceivedDialogCancelListener(
                /* [in] */ P2pStateMachine* host);

            CARAPI OnCancel(
                /* [in] */ IDialogInterface* dialog);
        private:
            P2pStateMachine* mHost;
        };

        class NotifyInvitationReceivedDialogKeyListener
            : public Object
            , public IDialogInterfaceOnKeyListener
        {
        public:
            CAR_INTERFACE_DECL()

            NotifyInvitationReceivedDialogKeyListener(
                /* [in] */ P2pStateMachine* host,
                /* [in] */ IAlertDialog* dialog);

            CARAPI OnKey(
                /* [in] */ IDialogInterface* dialog,
                /* [in] */ Int32 keyCode,
                /* [in] */ IKeyEvent* event,
                /* [out] */ Boolean* consumed);
        private:
            P2pStateMachine* mHost;
            AutoPtr<IAlertDialog> mDialog;
        };

    public:
        P2pStateMachine(
            /* [in] */ const String& name,
            /* [in] */ ILooper* looper,
            /* [in] */ Boolean p2pSupported,
            /* [in] */ WifiP2pServiceImpl* host);

        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

        CARAPI SendP2pStateChangedBroadcast(
            /* [in] */ Boolean enabled);

        CARAPI SendP2pDiscoveryChangedBroadcast(
            /* [in] */ Boolean started);

        CARAPI SendThisDeviceChangedBroadcast();

        CARAPI SendPeersChangedBroadcast();

        CARAPI SendP2pConnectionChangedBroadcast();

        CARAPI SendMiracastModeChanged(
            /* [in] */ Int32 mode);

        CARAPI SendP2pPersistentGroupsChangedBroadcast();

        CARAPI StartDhcpServer(
            /* [in] */ const String& intf);

        CARAPI StopDhcpServer(
            /* [in] */ const String& intf);

        CARAPI NotifyP2pEnableFailure();

        CARAPI AddRowToDialog(
            /* [in] */ IViewGroup* group,
            /* [in] */ Int32 stringId,
            /* [in] */ const String& value);

        CARAPI NotifyInvitationSent(
            /* [in] */ const String& pin,
            /* [in] */ const String& peerAddress);

        CARAPI NotifyP2pProvDiscShowPinRequest(
            /* [in] */ const String& pin,
            /* [in] */ const String& peerAddress);

        CARAPI NotifyInvitationReceived();

        /**
         * Synchronize the persistent group list between
         * wpa_supplicant and mGroups.
         */
        CARAPI UpdatePersistentNetworks(
            /* [in] */ Boolean reload);

        /**
         * A config is valid if it has a peer address that has already been
         * discovered
         * @return true if it is invalid, false otherwise
         */
        CARAPI_(Boolean) IsConfigInvalid(
            /* [in] */ IWifiP2pConfig* config);

        /* TODO: The supplicant does not provide group capability changes as an event.
         * Having it pushed as an event would avoid polling for this information right
         * before a connection
         */
        CARAPI_(AutoPtr<IWifiP2pDevice>) FetchCurrentDeviceDetails(
            /* [in] */ IWifiP2pConfig* config);

        /**
         * Start a p2p group negotiation and display pin if necessary
         * @param config for the peer
         */
        CARAPI P2pConnectWithPinDisplay(
            /* [in] */ IWifiP2pConfig* config);

        /**
         * Reinvoke a persistent group.
         *
         * @param config for the peer
         * @return true on success, false on failure
         */
        CARAPI_(Boolean) ReinvokePersistentGroup(
            /* [in] */ IWifiP2pConfig* config);

        /**
         * Return the network id of the group owner profile which has the p2p client with
         * the specified device address in it's client list.
         * If more than one persistent group of the same address is present in its client
         * lists, return the first one.
         *
         * @param deviceAddress p2p device address.
         * @return the network id. if not found, return -1.
         */
        CARAPI_(Int32) GetNetworkIdFromClientList(
            /* [in] */ const String& deviceAddress);

        /**
         * Return p2p client list associated with the specified network id.
         * @param netId network id.
         * @return p2p client list. if not found, return null.
         */
        CARAPI_(AutoPtr<ArrayOf<String> >) GetClientList(
            /* [in] */ Int32 netId);

        /**
         * Remove the specified p2p client from the specified profile.
         * @param netId network id of the profile.
         * @param addr p2p client address to be removed.
         * @param isRemovable if true, remove the specified profile if its client list becomes empty.
         * @return whether removing the specified p2p client is successful or not.
         */
        CARAPI_(Boolean) RemoveClientFromList(
            /* [in] */ Int32 netId,
            /* [in] */ const String& addr,
            /* [in] */ Boolean isRemovable);

        CARAPI SetWifiP2pInfoOnGroupFormation(
            /* [in] */ IInetAddress* serverInetAddress);

        CARAPI ResetWifiP2pInfo();

        CARAPI_(String) GetDeviceName(
            /* [in] */ const String& deviceAddress);

        CARAPI_(String) GetPersistedDeviceName();

        CARAPI_(Boolean) SetAndPersistDeviceName(
            /* [in] */ const String& devName);

        CARAPI_(Boolean) SetWfdInfo(
            /* [in] */ IWifiP2pWfdInfo* wfdInfo);

        CARAPI InitializeP2pSettings();

        CARAPI UpdateThisDevice(
            /* [in] */ Int32 status);

        CARAPI HandleGroupCreationFailure();

        CARAPI HandleGroupRemoved();

        //State machine initiated requests can have replyTo set to null indicating
        //there are no recipients, we ignore those reply actions
        CARAPI ReplyToMessage(
            /* [in] */ IMessage* msg,
            /* [in] */ Int32 what);

        CARAPI ReplyToMessage(
            /* [in] */ IMessage* msg,
            /* [in] */ Int32 what,
            /* [in] */ Int32 arg1);

        CARAPI ReplyToMessage(
            /* [in] */ IMessage* msg,
            /* [in] */ Int32 what,
            /* [in] */ IInterface* obj);

        using StateMachine::ObtainMessage;

        /* arg2 on the source message has a hash code that needs to be retained in replies
         * see WifiP2pManager for details */
        CARAPI_(AutoPtr<IMessage>) ObtainMessage(
            /* [in] */ IMessage* srcMsg);

        CARAPI Logd(
            /* [in] */ const String& s);

        CARAPI Loge(
            /* [in] */ const String& s);

        CARAPI Logd(
            /* [in] */ const char* s);

        CARAPI Loge(
            /* [in] */ const char* s);

        static CARAPI_(String) CmdToString(
            /* [in] */ Int32 what);

        /**
         * Update service discovery request to wpa_supplicant.
         */
        CARAPI_(Boolean) UpdateSupplicantServiceRequest();

        /**
         * Clear service discovery request in wpa_supplicant
         */
        CARAPI ClearSupplicantServiceRequest();

        /* TODO: We could track individual service adds separately and avoid
         * having to do update all service requests on every new request
         */
        CARAPI_(Boolean) AddServiceRequest(
            /* [in] */ IMessenger* m,
            /* [in] */ IWifiP2pServiceRequest* req);

        CARAPI RemoveServiceRequest(
            /* [in] */ IMessenger* m,
            /* [in] */ IWifiP2pServiceRequest* req);

        CARAPI ClearServiceRequests(
            /* [in] */ IMessenger* m);

        CARAPI_(Boolean) AddLocalService(
            /* [in] */ IMessenger* m,
            /* [in] */ IWifiP2pServiceInfo* servInfo);

        CARAPI RemoveLocalService(
            /* [in] */ IMessenger* m,
            /* [in] */ IWifiP2pServiceInfo* servInfo);

        CARAPI ClearLocalServices(
            /* [in] */ IMessenger* m);

        CARAPI ClearClientInfo(
            /* [in] */ IMessenger* m);

        /**
         * Send the service response to the WifiP2pManager.Channel.
         *
         * @param resp
         */
        CARAPI SendServiceResponse(
            /* [in] */ IWifiP2pServiceResponse* resp);

        /**
         * We dont get notifications of clients that have gone away.
         * We detect this actively when services are added and throw
         * them away.
         *
         * TODO: This can be done better with full async channels.
         */
        CARAPI ClearClientDeadChannels();

        /**
         * Return the specified ClientInfo.
         * @param m Messenger
         * @param createIfNotExist if true and the specified channel info does not exist,
         * create new client info.
         * @return the specified ClientInfo.
         */
        CARAPI GetClientInfo(
            /* [in] */ IMessenger* m,
            /* [in] */ Boolean createIfNotExist,
            /* [out] */ ClientInfo** clientInfo);

    public:
        AutoPtr<DefaultState> mDefaultState;// = new DefaultState();
        AutoPtr<P2pNotSupportedState> mP2pNotSupportedState;// = new P2pNotSupportedState();
        AutoPtr<P2pDisablingState> mP2pDisablingState;// = new P2pDisablingState();
        AutoPtr<P2pDisabledState> mP2pDisabledState;// = new P2pDisabledState();
        AutoPtr<P2pEnablingState> mP2pEnablingState;// = new P2pEnablingState();
        AutoPtr<P2pEnabledState> mP2pEnabledState;// = new P2pEnabledState();
        // Inactive is when p2p is enabled with no connectivity
        AutoPtr<InactiveState> mInactiveState;// = new InactiveState();
        AutoPtr<GroupCreatingState> mGroupCreatingState;// = new GroupCreatingState();
        AutoPtr<UserAuthorizingInviteRequestState> mUserAuthorizingInviteRequestState;// = new UserAuthorizingInviteRequestState();
        AutoPtr<UserAuthorizingNegotiationRequestState> mUserAuthorizingNegotiationRequestState;// = new UserAuthorizingNegotiationRequestState();
        AutoPtr<ProvisionDiscoveryState> mProvisionDiscoveryState;// = new ProvisionDiscoveryState();
        AutoPtr<GroupNegotiationState> mGroupNegotiationState;// = new GroupNegotiationState();
        AutoPtr<FrequencyConflictState> mFrequencyConflictState;// =new FrequencyConflictState();

        AutoPtr<GroupCreatedState> mGroupCreatedState;
        AutoPtr<UserAuthorizingJoinState> mUserAuthorizingJoinState;// = new UserAuthorizingJoinState();
        AutoPtr<OngoingGroupRemovalState> mOngoingGroupRemovalState;// = new OngoingGroupRemovalState();

        AutoPtr<WifiNative> mWifiNative;
        AutoPtr<WifiMonitor> mWifiMonitor;

        AutoPtr<IWifiP2pDeviceList> mPeers;

        // /* During a connection, supplicant can tell us that a device was lost. From a supplicant's
        //  * perspective, the discovery stops during connection and it purges device since it does
        //  * not get latest updates about the device without being in discovery state.
        //  *
        //  * From the framework perspective, the device is still there since we are connecting or
        //  * connected to it. so we keep these devices in a seperate list, so that they are removed
        //  * when connection is cancelled or lost
        //  */
        AutoPtr<IWifiP2pDeviceList> mPeersLostDuringConnection;
        AutoPtr<IWifiP2pGroupList> mGroups;

        AutoPtr<IWifiP2pInfo> mWifiP2pInfo;
        AutoPtr<IWifiP2pGroup> mGroup;
        Boolean mPendingReformGroupIndication;// = false;

        // Saved WifiP2pConfig for a peer connection
        AutoPtr<IWifiP2pConfig> mSavedPeerConfig;

        // Saved WifiP2pGroup from invitation request
        AutoPtr<IWifiP2pGroup> mSavedP2pGroup;

        WifiP2pServiceImpl* mHost;
    };

private:
    /**
     * Handles client connections
     */
    class ClientHandler : public Handler
    {
    public:
        TO_STRING_IMPL("WifiP2pServiceImpl::ClientHandler")

        ClientHandler(
            /* [in] */ WifiP2pServiceImpl* host,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        WifiP2pServiceImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    WifiP2pServiceImpl();

    ~WifiP2pServiceImpl();

    CARAPI constructor(
        /* [in] */ IContext * context);

    CARAPI ConnectivityServiceReady();

    CARAPI GetMessenger(
        /* [out] */ IMessenger** msg);

    /**
     * Get a reference to handler. This is used by a WifiStateMachine to establish
     * an AsyncChannel communication with P2pStateMachine
     * @hide
     */
    CARAPI GetP2pStateMachineMessenger(
        /* [out] */ IMessenger** messenger);

    CARAPI_(void) EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    /** This is used to provide information to drivers to optimize performance depending
     * on the current mode of operation.
     * 0 - disabled
     * 1 - source operation
     * 2 - sink operation
     *
     * As an example, the driver could reduce the channel dwell time during scanning
     * when acting as a source or sink to minimize impact on miracast.
     */
    CARAPI SetMiracastMode(
        /* [in] */ Int32 mode);

    static CARAPI_(P2pStatus) ValueOf(
        /* [in] */ Int32 error);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI EnforceAccessPermission();

    CARAPI EnforceChangePermission();

    CARAPI EnforceConnectivityInternalPermission();

    CARAPI_(Int32) CheckConnectivityInternalPermission();

    CARAPI_(Int32) CheckLocationHardwarePermission();

    CARAPI EnforceConnectivityInternalOrLocationHardwarePermission();
public:
    static const String TAG;
    static Boolean DBG;
    static const String NETWORKTYPE;

    static const Boolean JOIN_GROUP;
    static const Boolean FORM_GROUP;

    static const Boolean RELOAD;
    static const Boolean NO_RELOAD;

    /* Two minutes comes from the wpa_supplicant setting */
    static const Int32 GROUP_CREATING_WAIT_TIME_MS;

    static const Int32 DISABLE_P2P_WAIT_TIME_MS;

    /* Set a two minute discover timeout to avoid STA scans from being blocked */
    static const Int32 DISCOVER_TIMEOUT_S;

    /* Idle time after a peer is gone when the group is torn down */
    static const Int32 GROUP_IDLE_TIME_S;

    static const Int32 BASE;

    /* Delayed message to timeout group creation */
    static const Int32 GROUP_CREATING_TIMED_OUT = IProtocol::BASE_WIFI_P2P_SERVICE + 1;
    /* User accepted a peer request */
    static const Int32 PEER_CONNECTION_USER_ACCEPT = IProtocol::BASE_WIFI_P2P_SERVICE + 2;
    /* User rejected a peer request */
    static const Int32 PEER_CONNECTION_USER_REJECT = IProtocol::BASE_WIFI_P2P_SERVICE + 3;
    /* User wants to disconnect wifi in favour of p2p */
    static const Int32 DROP_WIFI_USER_ACCEPT = IProtocol::BASE_WIFI_P2P_SERVICE + 4;
    /* User wants to keep his wifi connection and drop p2p */
    static const Int32 DROP_WIFI_USER_REJECT = IProtocol::BASE_WIFI_P2P_SERVICE + 5;
    /* Delayed message to timeout p2p disable */
    static const Int32 DISABLE_P2P_TIMED_OUT = IProtocol::BASE_WIFI_P2P_SERVICE + 6;

    /* Commands to the WifiStateMachine */
    static const Int32 P2P_CONNECTION_CHANGED = IProtocol::BASE_WIFI_P2P_SERVICE + 11;

    /* These commands are used to temporarily disconnect wifi when we detect
     * a frequency conflict which would make it impossible to have with p2p
     * and wifi active at the same time.
     *
     * If the user chooses to disable wifi temporarily, we keep wifi disconnected
     * until the p2p connection is done and terminated at which point we will
     * bring back wifi up
     *
     * DISCONNECT_WIFI_REQUEST
     *      msg.arg1 = 1 enables temporary disconnect and 0 disables it.
     */
    static const Int32 DISCONNECT_WIFI_REQUEST = IProtocol::BASE_WIFI_P2P_SERVICE + 12;
    static const Int32 DISCONNECT_WIFI_RESPONSE = IProtocol::BASE_WIFI_P2P_SERVICE + 13;

    static const Int32 SET_MIRACAST_MODE = IProtocol::BASE_WIFI_P2P_SERVICE + 14;

    // During dhcp (and perhaps other times) we can't afford to drop packets
    // but Discovery will switch our channel enough we will.
    //   msg.arg1 = ENABLED for blocking, DISABLED for resumed.
    //   msg.arg2 = msg to send when blocked
    //   msg.obj  = StateMachine to send to when blocked
    static const Int32 BLOCK_DISCOVERY = IProtocol::BASE_WIFI_P2P_SERVICE + 15;

    // set country code
    static const Int32 SET_COUNTRY_CODE = IProtocol::BASE_WIFI_P2P_SERVICE + 16;
    static const Int32 P2P_MIRACAST_MODE_CHANGED = IProtocol::BASE_WIFI_P2P_SERVICE + 17;

    static const Int32 ENABLED;
    static const Int32 DISABLED;

    static const String SERVER_ADDRESS;

    AutoPtr<ClientHandler> mClientHandler;

    static Int32 mGroupCreatingTimeoutIndex;
    static Int32 mDisableP2pTimeoutIndex;

    AutoPtr<IContext> mContext;
    String mInterface;
    AutoPtr<INotification> mNotification;

    AutoPtr<IINetworkManagementService> mNwService;
    AutoPtr<IDhcpStateMachine> mDhcpStateMachine;

    AutoPtr<IActivityManager> mActivityMgr;

    AutoPtr<P2pStateMachine> mP2pStateMachine;
    AutoPtr<AsyncChannel> mReplyChannel;
    AutoPtr<AsyncChannel> mWifiChannel;

    Boolean mP2pSupported;

    AutoPtr<IWifiP2pDevice> mThisDevice;

    /* When a group has been explicitly created by an app, we persist the group
     * even after all clients have been disconnected until an explicit remove
     * is invoked */
    Boolean mAutonomousGroup;

    /* Invitation to join an existing p2p group */
    Boolean mJoinExistingGroup;

    Boolean mIsInvite;

    /* Track whether we are in p2p discovery. This is used to avoid sending duplicate
     * broadcasts
     */
    Boolean mDiscoveryStarted;

    /* Track whether servcice/peer discovery is blocked in favor of other wifi actions
     * (notably dhcp)
     */
    Boolean mDiscoveryBlocked;

    // Supplicant doesn't like setting the same country code multiple times (it may drop
    // current connected network), so we save the country code here to avoid redundency
    String mLastSetCountryCode;

    /*
     * remember if we were in a scan when it had to be stopped
     */
    Boolean mDiscoveryPostponed;

    AutoPtr<INetworkInfo> mNetworkInfo;

    Boolean mTemporarilyDisconnectedWifi;

    /* The transaction Id of service discovery request */
    Byte mServiceTransactionId;

    /* Service discovery request ID of wpa_supplicant.
     * null means it's not set yet. */
    String mServiceDiscReqId;

    /* clients(application) information list. */
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> > mClientInfoList;

};

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFIP2PSERVICEIMPL_H__
