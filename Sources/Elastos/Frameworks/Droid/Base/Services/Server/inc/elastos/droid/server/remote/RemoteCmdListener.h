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

#ifndef __ELASTOS_DROID_SERVER_REMOTE_REMOTECMDLISTENER_H__
#define __ELASTOS_DROID_SERVER_REMOTE_REMOTECMDLISTENER_H__

#include <ext/frameworkext.h>

using Elastos::Net::IServerSocket;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class RemoteCmdListener : public ThreadBase
{
public:
    /**********************************************
     *
     *  RemoteCmdListener Constructor
     *
     **********************************************/
    RemoteCmdListener(
        /* [in] */ IContext* context,
        /* [in] */ IServerSocket* listenSocket);

    CARAPI_(void) Quit();

    CARAPI Run();

    CARAPI_(void) AcceptStringCmd();

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 CMDTIMEOUT_SEC;

    AutoPtr<IServerSocket> mSocketListener;
    AutoPtr<IContext> mContext;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_REMOTECMDLISTENER_H__
