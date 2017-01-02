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

#ifndef __ELASTOS_DROID_SERVER_TRUST_TRUSTARCHIVE_H__
#define __ELASTOS_DROID_SERVER_TRUST_TRUSTARCHIVE_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.CoreLibrary.Core.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayDeque;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Trust {

/**
 * An archive of trust events.
 */
class TrustArchive
    : public Object
{
private:
    class Event
        : public Object
    {
        friend class TrustArchive;

    private:
        Event(
            /* [in] */ Int32 type,
            /* [in] */ Int32 userId,
            /* [in] */ IComponentName* agent,
            /* [in] */ const String& message,
            /* [in] */ Int64 duration,
            /* [in] */ Boolean userInitiated,
            /* [in] */ Boolean managingTrust);

    public:
        const Int32 mType;

        const Int32 mUserId;

        AutoPtr<IComponentName> mAgent;

        const Int64 mElapsedTimestamp;

        // grantTrust
        const String mMessage;

        const Int64 mDuration;

        const Boolean mUserInitiated;

        // managingTrust
        const Boolean mManagingTrust;
    };

public:
    TrustArchive();

    CARAPI LogGrantTrust(
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* agent,
        /* [in] */ const String& message,
        /* [in] */ Int64 duration,
        /* [in] */ Boolean userInitiated);

    CARAPI LogRevokeTrust(
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* agent);

    CARAPI LogTrustTimeout(
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* agent);

    CARAPI LogAgentDied(
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* agent);

    CARAPI LogAgentConnected(
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* agent);

    CARAPI LogAgentStopped(
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* agent);

    CARAPI LogManagingTrust(
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* agent,
        /* [in] */ Boolean managing);

    CARAPI Dump(
        /* [in] */ IPrintWriter* writer,
        /* [in] */ Int32 limit,
        /* [in] */ Int32 userId,
        /* [in] */ const String& linePrefix,
        /* [in] */ Boolean duplicateSimpleNames);

    static CARAPI FormatDuration(
        /* [in] */ Int64 duration,
        /* [out] */ String* result);

    /* package */
    static CARAPI GetSimpleName(
        /* [in] */ IComponentName* cn,
        /* [out] */ String* result);

private:
    CARAPI AddEvent(
        /* [in] */ Event* e);

    static CARAPI_(String) FormatElapsed(
        /* [in] */ Int64 elapsed);

    CARAPI_(String) DumpType(
        /* [in] */ Int32 type);

public:
    AutoPtr<IArrayDeque> mEvents;

private:
    static const Int32 TYPE_GRANT_TRUST;

    static const Int32 TYPE_REVOKE_TRUST;

    static const Int32 TYPE_TRUST_TIMEOUT;

    static const Int32 TYPE_AGENT_DIED;

    static const Int32 TYPE_AGENT_CONNECTED;

    static const Int32 TYPE_AGENT_STOPPED;

    static const Int32 TYPE_MANAGING_TRUST;

    static const Int32 HISTORY_LIMIT;
};

} // namespace Trust
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TRUST_TRUSTARCHIVE_H__
