//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_VIEW_NATIVEDISPLAYEVENTRECEIVER_H__
#define __ELASTOS_DROID_VIEW_NATIVEDISPLAYEVENTRECEIVER_H__

#include "elastos/droid/os/NativeMessageQueue.h"
#include <utils/Looper.h>
#include <gui/DisplayEventReceiver.h>

using Elastos::Droid::Os::MessageQueue;

namespace Elastos {
namespace Droid {
namespace View {

class DisplayEventReceiver;

class NativeDisplayEventReceiver : public android::LooperCallback
{
public:
    NativeDisplayEventReceiver(
        DisplayEventReceiver* receiverObj,
        MessageQueue* messageQueue);

    android::status_t initialize();
    void dispose();
    android::status_t scheduleVsync();

protected:
    virtual ~NativeDisplayEventReceiver();

private:
    AutoPtr<DisplayEventReceiver> mReceiverObjGlobal;
    AutoPtr<MessageQueue> mMessageQueue;
    android::DisplayEventReceiver mReceiver;
    bool mWaitingForVsync;

    virtual int handleEvent(int receiveFd, int events, void* data);
    bool processPendingEvents(nsecs_t* outTimestamp, int32_t* id, uint32_t* outCount);
    void dispatchVsync(nsecs_t timestamp, int32_t id, uint32_t count);
    void dispatchHotplug(nsecs_t timestamp, int32_t id, bool connected);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_NATIVEDISPLAYEVENTRECEIVER_H__
