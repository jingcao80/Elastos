#include "CApplicationOne.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ApplicationDemo {

static const String TAG("ApplicationDemo::CApplicationOne");

CAR_INTERFACE_IMPL(CApplicationOne, Application, IApplicationOne)

CAR_OBJECT_IMPL(CApplicationOne)

ECode CApplicationOne::constructor()
{
    Logger::I(TAG, " >>> create CApplicationOne: %s", TO_CSTR(this));
    return Application::constructor();
}

ECode CApplicationOne::OnCreate()
{
    SetValue(String("CustomAppcalition"));
    return Application::OnCreate();
}

ECode CApplicationOne::SetValue(
    /* [in] */ const String& value)
{
    mValue = value;
    return NOERROR;
}

ECode CApplicationOne::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}


} // namespace ApplicationDemo
} // namespace DevSamples
} // namespace Elastos