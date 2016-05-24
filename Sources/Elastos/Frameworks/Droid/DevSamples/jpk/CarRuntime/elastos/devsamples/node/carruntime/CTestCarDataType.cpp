
#include "CTestCarDataType.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestCarDataType, Object, ITestCarDataType)

CAR_OBJECT_IMPL(CTestCarDataType)

ECode CTestCarDataType::Test_NULL()
{
    ALOGD("CTestCarDataType::Test_NULL begin");
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Int16(
    /* [in] */ Int16 inValue,
    /* [out] */ Int16 * pOutValue)
{
    // TODO: Add your code here
    //return E_NOT_IMPLEMENTED;

    ALOGD("CTestCarDataType::Test_Int16 begin");

    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %d ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
}

ECode CTestCarDataType::Test_Int16_Ref(
    /* [in] */ Int16 * pInValue,
    /* [out] */ Int16 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Int32(
    /* [in] */ Int32 inValue,
    /* [out] */ Int32 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Int32_Ref(
    /* [in] */ Int32 * pInValue,
    /* [out] */ Int32 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Int64(
    /* [in] */ Int64 inValue,
    /* [out] */ Int64 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Int64_Ref(
    /* [in] */ Int64 * pInValue,
    /* [out] */ Int64 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Byte(
    /* [in] */ Byte inValue,
    /* [out] */ Byte * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Byte_Ref(
    /* [in] */ Byte * pInValue,
    /* [out] */ Byte * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Float(
    /* [in] */ Float inValue,
    /* [out] */ Float * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Float_Ref(
    /* [in] */ Float * pInValue,
    /* [out] */ Float * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Double(
    /* [in] */ Double inValue,
    /* [out] */ Double * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Double_Ref(
    /* [in] */ Double * pInValue,
    /* [out] */ Double * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Char16(
    /* [in] */ Char16 inValue,
    /* [out] */ Char16 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Char16_Ref(
    /* [in] */ Char16 * pInValue,
    /* [out] */ Char16 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_String(
    /* [in] */ const String& inStr,
    /* [out] */ String * pOutStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_String_Ref(
    /* [in] */ String * pInStr,
    /* [out] */ String * pOutStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Boolean(
    /* [in] */ Boolean inValue,
    /* [out] */ Boolean * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Boolean_Ref(
    /* [in] */ Boolean * pInValue,
    /* [out] */ Boolean * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_EMuid(
    /* [in] */ const EMuid & inId,
    /* [out] */ EMuid * pOutId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_EMuid_Ref(
    /* [in] */ EMuid * pInId,
    /* [out] */ EMuid * pOutId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_EGuid(
    /* [in] */ const EGuid & inId,
    /* [out] */ EGuid * pOutId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_EGuid_Ref(
    /* [in] */ EGuid * pInId,
    /* [out] */ EGuid * pOutId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ECode(
    /* [in] */ ECode inValue,
    /* [out] */ ECode * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ECode_Ref(
    /* [in] */ ECode * pInValue,
    /* [out] */ ECode * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Enum(
    /* [in] */ CarDataType inValue,
    /* [out] */ CarDataType * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Enum_Ref(
    /* [in] */ CarDataType * pInValue,
    /* [out] */ CarDataType * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Int16(
    /* [in] */ const ArrayOf<Int16> & inArrayOf,
    /* [out, callee] */ ArrayOf<Int16> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Ref(
    /* [in] */ ArrayOf<Int16> * pInArrayOf,
    /* [out, callee] */ ArrayOf<Int16> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Int32(
    /* [in] */ const ArrayOf<Int32> & inArrayOf,
    /* [out, callee] */ ArrayOf<Int32> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Int64(
    /* [in] */ const ArrayOf<Int64> & inArrayOf,
    /* [out, callee] */ ArrayOf<Int64> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Byte(
    /* [in] */ const ArrayOf<Byte> & inArrayOf,
    /* [out, callee] */ ArrayOf<Byte> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Float(
    /* [in] */ const ArrayOf<Float> & inArrayOf,
    /* [out, callee] */ ArrayOf<Float> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Double(
    /* [in] */ const ArrayOf<Double> & inArrayOf,
    /* [out, callee] */ ArrayOf<Double> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Char16(
    /* [in] */ const ArrayOf<Char16> & inArrayOf,
    /* [out, callee] */ ArrayOf<Char16> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_String(
    /* [in] */ const ArrayOf<String> & inArrayOf,
    /* [out, callee] */ ArrayOf<String> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Boolean(
    /* [in] */ const ArrayOf<Boolean> & inArrayOf,
    /* [out, callee] */ ArrayOf<Boolean> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_EMuid(
    /* [in] */ const ArrayOf<EMuid> & inArrayOf,
    /* [out, callee] */ ArrayOf<EMuid> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_EGuid(
    /* [in] */ const ArrayOf<EGuid> & inArrayOf,
    /* [out, callee] */ ArrayOf<EGuid> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_ECode(
    /* [in] */ const ArrayOf<ECode> & inArrayOf,
    /* [out, callee] */ ArrayOf<ECode> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Enum(
    /* [in] */ const ArrayOf<CarDataType> & inArrayOf,
    /* [out, callee] */ ArrayOf<CarDataType> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_IInterface(
    /* [in] */ const ArrayOf<IInterface*> & inArrayOf,
    /* [out, callee] */ ArrayOf<IInterface*> ** ppOutArrayOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Interface(
    /* [in] */ IInterface * pInObject,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Interface_Ref(
    /* [in] */ IInterface ** inObject,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

