#include "CApplicationOne.h"

namespace Elastos {
namespace DevSamples {
namespace ApplicationDemo {

CAR_INTERFACE_IMPL(CApplicationOne, Application, IApplicationOne)

CAR_OBJECT_IMPL(CApplicationOne)

ECode CApplicationOne::constructor()
{
    return NOERROR;
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