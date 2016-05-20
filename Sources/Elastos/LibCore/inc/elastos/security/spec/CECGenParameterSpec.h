
#ifndef __ELASTOS_SECURITY_SPEC_CECGENPARAMETERSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CECGENPARAMETERSPEC_H__

#include "_Elastos_Security_Spec_CECGenParameterSpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECGenParameterSpec)
    , public Object
    , public IECGenParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetName(
        /* [out] */ String *name);

    CARAPI constructor(
        /* [in] */ const String& name);

private:
    // Standard (or predefined) name for EC domain
    // parameters to be generated
    const String mName;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECGENPARAMETERSPEC_H__
