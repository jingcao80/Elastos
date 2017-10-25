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

#ifndef __ELASTOS_DROID_SERVER_AM_BROADCASTFILTER_H__
#define __ELASTOS_DROID_SERVER_AM_BROADCASTFILTER_H__

#include "_Elastos.Droid.Server.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/droid/content/IntentFilter.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IntentFilter;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ReceiverList;

class BroadcastFilter
    : public IntentFilter
    , public IBroadcastFilter
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IIntentFilter* filter);

    BroadcastFilter(
        /* [in] */ ReceiverList* receiverList,
        /* [in] */ const String& packageName,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Int32 owningUid,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean isSystem);

    ~BroadcastFilter();

    using IntentFilter::Dump;

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpBrief(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpInReceiverList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IPrinter* pr,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpBroadcastFilterState(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI OnCompareTie(
        /* [in] */ IIntentFilter* other,
        /* [out] */ Int32* result);

public:
    // Back-pointer to the list this filter is in.
    ReceiverList* mReceiverList; // BroadcastFilter will be PushBack to ReceiverList
    String mPackageName;
    String mRequiredPermission;
    Int32 mOwningUid;
    Int32 mOwningUserId;
    Boolean mIsSystem;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Server::Am::BroadcastFilter*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::BroadcastFilter)

#endif //__ELASTOS_DROID_SERVER_AM_BROADCASTFILTER_H__
