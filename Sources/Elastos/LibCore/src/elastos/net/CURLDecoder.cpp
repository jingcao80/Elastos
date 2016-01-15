
#include "CURLDecoder.h"
#include "URLDecoder.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CURLDecoder, Singleton, IURLDecoder)

CAR_SINGLETON_IMPL(CURLDecoder)

ECode CURLDecoder::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return URLDecoder::Decode(s, result);
}

ECode CURLDecoder::Decode(
    /* [in] */ const String& s,
    /* [in] */ const String& charsetName,
    /* [out] */ String* result)
{
    return URLDecoder::Decode(s, charsetName, result);
}

} // namespace Net
} // namespace Elastos
