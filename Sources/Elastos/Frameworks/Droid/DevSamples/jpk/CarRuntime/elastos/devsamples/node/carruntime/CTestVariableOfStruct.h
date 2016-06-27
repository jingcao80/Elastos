
#ifndef __CTESTVARIABLEOFSTRUCT_H__
#define __CTESTVARIABLEOFSTRUCT_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestVariableOfStruct.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestVariableOfStruct)
    , public Object
    , public ITestVariableOfStruct
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
        /* [out] */ ITestStructSetter ** ppSetter);

    CARAPI GetGetter(
        /* [out] */ ITestStructGetter ** ppGetter);

    CARAPI SetUsed(
        /* [in] */ Int32 used);

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

    CARAPI SetEnumElement(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetCharElement(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI SetStringElement(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

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

    CARAPI SetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [in] */ Int32 * pValue);

    CARAPI SetObjectPtrElement(
        /* [in] */ Int32 index,
        /* [in] */ PInterface pValue);

    CARAPI GetStructElementSetter(
        /* [in] */ Int32 index,
        /* [out] */ ITestStructSetter ** ppSetter);

    CARAPI GetCapacity(
        /* [out] */ Int32 * pCapacity);

    CARAPI GetUsed(
        /* [out] */ Int32 * pUsed);

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

    CARAPI GetEnumElement(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetCharElement(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

    CARAPI GetStringElement(
        /* [in] */ Int32 index,
        /* [out] */ String * pValue);

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

    CARAPI GetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetObjectPtrElement(
        /* [in] */ Int32 index,
        /* [out] */ PInterface * ppValue);

    CARAPI GetStructElementGetter(
        /* [in] */ Int32 index,
        /* [out] */ ITestStructGetter ** ppGetter);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IVariableOfStruct * pVariableOfStruct);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IVariableOfStruct> mVariableOfStruct;
};

}
}
}
}

#endif // __CTESTVARIABLEOFSTRUCT_H__
