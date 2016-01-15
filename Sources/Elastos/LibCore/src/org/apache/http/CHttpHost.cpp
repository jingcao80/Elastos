
#include "org/apache/http/CHttpHost.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "org/apache/http/utility/LangUtils.h"
#include "elastos/core/CString.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocale;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::LangUtils;

namespace Org {
namespace Apache {
namespace Http {

CAR_INTERFACE_IMPL_2(CHttpHost, Object, IHttpHost, ICloneable)

CAR_OBJECT_IMPL(CHttpHost)

ECode CHttpHost::GetHostName(
    /* [out] */ String* hostName)
{
    VALIDATE_NOT_NULL(hostName)
    *hostName = mHostName;
    return NOERROR;
}

ECode CHttpHost::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    *port = mPort;
    return NOERROR;
}

ECode CHttpHost::GetSchemeName(
    /* [out] */ String* sehemeName)
{
    VALIDATE_NOT_NULL(sehemeName)
    *sehemeName = mSchemeName;
    return NOERROR;
}

ECode CHttpHost::ToURI(
    /* [out] */ String* uri)
{
    VALIDATE_NOT_NULL(uri)

    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(32, (ICharArrayBuffer**)&buffer);
    buffer->Append(mSchemeName);
    buffer->Append(String("://"));
    buffer->Append(mHostName);
    if (mPort != -1) {
        buffer->Append(':');
        buffer->Append(StringUtils::ToString(mPort));
    }
    return IObject::Probe(buffer)->ToString(uri);
}

ECode CHttpHost::ToHostString(
    /* [out] */ String* hostString)
{
    VALIDATE_NOT_NULL(hostString)

    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(32, (ICharArrayBuffer**)&buffer);
    buffer->Append(mHostName);
    if (mPort != -1) {
        buffer->Append(':');
        buffer->Append(StringUtils::ToString(mPort));
    }
    return IObject::Probe(buffer)->ToString(hostString);
}

ECode CHttpHost::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)
    return ToURI(string);
}

ECode CHttpHost::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)

    if (obj == NULL) {
        *equals = FALSE;
        return NOERROR;
    }
    if (this->Probe(EIID_IInterface) == obj) {
        *equals = TRUE;
        return NOERROR;
    }
    if (IHttpHost::Probe(obj) != NULL) {
        AutoPtr<IHttpHost> that = IHttpHost::Probe(obj);
        String thatHostName;
        that->GetHostName(&thatHostName);
        Int32 thatPort;
        that->GetPort(&thatPort);
        String thatSchemeName;
        that->GetSchemeName(&thatSchemeName);
        *equals = mLcHostName.Equals(thatHostName.ToLowerCase())
                && mPort == thatPort
                && mSchemeName.Equals(thatSchemeName);
    }
    else {
        *equals = FALSE;
    }

    return NOERROR;
}

ECode CHttpHost::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    Int32 hash = LangUtils::HASH_SEED;
    AutoPtr<ICharSequence> cs1;
    CString::New(mLcHostName, (ICharSequence**)&cs1);
    hash = LangUtils::HashCode(hash, IObject::Probe(cs1));
    hash = LangUtils::HashCode(hash, mPort);
    AutoPtr<ICharSequence> cs2;
    CString::New(mSchemeName, (ICharSequence**)&cs2);
    hash = LangUtils::HashCode(hash, IObject::Probe(cs2));

    *hashCode = hash;
    return NOERROR;
}

ECode CHttpHost::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IHttpHost> host;
    CHttpHost::New(mHostName, mPort, mSchemeName, (IHttpHost**)&host);
    *object = host->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CHttpHost::constructor(
    /* [in] */ const String& hostName,
    /* [in] */ Int32 port,
    /* [in] */ const String& scheme)
{
    if (hostName.IsNull()) {
        Logger::E("CHttpHost", "Host name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHostName = hostName;
    mLcHostName = hostName.ToLowerCase(/*CLocale::ENGLISH*/);
    if (!scheme.IsNull()) {
        mSchemeName = scheme.ToLowerCase(/*CLocale::ENGLISH*/);
    }
    else {
        mSchemeName = DEFAULT_SCHEME_NAME;
    }
    mPort = port;
    return NOERROR;
}

ECode CHttpHost::constructor(
    /* [in] */ const String& hostName,
    /* [in] */ Int32 port)
{
    return constructor(hostName, port, String(NULL));
}

ECode CHttpHost::constructor(
    /* [in] */ const String& hostName)
{
    return constructor(hostName, -1, String(NULL));
}

ECode CHttpHost::constructor(
    /* [in] */ IHttpHost* httpHost)
{
    String hostname;
    httpHost->GetHostName(&hostname);
    Int32 port;
    httpHost->GetPort(&port);
    String schemeName;
    httpHost->GetSchemeName(&schemeName);
    return constructor(hostname, port, schemeName);
}

} // namespace Http
} // namespace Apache
} // namespace Org
