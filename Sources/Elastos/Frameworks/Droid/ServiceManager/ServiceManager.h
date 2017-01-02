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

#ifndef __HH_SERVICEMANAGER_H
#define __HH_SERVICEMANAGER_H

#ifndef __USE_MALLOC
#define __USE_MALLOC
#endif

#include <binder/Binder.h>
#include <binder/IBinder.h>
#include <binder/Parcel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <clsinfo.h>
#include <utils/List.h>
#include <elastos/utility/etl/HashMap.h>

#define MAX_SERVICE_NUMBER 32
#define ELASTOS_SERVICEMGR_NAME "elastos.servicemanager"

using namespace Elastos;
using Elastos::Utility::Etl::HashMap;

// come from marshal_ipc.h
typedef struct InterfacePack
{
    EMuid               m_clsid;             // clsid of object
    UInt32              m_uIndex;            // interface index in class
    android::sp<android::IBinder> m_pBinder;
} InterfacePack;

class ServiceManager : public android::BBinder
{
private:
    class ServiceDeathRecipient : public android::IBinder::DeathRecipient
    {
    public:
        ServiceDeathRecipient(
            /* [in] */ ServiceManager* host,
            /* [in] */ const String& name);

        void binderDied(
            /* [in] */ const android::wp<android::IBinder>& who);

    private:
        ServiceManager* mHost;
        String mName;
    };

public:
    ServiceManager();
    virtual ~ServiceManager();

    enum {
        ADD_SERVICE = android::IBinder::FIRST_CALL_TRANSACTION,
        GET_SERVICE,
        REMOVE_SERVICE,
        CHECK_SERVICE,
        LIST_SERVICES,
    };

protected:
    virtual android::status_t onTransact(
        /* [in] */ uint32_t code,
        /* [in] */ const android::Parcel& data,
        /* [in] */ android::Parcel* reply,
        /* [in] */ uint32_t flags = 0);

private:
    CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ InterfacePack * ip);

    CARAPI GetService(
        /* [in] */ const String& name,
        /* [out] */ InterfacePack * pIp);

    CARAPI RemoveService(
        /* [in] */ const String& name);

    CARAPI CheckService(
        /* [in] */ const String& name,
        /* [out] */ InterfacePack * pIp);

private:
    HashMap<String, InterfacePack*> mServices;
    android::List< android::sp<ServiceDeathRecipient> > mSdrList;
};

#endif // __HH_SERVICEMANAGER_H

