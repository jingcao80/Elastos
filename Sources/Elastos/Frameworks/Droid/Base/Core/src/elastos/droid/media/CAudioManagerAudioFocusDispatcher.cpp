
#include "elastos/droid/media/CAudioManagerAudioFocusDispatcher.h"
#include "elastos/droid/media/CAudioManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL_2(CAudioManagerAudioFocusDispatcher, Object, IIAudioFocusDispatcher, IBinder)

CAR_OBJECT_IMPL(CAudioManagerAudioFocusDispatcher)

CAudioManagerAudioFocusDispatcher::CAudioManagerAudioFocusDispatcher()
{
}

CAudioManagerAudioFocusDispatcher::~CAudioManagerAudioFocusDispatcher()
{
}

ECode CAudioManagerAudioFocusDispatcher::constructor(
    /* [in] */ IAudioManager* owner)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(owner);
    assert(wrs != NULL);
    wrs->GetWeakReference((IWeakReference**)&mOwnerWeak);
    return NOERROR;
}

ECode CAudioManagerAudioFocusDispatcher::DispatchAudioFocusChange(
    /* [in] */ Int32 focusChange,
    /* [in] */ const String& id)
{
    AutoPtr<IAudioManager> am;
    mOwnerWeak->Resolve(EIID_IAudioManager, (IInterface**)&am);
    if (am == NULL)
        return NOERROR;

    AutoPtr<ICharSequence> seq;
    CString::New(id, (ICharSequence**)&seq);

    AutoPtr<IHandler> handler = ((CAudioManager*)am.Get())->mAudioFocusEventHandlerDelegate->GetHandler();
    AutoPtr<IMessage> msg;
    handler->ObtainMessage(focusChange, seq, (IMessage**)&msg);
    Boolean result;
    return handler->SendMessage(msg, &result);
}

ECode CAudioManagerAudioFocusDispatcher::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("CAudioManagerAudioFocusDispatcher");
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
