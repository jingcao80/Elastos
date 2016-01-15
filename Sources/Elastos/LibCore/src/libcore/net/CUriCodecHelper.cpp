
#include "CUriCodecHelper.h"
#include "UriCodec.h"

namespace Libcore {
namespace Net {

CAR_INTERFACE_IMPL(CUriCodecHelper, Singleton, IUriCodecHelper)

CAR_SINGLETON_IMPL(CUriCodecHelper)

ECode CUriCodecHelper::ValidateSimple(
    /* [in] */ const String& s,
    /* [in] */ const String& legal)
{
    return UriCodec::ValidateSimple(s,legal);
}

ECode CUriCodecHelper::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return UriCodec::Decode(s,result);
}

ECode CUriCodecHelper::Decode(
    /* [in] */ const String& s,
    /* [in] */ Boolean convertPlus,
    /* [in] */ ICharset* charset,
    /* [in] */ Boolean throwOnFailure,
    /* [out] */ String* decodedString)
{
    return UriCodec::Decode(s,convertPlus,charset,throwOnFailure,decodedString);
}

} // namespace Net
} // namespace Libcore
