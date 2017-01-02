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

#ifndef __ELASTOS_DROID_SERVER_REMOTE_REMOTEUTILS_H__
#define __ELASTOS_DROID_SERVER_REMOTE_REMOTEUTILS_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class RemoteUtils
{
public:
    /**
     * composeResult: Add result number in the front of result
     * format: [rltNum][SPLITER][result]
     */
    static CARAPI_(String) ComposeResult(
        /* [in] */ Int32 rltNum,
        /* [in] */ const String& result);

    static CARAPI_(Boolean) IsDebug();

private:
    static const String TAG;

public:
    static const Boolean DBG;
    static const String SPLITER;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_REMOTEUTILS_H__
