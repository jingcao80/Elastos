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

#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeOnPingListener.h"
#include "org/alljoyn/bus/NativePendingAsyncPing.h"
#include "org/alljoyn/bus/CSessionOpts.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativeOnPingListener");

NativeOnPingListener::NativeOnPingListener(
    /* [in] */ IOnPingListener* OnPingListener)
    : mBusPtr(NULL)
{}

NativeOnPingListener::~NativeOnPingListener()
{
    if (mBusPtr) {
        mBusPtr->DecRef();
        mBusPtr = NULL;
    }
}

void NativeOnPingListener::Setup(NativeBusAttachment* jbap)
{
    mBusPtr = jbap;
    mBusPtr->IncRef();
}

void NativeOnPingListener::PingCB(
    QStatus status, void* context)
{
    NativePendingAsyncPing* pap = static_cast<NativePendingAsyncPing*>(context);
    assert(pap);

    pap->mOnPingListener->OnPing(status, pap->mContext);

    AutoLock lock(mBusPtr->mBaCommonLock);

    /*
     * We stored an object containing instances of the Java objects provided in the
     * original call to the async join that drove this process in case the call
     * got lost in a disconnect -- we don't want to leak them.  So we need to find
     * the matching object and delete it.
     */
    List< AutoPtr<NativePendingAsyncPing> >::Iterator it = mBusPtr->mPendingAsyncPings.Begin();
    for (; it != mBusPtr->mPendingAsyncPings.End(); ++it) {
        /*
         * If the pointer to the PendingAsyncPing in the bus attachment is equal
         * to the one passed in from the C++ async ping callback, then we are
         * talkiing about the same async ping instance.
         */
        if ((*it).Get() == pap) {
            /*
             * Double check that the pointers are consistent and nothing got
             * changed out from underneath us.  That would be bad (TM).
             */
            assert((*it)->mOnPingListener == pap->mOnPingListener);
            assert((*it)->mContext == pap->mContext);

            /*
             * We always release our hold on the user context object
             * irrespective of the outcome of the call since it will no longer
             * be used by this asynchronous ping instance.
             */
            (*it)->mContext = NULL;

            /*
             * We always release our hold on the OnPingListener object
             * and the user context object irrespective of the outcome of the
             * call since it will no longer be used by this asynchronous ping
             * instance.
             *
             * Releasing the Java OnPingListener is effectively a "delete
             * this" since the global reference to the Java object controls the
             * lifetime of its corresponding C++ object, which is what we are
             * executing in here.  We have got to make sure to do that last.
             */
            (*it)->mOnPingListener = NULL;
            mBusPtr->mPendingAsyncPings.Erase(it);

            return;
        }
    }

    Logger::E(TAG, "PingCB(): Unable to match context");
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org