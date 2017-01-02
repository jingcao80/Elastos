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

#ifndef __ELASTOS_DROID_SERVER_REMOTE_LANDISPATCHER_H__
#define __ELASTOS_DROID_SERVER_REMOTE_LANDISPATCHER_H__

#include <ext/frameworkext.h>
#include "remote/ServerSocketTimer.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Net::IDatagramSocket;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class LanDispatcher : public ThreadBase
{
public:
    LanDispatcher(
        /* [in] */ IContext* context);

    CARAPI Run();

private:
    CARAPI_(String) GetVerifiedPort(
        /* [in] */ const String& clientMsg);

private:
    static const String TAG;
    static const Boolean DBG;
    static const String LISTENADDR;
    static const Int32 LISTENPORT;
    List<AutoPtr<ServerSocketTimer> > mServerSocketList;

    AutoPtr<IContext> mContext;
    AutoPtr<IDatagramSocket> mLanListenSocket;
    String mLogin;
    String mCheck;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_LANDISPATCHER_H__
