
#include "CSample.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

ECode CSample::constructor(
    /* [in] */ IActivity* host,
    /* [in] */ Int32 titleResId,
    /* [in] */ IClassInfo* activityClass)
{
    mHost = (CMainActivity*)host;
    mActivityClass = activityClass;
    AutoPtr<IResources> res;
    mHost->GetResources((IResources**)&res);
    return res->GetString(titleResId, &mTitle);
}

CSample::~CSample()
{
Slogger::D("CMainActivity", "Sample==================~~~~~~~~~~=================1");
}

ECode CSample::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mTitle;
    return NOERROR;
}

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
