#ifndef __ELASTOS_DROID_MEDIA_MEDIASYNCEVENT_H__
#define __ELASTOS_DROID_MEDIA_MEDIASYNCEVENT_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

class MediaSyncEvent
    : public Object
    , public IMediaSyncEvent
{
public:
    MediaSyncEvent();

    virtual ~MediaSyncEvent();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 eventType);

    /**
     * Creates a synchronization event of the sepcified type.
     *
     * <p>The type specifies which kind of event is monitored.
     * For instance, event {@link #SYNC_EVENT_PRESENTATION_COMPLETE} corresponds to the audio being
     * presented to the user on a particular audio session.
     * @param eventType the synchronization event type.
     * @return the MediaSyncEvent created.
     * @throws java.lang.IllegalArgumentException
     */
    static CARAPI CreateEvent(
        /* [in] */ Int32 eventType,
        /* [out] */ IMediaSyncEvent** result);

    /**
     * Sets the event source audio session ID.
     *
     * <p>The audio session ID specifies on which audio session the synchronization event should be
     * monitored.
     * It is mandatory for certain event types (e.g. {@link #SYNC_EVENT_PRESENTATION_COMPLETE}).
     * For instance, the audio session ID can be retrieved via
     * {@link MediaPlayer#getAudioSessionId()} when monitoring an event on a particular MediaPlayer.
     * @param audioSessionId the audio session ID of the event source being monitored.
     * @return the MediaSyncEvent the method is called on.
     * @throws java.lang.IllegalArgumentException
     */
    CARAPI SetAudioSessionId(
        /* [in] */ Int32 audioSessionId);

    /**
     * Gets the synchronization event type.
     *
     * @return the synchronization event type.
     */
    CARAPI GetType(
        /* [out] */ Int32* result);

    /**
     * Gets the synchronization event audio session ID.
     *
     * @return the synchronization audio session ID. The returned audio session ID is 0 if it has
     * not been set.
     */
    CARAPI GetAudioSessionId(
        /* [out] */ Int32* result);

private:
    static CARAPI_(Boolean) IsValidType(
        /* [in] */ Int32 type);

private:
    Int32 mType;
    Int32 mAudioSession;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIASYNCEVENT_H__