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

#include "elastos/droid/server/am/CContentProviderConnection.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/ContentProviderRecord.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/TimeUtils.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(CContentProviderConnection, Object, IBinder)
CAR_OBJECT_IMPL(CContentProviderConnection)

CContentProviderConnection::CContentProviderConnection()
    : mCreateTime(0)
    , mStableCount(0)
    , mUnstableCount(0)
    , mWaiting(FALSE)
    , mDead(FALSE)
    , mNumStableIncs(0)
    , mNumUnstableIncs(0)
{}

CContentProviderConnection::~CContentProviderConnection()
{
}

ECode CContentProviderConnection::constructor(
    /* [in] */ HANDLE provider,
    /* [in] */ HANDLE client)
{
    mProvider = (ContentProviderRecord*)provider;
    mClient = (ProcessRecord*)client;
    mCreateTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode CContentProviderConnection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString();
    return NOERROR;
}

String CContentProviderConnection::ToString()
{
    StringBuilder sb(128);
    sb += ("CContentProviderConnection{");
    ToShortString(&sb);
    sb += ('}');
    return sb.ToString();
}

String CContentProviderConnection::ToShortString()
{
    StringBuilder sb(128);
    ToShortString(&sb);
    return sb.ToString();
}

String CContentProviderConnection::ToClientString()
{
    StringBuilder sb(128);
    ToClientString(&sb);
    return sb.ToString();
}

void CContentProviderConnection::ToShortString(
    /* [in] */ StringBuilder* sb)
{
    if (sb == NULL)
        return;

    *sb += mProvider->ToShortString();
    *sb += ("->");
    ToClientString(sb);
}

void CContentProviderConnection::ToClientString(
    /* [in] */ StringBuilder* sb)
{
    if (sb == NULL)
        return;

    if (mClient != NULL) {
        *sb += (mClient->ToShortString());
    }
    *sb += (" s");
    *sb += (mStableCount);
    *sb += ("/");
    *sb += (mNumStableIncs);
    *sb += (" u");
    *sb += (mUnstableCount);
    *sb += ("/");
    *sb += (mNumUnstableIncs);
    if (mWaiting) {
        *sb += (" WAITING");
    }
    if (mDead) {
        *sb += (" DEAD");
    }

    Int64 nowReal = SystemClock::GetElapsedRealtime();
    *sb += (" ");
    TimeUtils::FormatDuration(nowReal - mCreateTime, *sb);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
