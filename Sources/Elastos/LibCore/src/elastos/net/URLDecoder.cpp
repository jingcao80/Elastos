
#include "URLDecoder.h"
#include "UriCodec.h"
#include "charset/CCharsetHelper.h"

namespace Elastos {
namespace Net {

using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharset;
using Libcore::Net::UriCodec;

ECode URLDecoder::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* decodedS)
{
    VALIDATE_NOT_NULL(decodedS);
    AutoPtr<ICharset> charset;
    AutoPtr<ICharsetHelper> charsetHelper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&charsetHelper);
    charsetHelper->DefaultCharset((ICharset**)&charset);
    return UriCodec::Decode(s, TRUE, charset, TRUE, decodedS);
}

ECode URLDecoder::Decode(
    /* [in] */ const String& s,
    /* [in] */  const String& encoding,
    /* [out] */ String* decodedS)
{
    VALIDATE_NOT_NULL(decodedS);
    AutoPtr<ICharset> charset;
    AutoPtr<ICharsetHelper> charsetHelper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&charsetHelper);
    FAIL_RETURN(charsetHelper->ForName(encoding, (ICharset**)&charset));
    return UriCodec::Decode(s, TRUE, charset, TRUE, decodedS);
}

} // namespace Net
} // namespace Elastos
