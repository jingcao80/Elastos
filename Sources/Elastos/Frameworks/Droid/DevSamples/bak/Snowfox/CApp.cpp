
#include "CApp.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

ECode CApp::GetPackageName(
    /* [out] */ String* pPackageName)
{
    if (!pPackageName) return E_INVALID_ARGUMENT;
    *pPackageName = "org.mozilla.fennec_unofficial";
    return NOERROR;
}

ECode CApp::GetContentProcessName(
    /* [out] */ String* pProcessName)
{
    if (!pProcessName) return E_INVALID_ARGUMENT;
    *pProcessName = "plugin-container";
    return NOERROR;
}

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
