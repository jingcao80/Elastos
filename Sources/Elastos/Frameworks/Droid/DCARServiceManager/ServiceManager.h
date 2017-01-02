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

#ifndef __SERVICEMANAGER_H__
#define __SERVICEMANAGER_H__

#include <elastos/coredef.h>
#include <elastos/core/Mutex.h>
#include <elastos/utility/etl/HashMap.h>

using namespace Elastos;
using Elastos::Core::Mutex;
using Elastos::Utility::Etl::HashMap;

class ServiceManager : public ElLightRefBase
{
public:
    static CARAPI_(AutoPtr<ServiceManager>) GetInstance();

    CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service);

    CARAPI GetService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** service);

private:
    ServiceManager();

    CARAPI_(Boolean) ConnectServer();

private:
    static AutoPtr<ServiceManager> sInstance;

    Boolean mIsConnected;
    Mutex mConnectedLock;
    String mServerIpAddress;
    Int32 mServerPort;
};

#endif //__SERVICEMANAGER_H__
