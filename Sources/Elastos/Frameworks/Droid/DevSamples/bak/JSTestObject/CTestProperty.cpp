
#include "CTestProperty.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {


ECode CTestProperty::GetProperty(
    /* [out] */ IInterface ** ppProperty)
{
    *ppProperty = reinterpret_cast< IInterface* >(mProperty);
    return NOERROR;
}

ECode CTestProperty::SetProperty(
    /* [in] */ IInterface * pProperty)
{
    memcpy(mProperty,pProperty,sizeof(StructTestProperty));
    return NOERROR;
}

ECode CTestProperty::TestMethod_00()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestProperty::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestProperty::constructor(
    /* [in] */ IInterface * pProperty)
{
    *(IInterface**)&mProperty = pProperty;
    return NOERROR;
}

}
}
}
}

