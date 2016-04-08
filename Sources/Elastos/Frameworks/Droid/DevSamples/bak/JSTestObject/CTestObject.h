
#ifndef __CTESTOBJECT_H__
#define __CTESTOBJECT_H__

#include "_CTestObject.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestObject)
{
public:
    CARAPI Test_Require(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** outObject);

    CARAPI Test_Require_ModuleInfo(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** outObject);

    CARAPI Test_Require_ClassInfo(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** outObject);
};

}
}
}
}

#endif // __CTESTOBJECT_H__
