
#include "CTestCarDataType.h"

#include <cutils/log.h>

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {


ECode CTestCarDataType::Test_NULL()
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    return NOERROR;
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
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %d ====", __FILE__, __FUNCTION__, *pInValue);
    *pOutValue = *pInValue;
    return NOERROR;
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

ECode CTestCarDataType::Test_Char8(
    /* [in] */ Char8 inValue,
    /* [out] */ Char8 * pOutValue)
{
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %c ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
}

ECode CTestCarDataType::Test_Char8_Ref(
    /* [in] */ Char8 * pInValue,
    /* [out] */ Char8 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Char16(
    /* [in] */ Char16 inValue,
    /* [out] */ Char16 * pOutValue)
{
    assert(pOutValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %c ====", __FILE__, __FUNCTION__, inValue);
    *pOutValue = inValue;
    return NOERROR;
}

ECode CTestCarDataType::Test_Char16_Ref(
    /* [in] */ Char16 * pInValue,
    /* [out] */ Char16 * pOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_CString(
    /* [in] */ CString inCStr,
    /* [out] */ CString * pOutCStr)
{
    assert(pOutCStr != NULL);
    ALOGD("==== File: %s, Function: %s, inCStr: %s ====", __FILE__, __FUNCTION__, (const char*)inCStr);
    *pOutCStr = inCStr;
    return NOERROR;
}

ECode CTestCarDataType::Test_CString_Ref(
    /* [in] */ CString * pInCStr,
    /* [out] */ CString * pOutCStr)
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
    /* [in] */ const String& * pInStr,
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
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(pOutId != NULL);
    *pOutId = inId;
    return NOERROR;
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

ECode CTestCarDataType::Test_StringBuf(
    /* [in] */ CString inValue,
    /* [out, callee] */ StringBuf ** ppOutValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    ALOGD("CTestCarDataType::Test_StringBuf inValue:%s",(const char*)inValue);

    assert(ppOutValue != NULL);
    *ppOutValue = StringBuf::Alloc(inValue.GetLength());
    (*ppOutValue)->Copy((const char*)inValue);

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Int16(
    /* [in] */ const ArrayOf<Int16> & inArrayOf,
    /* [out, callee] */ ArrayOf<Int16> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Int16> > carArray = ArrayOf<Int16>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
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
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Int32> > carArray = ArrayOf<Int32>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Int64(
    /* [in] */ const ArrayOf<Int64> & inArrayOf,
    /* [out, callee] */ ArrayOf<Int64> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Int64> > carArray = ArrayOf<Int64>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Byte(
    /* [in] */ const ArrayOf<Byte> & inArrayOf,
    /* [out, callee] */ ArrayOf<Byte> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Byte> > carArray = ArrayOf<Byte>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Float(
    /* [in] */ const ArrayOf<Float> & inArrayOf,
    /* [out, callee] */ ArrayOf<Float> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Float> > carArray = ArrayOf<Float>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Double(
    /* [in] */ const ArrayOf<Double> & inArrayOf,
    /* [out, callee] */ ArrayOf<Double> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Double> > carArray = ArrayOf<Double>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Char8(
    /* [in] */ const ArrayOf<Char8> & inArrayOf,
    /* [out, callee] */ ArrayOf<Char8> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Char8> > carArray = ArrayOf<Char8>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Char16(
    /* [in] */ const ArrayOf<Char16> & inArrayOf,
    /* [out, callee] */ ArrayOf<Char16> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Char16> > carArray = ArrayOf<Char16>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_CString(
    /* [in] */ const ArrayOf<CString> & inArrayOf,
    /* [out, callee] */ ArrayOf<CString> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<CString> > carArray = ArrayOf<CString>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_String(
    /* [in] */ const ArrayOf<String> & inArrayOf,
    /* [out, callee] */ ArrayOf<String> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<String> > carArray = ArrayOf<String>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Boolean(
    /* [in] */ const ArrayOf<Boolean> & inArrayOf,
    /* [out, callee] */ ArrayOf<Boolean> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<Boolean> > carArray = ArrayOf<Boolean>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_EMuid(
    /* [in] */ const ArrayOf<EMuid> & inArrayOf,
    /* [out, callee] */ ArrayOf<EMuid> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<EMuid> > carArray = ArrayOf<EMuid>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_EGuid(
    /* [in] */ const ArrayOf<EGuid> & inArrayOf,
    /* [out, callee] */ ArrayOf<EGuid> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<EGuid> > carArray = ArrayOf<EGuid>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_ECode(
    /* [in] */ const ArrayOf<ECode> & inArrayOf,
    /* [out, callee] */ ArrayOf<ECode> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<ECode> > carArray = ArrayOf<ECode>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_Enum(
    /* [in] */ const ArrayOf<CarDataType> & inArrayOf,
    /* [out, callee] */ ArrayOf<CarDataType> ** ppOutArrayOf)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<CarDataType> > carArray = ArrayOf<CarDataType>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();

    return NOERROR;
}

ECode CTestCarDataType::Test_ArrayOf_IInterface(
    /* [in] */ const ArrayOf<IInterface> & inArrayOf,
    /* [out, callee] */ ArrayOf<IInterface> ** ppOutArrayOf)
{
    /*
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);

    Int32 length = inArrayOf.GetLength();
    //ArrayOf<Int32>* carArray = ArrayOf<Int32>::Alloc(length);
    AutoPtr< ArrayOf<IInterface> > carArray = ArrayOf<IInterface>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        carArray->Set(i,inArrayOf[i]);
    }

    *ppOutArrayOf = carArray;
    if (*ppOutArrayOf != NULL) (*ppOutArrayOf)->AddRef();
    */
    return NOERROR;
}

ECode CTestCarDataType::Test_BufferOf_Int32(
    /* [in] */ const BufferOf<Int32> & inValue,
    /* [out, callee] */ BufferOf<Int32> ** ppOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_BufferOf_Int32_Ref(
    /* [in] */ BufferOf<Int32> * pInValue,
    /* [out, callee] */ BufferOf<Int32> ** ppOutValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_BufferOf_IInterface(
    /* [in] */ const BufferOf<IInterface> & inBufferOf,
    /* [out, callee] */ BufferOf<IInterface> ** ppOutBufferOf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_MemoryBuf(
    /* [in] */ const MemoryBuf & inMemoryBuf,
    /* [out, callee] */ MemoryBuf ** ppOutMemoryBuf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_MemoryBuf_Ref(
    /* [in] */ MemoryBuf * pInMemoryBuf,
    /* [out, callee] */ MemoryBuf ** ppOutMemoryBuf)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarDataType::Test_Interface(
    /* [in] */ IInterface * pInObject,
    /* [out] */ IInterface ** ppOutObject)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(ppOutObject != NULL);
    *ppOutObject = pInObject;
    if (*ppOutObject != NULL) (*ppOutObject)->AddRef();
    return NOERROR;
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

