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

#ifndef __ELASTOS_DROID_OS_NATIVEMESSAGEQUEUE_H__
#define __ELASTOS_DROID_OS_NATIVEMESSAGEQUEUE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <utils/Looper.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

class MessageQueue : public Object
{
public:
    /* Gets the message queue's looper. */
    inline CARAPI_(android::sp<android::Looper>) GetLooper() const
    {
        return mLooper;
    }

protected:
    MessageQueue() {}
    virtual ~MessageQueue() {}

public:
    android::sp<android::Looper> mLooper;
};

class NativeMessageQueue : public MessageQueue
{
public:
    NativeMessageQueue();
    virtual ~NativeMessageQueue();

    CARAPI_(void) PollOnce(
        /* [in] */ Int32 timeoutMillis);

    CARAPI_(void) Wake();
};

/* Gets the native object associated with a MessageQueue. */
extern AutoPtr<NativeMessageQueue> Elastos_Os_MessageQueue_GetMessageQueue(
    /* [in] */ IMessageQueue* messageQueueObj);

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_NATIVEMESSAGEQUEUE_H__
