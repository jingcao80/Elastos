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

#ifndef __ELASTOS_DROID_SERVER_REMOTE_ROUTERSETTINGFACTORY_H__
#define __ELASTOS_DROID_SERVER_REMOTE_ROUTERSETTINGFACTORY_H__

#include "remote/AbstractDeviceFactory.h"

using Elastos::Net::IServerSocket;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class RouterSettingFactory : public AbstractDeviceFactory
{
private:
    // the thread for send intent to RouterSettings in box
    class SendIntentThread : public ThreadBase
    {
    public:
        SendIntentThread(
            /* [in] */ RouterSettingFactory* host);

        CARAPI Run();

    private:
        RouterSettingFactory* mHost;
    };


public:
    //Using public constructor for java reflection
    RouterSettingFactory(
        /* [in] */ IContext* ctx);

    CARAPI RunForResult(
        /* [in] */ const String& cmd,
        /* [out] */ String* result);

private:
    CARAPI_(Boolean) IsCorrectDevice();

    CARAPI_(String) SocketCmd_Task();

private:
    static const String TAG;
    static const Boolean DBG;

    String mRemoteCmd;
    Int32 mCmdCheckSN;
    Int32 mPort;
    String mCmd;
    String mKey;
    String mValue;
    AutoPtr<IServerSocket> mServersocket;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_ROUTERSETTINGFACTORY_H__
