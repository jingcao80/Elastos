#include "SelectorProvider.h"
#include "CSelectorProviderImpl.h"
#include "ServiceLoader.h"

using Elastos::Utility::ServiceLoader;
using Elastos::Utility::IServiceLoader;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

CAR_INTERFACE_IMPL(SelectorProvider, Object, ISelectorProvider)

AutoPtr<ISelectorProvider> SelectorProvider::sProvider;

SelectorProvider::SelectorProvider()
{
}

SelectorProvider::~SelectorProvider()
{
}

ECode SelectorProvider::GetProvider(
    /* [out] */ ISelectorProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    if (NULL == sProvider) {
        AutoPtr<CSelectorProviderImpl> impl;
        ASSERT_SUCCEEDED(CSelectorProviderImpl::NewByFriend((CSelectorProviderImpl**)&impl));
        sProvider = (ISelectorProvider*)impl.Get();
    }

    *provider = sProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode SelectorProvider::LoadProviderByJar(
    /* [out] */ ISelectorProvider** provider)
{
    VALIDATE_NOT_NULL(provider)

    AutoPtr<IServiceLoader> sl = ServiceLoader::Load(EIID_ISelectorProvider);
    if (sl) {
        AutoPtr<IIterator> it;
        ((ServiceLoader*)sl.Get())->GetIterator((IIterator**)&it);
        Boolean isflag = FALSE;
        while(it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            *provider = ISelectorProvider::Probe(outface);
            REFCOUNT_ADD(*provider)
            return NOERROR;
        }
    }

    *provider = NULL;
    return NOERROR;
}

ECode SelectorProvider::InheriteChannel(
    /* [out] */ IChannel** cannel)
{
    VALIDATE_NOT_NULL(cannel)

    // // Android never has stdin/stdout connected to a socket.
    *cannel = NULL;
    return NOERROR;
}

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos
