#include "elastos/droid/utility/ReflectiveProperty.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IChar8;
using Elastos::Core::CChar8;
using Elastos::Core::IChar16;
using Elastos::Core::CChar16;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;

namespace Elastos {
namespace Droid {
namespace Utility {

const String ReflectiveProperty::TAG("ReflectiveProperty");
const String ReflectiveProperty::PREFIX_GET("Get");
const String ReflectiveProperty::PREFIX_IS("Is");
const String ReflectiveProperty::PREFIX_SET("Set");

CAR_INTERFACE_IMPL_2(ReflectiveProperty, IProperty, IReflectiveProperty)

ReflectiveProperty::ReflectiveProperty(
    /* [in] */ ClassID clsId,
    /* [in] */ CarDataType dataType,
    /* [in] */ const String& name)
{
     AutoPtr<IModuleInfo> moduleInfo;
     String path(clsId.pUunm);
     ECode ec = _CReflector_AcquireModuleInfo(
            path, (IModuleInfo**)&moduleInfo);
     if (FAILED(ec)) {
        assert(0);
    }
    Int32 clsCount;
    moduleInfo->GetClassCount(&clsCount);
    BufferOf<IClassInfo*>* buf = BufferOf<IClassInfo*>::Alloc(clsCount);
    moduleInfo->GetAllClassInfos(buf);

    AutoPtr<IClassInfo> clsInfo;
    Int32 i = 0;
    for (; i < clsCount; i++) {
        ClassID id;
        id.pUunm = (char*)malloc(80);
        (*buf)[i]->GetId(&id);
        if (id == clsId) {
            clsInfo = (*buf)[i];
        }
        free(id.pUunm);
    }

    mDataType = dataType;
    mName = name;
    assert(clsInfo != NULL);
    String nmtmp = name.ToUpperCase(0, 1);

    String gName = PREFIX_GET + nmtmp;
    clsInfo->GetMethodInfo(gName, (IMethodInfo**)&mGetter);
    if (mGetter == NULL)
    {
        gName = PREFIX_IS + nmtmp;
        clsInfo->GetMethodInfo(gName, (IMethodInfo**)&mGetter);
    }

    String sName = PREFIX_SET + nmtmp;
    clsInfo->GetMethodInfo(sName, (IMethodInfo**)&mSetter);
    if(mGetter == NULL && mSetter == NULL)
    {
        mForbiddenOperate = TRUE;
    } else if(mGetter) {
        mReadOnly = TRUE;
    } else {
        mWriteOnly = TRUE;
    }
}

ECode ReflectiveProperty::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly);
    *readOnly = mReadOnly;
    return NOERROR;
}
ECode ReflectiveProperty::IsWriteOnly(
        /* [out] */ Boolean* writeOnly)
{
    VALIDATE_NOT_NULL(writeOnly);
    *writeOnly = mWriteOnly;
    return NOERROR;
}

ECode ReflectiveProperty::ForbiddenOperate(
        /* [out] */ Boolean* forbidden)
{
    VALIDATE_NOT_NULL(forbidden);
    *forbidden = mForbiddenOperate;
    return NOERROR;
}

ECode ReflectiveProperty::Set(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* value)
{
    if(mSetter == NULL) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    AutoPtr<IArgumentList> list;
    mSetter->CreateArgumentList((IArgumentList**)&list);
    switch(mDataType) {
        case CarDataType_Int16:
        {
            AutoPtr<IInteger16> tmp = IInteger16::Probe(value);
            assert(tmp);
            Int16 v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfInt16(0, v);
            break;
        }
        case CarDataType_Int32:
        {
            AutoPtr<IInteger32> tmp = IInteger32::Probe(value);
            assert(tmp);
            Int32 v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfInt32(0, v);
            break;
        }
        case CarDataType_Int64:
        {
            AutoPtr<IInteger64> tmp = IInteger64::Probe(value);
            assert(tmp);
            Int64 v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfInt64(0, v);
            break;
        }
        case CarDataType_Byte:
        {
            AutoPtr<IByte> tmp = IByte::Probe(value);
            assert(tmp);
            Byte v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfByte(0, v);
            break;
        }
        case CarDataType_Float:
        {
            AutoPtr<IFloat> tmp = IFloat::Probe(value);
            assert(tmp);
            Float v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfFloat(0, v);
            break;
        }
        case CarDataType_Double:
        {
            AutoPtr<IDouble> tmp = IDouble::Probe(value);
            assert(tmp);
            Double v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfDouble(0, v);
            break;
        }
        case CarDataType_Char8:
        {
            AutoPtr<IChar8> tmp = IChar8::Probe(value);
            assert(tmp);
            Char8 v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfChar8(0, v);
            break;
        }
        case CarDataType_Char16:
        {
            AutoPtr<IChar16> tmp = IChar16::Probe(value);
            assert(tmp);
            Char16 v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfChar16(0, v);
            break;
        }
        case CarDataType_String:
        {
            AutoPtr<ICharSequence> tmp = ICharSequence::Probe(value);
            assert(tmp);
            String v;
            tmp->ToString(&v);
            list->SetInputArgumentOfString(0, v);
            break;
        }
        case CarDataType_Boolean:
        {
            AutoPtr<IBoolean> tmp = IBoolean::Probe(value);
            assert(tmp);
            Boolean v;
            tmp->GetValue(&v);
            list->SetInputArgumentOfBoolean(0, v);
            break;
        }
        case CarDataType_Interface:
        {
            list->SetInputArgumentOfObjectPtr(0, value);
            break;
        }
        default :
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }
    return mSetter->Invoke(obj, list);
}

ECode ReflectiveProperty::Get(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** rst)
{
    VALIDATE_NOT_NULL(rst);

    if (mGetter == NULL) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    AutoPtr<IArgumentList> list;
    mGetter->CreateArgumentList((IArgumentList**)&list);
    AutoPtr<IFunctionInfo> funcInfo;
    list->GetFunctionInfo((IFunctionInfo**)&funcInfo);
    AutoPtr<IParamInfo> paramInfo;
    funcInfo->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
    AutoPtr<IDataTypeInfo> dataTypeInfo;
    paramInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    CarDataType carType;
    dataTypeInfo->GetDataType(&carType);
    switch(carType) {
        case CarDataType_Int16:
        {
            Int16 v;
            list->SetOutputArgumentOfInt16Ptr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IInteger16> rTmp;
            CInteger16::New(v, (IInteger16**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;
        }
        case CarDataType_Int32:
        {
            Int32 v;
            list->SetOutputArgumentOfInt32Ptr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IInteger32> rTmp;
            CInteger32::New(v, (IInteger32**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_Int64:
        {
            Int64 v;
            list->SetOutputArgumentOfInt64Ptr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IInteger64> rTmp;
            CInteger64::New(v, (IInteger64**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_Byte:
        {
            Byte v;
            list->SetOutputArgumentOfBytePtr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IByte> rTmp;
            CByte::New(v, (IByte**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_Float:
        {
            Float v;
            list->SetOutputArgumentOfFloatPtr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IFloat> rTmp;
            CFloat::New(v, (IFloat**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_Double:
        {
            Double v;
            list->SetOutputArgumentOfDoublePtr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IDouble> rTmp;
            CDouble::New(v, (IDouble**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_Char8:
        {
            Char8 v;
            list->SetOutputArgumentOfChar8Ptr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IChar8> rTmp;
            CChar8::New(v, (IChar8**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_Char16:
        {
            Char16 v;;
            list->SetOutputArgumentOfChar16Ptr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IChar16> rTmp;
            CChar16::New(v, (IChar16**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_String:
        {
            String v;
            list->SetOutputArgumentOfStringPtr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<ICharSequence> rTmp;
            CString::New(v, (ICharSequence**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst);
            return NOERROR;

        }
        case CarDataType_Boolean:
        {
            Boolean v;
            list->SetOutputArgumentOfBooleanPtr(0, &v);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            AutoPtr<IBoolean> rTmp;
            CBoolean::New(v, (IBoolean**)&rTmp);
            *rst = rTmp;
            REFCOUNT_ADD(*rst)
            return NOERROR;

        }
        case CarDataType_Interface:
        {
            list->SetOutputArgumentOfObjectPtrPtr(0, rst);
            FAIL_RETURN(mGetter->Invoke(obj, list));
            return NOERROR;

        }
        default :
        {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }
}

ECode ReflectiveProperty::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode ReflectiveProperty::GetType(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mClsId;
    return NOERROR;
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos
