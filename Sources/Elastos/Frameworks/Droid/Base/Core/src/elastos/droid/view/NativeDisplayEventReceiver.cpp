
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/NativeDisplayEventReceiver.h"
#include "elastos/droid/view/DisplayEventReceiver.h"
#include <utils/Log.h>
#include <utils/Looper.h>

#undef ALOGV
#define ALOGV(...) ((void)0)

namespace Elastos {
namespace Droid {
namespace View {

static const size_t EVENT_BUFFER_SIZE = 100;

NativeDisplayEventReceiver::NativeDisplayEventReceiver(
    DisplayEventReceiver* receiverObj,
    MessageQueue* messageQueue)
    : mReceiverObjGlobal(receiverObj)
    , mMessageQueue(messageQueue)
    , mWaitingForVsync(false)
{
    ALOGV("receiver %p ~ Initializing input event receiver.", this);
}

NativeDisplayEventReceiver::~NativeDisplayEventReceiver()
{
    mReceiverObjGlobal = NULL;
}

android::status_t NativeDisplayEventReceiver::initialize()
{
    android::status_t result = mReceiver.initCheck();
    if (result) {
        ALOGW("Failed to initialize display event receiver, status=%d", result);
        return result;
    }

    int rc = mMessageQueue->GetLooper()->addFd(mReceiver.getFd(), 0, android::Looper::EVENT_INPUT,
            this, NULL);
    if (rc < 0) {
        return android::UNKNOWN_ERROR;
    }
    return android::OK;
}

void NativeDisplayEventReceiver::dispose()
{
    ALOGV("receiver %p ~ Disposing display event receiver.", this);

    if (!mReceiver.initCheck()) {
        mMessageQueue->GetLooper()->removeFd(mReceiver.getFd());
    }
}

android::status_t NativeDisplayEventReceiver::scheduleVsync()
{
    if (!mWaitingForVsync) {
        ALOGV("receiver %p ~ Scheduling vsync.", this);

        // Drain all pending events.
        nsecs_t vsyncTimestamp;
        int32_t vsyncDisplayId;
        uint32_t vsyncCount;
        processPendingEvents(&vsyncTimestamp, &vsyncDisplayId, &vsyncCount);

        android::status_t status = mReceiver.requestNextVsync();
        if (status) {
            ALOGW("Failed to request next vsync, status=%d", status);
            return status;
        }

        mWaitingForVsync = true;
    }
    return android::OK;
}

int NativeDisplayEventReceiver::handleEvent(int receiveFd, int events, void* data)
{
    if (events & (android::Looper::EVENT_ERROR | android::Looper::EVENT_HANGUP)) {
        ALOGE("Display event receiver pipe was closed or an error occurred.  "
                "events=0x%x", events);
        return 0; // remove the callback
    }

    if (!(events & android::Looper::EVENT_INPUT)) {
        ALOGW("Received spurious callback for unhandled poll event.  "
                "events=0x%x", events);
        return 1; // keep the callback
    }

    // Drain all pending events, keep the last vsync.
    nsecs_t vsyncTimestamp;
    int32_t vsyncDisplayId;
    uint32_t vsyncCount;
    if (!processPendingEvents(&vsyncTimestamp, &vsyncDisplayId, &vsyncCount)) {
        ALOGV("receiver %p ~ Vsync pulse: timestamp=%lld, id=%d, count=%d",
                this, vsyncTimestamp, vsyncDisplayId, vsyncCount);
        mWaitingForVsync = false;
        dispatchVsync(vsyncTimestamp, vsyncDisplayId, vsyncCount);
    }
    return 1; // keep the callback
}

bool NativeDisplayEventReceiver::processPendingEvents(
        nsecs_t* outTimestamp, int32_t* outId, uint32_t* outCount)
{
    bool gotVsync = false;
    android::DisplayEventReceiver::Event buf[EVENT_BUFFER_SIZE];
    ssize_t n;
    while ((n = mReceiver.getEvents(buf, EVENT_BUFFER_SIZE)) > 0) {
        ALOGV("receiver %p ~ Read %d events.", this, int(n));
        for (ssize_t i = 0; i < n; i++) {
            const android::DisplayEventReceiver::Event& ev = buf[n];
            switch (ev.header.type) {
                case android::DisplayEventReceiver::DISPLAY_EVENT_VSYNC:
                    // Later vsync events will just overwrite the info from earlier
                    // ones. That's fine, we only care about the most recent.
                    gotVsync = true;
                    *outTimestamp = ev.header.timestamp;
                    *outId = ev.header.id;
                    *outCount = ev.vsync.count;
                    break;
                case android::DisplayEventReceiver::DISPLAY_EVENT_HOTPLUG:
                    dispatchHotplug(ev.header.timestamp, ev.header.id, ev.hotplug.connected);
                    break;
                default:
                    ALOGW("receiver %p ~ ignoring unknown event type %#x", this, ev.header.type);
                    break;
                }
        }
    }
    if (n < 0) {
        ALOGW("Failed to get events from display event receiver, status=%d", android::status_t(n));
    }
    return gotVsync;
}

void NativeDisplayEventReceiver::dispatchVsync(
    nsecs_t timestamp, int32_t id, uint32_t count)
{
    ALOGV("receiver %p ~ Invoking vsync handler.", this);

    mReceiverObjGlobal->DispatchVsync(timestamp, id, count);

    ALOGV("receiver %p ~ Returned from vsync handler.", this);
}

void NativeDisplayEventReceiver::dispatchHotplug(
    nsecs_t timestamp, int32_t id, bool connected)
{
    ALOGV("receiver %p ~ Invoking hotplug handler.", this);

    mReceiverObjGlobal->DispatchHotplug(timestamp, id, connected);

    ALOGV("receiver %p ~ Returned from hotplug handler.", this);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
