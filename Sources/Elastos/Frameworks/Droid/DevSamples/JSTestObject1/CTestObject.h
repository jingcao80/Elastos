
#ifndef __CTESTOBJECT_H__
#define __CTESTOBJECT_H__

#include "_CTestObject.h"


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestObject)
{
public:
    class _Thread : public ThreadBase
    {
    public:
        _Thread(
            /* [in] */ ITestEventListener* listener)
            : mListener(listener)
        {
            ThreadBase::Init();
        }

        CARAPI Run();

    private:
        AutoPtr<ITestEventListener> mListener;
    };

public:
    CARAPI Test_NULL();

    CARAPI Test_Int16(
        /* [in] */ Int16 inValue,
        /* [out] */ Int16* outValue);

    CARAPI Test_Int32(
        /* [in] */ Int32 inValue,
        /* [out] */ Int32* outValue);

    CARAPI Test_Int64(
        /* [in] */ Int64 inValue,
        /* [out] */ Int64* outValue);

    CARAPI Test_Byte(
        /* [in] */ Byte inValue,
        /* [out] */ Byte* outValue);

    CARAPI Test_Float(
        /* [in] */ Float inValue,
        /* [out] */ Float* outValue);

    CARAPI Test_Double(
        /* [in] */ Double inValue,
        /* [out] */ Double* outValue);

    CARAPI Test_Char(
        /* [in] */ Char32 inValue,
        /* [out] */ Char32* outValue);

    CARAPI Test_String(
        /* [in] */ const String& inStr,
        /* [out] */ String* outStr);

    CARAPI Test_Boolean(
        /* [in] */ Boolean inValue,
        /* [out] */ Boolean* outValue);

    CARAPI Test_EMuid(
        /* [in] */ const EMuid& inId,
        /* [out] */ EMuid* outId);

    CARAPI Test_EGuid(
        /* [in] */ const EGuid& inId,
        /* [out] */ EGuid* outId);

    CARAPI Test_ECode(
        /* [in] */ ECode inValue,
        /* [out] */ ECode* outValue);

    CARAPI Test_Enum(
        /* [in] */ TestObjectState inValue,
        /* [out] */ TestObjectState* outValue);

    CARAPI Test_ArrayOf_Int16(
        /* [in] */ const ArrayOf<Int16>& inValue,
        /* [out, callee] */ ArrayOf<Int16>** outValue);

    CARAPI Test_ArrayOf_Int32(
        /* [in] */ const ArrayOf<Int32>& inValue,
        /* [out, callee] */ ArrayOf<Int32>** outValue);

    CARAPI Test_ArrayOf_Int64(
        /* [in] */ const ArrayOf<Int64>& inValue,
        /* [out, callee] */ ArrayOf<Int64>** outValue);

    CARAPI Test_ArrayOf_Byte(
        /* [in] */ const ArrayOf<Byte>& inValue,
        /* [out, callee] */ ArrayOf<Byte>** outValue);

    CARAPI Test_ArrayOf_Float(
        /* [in] */ const ArrayOf<Float>& inValue,
        /* [out, callee] */ ArrayOf<Float>** outValue);

    CARAPI Test_ArrayOf_Double(
        /* [in] */ const ArrayOf<Double>& inValue,
        /* [out, callee] */ ArrayOf<Double>** outValue);

    CARAPI Test_ArrayOf_Char(
        /* [in] */ const ArrayOf<Char32>& inValue,
        /* [out, callee] */ ArrayOf<Char32>** outValue);

    CARAPI Test_ArrayOf_String(
        /* [in] */ const ArrayOf<String>& inValue,
        /* [out, callee] */ ArrayOf<String>** outValue);

    CARAPI Test_ArrayOf_Boolean(
        /* [in] */ const ArrayOf<Boolean>& inValue,
        /* [out, callee] */ ArrayOf<Boolean>** outValue);

    CARAPI Test_ArrayOf_EMuid(
        /* [in] */ const ArrayOf<EMuid>& inValue,
        /* [out, callee] */ ArrayOf<EMuid>** outValue);

    CARAPI Test_ArrayOf_EGuid(
        /* [in] */ const ArrayOf<EGuid>& inValue,
        /* [out, callee] */ ArrayOf<EGuid>** outValue);

    CARAPI Test_ArrayOf_ECode(
        /* [in] */ const ArrayOf<ECode>& inValue,
        /* [out, callee] */ ArrayOf<ECode>** outValue);

    CARAPI Test_ArrayOf_Enum(
        /* [in] */ const ArrayOf<TestObjectState>& inValue,
        /* [out, callee] */ ArrayOf<TestObjectState>** outValue);

    CARAPI Test_ArrayOf_IInterface(
        /* [in] */ const ArrayOf<IInterface*>& inValue,
        /* [out, callee] */ ArrayOf<IInterface*>** outValue);

    CARAPI Test_Interface(
        /* [in] */ IInterface* inObject,
        /* [out] */ IInterface** outObject);

    CARAPI Test_CreateInstance(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** object);

    CARAPI Test_AddEventListener(
        /* [in] */ ITestEventListener* listener);
};

}
}
}
}

#endif // __CTESTOBJECT_H__
