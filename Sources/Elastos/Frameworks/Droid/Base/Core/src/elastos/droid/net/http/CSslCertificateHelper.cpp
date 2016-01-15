
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CSslCertificate.h"
#include "elastos/droid/net/http/CSslCertificateHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CSslCertificateHelper, Singleton, ISslCertificateHelper)

CAR_SINGLETON_IMPL(CSslCertificateHelper)

ECode CSslCertificateHelper::SaveState(
    /* [in] */ ISslCertificate* certificate,
    /* [out] */ IBundle** bundle)
{
    return CSslCertificate::SaveState(certificate, bundle);
}

ECode CSslCertificateHelper::RestoreState(
    /* [in] */ IBundle* bundle,
    /* [out] */ ISslCertificate** certificate)
{
    return CSslCertificate::RestoreState(bundle, certificate);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
