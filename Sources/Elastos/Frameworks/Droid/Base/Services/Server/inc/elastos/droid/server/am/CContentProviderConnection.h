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

#ifndef __ELASTOS_DROID_SERVER_AM_CCONTENTPROVIDERCONNECTION_H__
#define __ELASTOS_DROID_SERVER_AM_CCONTENTPROVIDERCONNECTION_H__

#include "_Elastos_Droid_Server_Am_CContentProviderConnection.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ContentProviderRecord;
class ProcessRecord;

CarClass(CContentProviderConnection)
    , public Object
    , public IBinder
{
public:
    CContentProviderConnection();

    ~CContentProviderConnection();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle32 provider,
        /* [in] */ Handle32 client);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

    CARAPI_(String) ToShortString();

    CARAPI_(String) ToClientString();

    CARAPI_(void) ToShortString(
        /* [in] */ StringBuilder* sb);

    CARAPI_(void) ToClientString(
        /* [in] */ StringBuilder* sb);

public:
    AutoPtr<ContentProviderRecord> mProvider;
    AutoPtr<ProcessRecord> mClient;
    Int64 mCreateTime;
    Int32 mStableCount;
    Int32 mUnstableCount;
    // The client of this connection is currently waiting for the provider to appear.
    // Protected by the provider lock.
    Boolean mWaiting;
    // The provider of this connection is now dead.
    Boolean mDead;

    // For debugging.
    Int32 mNumStableIncs;
    Int32 mNumUnstableIncs;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_SERVER_AM_CCONTENTPROVIDERCONNECTION_H__
