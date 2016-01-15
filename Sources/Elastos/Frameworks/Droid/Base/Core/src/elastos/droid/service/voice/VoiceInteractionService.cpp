
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/service/voice/CAlwaysOnHotwordDetector.h"
#include "elastos/droid/service/voice/VoiceInteractionService.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Voice::CAlwaysOnHotwordDetector;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

//==================================
// VoiceInteractionService::MInterface
//==================================

CAR_INTERFACE_IMPL_2(VoiceInteractionService::MInterface, Object, IIVoiceInteractionService, IBinder)

VoiceInteractionService::MInterface::MInterface(
    /* [in] */ VoiceInteractionService* host)
    : mHost(host)
{
}

ECode VoiceInteractionService::MInterface::Ready()
{
    Boolean result;
    return mHost->mHandler->SendEmptyMessage(VoiceInteractionService::MSG_READY, &result);
}

ECode VoiceInteractionService::MInterface::Shutdown()
{
    Boolean result;
    return mHost->mHandler->SendEmptyMessage(VoiceInteractionService::MSG_SHUTDOWN, &result);
}

ECode VoiceInteractionService::MInterface::SoundModelsChanged()
{
    Boolean result;
    return mHost->mHandler->SendEmptyMessage(VoiceInteractionService::MSG_SOUND_MODELS_CHANGED, &result);
}

//==================================
// VoiceInteractionService::MyHandler
//==================================

VoiceInteractionService::MyHandler::MyHandler(
    /* [in] */ VoiceInteractionService* host)
    : mHost(host)
{
}

ECode VoiceInteractionService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case VoiceInteractionService::MSG_READY:
            mHost->OnReady();
            break;
        case VoiceInteractionService::MSG_SHUTDOWN:
            mHost->OnShutdownInternal();
            break;
        case VoiceInteractionService::MSG_SOUND_MODELS_CHANGED:
            mHost->OnSoundModelsChangedInternal();
            break;
        default:
            Handler::HandleMessage(msg);
    }
    return NOERROR;
}

//==================================
// VoiceInteractionService
//==================================

const Int32 VoiceInteractionService::MSG_READY;
const Int32 VoiceInteractionService::MSG_SHUTDOWN;
const Int32 VoiceInteractionService::MSG_SOUND_MODELS_CHANGED;

CAR_INTERFACE_IMPL(VoiceInteractionService, Service, IVoiceInteractionService)

VoiceInteractionService::VoiceInteractionService()
{
    AutoPtr<MInterface> mi = new MInterface(this);
    mInterface = (IIVoiceInteractionService*)(mi.Get());
}

ECode VoiceInteractionService::constructor()
{
    return NOERROR;
}

ECode VoiceInteractionService::IsActiveService(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* service,
    /* [out] */ Boolean* isActiveService)
{
    VALIDATE_NOT_NULL(isActiveService)
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    String cur;
    Settings::Secure::GetString(cr, ISettingsSecure::VOICE_INTERACTION_SERVICE, &cur);
    if (cur.IsNullOrEmpty()) {
        *isActiveService = FALSE;
        return NOERROR;
    }
    AutoPtr<IComponentName> curComp;
    CComponentName::UnflattenFromString(cur, (IComponentName**)&curComp);
    if (curComp == NULL) {
        *isActiveService = FALSE;
        return NOERROR;
    }
    IObject::Probe(curComp)->Equals(service, isActiveService);
    return NOERROR;
}

ECode VoiceInteractionService::StartSession(
    /* [in] */ IBundle* args)
{
    if (mSystemService == NULL) {
        Slogger::E("VoiceInteractionService", "Not available until onReady() is called");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    ECode ec = mSystemService->StartSession(mInterface, args);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VoiceInteractionService::OnCreate()
{
    FAIL_RETURN(Service::OnCreate());
    mHandler = new MyHandler(this);
    return NOERROR;
}

ECode VoiceInteractionService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    String action;
    intent->GetAction(&action);
    if (IVoiceInteractionService::SERVICE_INTERFACE.Equals(action)) {
        *b = IBinder::Probe(mInterface);
        REFCOUNT_ADD(*b)
        return NOERROR;
    }
    *b = NULL;
    return NOERROR;
}

ECode VoiceInteractionService::OnReady()
{
    mSystemService = (IIVoiceInteractionManagerService*)
        ServiceManager::GetService(IContext::VOICE_INTERACTION_MANAGER_SERVICE).Get();
    AutoPtr<IPackageManager> pm;
    this->GetPackageManager((IPackageManager**)&pm);
    //TODO //no CKeyphraseEnrollmentInfo provided
    // CKeyphraseEnrollmentInfo::New(pm.Get(), (IKeyphraseEnrollmentInfo**)&mKeyphraseEnrollmentInfo);
    return NOERROR;
}

ECode VoiceInteractionService::OnShutdownInternal()
{
    OnShutdown();
    // Stop any active recognitions when shutting down.
    // This ensures that if implementations forget to stop any active recognition,
    // It's still guaranteed to have been stopped.
    // This helps with cases where the voice interaction implementation is changed
    // by the user.
    SafelyShutdownHotwordDetector();
    return NOERROR;
}

ECode VoiceInteractionService::OnShutdown()
{
    return NOERROR;
}

ECode VoiceInteractionService::OnSoundModelsChangedInternal()
{
    synchronized(this) {
        if (mHotwordDetector != NULL) {
            // TODO: Stop recognition if a sound model that was being recognized gets deleted.
            mHotwordDetector->OnSoundModelsChanged();
        }
    }
    return NOERROR;
}

ECode VoiceInteractionService::CreateAlwaysOnHotwordDetector(
    /* [in] */ const String& keyphrase,
    /* [in] */ ILocale* locale,
    /* [in] */ IAlwaysOnHotwordDetectorCallback* callback,
    /* [out] */ IAlwaysOnHotwordDetector** aohd)
{
    VALIDATE_NOT_NULL(aohd)
    if (mSystemService == NULL) {
        Slogger::E("VoiceInteractionService", "Not available until onReady() is called");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    synchronized(mLock) {
        // Allow only one concurrent recognition via the APIs.
        SafelyShutdownHotwordDetector();
        CAlwaysOnHotwordDetector::New(keyphrase, locale, callback, mKeyphraseEnrollmentInfo,
            mInterface, mSystemService, (IAlwaysOnHotwordDetector**)&mHotwordDetector);
    }
    *aohd = mHotwordDetector;
    REFCOUNT_ADD(*aohd)
    return NOERROR;
}

ECode VoiceInteractionService::GetKeyphraseEnrollmentInfo(
    /* [out] */ IKeyphraseEnrollmentInfo** kei)
{
    VALIDATE_NOT_NULL(kei)
    *kei = mKeyphraseEnrollmentInfo;
    REFCOUNT_ADD(*kei)
    return NOERROR;
}

ECode VoiceInteractionService::SafelyShutdownHotwordDetector()
{
    synchronized(mLock) {
        if (mHotwordDetector != NULL) {
            Boolean result;
            mHotwordDetector->StopRecognition(&result);
            mHotwordDetector->Invalidate();
            mHotwordDetector = NULL;
        }
    }
    return NOERROR;
}

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos
