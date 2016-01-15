
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/http/multipart/StringPart.h"
#include <elastos/utility/logging/Logger.h>
#include <org/apache/http/utility/EncodingUtils.h>

using Elastos::Utility::Logging::Logger;
// using Org::Apache::Commons::Logging::ILogFactory;
using Org::Apache::Http::Utility::EncodingUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

// TODO:
// static AutoPtr<ILog> InitStaticLOG()
// {
//     AutoPtr<ILogFactory> factory;
//     CLogFactory::AcquireSingleton((ILogFactory**)&factory);
//     AutoPtr<ILog> log;
//     factory->GetLog(String("StringPart"), (ILog**)&log)
//     return log;
// }
// const AutoPtr<ILog> FilePart::LOG = InitStaticLOG();

CAR_INTERFACE_IMPL(StringPart, PartBase, IStringPart)

ECode StringPart::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ const String& charset)
{
    FAIL_RETURN(PartBase::constructor(name,
        DEFAULT_CONTENT_TYPE,
        charset == NULL ? DEFAULT_CHARSET : charset,
        DEFAULT_TRANSFER_ENCODING))

    if (value == NULL) {
        Logger::E("StringPart", "Value may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (value.ToByteIndex(0) != -1) {
        // See RFC 2048, 2.8. "8bit Data"
        Logger::E("StringPart", "NULs may not be present in string parts");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mValue = value;
    return NOERROR;
}

ECode StringPart::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return constructor(name, value, String(NULL));
}

AutoPtr<ArrayOf<Byte> > StringPart::GetContent()
{
    if (mContent == NULL) {
        String charset;
        GetCharSet(&charset);
        EncodingUtils::GetBytes(mValue, charset, (ArrayOf<Byte>**)&mContent);
    }
    return mContent;
}

ECode StringPart::SendData(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter sendData(OutputStream)"));
    return outStream->Write(GetContent());
}

ECode StringPart::LengthOfData(
    /* [out] */ Int64* length)
{
    // LOG->Trace(String("enter lengthOfData()"));
    return GetContent()->GetLength();
}

ECode StringPart::SetCharSet(
    /* [in] */ const String& charSet)
{
    PartBase::SetCharSet(charSet);
    mContent = NULL;
    return NOERROR;
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
