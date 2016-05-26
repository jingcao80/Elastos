#include "CGetSystemTimeService.h"
#include <Elastos.Droid.Text.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::CTime;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

CAR_INTERFACE_IMPL_2(CGetSystemTimeService, Object, IGetSystemTimeService, IBinder)

CAR_OBJECT_IMPL(CGetSystemTimeService)

ECode CGetSystemTimeService::constructor()
{
    return NOERROR;
}

ECode CGetSystemTimeService::GetSystemTime(
    /* [out] */ String* timeStr)
{
    VALIDATE_NOT_NULL(timeStr)
    AutoPtr<ITime> t;
    CTime::New((ITime**)&t);
    t->SetToNow();
    *timeStr = Object::ToString(t);
    return NOERROR;
}

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos