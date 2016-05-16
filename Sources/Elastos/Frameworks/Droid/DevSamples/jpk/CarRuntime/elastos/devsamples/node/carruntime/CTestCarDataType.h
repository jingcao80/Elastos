
#ifndef __CTESTCARDATATYPE_H__
#define __CTESTCARDATATYPE_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestCarDataType.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestCarDataType)
    , public Object
    , public ITestCarDataType
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Test_NULL();

    CARAPI Test_Int16(
        /* [in] */ Int16 inValue,
        /* [out] */ Int16 * pOutValue);

    CARAPI Test_Int16_Ref(
        /* [in] */ Int16 * pInValue,
        /* [out] */ Int16 * pOutValue);

    CARAPI Test_Int32(
        /* [in] */ Int32 inValue,
        /* [out] */ Int32 * pOutValue);

    CARAPI Test_Int32_Ref(
        /* [in] */ Int32 * pInValue,
        /* [out] */ Int32 * pOutValue);

    CARAPI Test_Int64(
        /* [in] */ Int64 inValue,
        /* [out] */ Int64 * pOutValue);

    CARAPI Test_Int64_Ref(
        /* [in] */ Int64 * pInValue,
        /* [out] */ Int64 * pOutValue);

    CARAPI Test_Byte(
        /* [in] */ Byte inValue,
        /* [out] */ Byte * pOutValue);

    CARAPI Test_Byte_Ref(
        /* [in] */ Byte * pInValue,
        /* [out] */ Byte * pOutValue);

    CARAPI Test_Float(
        /* [in] */ Float inValue,
        /* [out] */ Float * pOutValue);

    CARAPI Test_Float_Ref(
        /* [in] */ Float * pInValue,
        /* [out] */ Float * pOutValue);

    CARAPI Test_Double(
        /* [in] */ Double inValue,
        /* [out] */ Double * pOutValue);

    CARAPI Test_Double_Ref(
        /* [in] */ Double * pInValue,
        /* [out] */ Double * pOutValue);

    CARAPI Test_Char16(
        /* [in] */ Char16 inValue,
        /* [out] */ Char16 * pOutValue);

    CARAPI Test_Char16_Ref(
        /* [in] */ Char16 * pInValue,
        /* [out] */ Char16 * pOutValue);

    CARAPI Test_String(
        /* [in] */ const String& inStr,
        /* [out] */ String * pOutStr);

    CARAPI Test_String_Ref(
        /* [in] */ String * pInStr,
        /* [out] */ String * pOutStr);

    CARAPI Test_Boolean(
        /* [in] */ Boolean inValue,
        /* [out] */ Boolean * pOutValue);

    CARAPI Test_Boolean_Ref(
        /* [in] */ Boolean * pInValue,
        /* [out] */ Boolean * pOutValue);

    CARAPI Test_EMuid(
        /* [in] */ const EMuid & inId,
        /* [out] */ EMuid * pOutId);

    CARAPI Test_EMuid_Ref(
        /* [in] */ EMuid * pInId,
        /* [out] */ EMuid * pOutId);

    CARAPI Test_EGuid(
        /* [in] */ const EGuid & inId,
        /* [out] */ EGuid * pOutId);

    CARAPI Test_EGuid_Ref(
        /* [in] */ EGuid * pInId,
        /* [out] */ EGuid * pOutId);

    CARAPI Test_ECode(
        /* [in] */ ECode inValue,
        /* [out] */ ECode * pOutValue);

    CARAPI Test_ECode_Ref(
        /* [in] */ ECode * pInValue,
        /* [out] */ ECode * pOutValue);

    CARAPI Test_Enum(
        /* [in] */ CarDataType inValue,
        /* [out] */ CarDataType * pOutValue);

    CARAPI Test_Enum_Ref(
        /* [in] */ CarDataType * pInValue,
        /* [out] */ CarDataType * pOutValue);

    CARAPI Test_ArrayOf_Int16(
        /* [in] */ const ArrayOf<Int16> & inArrayOf,
        /* [out, callee] */ ArrayOf<Int16> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Ref(
        /* [in] */ ArrayOf<Int16> * pInArrayOf,
        /* [out, callee] */ ArrayOf<Int16> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Int32(
        /* [in] */ const ArrayOf<Int32> & inArrayOf,
        /* [out, callee] */ ArrayOf<Int32> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Int64(
        /* [in] */ const ArrayOf<Int64> & inArrayOf,
        /* [out, callee] */ ArrayOf<Int64> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Byte(
        /* [in] */ const ArrayOf<Byte> & inArrayOf,
        /* [out, callee] */ ArrayOf<Byte> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Float(
        /* [in] */ const ArrayOf<Float> & inArrayOf,
        /* [out, callee] */ ArrayOf<Float> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Double(
        /* [in] */ const ArrayOf<Double> & inArrayOf,
        /* [out, callee] */ ArrayOf<Double> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Char16(
        /* [in] */ const ArrayOf<Char16> & inArrayOf,
        /* [out, callee] */ ArrayOf<Char16> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_String(
        /* [in] */ const ArrayOf<String> & inArrayOf,
        /* [out, callee] */ ArrayOf<String> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Boolean(
        /* [in] */ const ArrayOf<Boolean> & inArrayOf,
        /* [out, callee] */ ArrayOf<Boolean> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_EMuid(
        /* [in] */ const ArrayOf<EMuid> & inArrayOf,
        /* [out, callee] */ ArrayOf<EMuid> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_EGuid(
        /* [in] */ const ArrayOf<EGuid> & inArrayOf,
        /* [out, callee] */ ArrayOf<EGuid> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_ECode(
        /* [in] */ const ArrayOf<ECode> & inArrayOf,
        /* [out, callee] */ ArrayOf<ECode> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_Enum(
        /* [in] */ const ArrayOf<CarDataType> & inArrayOf,
        /* [out, callee] */ ArrayOf<CarDataType> ** ppOutArrayOf);

    CARAPI Test_ArrayOf_IInterface(
        /* [in] */ const ArrayOf<IInterface*> & inArrayOf,
        /* [out, callee] */ ArrayOf<IInterface*> ** ppOutArrayOf);

    CARAPI Test_Interface(
        /* [in] */ IInterface * pInObject,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_Interface_Ref(
        /* [in] */ IInterface ** inObject,
        /* [out] */ IInterface ** ppOutObject);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CTESTCARDATATYPE_H__
