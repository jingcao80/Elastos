
#ifndef __CTESTARGUMENTLIST_H__
#define __CTESTARGUMENTLIST_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestArgumentList.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestArgumentList)
    , public Object
    , public ITestArgumentList
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetFunctionInfo(
        /* [out] */ ITestFunctionInfo ** ppFunctionInfo);

    CARAPI SetInputArgumentOfInt16(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI SetInputArgumentOfInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetInputArgumentOfInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI SetInputArgumentOfByte(
        /* [in] */ Int32 index,
        /* [in] */ Byte value);

    CARAPI SetInputArgumentOfFloat(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI SetInputArgumentOfDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI SetInputArgumentOfChar(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI SetInputArgumentOfString(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

    CARAPI SetInputArgumentOfBoolean(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    CARAPI SetInputArgumentOfEMuid(
        /* [in] */ Int32 index,
        /* [in] */ EMuid * pValue);

    CARAPI SetInputArgumentOfEGuid(
        /* [in] */ Int32 index,
        /* [in] */ EGuid * pValue);

    CARAPI SetInputArgumentOfECode(
        /* [in] */ Int32 index,
        /* [in] */ ECode value);

    CARAPI SetInputArgumentOfLocalPtr(
        /* [in] */ Int32* pLocalPtrType,
        /* [in] */ Int32 index,
        ///* [in] */ Int32 * pValue);
        /* [in] */ Int32 value);

    CARAPI SetInputArgumentOfLocalType(
        /* [in] */ Int32 index,
        /* [in] */ Int32 * pValue);

    CARAPI SetInputArgumentOfEnum(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetInputArgumentOfCarArray(
        /* [in] */ Int32 index,
        /* [in] */ PCarQuintet pValue);

    CARAPI SetInputArgumentOfStructPtr(
        /* [in] */ Int32 index,
        /* [in] */ Int32 * pValue);

    CARAPI SetInputArgumentOfObjectPtr(
        /* [in] */ Int32 index,
        /* [in] */ PInterface pValue);

    CARAPI SetOutputArgumentOfInt16Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int16 * pValue);

    CARAPI SetOutputArgumentOfInt32Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfInt64Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int64 * pValue);

    CARAPI SetOutputArgumentOfBytePtr(
        /* [in] */ Int32 index,
        /* [out] */ Byte * pValue);

    CARAPI SetOutputArgumentOfFloatPtr(
        /* [in] */ Int32 index,
        /* [out] */ Float * pValue);

    CARAPI SetOutputArgumentOfDoublePtr(
        /* [in] */ Int32 index,
        /* [out] */ Double * pValue);

    CARAPI SetOutputArgumentOfCharPtr(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

    CARAPI SetOutputArgumentOfStringPtr(
        /* [in] */ Int32 index,
        /* [out] */ String * pValue);

    CARAPI SetOutputArgumentOfBooleanPtr(
        /* [in] */ Int32 index,
        /* [out] */ Boolean * pValue);

    CARAPI SetOutputArgumentOfEMuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EMuid * pValue);

    CARAPI SetOutputArgumentOfEGuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EGuid * pValue);

    CARAPI SetOutputArgumentOfECodePtr(
        /* [in] */ Int32 index,
        /* [out] */ ECode * pValue);

    CARAPI SetOutputArgumentOfLocalPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfLocalTypePtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfEnumPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfCarArrayPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfCarArrayPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfStructPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfStructPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfObjectPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PInterface * ppValue);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IArgumentList * pArgumentList);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IArgumentList> mArgumentList;
};

}
}
}
}

#endif // __CTESTARGUMENTLIST_H__
