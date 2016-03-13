
#include "ServiceManager.h"
#include <utils/Errors.h>

ServiceManager::ServiceDeathRecipient::ServiceDeathRecipient(
    /* [in] */ ServiceManager* host,
    /* [in] */ const String& name)
    : mHost(host)
    , mName(name)
{
}

void ServiceManager::ServiceDeathRecipient::binderDied(
    /* [in] */ const android::wp<android::IBinder>& who)
{
    ALOGE("[ServiceManager::ServiceDeathRecipient], the service may be died, so unregister the service's binder. name=[%s]",
        mName.string());
    HashMap<String, InterfacePack*>::Iterator it = mHost->mServices.Find(mName);
    if (it != mHost->mServices.End()) {
        if (it->mSecond != NULL) {
            it->mSecond->m_pBinder = NULL;
            free(it->mSecond);
        }
        mHost->mServices.Erase(it);

        if (mHost->mSdrList.size() > 0) {
            android::List< android::sp<ServiceDeathRecipient> >::iterator iter;
            for (iter = mHost->mSdrList.begin(); iter != mHost->mSdrList.end(); iter++) {
                if (*iter == this) {
                    mHost->mSdrList.erase(iter);
                    return;
                }
            }
        }
    }
}

ServiceManager::ServiceManager()
    : mServices(MAX_SERVICE_NUMBER)
{}

ServiceManager::~ServiceManager()
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Begin();
    while (it != mServices.End()) {
        if (it->mSecond != NULL) {
            it->mSecond->m_pBinder = NULL;
            free(it->mSecond);
        }
        it++;
    }

    mSdrList.clear();
}

android::status_t ServiceManager::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [in] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    ECode ec = NOERROR;
    const char * name;

    switch (code) {
        case ADD_SERVICE: {
            InterfacePack * ip = NULL;
            name = data.readCString();
            int32_t size = data.readInt32();
            ip = (InterfacePack *)malloc(size);
            if (!ip) {
                reply->writeInt32((int32_t)E_OUT_OF_MEMORY);
            }
            else {
                const android::sp<android::IBinder> binder = data.readStrongBinder();
                data.read((void *)ip, (size_t)size);
                ip->m_pBinder = binder;
                ec = AddService(String(name), ip);
                reply->writeInt32((int32_t)ec);

                //Register the DeathRecipient callback
                assert(ip->m_pBinder != NULL);
                android::sp<ServiceDeathRecipient> sdr = new ServiceDeathRecipient(this, String(name));
                ip->m_pBinder->linkToDeath(sdr, sdr.get(), 0);
                mSdrList.push_back(sdr);
            }
            return android::NO_ERROR;
        } break;

        case GET_SERVICE: {
            InterfacePack ip;
            name = data.readCString();
            ec = GetService(String(name), &ip);
            reply->writeInt32((int32_t)ec);
            if (SUCCEEDED(ec)) {
                int32_t size = sizeof(InterfacePack);
                reply->writeInt32((int32_t)size);
                reply->writeStrongBinder(ip.m_pBinder);
                ip.m_pBinder = NULL;
                reply->write((const void *)&ip, (int32_t)size);
            }
            return android::NO_ERROR;
        } break;

        case REMOVE_SERVICE: {
            name = data.readCString();
            ec = RemoveService(String(name));
            reply->writeInt32((int32_t)ec);
            return android::NO_ERROR;
        } break;

        case CHECK_SERVICE: {
            InterfacePack ip;
            name = data.readCString();
            ALOGD(" >> CHECK_SERVICE %s", name);
            ec = GetService(String(name), &ip);
            reply->writeInt32((int32_t)ec);
            if (SUCCEEDED(ec)) {
                int32_t size = sizeof(InterfacePack);
                reply->writeInt32((int32_t)size);
                reply->writeStrongBinder(ip.m_pBinder);
                ip.m_pBinder = NULL;
                reply->write((const void *)&ip, (int32_t)size);
            }
            return android::NO_ERROR;
        } break;

        case LIST_SERVICES: {
            int32_t size = mServices.GetSize();
            reply->writeInt32((int32_t)size);
            HashMap<String, InterfacePack*>::Iterator it = mServices.Begin();
            for (; it != mServices.End(); ++it) {
                reply->writeCString(it->mFirst.string());
            }
            return android::NO_ERROR;
        } break;

        default:
            return android::BBinder::onTransact(code, data, reply, flags);
    }
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ InterfacePack * ip)
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Find(name);
    if (it != mServices.End()) {
        if (it->mSecond != NULL) {
            it->mSecond->m_pBinder = NULL;
            free(it->mSecond);
        }
    }
    mServices[name] = ip;
    return NOERROR;
}

ECode ServiceManager::GetService(
    /* [in] */ const String& name,
    /* [out] */ InterfacePack * pIp)
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Find(name);
    if (it == mServices.End()) return E_FAIL;
    InterfacePack * ip = it->mSecond;
    memset(pIp, 0, sizeof(InterfacePack));
    pIp->m_clsid = ip->m_clsid;
    pIp->m_uIndex = ip->m_uIndex;
    pIp->m_pBinder = ip->m_pBinder;
    return NOERROR;
}

ECode ServiceManager::RemoveService(
    /* [in] */ const String& name)
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Find(name);
    if (it != mServices.End()) {
        InterfacePack* ip = it->mSecond;
        mServices.Erase(it);
        ip->m_pBinder = NULL;
        free(ip);
    }
    return NOERROR;
}

ECode ServiceManager::CheckService(
    /* [in] */ const String& name,
    /* [out] */ InterfacePack * pIp)
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Find(name);
    if (it == mServices.End()) return E_FAIL;
    InterfacePack * ip = it->mSecond;
    memset(pIp, 0, sizeof(InterfacePack));
    pIp->m_clsid = ip->m_clsid;
    pIp->m_uIndex = ip->m_uIndex;
    pIp->m_pBinder = ip->m_pBinder;
    return NOERROR;
}
