#include "CSelectorProviderHelper.h"
#include "SelectorProvider.h"

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

CAR_INTERFACE_IMPL(CSelectorProviderHelper, Singleton, ISelectorProviderHelper)

CAR_SINGLETON_IMPL(CSelectorProviderHelper)

ECode CSelectorProviderHelper::GetProvider(
        /* [out] */ ISelectorProvider** provider)
{
    return SelectorProvider::GetProvider(provider);
}

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos
