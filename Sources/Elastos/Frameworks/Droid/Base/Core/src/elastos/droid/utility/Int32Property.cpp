
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/Int32Property.h"

using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL_2(Int32Property, Object, IInt32Property, IProperty)

Int32Property::Int32Property(
    /* [in] */ const String& name)
{
    mName = name;
}

Int32Property::~Int32Property()
{}

ECode Int32Property::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly)
    *readOnly = FALSE;
    return NOERROR;
}

ECode Int32Property::Set(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* value)
{
    IInteger32* iobj = IInteger32::Probe(value);
    if (iobj == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 v;
    iobj->GetValue(&v);
    return SetValue(obj, v);
}

ECode Int32Property::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode Int32Property::GetType(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ECLSID_CInteger32;
    return NOERROR;
}

ECode Int32Property::IsWriteOnly(
    /* [out] */ Boolean* writeOnly)
{
    VALIDATE_NOT_NULL(writeOnly)
    *writeOnly = FALSE;
    return NOERROR;
}

ECode Int32Property::ForbiddenOperate(
    /* [out] */ Boolean* forbidden)
{
    VALIDATE_NOT_NULL(forbidden)
    *forbidden = FALSE;
    return NOERROR;
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos
