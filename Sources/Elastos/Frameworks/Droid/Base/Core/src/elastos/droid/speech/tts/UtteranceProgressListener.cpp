#include "elastos/droid/speech/tts/UtteranceProgressListener.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/******************
 * UtteranceProgressListener::UtteranceProgressListenerStand
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(UtteranceProgressListener::UtteranceProgressListenerStand, Object,IUtteranceProgressListener);

UtteranceProgressListener::UtteranceProgressListenerStand::UtteranceProgressListenerStand()
{}

UtteranceProgressListener::UtteranceProgressListenerStand::~UtteranceProgressListenerStand()
{}

ECode UtteranceProgressListener::UtteranceProgressListenerStand::constructor()
{
    return NOERROR;
}

ECode UtteranceProgressListener::UtteranceProgressListenerStand::constructor(
    /* [in] */ ITextToSpeechOnUtteranceCompletedListener* listener)
{
    mListener = listener;

    return NOERROR;
}

ECode UtteranceProgressListener::UtteranceProgressListenerStand::OnDone(
    /* [in] */ const String& utteranceId)
{
    AutoLock lock(mMutex);
    mListener->OnUtteranceCompleted(utteranceId);
    return NOERROR;
}

ECode UtteranceProgressListener::UtteranceProgressListenerStand::OnError(
    /* [in] */ const String& utteranceId)
{
    mListener->OnUtteranceCompleted(utteranceId);
    return NOERROR;
}

ECode UtteranceProgressListener::UtteranceProgressListenerStand::OnError(
    /* [in] */ const String& utteranceId,
    /* [in] */ Int32 errorCode)
{
    OnError(utteranceId);
    return NOERROR;
}

ECode UtteranceProgressListener::UtteranceProgressListenerStand::OnStart(
    /* [in] */ const String& utteranceId)
{
    // Left unimplemented, has no equivalent in the old
    // API.
    return NOERROR;
}

/******************
 * UtteranceProgressListener
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(UtteranceProgressListener, Object,IUtteranceProgressListener);

UtteranceProgressListener::UtteranceProgressListener()
{}

UtteranceProgressListener::~UtteranceProgressListener()
{}

ECode UtteranceProgressListener::constructor()
{
    return NOERROR;
}

AutoPtr<IUtteranceProgressListener> UtteranceProgressListener::From(
    /* [in] */ ITextToSpeechOnUtteranceCompletedListener* listener)
{
    UtteranceProgressListenerStand *upls = new UtteranceProgressListenerStand();
    AutoPtr<IUtteranceProgressListener> ret = upls;
    upls->constructor(listener);
    return ret;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
