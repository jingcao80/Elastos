
#include "elastos/droid/media/CMediaCryptoHelper.h"
#include "elastos/droid/media/CMediaCrypto.h"

using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCryptoHelper, Singleton, IMediaCryptoHelper)

CAR_SINGLETON_IMPL(CMediaCryptoHelper)

ECode CMediaCryptoHelper::IsCryptoSchemeSupported(
    /* [in] */ IUUID* uuid,
    /* [out] */ Boolean* result)
{
    return CMediaCrypto::IsCryptoSchemeSupported(uuid, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
