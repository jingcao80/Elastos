
#include <cmdef.h>
#include "CTestObject.h"
#include <cutils/log.h>

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

ECode CTestObject::_Thread::Run()
{
    ALOGD("==== File: %s, Line: %d, tid: %d ====", __FILE__, __LINE__, gettid());
    mListener->OnEvent2(String("Hello World"));
    return NOERROR;
}

ECode CTestObject::Test_NULL()
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    return NOERROR;
}

ECode CTestObject::Test_Int16(
    /* [in] */ Int16 inValue,
    /* [out] */ Int16* outValue)
{
    assert(outValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %d ====", __FILE__, __FUNCTION__, inValue);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_Int32(
    /* [in] */ Int32 inValue,
    /* [out] */ Int32* outValue)
{
    assert(outValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %d ====", __FILE__, __FUNCTION__, inValue);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_Int64(
    /* [in] */ Int64 inValue,
    /* [out] */ Int64* outValue)
{
    assert(outValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %lld ====", __FILE__, __FUNCTION__, inValue);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_Byte(
    /* [in] */ Byte inValue,
    /* [out] */ Byte* outValue)
{
    assert(outValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %d ====", __FILE__, __FUNCTION__, inValue);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_Float(
    /* [in] */ Float inValue,
    /* [out] */ Float* outValue)
{
    assert(outValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %f ====", __FILE__, __FUNCTION__, inValue);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_Double(
    /* [in] */ Double inValue,
    /* [out] */ Double* outValue)
{
    assert(outValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %f ====", __FILE__, __FUNCTION__, inValue);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_Char(
    /* [in] */ Char32 inValue,
    /* [out] */ Char32* outValue)
{
    assert(outValue != NULL);
    ALOGD("==== File: %s, Function: %s, inValue: %c ====", __FILE__, __FUNCTION__, inValue);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_String(
    /* [in] */ const String& inStr,
    /* [out] */ String* outStr)
{
    assert(outStr != NULL);
    ALOGD("==== File: %s, Function: %s, inStr: %s ====", __FILE__, __FUNCTION__, inStr.string());
    *outStr = inStr;
    return NOERROR;
}

ECode CTestObject::Test_Boolean(
    /* [in] */ Boolean inValue,
    /* [out] */ Boolean* outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outValue != NULL);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_EMuid(
    /* [in] */ const EMuid& inId,
    /* [out] */ EMuid* outId)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outId != NULL);
    *outId = inId;
    return NOERROR;
}

ECode CTestObject::Test_EGuid(
    /* [in] */ const EGuid& inId,
    /* [out] */ EGuid* outId)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outId != NULL);
    *outId = inId;
    return NOERROR;
}

ECode CTestObject::Test_ECode(
    /* [in] */ ECode inValue,
    /* [out] */ ECode* outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outValue != NULL);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_Enum(
    /* [in] */ TestObjectState inValue,
    /* [out] */ TestObjectState * outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outValue != NULL);
    *outValue = inValue;
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Int16(
    /* [in] */ const ArrayOf<Int16>& inValue,
    /* [out, callee] */ ArrayOf<Int16>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Int16> > carArray = ArrayOf<Int16>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Int32(
    /* [in] */ const ArrayOf<Int32>& inValue,
    /* [out, callee] */ ArrayOf<Int32>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Int32> > carArray = ArrayOf<Int32>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Int64(
    /* [in] */ const ArrayOf<Int64>& inValue,
    /* [out, callee] */ ArrayOf<Int64>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Int64> > carArray = ArrayOf<Int64>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Byte(
    /* [in] */ const ArrayOf<Byte>& inValue,
    /* [out, callee] */ ArrayOf<Byte>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Byte> > carArray = ArrayOf<Byte>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Float(
    /* [in] */ const ArrayOf<Float>& inValue,
    /* [out, callee] */ ArrayOf<Float>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Float> > carArray = ArrayOf<Float>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Double(
    /* [in] */ const ArrayOf<Double>& inValue,
    /* [out, callee] */ ArrayOf<Double>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Double> > carArray = ArrayOf<Double>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Char(
    /* [in] */ const ArrayOf<Char32>& inValue,
    /* [out, callee] */ ArrayOf<Char32>** outValue)
{
    ALOGD("==== File: %s, Function: %s, inValue[0]: %d, inValue[1]: %d, inValue[2]: %d ====", __FILE__, __FUNCTION__,
            inValue[0], inValue[1], inValue[2]);
    AutoPtr< ArrayOf<Char32> > carArray = ArrayOf<Char32>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_String(
    /* [in] */ const ArrayOf<String>& inValue,
    /* [out, callee] */ ArrayOf<String>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<String> > carArray = ArrayOf<String>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Boolean(
    /* [in] */ const ArrayOf<Boolean>& inValue,
    /* [out, callee] */ ArrayOf<Boolean>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<Boolean> > carArray = ArrayOf<Boolean>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_EMuid(
    /* [in] */ const ArrayOf<EMuid>& inValue,
    /* [out, callee] */ ArrayOf<EMuid>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<EMuid> > carArray = ArrayOf<EMuid>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_EGuid(
    /* [in] */ const ArrayOf<EGuid>& inValue,
    /* [out, callee] */ ArrayOf<EGuid>** outValue)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    AutoPtr< ArrayOf<EGuid> > carArray = ArrayOf<EGuid>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_ECode(
    /* [in] */ const ArrayOf<ECode>& inValue,
    /* [out, callee] */ ArrayOf<ECode>** outValue)
{
    ALOGD("==== File: %s, Function: %s, inValue[0]: 0x%08x, inValue[1]: 0x%08x, inValue[2]: 0x%08x ====", __FILE__, __FUNCTION__,
            inValue[0], inValue[1], inValue[2]);
    AutoPtr< ArrayOf<ECode> > carArray = ArrayOf<ECode>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_Enum(
    /* [in] */ const ArrayOf<TestObjectState>& inValue,
    /* [out, callee] */ ArrayOf<TestObjectState>** outValue)
{
    ALOGD("==== File: %s, Function: %s, inValue[0]: %d, inValue[1]: %d, inValue[2]: %d ====", __FILE__, __FUNCTION__,
            inValue[0], inValue[1], inValue[2]);
    AutoPtr< ArrayOf<TestObjectState> > carArray = ArrayOf<TestObjectState>::Alloc(inValue.GetLength());
    carArray->Copy(&inValue);
    *outValue = carArray;
    REFCOUNT_ADD(*outValue);
    return NOERROR;
}

ECode CTestObject::Test_ArrayOf_IInterface(
    /* [in] */ const ArrayOf<IInterface*>& inValue,
    /* [out, callee] */ ArrayOf<IInterface*>** outValue)
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

ECode CTestObject::Test_Interface(
    /* [in] */ IInterface* inObject,
    /* [out] */ IInterface** outObject)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outObject != NULL);
    *outObject = inObject;
    REFCOUNT_ADD(*outObject);
    return NOERROR;
}

ECode CTestObject::Test_CreateInstance(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface** object)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(object != NULL);

    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = _CReflector_AcquireModuleInfo(
            moduleName.string(), (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    AutoPtr<IClassInfo> classInfo;
    ec = moduleInfo->GetClassInfo(
            className.string(), (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" class info failed!\n", className.string());
        return ec;
    }

    AutoPtr<IInterface> testObject;
    ec = classInfo->CreateObject((IInterface**)&testObject);
    if (FAILED(ec)) {
        ALOGD("Create object failed!\n");
        return ec;
    }

    *object = testObject;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CTestObject::Test_AddEventListener(
    /* [in] */ ITestEventListener* listener)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    listener->OnEvent1(9);
    AutoPtr<_Thread> t = new _Thread(listener);
    t->Start();
    return NOERROR;
}

}
}
}
}

