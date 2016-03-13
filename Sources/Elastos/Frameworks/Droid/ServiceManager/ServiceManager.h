
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

