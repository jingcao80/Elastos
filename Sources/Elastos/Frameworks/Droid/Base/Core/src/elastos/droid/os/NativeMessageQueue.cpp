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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/NativeMessageQueue.h"

namespace Elastos {
namespace Droid {
namespace Os {

NativeMessageQueue::NativeMessageQueue()
{
    mLooper = android::Looper::getForThread();
    if (mLooper == NULL) {
        mLooper = new android::Looper(false);
        android::Looper::setForThread(mLooper);
    }
}

NativeMessageQueue::~NativeMessageQueue()
{}

void NativeMessageQueue::PollOnce(
    /* [in] */ Int32 timeoutMillis)
{
    mLooper->pollOnce(timeoutMillis);
}

void NativeMessageQueue::Wake()
{
    mLooper->wake();
}

AutoPtr<NativeMessageQueue> Elastos_Os_MessageQueue_GetMessageQueue(
    /* [in] */ IMessageQueue* messageQueueObj)
{
    HANDLE ptr = 0;
    messageQueueObj->GetNativeMessageQueue(&ptr);
    return reinterpret_cast<NativeMessageQueue*>(ptr);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
