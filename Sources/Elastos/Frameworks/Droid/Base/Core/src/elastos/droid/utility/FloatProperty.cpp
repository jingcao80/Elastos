
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/FloatProperty.h"

using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL_2(FloatProperty, Object, IFloatProperty, IProperty)

FloatProperty::FloatProperty(
    /* [in] */ const String& name)
{
    mName = name;
}

FloatProperty::~FloatProperty()
{}

ECode FloatProperty::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly)
    *readOnly = FALSE;
    return NOERROR;
}

ECode FloatProperty::Set(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* value)
{
    if (IFloat::Probe(value) == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Float v;
    IFloat::Probe(value)->GetValue(&v);
    return SetValue(obj, v);
}

ECode FloatProperty::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode FloatProperty::GetType(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ECLSID_CFloat;
    return NOERROR;
}

ECode FloatProperty::IsWriteOnly(
    /* [out] */ Boolean* writeOnly)
{
    VALIDATE_NOT_NULL(writeOnly)
    *writeOnly = FALSE;
    return NOERROR;
}

ECode FloatProperty::ForbiddenOperate(
    /* [out] */ Boolean* forbidden)
{
    VALIDATE_NOT_NULL(forbidden)
    *forbidden = FALSE;
    return NOERROR;
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos
