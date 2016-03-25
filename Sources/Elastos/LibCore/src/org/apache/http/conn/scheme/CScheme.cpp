
#include "org/apache/http/conn/scheme/CScheme.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Scheme {

CScheme::CScheme()
    : mDefaultPort(0)
    , mLayered(FALSE)
{}

CAR_INTERFACE_IMPL(CScheme, Object, IScheme)

CAR_OBJECT_IMPL(CScheme)

ECode CScheme::GetDefaultPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    *port = mDefaultPort;
    return NOERROR;
}

ECode CScheme::GetSocketFactory(
    /* [out] */ ISocketFactory** socketFactory)
{
    VALIDATE_NOT_NULL(socketFactory)
    *socketFactory = mSocketFactory;
    REFCOUNT_ADD(*socketFactory)
    return NOERROR;
}

ECode CScheme::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode CScheme::IsLayered(
    /* [out] */ Boolean* isLayered)
{
    VALIDATE_NOT_NULL(isLayered)
    *isLayered = mLayered;
    return NOERROR;
}

ECode CScheme::ResolvePort(
    /* [in] */ Int32 port,
    /* [out] */ Int32* resolvedPort)
{
    VALIDATE_NOT_NULL(resolvedPort)
    *resolvedPort = ((port <= 0) || (port > 0xffff)) ? mDefaultPort : port;
    return NOERROR;
}

ECode CScheme::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mStringRep.IsNull()) {
        StringBuilder buffer;
        buffer.Append(mName);
        buffer.AppendChar(':');
        buffer.Append(StringUtils::ToString(mDefaultPort));
        mStringRep = buffer.ToString();
    }
    *result = mStringRep;
    return NOERROR;
}

ECode CScheme::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (other == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (TO_IINTERFACE(this) == other) {
        *result = TRUE;
        return NOERROR;
    }
    if (IScheme::Probe(other) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<CScheme> s = (CScheme*)IScheme::Probe(other);
    Boolean isEquals;
    *result = (mName.Equals(s->mName) &&
            mDefaultPort == s->mDefaultPort &&
            mLayered == s->mLayered &&
            (IObject::Probe(mSocketFactory)->Equals(s->mSocketFactory, &isEquals), isEquals));
    return NOERROR;
}

ECode CScheme::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    assert(0);
    // Int32 hash = LangUtils.HASH_SEED;
    // hash = LangUtils.hashCode(hash, this.defaultPort);
    // hash = LangUtils.hashCode(hash, this.name);
    // hash = LangUtils.hashCode(hash, this.layered);
    // hash = LangUtils.hashCode(hash, this.socketFactory);
    // return hash;
    return E_NOT_IMPLEMENTED;
}

ECode CScheme::constructor(
    /* [in] */ const String& name,
    /* [in] */ ISocketFactory* factory,
    /* [in] */ Int32 port)
{
    if (name.IsNull()) {
        Logger::E("CPlainSocketFactory", "Scheme name may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (factory == NULL) {
        Logger::E("CPlainSocketFactory", "Socket factory may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((port <= 0) || (port > 0xffff)) {
        Logger::E("CPlainSocketFactory", "Port is invalid: %d", port);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mName = name.ToLowerCase(/*ILocale::ENGLISH*/);
    mSocketFactory = factory;
    mDefaultPort = port;
    mLayered = (ILayeredSocketFactory::Probe(factory) != NULL);
    return NOERROR;
}

} // namespace Scheme
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
