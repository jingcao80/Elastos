
#ifndef __CTESTCARDATATYPE_H__
#define __CTESTCARDATATYPE_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestCarDataType.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

#include <cutils/log.h>

using Elastos::Core::Thread;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestCarDataType)
    , public Object
    , public ITestCarDataType
    , public ITestCarRuntime
{
public:
    class _Thread
        : public Thread
    {
    public:
        _Thread(
            /* [in] */ INodeListener* listener);

        CARAPI Run();

    private:
        AutoPtr<INodeListener> mListener;
    };

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

    CARAPI Test_Char(
        /* [in] */ Char32 inValue,
        /* [out] */ Char32 * pOutValue);

    CARAPI Test_Char_Ref(
        /* [in] */ Char32 * pInValue,
        /* [out] */ Char32 * pOutValue);

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
        /* [in] */ TestObjectState inValue,
        /* [out] */ TestObjectState * pOutValue);

    CARAPI Test_Enum_Ref(
        /* [in] */ TestObjectState * pInValue,
        /* [out] */ TestObjectState * pOutValue);

    CARAPI Test_ArrayOf_Int16(
        /* [in] */ const ArrayOf<Int16> & inValue,
        /* [out, callee] */ ArrayOf<Int16> ** ppOutValue);

    CARAPI Test_ArrayOf_Ref(
        /* [in] */ ArrayOf<Int16> * pInValue,
        /* [out, callee] */ ArrayOf<Int16> ** ppOutValue);

    CARAPI Test_ArrayOf_Int32(
        /* [in] */ const ArrayOf<Int32> & inValue,
        /* [out, callee] */ ArrayOf<Int32> ** ppOutValue);

    CARAPI Test_ArrayOf_Int64(
        /* [in] */ const ArrayOf<Int64> & inValue,
        /* [out, callee] */ ArrayOf<Int64> ** ppOutValue);

    CARAPI Test_ArrayOf_Byte(
        /* [in] */ const ArrayOf<Byte> & inValue,
        /* [out, callee] */ ArrayOf<Byte> ** ppOutValue);

    CARAPI Test_ArrayOf_Float(
        /* [in] */ const ArrayOf<Float> & inValue,
        /* [out, callee] */ ArrayOf<Float> ** ppOutValue);

    CARAPI Test_ArrayOf_Double(
        /* [in] */ const ArrayOf<Double> & inValue,
        /* [out, callee] */ ArrayOf<Double> ** ppOutValue);

    CARAPI Test_ArrayOf_Char(
        /* [in] */ const ArrayOf<Char32> & inValue,
        /* [out, callee] */ ArrayOf<Char32> ** ppOutValue);

    CARAPI Test_ArrayOf_String(
        /* [in] */ const ArrayOf<String> & inValue,
        /* [out, callee] */ ArrayOf<String> ** ppOutValue);

    CARAPI Test_ArrayOf_Boolean(
        /* [in] */ const ArrayOf<Boolean> & inValue,
        /* [out, callee] */ ArrayOf<Boolean> ** ppOutValue);

    CARAPI Test_ArrayOf_EMuid(
        /* [in] */ const ArrayOf<EMuid> & inValue,
        /* [out, callee] */ ArrayOf<EMuid> ** ppOutValue);

    CARAPI Test_ArrayOf_EGuid(
        /* [in] */ const ArrayOf<EGuid> & inValue,
        /* [out, callee] */ ArrayOf<EGuid> ** ppOutValue);

    CARAPI Test_ArrayOf_ECode(
        /* [in] */ const ArrayOf<ECode> & inValue,
        /* [out, callee] */ ArrayOf<ECode> ** ppOutValue);

    CARAPI Test_ArrayOf_Enum(
        /* [in] */ const ArrayOf<TestObjectState> & inValue,
        /* [out, callee] */ ArrayOf<TestObjectState> ** ppOutValue);

    CARAPI Test_ArrayOf_IInterface(
        /* [in] */ const ArrayOf<IInterface*> & inValue,
        /* [out, callee] */ ArrayOf<IInterface*> ** ppOutValue);

    CARAPI Test_Interface(
        /* [in] */ IInterface * pInValue,
        /* [out] */ IInterface ** ppOutValue);

    CARAPI Test_Interface_Ref(
        /* [in] */ IInterface ** inValue,
        /* [out] */ IInterface ** ppOutValue);

    CARAPI Test_CreateInstance(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** object);

    CARAPI Test_AddEventListener(
        /* [in] */ INodeListener* listener);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CTESTCARDATATYPE_H__
