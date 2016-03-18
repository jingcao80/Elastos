
#include "CURLEncoder.h"
#include "charset/CCharsetHelper.h"
#include "charset/StandardCharsets.h"
#include "Character.h"

using Elastos::IO::Charset::StandardCharsets;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::CCharsetHelper;

namespace Elastos {
namespace Net {

CURLEncoder::MyUriCodec CURLEncoder::ENCODER;

CAR_INTERFACE_IMPL(CURLEncoder, Singleton, IURLEncoder)

CAR_SINGLETON_IMPL(CURLEncoder)

ECode CURLEncoder::Encode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    AutoPtr<ICharset> charset = UriCodec::GetDefaultCharset();
    ENCODER.Encode(s, charset, result);
    return NOERROR;
}

ECode CURLEncoder::Encode(
    /* [in] */ const String& s,
    /* [in] */ const String& charsetName,
    /* [out] */ String* result)
{
    AutoPtr<ICharsetHelper> helper;
    AutoPtr<ICharset> charset = StandardCharsets::UTF_8;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
    FAIL_RETURN(helper->ForName(charsetName, (ICharset**)&charset));
    return ENCODER.Encode(s, charset, result);
}

} // namespace Net
} // namespace Elastos
