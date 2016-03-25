
#include "org/apache/http/message/BasicRequestLine.h"
#include "org/apache/http/message/BasicLineFormatter.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL_2(BasicRequestLine, Object, IRequestLine, ICloneable)

ECode BasicRequestLine::Init(
    /* [in] */ const String& method,
    /* [in] */ const String& uri,
    /* [in] */ IProtocolVersion* version)
{
    if (method.IsNull()) {
        Logger::E("BasicRequestLine", "Method must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (uri.IsNull()) {
        Logger::E("BasicRequestLine", "URI must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (version == NULL) {
        Logger::E("BasicRequestLine", "Protocol version must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMethod = method;
    mUri = uri;
    mProtoVersion = version;
    return NOERROR;
}

ECode BasicRequestLine::GetMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    *method = mMethod;
    return NOERROR;
}

ECode BasicRequestLine::GetProtocolVersion(
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = mProtoVersion;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

ECode BasicRequestLine::GetUri(
    /* [out] */ String* uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    return NOERROR;
}

ECode BasicRequestLine::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    // no need for non-default formatting in toString()
    AutoPtr<ILineFormatter> formatter = ILineFormatter::Probe(BasicLineFormatter::DEFAULT);
    AutoPtr<ICharArrayBuffer> buf;
    formatter->FormatRequestLine(NULL, this, (ICharArrayBuffer**)&buf);
    return IObject::Probe(buf)->ToString(str);
}

void BasicRequestLine::CloneImpl(
    /* [in] */ BasicRequestLine* obj)
{
    obj->mMethod = mMethod;
    obj->mUri = mUri;
    obj->mProtoVersion = mProtoVersion;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
