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
#include "org/alljoyn/bus/NativeOnJoinSessionListener.h"
#include "org/alljoyn/bus/NativePendingAsyncJoin.h"
#include "org/alljoyn/bus/CSessionOpts.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativeOnJoinSessionListener");

NativeOnJoinSessionListener::NativeOnJoinSessionListener(
    /* [in] */ IOnJoinSessionListener* OnJoinSessionListener)
    : mBusPtr(NULL)
{}

NativeOnJoinSessionListener::~NativeOnJoinSessionListener()
{
    if (mBusPtr) {
        mBusPtr->DecRef();
        mBusPtr = NULL;
    }
}

void NativeOnJoinSessionListener::Setup(NativeBusAttachment* jbap)
{
    mBusPtr = jbap;
    mBusPtr->IncRef();
}

void NativeOnJoinSessionListener::JoinSessionCB(
    QStatus status, ajn::SessionId sessionId, const ajn::SessionOpts& sessionOpts, void* context)
{
    NativePendingAsyncJoin* paj = static_cast<NativePendingAsyncJoin*>(context);
    assert(paj);

    AutoPtr<ISessionOpts> options;
    CSessionOpts::New((ISessionOpts**)&options);
    options->SetTraffic(sessionOpts.traffic);
    options->SetMultipoint(sessionOpts.isMultipoint);
    options->SetProximity(sessionOpts.proximity);
    options->SetTransports(sessionOpts.transports);

    paj->mOnJoinSessionListener->OnJoinSession(
        status, sessionId, options, paj->mContext);

    AutoLock lock(mBusPtr->mBaCommonLock);

    /*
     * We stored an object containing instances of the Java objects provided in the
     * original call to the async join that drove this process in case the call
     * got lost in a disconnect -- we don't want to leak them.  So we need to find
     * the matching object and delete it.
     */
    List< AutoPtr<NativePendingAsyncJoin> >::Iterator it = mBusPtr->mPendingAsyncJoins.Begin();
    for (; it != mBusPtr->mPendingAsyncJoins.End(); ++it) {
        /*
         * If the pointer to the PendingAsyncJoin in the bus attachment is equal
         * to the one passed in from the C++ async join callback, then we are
         * talkiing about the same async join instance.
         */
        if ((*it).Get() == paj) {
            /*
             * Double check that the pointers are consistent and nothing got
             * changed out from underneath us.  That would be bad (TM).
             */
            assert((*it)->mOnJoinSessionListener == paj->mOnJoinSessionListener);
            assert((*it)->mSessionListener == paj->mSessionListener);
            assert((*it)->mContext == paj->mContext);

            /*
             * If the join succeeded, we need to keep on holding the session
             * listener in case something happens to the now "up" session.  This
             * reference must go in the sessionListenerMap and we are delegating
             * responsibility for cleaning up to that map.  If the async call
             * failed, we are done with the session listener as well and we need
             * to release our hold on it since no callback will be made on a
             * failed session.
             */
            if (status == ER_OK) {
                mBusPtr->mSessionListenerMap[sessionId].mListener = (*it)->mSessionListener;
                (*it)->mSessionListener = NULL;
            }

            /*
             * We always release our hold on the user context object
             * irrespective of the outcome of the call since it will no longer
             * be used by this asynchronous join instance.
             */
            if ((*it)->mContext) {
                (*it)->mContext = NULL;
            }

            /*
             * We always release our hold on the OnJoinSessionListener object
             * and the user context object irrespective of the outcome of the
             * call since it will no longer be used by this asynchronous join
             * instance.
             *
             * Releasing the Java OnJoinSessionListener is effectively a "delete
             * this" since the global reference to the Java object controls the
             * lifetime of its corresponding C++ object, which is what we are
             * executing in here.  We have got to make sure to do that last.
             */
            assert((*it)->mOnJoinSessionListener);
            (*it)->mOnJoinSessionListener = NULL;
            mBusPtr->mPendingAsyncJoins.Erase(it);

            return;
        }
    }

    Logger::E(TAG, "JoinSessionCB(): Unable to match context");
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org