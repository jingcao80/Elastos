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

#ifndef __ORG_ALLJOYN_BUS_NATIVESESSIONPORTLISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVESESSIONPORTLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/SessionPortListener.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeSessionPortListener : public ajn::SessionPortListener
{
public:
    NativeSessionPortListener(
        /* [in] */ ISessionPortListener* sessionPortListener);

    ~NativeSessionPortListener();

    bool AcceptSessionJoiner(
        /* [in] */ ajn::SessionPort sessionPort,
        /* [in] */ const char* joiner,
        /* [in] */ const ajn::SessionOpts& opts);

    void SessionJoined(
        /* [in] */ ajn::SessionPort sessionPort,
        /* [in] */ ajn::SessionId id,
        /* [in] */ const char* joiner);

private:
    NativeSessionPortListener(const NativeSessionPortListener& other);
    NativeSessionPortListener& operator =(const NativeSessionPortListener& other);

    AutoPtr<IWeakReference> mSessionPortListener;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVESESSIONPORTLISTENER_H__
