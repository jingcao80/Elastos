#include "elastos/droid/media/CMediaHTTPService.h"
#include "elastos/droid/media/CMediaHTTPConnection.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaHTTPService::TAG("MediaHTTPService");

CAR_INTERFACE_IMPL_2(CMediaHTTPService, Object, IIMediaHTTPService, IBinder)

CAR_OBJECT_IMPL(CMediaHTTPService)

CMediaHTTPService::CMediaHTTPService()
{
}

CMediaHTTPService::~CMediaHTTPService()
{
}

ECode CMediaHTTPService::constructor()
{
    return NOERROR;
}

ECode CMediaHTTPService::MakeHTTPConnection(
    /* [out] */ IIMediaHTTPConnection** result)
{
    AutoPtr<IIMediaHTTPConnection> conn;
    CMediaHTTPConnection::New((IIMediaHTTPConnection**)&conn);
    *result = IIMediaHTTPConnection::Probe(conn);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaHTTPService::CreateHttpServiceBinderIfNecessary(
    /* [in] */ const String& path,
    /* [out] */ IBinder** result)
{
    if (path.StartWith("http://")
            || path.StartWith("https://")
            || path.StartWith("widevine://")) {
        AutoPtr<IIMediaHTTPService> service;
        CMediaHTTPService::New((IIMediaHTTPService**)&service);
        *result = IBinder::Probe(service);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    *result = NULL;
    return NOERROR;
}

ECode CMediaHTTPService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CMediaHTTPService";
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
