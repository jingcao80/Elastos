
#include "CCertStoreHelper.h"
#include "CertStore.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CCertStoreHelper, Singleton, ICertStoreHelper)
CAR_SINGLETON_IMPL(CCertStoreHelper)
ECode CCertStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters *params,
    /* [out] */ ICertStore **certStore)
{
    return CertStore::GetInstance(type, params, certStore);
}

ECode CCertStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters *params,
    /* [in] */ const String& provider,
    /* [out] */ ICertStore **certStore)
{
    return CertStore::GetInstance(type, params, provider, certStore);
}

ECode CCertStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters * params,
    /* [in] */ IProvider *provider,
    /* [out] */ ICertStore **certStore)
{
    return CertStore::GetInstance(type, params, provider, certStore);
}

ECode CCertStoreHelper::GetDefaultType(
    /* [out] */ String *type)
{
    return CertStore::GetDefaultType(type);
}

}
}
}
