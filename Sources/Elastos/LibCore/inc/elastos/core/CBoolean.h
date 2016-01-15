
#ifndef __ELASTOS_CORE_CBOOLEAN_H__
#define __ELASTOS_CORE_CBOOLEAN_H__

#include "_Elastos_Core_CBoolean.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Core {

CarClass(CBoolean)
    , public Object
    , public IBoolean
    , public ISerializable
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Boolean value);

    CARAPI GetValue(
        /* [out] */ Boolean* result);

    CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Boolean mValue;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CBOOLEAN_H__
