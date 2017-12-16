
#include "elastos/droid/internal/app/IVoiceInteractor.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(IVoiceInteractorProxy, Object, IIVoiceInteractor);

IVoiceInteractorProxy::IVoiceInteractorProxy(
    /* [in] */ const android::sp<android::IBinder>& bpBinder)
    : mBpBinder(bpBinder)
{}

ECode IVoiceInteractorProxy::StartConfirmation(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ ICharSequence* prompt,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IVoiceInteractorProxy::StartCompleteVoice(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IVoiceInteractorProxy::StartAbortVoice(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IVoiceInteractorProxy::StartCommand(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IVoiceInteractorProxy::SupportsCommands(
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<String>* commands,
    /* [out, callee] */ ArrayOf<Boolean>** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
