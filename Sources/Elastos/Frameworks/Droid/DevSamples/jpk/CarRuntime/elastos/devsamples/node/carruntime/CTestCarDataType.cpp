
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
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %d ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
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
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %lld ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
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
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %d ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
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
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %f ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
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
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %f ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
}

ECode CTestCarDataType::Test_Double_Ref(
    /* [in] */ Double * pInValue,
    /* [out] */ Double * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Char(
    /* [in] */ Char32 inValue,
    /* [out] */ Char32 * pOutValue)
{
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %c ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
}

ECode CTestCarDataType::Test_Char_Ref(
    /* [in] */ Char32 * pInValue,
    /* [out] */ Char32 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_String(
    /* [in] */ const String& inStr,
    /* [out] */ String * pOutStr)
{
    assert(pOutStr != NULL);
    ALOGD("==== File: %s, Function: %s, inStr: %s ====", __FILE__, __FUNCTION__, inStr.string());
    *pOutStr = inStr;
    return NOERROR;
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
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(pOutValue != NULL);
    *pOutValue = inValue;
    return NOERROR;
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
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(pOutId != NULL);
    *pOutId = inId;
    return NOERROR;
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
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(pOutValue != NULL);
    *pOutValue = inValue;
    return NOERROR;
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
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(pOutValue != NULL);
    *pOutValue = inValue;
    return NOERROR;
}

ECode CTestCarDataType::Test_Enum_Ref(
    /* [in] */ CarDataType * pInValue,
    /* [out] */ CarDataType * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Int16(
    /* [in] */ const ArrayOf<Int16> & inValue,
    /* [out, callee] */ ArrayOf<Int16> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Int16> > carArray = ArrayOf<Int16>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Ref(
    /* [in] */ ArrayOf<Int16> * pInValue,
    /* [out, callee] */ ArrayOf<Int16> ** ppOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_ArrayOf_Int32(
    /* [in] */ const ArrayOf<Int32> & inValue,
    /* [out, callee] */ ArrayOf<Int32> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Int32> > carArray = ArrayOf<Int32>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Int64(
    /* [in] */ const ArrayOf<Int64> & inValue,
    /* [out, callee] */ ArrayOf<Int64> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Int64> > carArray = ArrayOf<Int64>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Byte(
    /* [in] */ const ArrayOf<Byte> & inValue,
    /* [out, callee] */ ArrayOf<Byte> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Byte> > carArray = ArrayOf<Byte>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Float(
    /* [in] */ const ArrayOf<Float> & inValue,
    /* [out, callee] */ ArrayOf<Float> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Float> > carArray = ArrayOf<Float>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Double(
    /* [in] */ const ArrayOf<Double> & inValue,
    /* [out, callee] */ ArrayOf<Double> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Double> > carArray = ArrayOf<Double>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Char(
    /* [in] */ const ArrayOf<Char32> & inValue,
    /* [out, callee] */ ArrayOf<Char32> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s, inValue[0]: %d, inValue[1]: %d, inValue[2]: %d ====", __FILE__, __FUNCTION__,
            inValue[0], inValue[1], inValue[2]);
    AutoPtr< ArrayOf<Char32> > carArray = ArrayOf<Char32>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_String(
    /* [in] */ const ArrayOf<String> & inValue,
    /* [out, callee] */ ArrayOf<String> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<String> > carArray = ArrayOf<String>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Boolean(
    /* [in] */ const ArrayOf<Boolean> & inValue,
    /* [out, callee] */ ArrayOf<Boolean> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Boolean> > carArray = ArrayOf<Boolean>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_EMuid(
    /* [in] */ const ArrayOf<EMuid> & inValue,
    /* [out, callee] */ ArrayOf<EMuid> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<EMuid> > carArray = ArrayOf<EMuid>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_EGuid(
    /* [in] */ const ArrayOf<EGuid> & inValue,
    /* [out, callee] */ ArrayOf<EGuid> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<EGuid> > carArray = ArrayOf<EGuid>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_ECode(
    /* [in] */ const ArrayOf<ECode> & inValue,
    /* [out, callee] */ ArrayOf<ECode> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s, inValue[0]: 0x%08x, inValue[1]: 0x%08x, inValue[2]: 0x%08x ====", __FILE__, __FUNCTION__,
            inValue[0], inValue[1], inValue[2]);
    AutoPtr< ArrayOf<ECode> > carArray = ArrayOf<ECode>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Enum(
    /* [in] */ const ArrayOf<CarDataType> & inValue,
    /* [out, callee] */ ArrayOf<CarDataType> ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s, inValue[0]: %d, inValue[1]: %d, inValue[2]: %d ====", __FILE__, __FUNCTION__,
            inValue[0], inValue[1], inValue[2]);
    AutoPtr< ArrayOf<TestObjectState> > carArray = ArrayOf<TestObjectState>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *ppOutValue = carArray;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_IInterface(
    /* [in] */ const ArrayOf<IInterface*> & inValue,
    /* [out, callee] */ ArrayOf<IInterface*> ** ppOutValue)
{
    /*
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inValue.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<IInterface> > carArray = ArrayOf<IInterface>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inValue[i]);
    }

    *outValue = carArray;
    if (*outValue != NULL) (*outValue)->AddRef();
    */
    return NOERROR;
}

ECode CTestCarDataType::Test_Interface(
    /* [in] */ IInterface * pInValue,
    /* [out] */ IInterface ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(ppOutValue != NULL);
    *ppOutValue = pInValue;
    REFCOUNT_ADD(*ppOutValue);
    return NOERROR;
}

ECode CTestCarDataType::Test_Interface_Ref(
    /* [in] */ IInterface ** inValue,
    /* [out] */ IInterface ** ppOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

