
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Media.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/tv/TvInputService.h"
#include "elastos/droid/media/tv/CITvInputSessionWrapper.h"
#include "elastos/droid/media/tv/TvInputServiceHardwareSession.h"
#include "elastos/droid/media/tv/TvInputServiceSession.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/os/CRemoteCallbackList.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

//==============================================================================
// TvInputService::TvInputServiceStub
//==============================================================================

CAR_INTERFACE_IMPL_2(TvInputService::TvInputServiceStub, Object, IITvInputService, IBinder)

ECode TvInputService::TvInputServiceStub::RegisterCallback(
    /* [in] */ IITvInputServiceCallback* cb)
{
    if (cb != NULL) {
        Boolean b;
        mHost->mCallbacks->Register(cb, &b);
    }
    return NOERROR;
}

ECode TvInputService::TvInputServiceStub::UnregisterCallback(
    /* [in] */ IITvInputServiceCallback* cb)
{
    if (cb != NULL) {
        Boolean b;
        mHost->mCallbacks->Unregister(cb, &b);
    }
    return NOERROR;
}

ECode TvInputService::TvInputServiceStub::CreateSession(
    /* [in] */ IInputChannel* channel,
    /* [in] */ IITvInputSessionCallback* cb,
    /* [in] */ const String& inputId)
{
    if (channel == NULL) {
        Logger::W(TAG, "Creating session without input channel");
    }
    if (cb == NULL) {
        return NOERROR;
    }
    AutoPtr<SomeArgs> someArgs = SomeArgs::Obtain();
    someArgs->mArg1 = channel;
    someArgs->mArg2 = cb;
    AutoPtr<ICharSequence> csq;
    CString::New(inputId, (ICharSequence**)&csq);
    someArgs->mArg3 = csq;
    AutoPtr<IMessage> msg;
    mHost->mServiceHandler->ObtainMessage(TvInputService::ServiceHandler::DO_CREATE_SESSION,
            (IInterface*)(IObject*)someArgs, (IMessage**)&msg);
    return msg->SendToTarget();
}

