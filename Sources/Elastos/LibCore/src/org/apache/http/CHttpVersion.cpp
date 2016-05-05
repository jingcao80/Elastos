
#include "org/apache/http/CHttpVersion.h"

namespace Org {
namespace Apache {
namespace Http {

const String CHttpVersion::HTTP("HTTP");

static AutoPtr<IHttpVersion> InitHttpVersion(
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor)
{
    AutoPtr<IHttpVersion> ver;
    CHttpVersion::New(major, minor, (IHttpVersion**)&ver);
    return ver;
}
const AutoPtr<IHttpVersion> CHttpVersion::HTTP_0_9 = InitHttpVersion(0, 9);
const AutoPtr<IHttpVersion> CHttpVersion::HTTP_1_0 = InitHttpVersion(1, 0);
const AutoPtr<IHttpVersion> CHttpVersion::HTTP_1_1 = InitHttpVersion(1, 1);
const Int64 CHttpVersion::serialVersionUID;

CAR_INTERFACE_IMPL(CHttpVersion, ProtocolVersion, IHttpVersion)

CAR_OBJECT_IMPL(CHttpVersion)

ECode CHttpVersion::ForVersion(
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor,
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)

    if ((major == mMajor) && (minor == mMinor)) {
        *ver = this;
        REFCOUNT_ADD(*ver)
        return NOERROR;
    }

    if (major == 1) {
        if (minor == 0) {
            *ver = IProtocolVersion::Probe(HTTP_1_0);
            REFCOUNT_ADD(*ver)
            return NOERROR;
        }
        if (minor == 1) {
            *ver = IProtocolVersion::Probe(HTTP_1_1);
            REFCOUNT_ADD(*ver)
            return NOERROR;
        }
    }
    if ((major == 0) && (minor == 9)) {
        *ver = IProtocolVersion::Probe(HTTP_0_9);
        REFCOUNT_ADD(*ver)
        return NOERROR;
    }

    // argument checking is done in the constructor
    return CHttpVersion::New(major, minor, ver);
}

ECode CHttpVersion::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<IHttpVersion> ver;
    CHttpVersion::New((IHttpVersion**)&ver);
    CloneImpl(IProtocolVersion::Probe(ver));
    *obj = ver->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CHttpVersion::constructor(
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor)
{
    return ProtocolVersion::Init(HTTP, major, minor);
}

ECode CHttpVersion::constructor()
{
    return NOERROR;
}

} // namespace Http
} // namespace Apache
} // namespace Org
