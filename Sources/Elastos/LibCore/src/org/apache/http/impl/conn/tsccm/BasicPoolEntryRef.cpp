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

#include "org/apache/http/impl/conn/tsccm/BasicPoolEntryRef.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

extern "C" const InterfaceID EIID_BasicPoolEntryRef =
        { 0x28561d66, 0xcaba, 0x42b1, { 0xaa, 0x24, 0x4f, 0x27, 0xd6, 0xf, 0xb6, 0xa8 } };

BasicPoolEntryRef::BasicPoolEntryRef(
    /* [in] */ BasicPoolEntry* entry,
    /* [in] */ IQueue* queue)
{
    // super(entry, queue);

    if (entry == NULL) {
        Logger::E("BasicPoolEntryRef", "Pool entry must not be null");
        assert(0);
        // throw new IllegalArgumentException("Pool entry must not be null.");
    }
    mRoute = entry->GetPlannedRoute();
}

PInterface BasicPoolEntryRef::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_BasicPoolEntryRef) {
        return reinterpret_cast<PInterface>(this);
    }
    else {
        return Object::Probe(riid);
    }
}

AutoPtr<IHttpRoute> BasicPoolEntryRef::GetRoute()
{
    return mRoute;
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org