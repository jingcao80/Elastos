#ifndef __ELASTOS_DROID_UTILITY_INT32PROPERTY_H__
#define __ELASTOS_DROID_UTILITY_INT32PROPERTY_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Utility {

class ECO_PUBLIC Int32Property
    : public Object
    , public IInt32Property
    , public IProperty
{
public :
    CAR_INTERFACE_DECL()

    Int32Property(
        /* [in] */ const String& name);

    virtual ~Int32Property();

    CARAPI IsReadOnly(
        /* [out] */ Boolean* readOnly);

    CARAPI Set(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* value);

    CARAPI Get(
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** rst) = 0;

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetType(
        /* [out] */ ClassID* info);

    CARAPI IsWriteOnly(
        /* [out] */ Boolean* writeOnly);

    CARAPI ForbiddenOperate(
        /* [out] */ Boolean* forbidden);

    CARAPI SetValue(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 value) = 0;

private:
    String mName;
};

} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif
