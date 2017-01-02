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

#ifndef __MONKEYNETWORKMONITOR_H__
#define __MONKEYNETWORKMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::IIActivityManager;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyNetworkMonitor
{
public:
    MonkeyNetworkMonitor();

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

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

private:
        CARAPI UpdateNetworkStats();

private:
    static const Boolean LDEBUG;
    const AutoPtr<IIntentFilter> mFilter;
    Int64 mCollectionStartTime;
    Int64 mEventTime;
    Int32 mLastNetworkType;
    Int64 mWifiElapsedTime;
    Int64 mMobileElapsedTime;
    Int64 mElapsedTime;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYNETWORKMONITOR_H__
