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

#ifndef __ELASTOS_DROID_OS_BROADCASTER_H__
#define __ELASTOS_DROID_OS_BROADCASTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
class Broadcaster
    : public Object
{
public:
    Broadcaster();

    /**
     *  Sign up for notifications about something.
     *
     *  When this broadcaster pushes a message with senderWhat in the what field,
     *  target will be sent a copy of that message with targetWhat in the what field.
     */
    CARAPI_(Void) Request(
        /* [in] */ Int32 senderWhat,
        /* [in] */ IHandler* target,
        /* [in] */ Int32 targetWhat);

    /**
     * Unregister for notifications for this senderWhat/target/targetWhat tuple.
     */
    CARAPI_(Void) CancelRequest(
        /* [in] */ Int32 senderWhat,
        /* [in] */ IHandler* target,
        /* [in] */ Int32 targetWhat);

    /**
     * For debugging purposes, print the registrations to System.out
     */
    CARAPI_(Void) DumpRegistrations();

    /**
     * Send out msg.  Anyone who has registered via the request() method will be
     * sent the message.
     */
    CARAPI_(Void) Broadcast(
        /* [in] */ IMessage* msg);

private:
    class Registration : public Object
    {
    public:
        Registration();

        AutoPtr<Registration> mNext;
        AutoPtr<Registration> mPrev;

        Int32 mSenderWhat;
        AutoPtr<ArrayOf<IHandler*> > mTargets;
        AutoPtr<ArrayOf<Int32> > mTargetWhats;
    };

    AutoPtr<Registration> mReg;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_BROADCASTER_H__
