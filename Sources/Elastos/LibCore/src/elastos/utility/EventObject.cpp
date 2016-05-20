
#include "EventObject.h"
#include "StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::IO::EIID_ISerializable;

namespace Elastos{
namespace Utility{

CAR_INTERFACE_IMPL_2(EventObject, Object, IEventObject, ISerializable)

EventObject::EventObject()
{
}

EventObject::~EventObject()
{
}

ECode EventObject::constructor(
    /* [in] */ IObject* source)
{
    VALIDATE_NOT_NULL(source)
    mSource = source;
    return NOERROR;
}

ECode EventObject::GetSource(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = mSource;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode EventObject::ToString(
    /* [in] */ String* str)
{
    // return getClass().getName() + "[source=" + source + ']';
    VALIDATE_NOT_NULL(str);
    *str = String("EventObject:");
    *str += "[source=";
    *str += Object::ToString(mSource);
    *str += ']';
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