// For hardware TvInputService
ECode TvInputService::TvInputServiceStub::NotifyHardwareAdded(
    /* [in] */ ITvInputHardwareInfo* hardwareInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mServiceHandler->ObtainMessage(ServiceHandler::DO_ADD_HARDWARE_TV_INPUT,
                hardwareInfo, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode TvInputService::TvInputServiceStub::NotifyHardwareRemoved(
    /* [in] */ ITvInputHardwareInfo* hardwareInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mServiceHandler->ObtainMessage(ServiceHandler::DO_REMOVE_HARDWARE_TV_INPUT,
                hardwareInfo, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode TvInputService::TvInputServiceStub::NotifyHdmiDeviceAdded(
    /* [in] */ IHdmiDeviceInfo* deviceInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mServiceHandler->ObtainMessage(ServiceHandler::DO_ADD_HDMI_TV_INPUT,
                deviceInfo, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode TvInputService::TvInputServiceStub::NotifyHdmiDeviceRemoved(
    /* [in] */ IHdmiDeviceInfo* deviceInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mServiceHandler->ObtainMessage(ServiceHandler::DO_REMOVE_HDMI_TV_INPUT,
                deviceInfo, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode TvInputService::TvInputServiceStub::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "TvInputService::TvInputServiceStub";
    return NOERROR;
}

//==============================================================================
// TvInputService::ServiceHandler
//==============================================================================

const Int32 TvInputService::ServiceHandler::DO_CREATE_SESSION = 1;
const Int32 TvInputService::ServiceHandler::DO_NOTIFY_SESSION_CREATED = 2;
const Int32 TvInputService::ServiceHandler::DO_ADD_HARDWARE_TV_INPUT = 3;
const Int32 TvInputService::ServiceHandler::DO_REMOVE_HARDWARE_TV_INPUT = 4;
const Int32 TvInputService::ServiceHandler::DO_ADD_HDMI_TV_INPUT = 5;
const Int32 TvInputService::ServiceHandler::DO_REMOVE_HDMI_TV_INPUT = 6;

ECode TvInputService::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case DO_CREATE_SESSION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<SomeArgs> args = (SomeArgs*)(IObject*)obj.Get();

            AutoPtr<IInputChannel> channel = IInputChannel::Probe(args->mArg1);
            AutoPtr<IITvInputSessionCallback> cb = IITvInputSessionCallback::Probe(args->mArg2);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(args->mArg3);
            String inputId;
            cs->ToString(&inputId);
            args->Recycle();

            AutoPtr<ITvInputServiceSession> sessionImpl;
            mHost->OnCreateSession(inputId, (ITvInputServiceSession**)&sessionImpl);
            if (sessionImpl == NULL) {
                // try {
                    // Failed to create a session.
                cb->OnSessionCreated(NULL, NULL);
                // } catch (RemoteException e) {
                //     Log.e(TAG, "error in onSessionCreated");
                // }
                return NOERROR;
            }
            ((TvInputServiceSession*)sessionImpl.Get())->SetSessionCallback(cb);
            AutoPtr<IITvInputSession> stub;
            CITvInputSessionWrapper::New(mHost, sessionImpl, channel, (IITvInputSession**)&stub);

            if (ITvInputServiceHardwareSession::Probe(sessionImpl) != NULL) {
                AutoPtr<ITvInputServiceHardwareSession> proxySession =
                        ITvInputServiceHardwareSession::Probe(sessionImpl);
                String harewareInputId;
                proxySession->GetHardwareInputId(&harewareInputId);
                if (harewareInputId.IsEmpty() || !mHost->IsPassthroughInput(harewareInputId)) {
                    if (harewareInputId.IsEmpty()) {
                        Logger::W(TAG, "Hardware input id is not setup yet.");
                    } else {
                        Logger::W(TAG, String("Invalid hardware input id : ") + harewareInputId);
                    }
                    sessionImpl->OnRelease();
                    // try {
                    cb->OnSessionCreated(NULL, NULL);
                    // } catch (RemoteException e) {
                    //     Log.e(TAG, "error in onSessionCreated");
                    // }
                    return NOERROR;
                }

                AutoPtr<TvInputServiceHardwareSession> session = (TvInputServiceHardwareSession*)proxySession.Get();
                session->mProxySession = stub;
                session->mProxySessionCallback = cb;
                session->mServiceHandler = mHost->mServiceHandler;

                AutoPtr<IInterface> service;
                mHost->GetSystemService(IContext::TV_INPUT_SERVICE, ((IInterface**)&service));
                AutoPtr<ITvInputManager> manager = ITvInputManager::Probe(service);
                manager->CreateSession(harewareInputId, session->mHardwareSessionCallback, mHost->mServiceHandler);
            } else {
                AutoPtr<SomeArgs> someArgs = SomeArgs::Obtain();
                someArgs->mArg1 = stub;
                someArgs->mArg2 = cb;
                someArgs->mArg3 = NULL;
                AutoPtr<IMessage> msg;
                mHost->mServiceHandler->ObtainMessage(TvInputService::ServiceHandler::DO_NOTIFY_SESSION_CREATED,
                        (IInterface*)(IObject*)someArgs, (IMessage**)&msg);
                msg->SendToTarget();
            }
            return NOERROR;
        }
        case DO_NOTIFY_SESSION_CREATED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<SomeArgs> args = (SomeArgs*)(IObject*)obj.Get();

            AutoPtr<IITvInputSession> stub = IITvInputSession::Probe(args->mArg1);
            AutoPtr<IITvInputSessionCallback> cb = IITvInputSessionCallback::Probe(args->mArg2);
            AutoPtr<IBinder> hardwareSessionToken = IBinder::Probe(args->mArg3);
            // try {
            cb->OnSessionCreated(stub, hardwareSessionToken);
            // } catch (RemoteException e) {
            //     Log.e(TAG, "error in onSessionCreated");
            // }
            args->Recycle();
            return NOERROR;
        }
        case DO_ADD_HARDWARE_TV_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ITvInputHardwareInfo> hardwareInfo = ITvInputHardwareInfo::Probe(obj);

            AutoPtr<ITvInputInfo> inputInfo;
            mHost->OnHardwareAdded(hardwareInfo, (ITvInputInfo**)&inputInfo);
            if (inputInfo != NULL) {
                Int32 id;
                hardwareInfo->GetDeviceId(&id);
                BroadcastAddHardwareTvInput(id, inputInfo);
            }
            return NOERROR;
        }
        case DO_REMOVE_HARDWARE_TV_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ITvInputHardwareInfo> hardwareInfo = ITvInputHardwareInfo::Probe(obj);

            String inputId;
            mHost->OnHardwareRemoved(hardwareInfo, &inputId);
            if (!inputId.IsNull()) {
                BroadcastRemoveTvInput(inputId);
            }
            return NOERROR;
        }
        case DO_ADD_HDMI_TV_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IHdmiDeviceInfo> deviceInfo = IHdmiDeviceInfo::Probe(obj);

            AutoPtr<ITvInputInfo> inputInfo;
            mHost->OnHdmiDeviceAdded(deviceInfo, (ITvInputInfo**)&inputInfo);
            if (inputInfo != NULL) {
                Int32 id;
                deviceInfo->GetId(&id);
                BroadcastAddHdmiTvInput(id, inputInfo);
            }
            return NOERROR;
        }
        case DO_REMOVE_HDMI_TV_INPUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IHdmiDeviceInfo> deviceInfo = IHdmiDeviceInfo::Probe(obj);

            String inputId;
            mHost->OnHdmiDeviceRemoved(deviceInfo, &inputId);
            if (!inputId.IsNull()) {
                BroadcastRemoveTvInput(inputId);
            }
            return NOERROR;
        }
        default: {
            Logger::W(TAG, "Unhandled message code: %d", what);
            return NOERROR;
        }
    }
    return NOERROR;
}

