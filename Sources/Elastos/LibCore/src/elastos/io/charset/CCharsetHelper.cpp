#include "CCharsetHelper.h"
#include "Charset.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CCharsetHelper, Singleton, ICharsetHelper)

CAR_SINGLETON_IMPL(CCharsetHelper)

ECode CCharsetHelper::AvailableCharsets(
    /* [out] */ ISortedMap** outsm)
{
    return Charset::AvailableCharsets(outsm);
}

ECode CCharsetHelper::ForName(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** charset)
{
    return Charset::ForName(charsetName, charset);
}

ECode CCharsetHelper::IsSupported(
    /* [in] */ const String& charsetName,
    /* [out] */ Boolean* result)
{
    return Charset::IsSupported(charsetName, result);
}

ECode CCharsetHelper::DefaultCharset(
    /* [out] */ICharset** charset)
{
    return Charset::DefaultCharset(charset);
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
