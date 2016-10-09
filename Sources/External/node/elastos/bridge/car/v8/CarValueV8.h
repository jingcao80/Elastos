
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
    {
        value.mCid.mUunm = mUUnm;
    }

    ~CarValue()
    {
        bool bRet = false;
        CarDataType elementType;
        AutoPtr<IDataTypeInfo> elementTypeInfo;
        switch (mType) {
            case CarDataType_ArrayOf:
            {
                ICarArrayInfo::Probe(mTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementTypeInfo);
                elementTypeInfo->GetDataType(&elementType);
                break;
            }
            case CarDataType_LocalPtr:
            {
                (*(ILocalPtrInfo **)&mTypeInfo)->GetTargetTypeInfo((IDataTypeInfo**)&elementTypeInfo);
                elementTypeInfo->GetDataType(&elementType);
                break;
            }
            default:
                bRet = true;
                break;
        }
        if (bRet) return;

        switch (elementType) {
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
                _CarQuintet_Release(value.mCarQuintet);
                break;
            case CarDataType_String:
            {
                Elastos::ArrayOf<Elastos::String>* strArray = reinterpret_cast< Elastos::ArrayOf<Elastos::String>* >(value.mCarQuintet);
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

    union value {
        Elastos::Int16 mInt16Value;
        Elastos::Int32 mInt32Value;
        Elastos::Int64 mInt64Value;
        Elastos::Byte mByteValue;
        Elastos::Char32 mCharValue;
        Elastos::Float mFloatValue;
        Elastos::Double mDoubleValue;
        //Elastos::String mStringValue;
        Elastos::Boolean mBooleanValue;
        Elastos::ECode mECodeValue;
        Elastos::Int32 mEnumValue;
        Elastos::CarQuintet* mCarQuintet;
        Elastos::EMuid mIid;    //to be replaced with ptr
        Elastos::EGuid mCid;    //to be replaced with ptr

        //char mUUnm[256];
        LocalPtr mLocalPtr;
        IInterface* mObjectValue;
    } value;

    Elastos::String mStringValue;   //to be replaced with ptr, string wrapper, really trouble in stack position

    char mUUnm[256];    //to be deleted

    Elastos::Boolean mTagSetLocalPtr;   //just for test of TestArgumentList::SetInputArgumentOfLocalPtr

    Elastos::AutoPtr<CobjectWrapper> mObjectWrapper;    //to be deleted

    AutoPtr<IDataTypeInfo> mTypeInfo;   //to be deleted after quintet resolved
};

struct DispParams
{
    CarValue* rgvarg;
    Elastos::Int32 cArgs;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARVALUEV8_H__