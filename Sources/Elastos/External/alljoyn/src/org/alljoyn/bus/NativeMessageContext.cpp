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

#include "org/alljoyn/bus/NativeMessageContext.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Map.h>
#include <elastos/utility/etl/Pair.h>
#include <qcc/Thread.h>

using Elastos::Core::AutoLock;
using Elastos::Core::Object;
using Elastos::Utility::Etl::Map;
using Elastos::Utility::Etl::Pair;

namespace Org {
namespace Alljoyn {
namespace Bus {

Map<qcc::Thread*, ajn::Message> gMessageMap;

Object gMessageMapLock;

ajn::Message NativeMessageContext::GetMessage()
{
    AutoLock lock(gMessageMapLock);
    Map<qcc::Thread*, ajn::Message>::Iterator it =
            gMessageMap.Find(qcc::Thread::GetThread());
    assert(gMessageMap.End() != it);
    ajn::Message m = it->mSecond;
    return m;
}

NativeMessageContext::NativeMessageContext(
    /* [in] */ const ajn::Message& msg)
{
    AutoLock lock(gMessageMapLock);
    gMessageMap.Insert(Pair<qcc::Thread*, ajn::Message>(qcc::Thread::GetThread(), msg));
}

NativeMessageContext::~NativeMessageContext()
{
    AutoLock lock(gMessageMapLock);
    Map<qcc::Thread*, ajn::Message>::Iterator it =
            gMessageMap.Find(qcc::Thread::GetThread());
    gMessageMap.Erase(it);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
