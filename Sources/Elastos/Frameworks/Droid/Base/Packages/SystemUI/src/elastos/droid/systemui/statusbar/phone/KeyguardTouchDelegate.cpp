
#include "elastos/droid/systemui/statusbar/phone/KeyguardTouchDelegate.h"
#include "Elastos.Droid.Os.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Boolean KeyguardTouchDelegate::DEBUG = FALSE;
const String KeyguardTouchDelegate::TAG("KeyguardTouchDelegate");
const String KeyguardTouchDelegate::KEYGUARD_PACKAGE("Elastos.Droid.SystemUI");
const String KeyguardTouchDelegate::KEYGUARD_CLASS("Elastos.Droid.SystemUI.Keyguard.CKeyguardService");
AutoPtr<KeyguardTouchDelegate> KeyguardTouchDelegate::sInstance;
AutoPtr<IList> KeyguardTouchDelegate::sConnectionListeners = InitList();

CAR_INTERFACE_IMPL(KeyguardTouchDelegate::KeyguardConnection, Object, IServiceConnection);
KeyguardTouchDelegate::KeyguardConnection::KeyguardConnection(
    /* [in] */ KeyguardTouchDelegate* host)
    : mHost(host)
{}

ECode KeyguardTouchDelegate::KeyguardConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    Slogger::V(TAG, "Connected to keyguard");
    mHost->mService = IIKeyguardService::Probe(service);

    Int32 size = 0;
    sConnectionListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        sConnectionListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IOnKeyguardConnectionListener> listener = IOnKeyguardConnectionListener::Probe(obj);
        listener->OnKeyguardServiceConnected(mHost);
    }
    return NOERROR;
}

ECode KeyguardTouchDelegate::KeyguardConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    Slogger::V(TAG, "Disconnected from keyguard");
    mHost->mService = NULL;
    mHost->sInstance = NULL; // force reconnection if this goes away

    Int32 size = 0;
    mHost->sConnectionListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHost->sConnectionListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IOnKeyguardConnectionListener> listener = IOnKeyguardConnectionListener::Probe(obj);
        listener->OnKeyguardServiceDisconnected(mHost);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyguardTouchDelegate, Object, IKeyguardTouchDelegate);
KeyguardTouchDelegate::KeyguardTouchDelegate(
    /* [in] */ IContext* context)
{
    mKeyguardConnection = new KeyguardConnection(this);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(KEYGUARD_PACKAGE, KEYGUARD_CLASS);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> OWNER;
    helper->GetOWNER((IUserHandle**)&OWNER);

    Boolean tmp = FALSE;
    context->BindServiceAsUser(intent, mKeyguardConnection,
            IContext::BIND_AUTO_CREATE, OWNER, &tmp);
    if (!tmp) {
        if (DEBUG) Slogger::V(TAG, "*** Keyguard: can't bind to %s", KEYGUARD_CLASS.string());
    }
    else {
        if (DEBUG) Slogger::V(TAG, "*** Keyguard started");
    }
}

AutoPtr<IList> KeyguardTouchDelegate::InitList()
{
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    return list;
}

AutoPtr<KeyguardTouchDelegate> KeyguardTouchDelegate::GetInstance(
    /* [in] */ IContext* context)
{
    AutoPtr<KeyguardTouchDelegate> instance = sInstance;
    if (instance == NULL) {
        instance = sInstance = new KeyguardTouchDelegate(context);
    }
    return instance;
}

ECode KeyguardTouchDelegate::IsSecure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIKeyguardService> service = mService;
    if (service != NULL) {
        if (!FAILED(service->IsSecure(result))) {
            return NOERROR;
        }
        Slogger::E(TAG, "RemoteException calling keyguard.isSecure()!");
    }
    else {
        Slogger::W(TAG, "isSecure(): NO SERVICE!");
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardTouchDelegate::Dispatch(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIKeyguardService> service = mService;
    if (service != NULL) {
        if (!FAILED(service->Dispatch(event))) {
            *result = TRUE;
            return NOERROR;
        }
        // What to do?
        Slogger::E(TAG, "RemoteException sending event to keyguard!");
    }
    else {
        Slogger::W(TAG, "dispatch(event): NO SERVICE!");
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardTouchDelegate::IsInputRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIKeyguardService> service = mService;
    if (service != NULL) {
        if (!FAILED(service->IsInputRestricted(result))) {
            return NOERROR;
        }
        Slogger::W(TAG , "Remote Exception");
    }
    else {
        Slogger::W(TAG, "isInputRestricted(): NO SERVICE!");
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardTouchDelegate::IsShowingAndNotOccluded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIKeyguardService> service = mService;
    if (service != NULL) {
        if (!FAILED(service->IsShowingAndNotOccluded(result))) {
            return NOERROR;
        }
        Slogger::W(TAG , "Remote Exception");
    }
    else {
        Slogger::W(TAG, "isShowingAndNotOccluded(): NO SERVICE!");
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardTouchDelegate::ShowAssistant()
{
    AutoPtr<IIKeyguardService> service = mService;
    if (service != NULL) {
        if (FAILED(service->ShowAssistant())) {
            // What to do?
            Slogger::E(TAG, "RemoteException launching assistant!");
        }
    }
    else {
        Slogger::W(TAG, "showAssistant(event): NO SERVICE!");
    }
    return NOERROR;
}

ECode KeyguardTouchDelegate::LaunchCamera()
{
    AutoPtr<IIKeyguardService> service = mService;
    if (service != NULL) {
        if (FAILED(service->LaunchCamera())) {
            // What to do?
            Slogger::E(TAG, "RemoteException launching camera!");
        }
    }
    else {
        Slogger::W(TAG, "launchCamera(): NO SERVICE!");
    }
    return NOERROR;
}

ECode KeyguardTouchDelegate::Dismiss()
{
    AutoPtr<IIKeyguardService> service = mService;
    if (service != NULL) {
        if (FAILED(service->Dismiss())) {
            // What to do?
            Slogger::E(TAG, "RemoteException dismissing keyguard!");
        }
    }
    else {
        Slogger::W(TAG, "dismiss(): NO SERVICE!");
    }
    return NOERROR;
}

void KeyguardTouchDelegate::AddListener(
    /* [in] */ IOnKeyguardConnectionListener* listener)
{
    sConnectionListeners->Add(listener);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
