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

#ifndef __DCARSERVER_H__
#define __DCARSERVER_H__

#ifndef __USE_MALLOC
#define __USE_MALLOC
#endif

#include <elastos.h>
#include <elastos/utility/etl/HashMap.h>
#include "DataWrapper.h"

using namespace Elastos;
using Elastos::Utility::Etl::HashMap;

// come from marshal_rpc.h
typedef struct InterfacePack
{
    EMuid               m_clsid;            // clsid of object
    UInt32              m_uIndex;           // interface index in class
    String              m_sNetAddress;      // net address of stub
}   InterfacePack;

class DCARServer : public ElLightRefBase
{
public:
    static CARAPI_(AutoPtr<DCARServer>) Self();

    CARAPI Start();

    CARAPI_(String) GetNetAddress();

    CARAPI_(void) WaitForReady();

private:
    DCARServer();

    ~DCARServer();

    static void* ThreadEntry(void* arg);

    CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ InterfacePack* ip);

    CARAPI GetService(
        /* [in] */ const String& name,
        /* [out] */ InterfacePack* ip);

    CARAPI RemoveService(
        /* [in] */ const String& name);

    CARAPI DoCommand(
        /* [in] */ DataWrapper& data,
        /* [out] */ DataWrapper& reply);

    CARAPI_(Boolean) IsValidNetAddress(
        /* [in] */ const String& netAddress);

    CARAPI_(Boolean) IsValidIPAddress(
        /* [in] */ const String& ip);

    CARAPI_(Boolean) IsValidPort(
        /* [in] */ const String& port);

private:
    static AutoPtr<DCARServer> sServer;

    HashMap<String, InterfacePack*> mServices;
    String mNetAddress;

    Boolean mIsReady;
    pthread_mutex_t mWaitMutex;
    pthread_cond_t mWaitCond;
};

#endif //__DCARSERVER_H__