void TvInputService::ServiceHandler::BroadcastAddHardwareTvInput(
    /* [in] */ Int32 deviceId,
    /* [in] */ ITvInputInfo* inputInfo)
{
    Int32 n;
    mHost->mCallbacks->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; ++i) {
        // try {
        AutoPtr<IInterface> item;
        mHost->mCallbacks->GetBroadcastItem(i, (IInterface**)&item);
        IITvInputServiceCallback::Probe(item)->AddHardwareTvInput(deviceId, inputInfo);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Error while broadcasting.", e);
        // }
    }
    mHost->mCallbacks->FinishBroadcast();
}

void TvInputService::ServiceHandler::BroadcastAddHdmiTvInput(
    /* [in] */ Int32 id,
    /* [in] */ ITvInputInfo* inputInfo)
{
    Int32 n;
    mHost->mCallbacks->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; ++i) {
        // try {
        AutoPtr<IInterface> item;
        mHost->mCallbacks->GetBroadcastItem(i, (IInterface**)&item);
        IITvInputServiceCallback::Probe(item)->AddHdmiTvInput(id, inputInfo);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Error while broadcasting.", e);
        // }
    }
    mHost->mCallbacks->FinishBroadcast();
}

void TvInputService::ServiceHandler::BroadcastRemoveTvInput(
    /* [in] */ const String& inputId)
{
    Int32 n;
    mHost->mCallbacks->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; ++i) {
        // try {
        AutoPtr<IInterface> item;
        mHost->mCallbacks->GetBroadcastItem(i, (IInterface**)&item);
        IITvInputServiceCallback::Probe(item)->RemoveTvInput(inputId);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Error while broadcasting.", e);
        // }
    }
    mHost->mCallbacks->FinishBroadcast();
}

//==============================================================================
// TvInputService
//==============================================================================

Boolean TvInputService::DEBUG = FALSE;
String TvInputService::TAG("TvInputService");

CAR_INTERFACE_IMPL(TvInputService, Elastos::Droid::App::Service, ITvInputService)

TvInputService::TvInputService()
{
    mServiceHandler = new ServiceHandler(this);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mCallbacks);
}

TvInputService::~TvInputService()
{
}

ECode TvInputService::constructor()
{
    return NOERROR;
}

ECode TvInputService::GetRegisteredCallbackCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mCallbacks->GetRegisteredCallbackCount(result);
}

ECode TvInputService::OnCreateSession(
    /* [in] */ const String& inputId,
    /* [out] */ ITvInputServiceSession** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode TvInputService::OnHardwareAdded(
    /* [in] */ ITvInputHardwareInfo* hardwareInfo,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode TvInputService::OnHardwareRemoved(
    /* [in] */ ITvInputHardwareInfo* hardwareInfo,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode TvInputService::OnHdmiDeviceAdded(
    /* [in] */ IHdmiDeviceInfo* deviceInfo,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode TvInputService::OnHdmiDeviceRemoved(
    /* [in] */ IHdmiDeviceInfo* deviceInfo,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode TvInputService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<TvInputServiceStub> stub = new TvInputServiceStub(this, intent);
    *result = IBinder::Probe(stub);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TvInputService::IsNavigationKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        case IKeyEvent::KEYCODE_DPAD_UP:
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_PAGE_UP:
        case IKeyEvent::KEYCODE_PAGE_DOWN:
        case IKeyEvent::KEYCODE_MOVE_HOME:
        case IKeyEvent::KEYCODE_MOVE_END:
        case IKeyEvent::KEYCODE_TAB:
        case IKeyEvent::KEYCODE_SPACE:
        case IKeyEvent::KEYCODE_ENTER:
            *result = TRUE;
    }
    *result = FALSE;
    return NOERROR;
}

Boolean TvInputService::IsPassthroughInput(
    /* [in] */ const String& inputId)
{
    if (mTvInputManager == NULL) {
        AutoPtr<IInterface> service;
        GetSystemService(IContext::TV_INPUT_SERVICE, ((IInterface**)&service));
        mTvInputManager = ITvInputManager::Probe(service);
    }
    AutoPtr<ITvInputInfo> info;
    mTvInputManager->GetTvInputInfo(inputId, (ITvInputInfo**)&info);
    if (info != NULL) {
        Boolean b;
        info->IsPassthroughInput(&b);
        if (b) {
            return TRUE;
        }
    }
    return FALSE;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
