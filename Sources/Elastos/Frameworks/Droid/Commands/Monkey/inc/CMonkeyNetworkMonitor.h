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

#ifndef __CMONKEYNETWORKMONITOR_H__
#define __CMONKEYNETWORKMONITOR_H__

#include "_CMonkeyNetworkMonitor.h"
#include "MonkeyNetworkMonitor.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyNetworkMonitor) , public MonkeyNetworkMonitor
{
public:
    CAR_OBJECT_DECL()

    CARAPI PerformReceive(
        /* [in] */ IIntent *intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle *extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI Start();

    CARAPI Register(
        /* [in] */ IIActivityManager *am);

    CARAPI Unregister(
        /* [in] */ IIActivityManager *am);

    CARAPI Stop();

    CARAPI Dump();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYNETWORKMONITOR_H__
