#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/MediaSyncEvent.h"
#include "elastos/droid/media/CMediaSyncEvent.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {

MediaSyncEvent::MediaSyncEvent()
    : mType(0)
    , mAudioSession(0)
{}

MediaSyncEvent::~MediaSyncEvent()
{}

CAR_INTERFACE_IMPL(MediaSyncEvent, Object, IMediaSyncEvent)

ECode MediaSyncEvent::constructor(
    /* [in] */ Int32 eventType)
{
    mType = eventType;
    return NOERROR;
}

ECode MediaSyncEvent::CreateEvent(
    /* [in] */ Int32 eventType,
    /* [out] */ IMediaSyncEvent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (!IsValidType(eventType)) {
        Slogger::E("MediaSyncEvent", "%d is not a valid MediaSyncEvent type.", eventType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IMediaSyncEvent> event;
    CMediaSyncEvent::New(eventType, (IMediaSyncEvent**)&event);
    *result = event.Get();
    return NOERROR;
}

ECode MediaSyncEvent::SetAudioSessionId(
    /* [in] */ Int32 audioSessionId)
{
    if (audioSessionId > 0) {
        mAudioSession = audioSessionId;
    }
    else {
        Slogger::E("MediaSyncEvent", "%d is not a valid session ID.", audioSessionId);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode MediaSyncEvent::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mType;
    return NOERROR;
}

ECode MediaSyncEvent::GetAudioSessionId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAudioSession;
    return NOERROR;
}

Boolean MediaSyncEvent::IsValidType(
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