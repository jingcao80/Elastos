
#ifndef __CARVALUEV8_H__
#define __CARVALUEV8_H__

#if ENABLE(CAR_BRIDGE)

#include "CobjectWrapper.h"

#include <cutils/log.h>

namespace JSC {
namespace Bindings {

class CarInstance;

// A variant used to represent a Java value, almost identical to the JNI
// jvalue type. It exists because the logic to convert between JavaScript
// objects (as JavaNPObject or JSValue) and Java objects should not depend upon
// JNI, to allow ports to provide a JavaInstance object etc which does not use
// JNI. This means that the 'object' field of this variant uses JavaInstance,
// not jobject.
//
// Note that this class is independent of the JavaScript engine, but is
// currently used only with V8.
// See https://bugs.webkit.org/show_bug.cgi?id=57023.
struct CarValue
{
    CarValue()
        : mType(0)
        , mIOAttribute(ParamIOAttribute_In)
        , mInt16Value(0)
        , mInt32Value(0)
        , mInt64Value(0)
        , mByteValue(0)
        , mCharValue(0)
        , mFloatValue(0.0)
        , mDoubleValue(0.0)
        , mBooleanValue(FALSE)
        , mECodeValue(NOERROR)
        , mEnumValue(0)
        , mCarQuintet(NULL)
    {
        mCid.mUunm = mUUnm;
    }

    ~CarValue()
    {
        switch (mElementType) {
            case CarDataType_Int16:
            case CarDataType_Int32:
            case CarDataType_Int64:
            case CarDataType_Byte:
            case CarDataType_Char32:
            case CarDataType_Float:
            case CarDataType_Double:
            case CarDataType_Boolean:
            case CarDataType_ECode:
            case CarDataType_Enum:
                _CarQuintet_Release(mCarQuintet);
                break;
            case CarDataType_String:
            {
                Elastos::ArrayOf<Elastos::String>* strArray = reinterpret_cast< Elastos::ArrayOf<Elastos::String>* >(mCarQuintet);
                strArray->Release();
                break;
            }
            case CarDataType_Interface:
            case CarDataType_EMuid:
            case CarDataType_EGuid:
            default:
                break;
        }
    }

    ParamIOAttribute mIOAttribute;

    CarDataType mType;
    CarDataType mElementType;
    IInterface* mObjectValue;

    Elastos::AutoPtr<CobjectWrapper> mObjectWrapper;

    Elastos::Int16 mInt16Value;
    Elastos::Int32 mInt32Value;
    Elastos::Int64 mInt64Value;
    Elastos::Byte mByteValue;
    Elastos::Char32 mCharValue;
    Elastos::Float mFloatValue;
    Elastos::Double mDoubleValue;
    Elastos::String mStringValue;
    Elastos::Boolean mBooleanValue;
    Elastos::ECode mECodeValue;
    Elastos::Int32 mEnumValue;
    Elastos::CarQuintet* mCarQuintet;
    Elastos::EMuid mIid;
    Elastos::EGuid mCid;
    char mUUnm[256];

    LocalPtr mLocalPtr;

    Elastos::Boolean mTagSetLocalPtr;   //just for test of TestArgumentList::SetInputArgumentOfLocalPtr
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARVALUEV8_H__
