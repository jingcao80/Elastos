
#ifndef __CTESTVARIABLEOFCPPVECTOR_H__
#define __CTESTVARIABLEOFCPPVECTOR_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestVariableOfCppVector.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestVariableOfCppVector)
    , public Object
    , public ITestVariableOfCppVector
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetTypeInfo(
        /* [out] */ ITestDataTypeInfo ** ppTypeInfo);

    CARAPI GetPayload(
        /* [out] */ Int32 * pPayload);

    CARAPI Rebox(
        /* [in] */ Int32 * pLocalVariablePtr);

    CARAPI GetSetter(
        /* [out] */ ITestCppVectorSetter ** ppSetter);

    CARAPI GetGetter(
        /* [out] */ ITestCppVectorGetter ** ppGetter);

    CARAPI ZeroAllElements();

    CARAPI SetAllElements(
        /* [in] */ Int32 * pValue,
        /* [in] */ MemorySize size);

    CARAPI SetInt16Element(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI SetInt32Element(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetInt64Element(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI SetByteElement(
        /* [in] */ Int32 index,
        /* [in] */ Byte value);

    CARAPI SetFloatElement(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI SetDoubleElement(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI SetCharElement(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI SetBooleanElement(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    CARAPI SetEMuidElement(
        /* [in] */ Int32 index,
        /* [in] */ EMuid * pValue);

    CARAPI SetEGuidElement(
        /* [in] */ Int32 index,
        /* [in] */ EGuid * pValue);

    CARAPI SetECodeElement(
        /* [in] */ Int32 index,
        /* [in] */ ECode value);

    CARAPI SetLocalPtrElement(
        /* [in] */ Int32 index,
        /* [in] */ Int32 * pValue);

    CARAPI SetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [in] */ Int32 * pValue);

    CARAPI SetEnumElement(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI GetStructElementSetter(
        /* [in] */ Int32 index,
        /* [out] */ ITestStructSetter ** ppSetter);

    CARAPI GetCppVectorElementSetter(
        /* [in] */ Int32 index,
        /* [out] */ ITestCppVectorSetter ** ppSetter);

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetRank(
        /* [out] */ Int32 * pRank);

    CARAPI GetInt16Element(
        /* [in] */ Int32 index,
        /* [out] */ Int16 * pValue);

    CARAPI GetInt32Element(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetInt64Element(
        /* [in] */ Int32 index,
        /* [out] */ Int64 * pValue);

    CARAPI GetByteElement(
        /* [in] */ Int32 index,
        /* [out] */ Byte * pValue);

    CARAPI GetFloatElement(
        /* [in] */ Int32 index,
        /* [out] */ Float * pValue);

    CARAPI GetDoubleElement(
        /* [in] */ Int32 index,
        /* [out] */ Double * pValue);

    CARAPI GetCharElement(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

    CARAPI GetBooleanElement(
        /* [in] */ Int32 index,
        /* [out] */ Boolean * pValue);

    CARAPI GetEMuidElement(
        /* [in] */ Int32 index,
        /* [out] */ EMuid * pValue);

    CARAPI GetEGuidElement(
        /* [in] */ Int32 index,
        /* [out] */ EGuid * pValue);

    CARAPI GetECodeElement(
        /* [in] */ Int32 index,
        /* [out] */ ECode * pValue);

    CARAPI GetLocalPtrElement(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetEnumElement(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetStructElementGetter(
        /* [in] */ Int32 index,
        /* [out] */ ITestStructGetter ** ppGetter);

    CARAPI GetCppVectorElementGetter(
        /* [in] */ Int32 index,
        /* [out] */ ITestCppVectorGetter ** ppGetter);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICppVectorSetter * pCppVectorSetter);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IInterface> mVariableOfCppVector;
};

}
}
}
}

#endif // __CTESTVARIABLEOFCPPVECTOR_H__
