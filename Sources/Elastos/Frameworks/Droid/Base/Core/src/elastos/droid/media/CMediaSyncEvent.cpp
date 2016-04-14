
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CMediaSyncEvent.h"
#include <elastos/utility/logging/Slogger.h>

using namespace Elastos::Core;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {

CMediaSyncEvent::CMediaSyncEvent()
    : mType(0)
    , mAudioSession(0)
{}

ECode CMediaSyncEvent::constructor(
    /* [in] */ Int32 eventType)
{
    mType = eventType;
    return NOERROR;
}

ECode CMediaSyncEvent::CreateEvent(
    /* [in] */ Int32 eventType,
    /* [out] */ IMediaSyncEvent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (!IsValidType(eventType)) {
        Slogger::E("CMediaSyncEvent", "%d is not a valid MediaSyncEvent type.", eventType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return CMediaSyncEvent::New(eventType, result);
}

ECode CMediaSyncEvent::SetAudioSessionId(
    /* [in] */ Int32 audioSessionId)
{
    if (audioSessionId > 0) {
        mAudioSession = audioSessionId;
    }
    else {
        Slogger::E("CMediaSyncEvent", "%d is not a valid session ID.", audioSessionId);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaSyncEvent::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mType;
    return NOERROR;
}

ECode CMediaSyncEvent::GetAudioSessionId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAudioSession;
    return NOERROR;
}

Boolean CMediaSyncEvent::IsValidType(
    /* [in] */ Int32 type)
{
    switch (type) {
    case SYNC_EVENT_NONE:
    case SYNC_EVENT_PRESENTATION_COMPLETE:
        return TRUE;
    default:
        return FALSE;
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
