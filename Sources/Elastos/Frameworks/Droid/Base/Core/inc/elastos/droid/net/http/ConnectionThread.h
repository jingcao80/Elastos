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

#ifndef __ELASTOS_DROID_NET_HTTP_CONNECTIONTHREAD_H__
#define __ELASTOS_DROID_NET_HTTP_CONNECTIONTHREAD_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::IContext;

using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * {@hide}
 */
class ConnectionThread
    : public Thread
{
public:
    ConnectionThread();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [in] */ IRequestQueueConnectionManager* connectionManager,
        /* [in] */ IRequestFeeder* requestFeeder);

    CARAPI RequestStop();

    /**
     * Loop until app shutdown. Runs connections in priority
     * order.
     */
    CARAPI Run();

    CARAPI ToString(
        /* [out] */ String* result);

public:
    static const Int32 WAIT_TIMEOUT;

    static const Int32 WAIT_TICK;

    // Performance probe
    Int64 mCurrentThreadTime;

    Int64 mTotalThreadTime;

    AutoPtr<IConnection> mConnection;

private:
    Boolean mWaiting;

    /* volatile */ Boolean mRunning;

    AutoPtr<IContext> mContext;

    AutoPtr<IRequestQueueConnectionManager> mConnectionManager;

    AutoPtr<IRequestFeeder> mRequestFeeder;

    Int32 mId;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Net::Http::ConnectionThread, IInterface)

#endif // __ELASTOS_DROID_NET_HTTP_CONNECTIONTHREAD_H__
