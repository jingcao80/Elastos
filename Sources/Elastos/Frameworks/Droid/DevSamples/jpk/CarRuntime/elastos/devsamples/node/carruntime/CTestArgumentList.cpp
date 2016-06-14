
#include "CTestArgumentList.h"

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestArgumentList, Object, ITestArgumentList)

CAR_OBJECT_IMPL(CTestArgumentList)

ECode CTestArgumentList::GetFunctionInfo(
    /* [out] */ ITestFunctionInfo ** ppFunctionInfo)
{
    return mArgumentList->GetFunctionInfo((IFunctionInfo**)ppFunctionInfo);
}

ECode CTestArgumentList::SetInputArgumentOfInt16(
    /* [in] */ Int32 index,
    /* [in] */ Int16 value)
{
    return mArgumentList->SetInputArgumentOfInt16(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfInt32(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return mArgumentList->SetInputArgumentOfInt32(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfInt64(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return mArgumentList->SetInputArgumentOfInt64(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfByte(
    /* [in] */ Int32 index,
    /* [in] */ Byte value)
{
    return mArgumentList->SetInputArgumentOfByte(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfFloat(
    /* [in] */ Int32 index,
    /* [in] */ Float value)
{
    return mArgumentList->SetInputArgumentOfFloat(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfDouble(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return mArgumentList->SetInputArgumentOfDouble(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfChar(
    /* [in] */ Int32 index,
    /* [in] */ Char32 value)
{
    return mArgumentList->SetInputArgumentOfChar(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfString(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    String* tt = new String(value);
    return mArgumentList->SetInputArgumentOfString(index,*tt);
}

ECode CTestArgumentList::SetInputArgumentOfBoolean(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    return mArgumentList->SetInputArgumentOfBoolean(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfEMuid(
    /* [in] */ Int32 index,
    /* [in] */ EMuid * pValue)
{
    return mArgumentList->SetInputArgumentOfEMuid(index, pValue);
}

ECode CTestArgumentList::SetInputArgumentOfEGuid(
    /* [in] */ Int32 index,
    /* [in] */ EGuid * pValue)
{
    return mArgumentList->SetInputArgumentOfEGuid(index, pValue);
}

ECode CTestArgumentList::SetInputArgumentOfECode(
    /* [in] */ Int32 index,
    /* [in] */ ECode value)
{
    return mArgumentList->SetInputArgumentOfECode(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfLocalPtr(
    /* [in] */ Int32* pLocalPtrType,
    /* [in] */ Int32 index,
    ///* [in] */ Int32 * pValue)
    /* [in] */ Int32 value)
{
    ECode ec = mArgumentList->SetInputArgumentOfLocalPtr(index, (LocalPtr)value);

    if (FAILED(ec)) {
        ALOGD("==============CTestArgumentList::SetInputArgumentOfLocalPtr.Error============ECode:%x", ec);
    }

    return ec;
}

ECode CTestArgumentList::SetInputArgumentOfLocalType(
    /* [in] */ Int32 index,
    /* [in] */ Int32 * pValue)
{
    return mArgumentList->SetInputArgumentOfLocalType(index, pValue);
}

ECode CTestArgumentList::SetInputArgumentOfEnum(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return mArgumentList->SetInputArgumentOfEnum(index, value);
}

ECode CTestArgumentList::SetInputArgumentOfCarArray(
    /* [in] */ Int32 index,
    /* [in] */ PCarQuintet pValue)
{
    return mArgumentList->SetInputArgumentOfCarArray(index, pValue);
}

ECode CTestArgumentList::SetInputArgumentOfStructPtr(
    /* [in] */ Int32 index,
    /* [in] */ Int32 * pValue)
{
    return mArgumentList->SetInputArgumentOfStructPtr(index, pValue);
}

ECode CTestArgumentList::SetInputArgumentOfObjectPtr(
    /* [in] */ Int32 index,
    /* [in] */ PInterface pValue)
{
    return mArgumentList->SetInputArgumentOfObjectPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfInt16Ptr(
    /* [in] */ Int32 index,
    /* [out] */ Int16 * pValue)
{
    return mArgumentList->SetOutputArgumentOfInt16Ptr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfInt32Ptr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfInt32Ptr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfInt64Ptr(
    /* [in] */ Int32 index,
    /* [out] */ Int64 * pValue)
{
    return mArgumentList->SetOutputArgumentOfInt64Ptr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfBytePtr(
    /* [in] */ Int32 index,
    /* [out] */ Byte * pValue)
{
    return mArgumentList->SetOutputArgumentOfBytePtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfFloatPtr(
    /* [in] */ Int32 index,
    /* [out] */ Float * pValue)
{
    return mArgumentList->SetOutputArgumentOfFloatPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfDoublePtr(
    /* [in] */ Int32 index,
    /* [out] */ Double * pValue)
{
    return mArgumentList->SetOutputArgumentOfDoublePtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfCharPtr(
    /* [in] */ Int32 index,
    /* [out] */ Char32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfCharPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfStringPtr(
    /* [in] */ Int32 index,
    /* [out] */ String * pValue)
{
    return mArgumentList->SetOutputArgumentOfStringPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfBooleanPtr(
    /* [in] */ Int32 index,
    /* [out] */ Boolean * pValue)
{
    return mArgumentList->SetOutputArgumentOfBooleanPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfEMuidPtr(
    /* [in] */ Int32 index,
    /* [out] */ EMuid * pValue)
{
    return mArgumentList->SetOutputArgumentOfEMuidPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfEGuidPtr(
    /* [in] */ Int32 index,
    /* [out] */ EGuid * pValue)
{
    return mArgumentList->SetOutputArgumentOfEGuidPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfECodePtr(
    /* [in] */ Int32 index,
    /* [out] */ ECode * pValue)
{
    return mArgumentList->SetOutputArgumentOfECodePtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfLocalPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfLocalPtrPtr(index,(void**)pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfLocalTypePtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfLocalTypePtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfEnumPtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfEnumPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfCarArrayPtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfCarArrayPtr(index,(PCarQuintet)pValue);
    //return E_NOT_IMPLEMENTED;
}

ECode CTestArgumentList::SetOutputArgumentOfCarArrayPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfCarArrayPtrPtr(index,(PCarQuintet*)pValue);
    //return E_NOT_IMPLEMENTED;
}

ECode CTestArgumentList::SetOutputArgumentOfStructPtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfStructPtr(index,pValue);
}

ECode CTestArgumentList::SetOutputArgumentOfStructPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return mArgumentList->SetOutputArgumentOfStructPtrPtr(index,(PVoid*)pValue);
    //return E_NOT_IMPLEMENTED;
}

ECode CTestArgumentList::SetOutputArgumentOfObjectPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ PInterface * ppValue)
{
    return mArgumentList->SetOutputArgumentOfObjectPtrPtr(index,ppValue);
}

ECode CTestArgumentList::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestArgumentList::constructor(
    /* [in] */ IArgumentList * pArgumentList)
{
    mArgumentList = pArgumentList;

    //mArgumentList->AddRef();

    return NOERROR;
}

ECode CTestArgumentList::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mArgumentList;

    return NOERROR;
}

}
}
}
}

