
#include "elastos/droid/internal/http/CHttpDateTime.h"
#include "elastos/droid/internal/http/HttpDateTime.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {

CAR_INTERFACE_IMPL(CHttpDateTime, Singleton, IHttpDateTime)

CAR_SINGLETON_IMPL(CHttpDateTime)

ECode CHttpDateTime::Parse(
    /* [in] */ const String& timeString,
    /* [out] */ Int64* time)
{
    return HttpDateTime::Parse(timeString, time);
}

} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
