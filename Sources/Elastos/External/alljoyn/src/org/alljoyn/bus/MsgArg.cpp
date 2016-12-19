
#include "org/alljoyn/bus/MsgArg.h"
#include "org/alljoyn/bus/CVariant.h"
#include "org/alljoyn/bus/Signature.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/MsgArg.h>
#include <alljoyn/MsgArgUtils.h>
#include "_Org.Alljoyn.Bus.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::EIID_IByte;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::EIID_IInteger16;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IDouble;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ISet;;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("MsgArg");
static const Boolean DEBUG_MAP = TRUE;

MsgArg::CarValue::CarValue(
    /* [in] */ CarDataType type)
    : mIOAttribute(ParamIOAttribute_In)
    , mType(type)
    , mByteValue(0)
    , mBooleanValue(FALSE)
    , mInt16Value(0)
    , mInt32Value(0)
    , mInt64Value(0)
    , mDoubleValue(0.0)
    , mEnumValue(0)
    , mCarQuintet(NULL)
    , mElementType(0)
{
}

MsgArg::CarValue::~CarValue()
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
        case CarDataType_EMuid:
        case CarDataType_EGuid:
            _CarQuintet_Release(mCarQuintet);
            mCarQuintet = NULL;
            break;
        case CarDataType_String: {
            ArrayOf<String>* strArray = (ArrayOf<String>*)mCarQuintet;
            strArray->Release();
            mCarQuintet = NULL;
            break;
        }
        case CarDataType_Interface: {
            ArrayOf<IInterface*>* itfArray = (ArrayOf<IInterface*>*)mCarQuintet;
            itfArray->Release();
            mCarQuintet = NULL;
            break;
        }
        default:
            break;
    }
}

PVoid MsgArg::CarValue::ToValuePtr()
{
    PVoid arg = NULL;
    switch (mType) {
        case CarDataType_Int16:
            arg = &mInt16Value;
            break;
        case CarDataType_Int32:
            arg = &mInt32Value;
            break;
        case CarDataType_Int64:
            arg = &mInt64Value;
            break;
        case CarDataType_Byte:
            arg = &mByteValue;
            break;
        case CarDataType_Double:
            arg = &mDoubleValue;
            break;
        case CarDataType_String:
            arg = &mStringValue;
            break;
        case CarDataType_Boolean:
            arg = &mBooleanValue;
            break;
        case CarDataType_Enum:
            arg = &mEnumValue;
            break;
        case CarDataType_ArrayOf:
            arg = &mCarQuintet;
            break;
        case CarDataType_Interface:
            arg = &mObjectValue;
            break;
        default:
            Logger::E(TAG, "CarValue::ToValuePtr unimplemented type = %d", mType);
            assert(0);
            break;
    }
    return arg;
}

ECode MsgArg::CarValue::SetToArgumentList(
    /* [in] */ IArgumentList* args,
    /* [in] */ Int32 index)
{
    if (mIOAttribute == ParamIOAttribute_In) {
        switch (mType) {
        case CarDataType_Int16:
            args->SetInputArgumentOfInt16(index, mInt16Value);
            break;
        case CarDataType_Int32:
            args->SetInputArgumentOfInt32(index, mInt32Value);
            break;
        case CarDataType_Int64:
            args->SetInputArgumentOfInt64(index, mInt64Value);
            break;
        case CarDataType_Byte:
            args->SetInputArgumentOfByte(index, mByteValue);
            break;
        case CarDataType_Double:
            args->SetInputArgumentOfDouble(index, mDoubleValue);
            break;
        case CarDataType_String:
            args->SetInputArgumentOfString(index, mStringValue);
            break;
        case CarDataType_Boolean:
            args->SetInputArgumentOfBoolean(index, mBooleanValue);
            break;
        case CarDataType_Enum:
            args->SetInputArgumentOfEnum(index, mEnumValue);
            break;
        case CarDataType_ArrayOf:
            args->SetInputArgumentOfCarArray(index, mCarQuintet);
            break;
        case CarDataType_Interface:
            args->SetInputArgumentOfObjectPtr(index, mObjectValue);
            break;
        default:
            Logger::E(TAG, "CarValue::SetToArgumentList unimplemented index = %d, type = %d", index, mType);
            assert(0);
            break;
        }
    }
    else {
        switch (mType) {
        case CarDataType_Int16:
            args->SetOutputArgumentOfInt16Ptr(index, &mInt16Value);
            break;
        case CarDataType_Int32:
            args->SetOutputArgumentOfInt32Ptr(index, &mInt32Value);
            break;
        case CarDataType_Int64:
            args->SetOutputArgumentOfInt64Ptr(index, &mInt64Value);
            break;
        case CarDataType_Byte:
            args->SetOutputArgumentOfBytePtr(index, &mByteValue);
            break;
        case CarDataType_Double:
            args->SetOutputArgumentOfDoublePtr(index, &mDoubleValue);
            break;
        case CarDataType_String:
            args->SetOutputArgumentOfStringPtr(index, &mStringValue);
            break;
        case CarDataType_Boolean:
            args->SetOutputArgumentOfBooleanPtr(index, &mBooleanValue);
            break;
        case CarDataType_Enum:
            args->SetOutputArgumentOfEnumPtr(index, &mEnumValue);
            break;
        case CarDataType_ArrayOf:
            args->SetOutputArgumentOfCarArrayPtrPtr(index, &mCarQuintet);
            break;
        case CarDataType_Interface:
            args->SetOutputArgumentOfObjectPtrPtr(index, (IInterface**)&mObjectValue);
            break;
        default:
            Logger::E(TAG, "CarValue::SetToArgumentList unimplemented index = %d, type = %d", index, mType);
            assert(0);
            break;
        }
    }
    return NOERROR;
}

ECode MsgArg::CarValue::GetFromArgumentList(
    /* [in] */ IArgumentList* args,
    /* [in] */ Int32 index)
{
    if (mIOAttribute == ParamIOAttribute_In) {
        switch (mType) {
        case CarDataType_Int16:
            args->GetInputArgumentOfInt16(index, &mInt16Value);
            break;
        case CarDataType_Int32:
            args->GetInputArgumentOfInt32(index, &mInt32Value);
            break;
        case CarDataType_Int64:
            args->GetInputArgumentOfInt64(index, &mInt64Value);
            break;
        case CarDataType_Byte:
            args->GetInputArgumentOfByte(index, &mByteValue);
            break;
        case CarDataType_Double:
            args->GetInputArgumentOfDouble(index, &mDoubleValue);
            break;
        case CarDataType_String:
            args->GetInputArgumentOfString(index, &mStringValue);
            break;
        case CarDataType_Boolean:
            args->GetInputArgumentOfBoolean(index, &mBooleanValue);
            break;
        case CarDataType_Enum:
            args->GetInputArgumentOfEnum(index, &mEnumValue);
            break;
        case CarDataType_ArrayOf:
            args->GetInputArgumentOfCarArray(index, &mCarQuintet);
            break;
        case CarDataType_Interface:
            args->GetInputArgumentOfObjectPtr(index, (IInterface**)&mObjectValue);
            break;
        default:
            Logger::E(TAG, "CarValue::GetFromArgumentList unimplemented index = %d, type = %d", index, mType);
            assert(0);
            break;
        }
    }
    else {
        assert(0);
    }

    return NOERROR;
}

ECode MsgArg::CarValue::AssignArgumentListOutput(
    /* [in] */ IArgumentList* args,
    /* [in] */ Int32 index)
{
    if (mIOAttribute != ParamIOAttribute_In) {
        switch (mType) {
        case CarDataType_Int16:
            args->AssignOutputArgumentOfInt16Ptr(index, mInt16Value);
            break;
        case CarDataType_Int32:
            args->AssignOutputArgumentOfInt32Ptr(index, mInt32Value);
            break;
        case CarDataType_Int64:
            args->AssignOutputArgumentOfInt64Ptr(index, mInt64Value);
            break;
        case CarDataType_Byte:
            args->AssignOutputArgumentOfBytePtr(index, mByteValue);
            break;
        case CarDataType_Double:
            args->AssignOutputArgumentOfDoublePtr(index, mDoubleValue);
            break;
        case CarDataType_String:
            args->AssignOutputArgumentOfStringPtr(index, mStringValue);
            break;
        case CarDataType_Boolean:
            args->AssignOutputArgumentOfBooleanPtr(index, mBooleanValue);
            break;
        case CarDataType_Enum:
            args->AssignOutputArgumentOfEnumPtr(index, mEnumValue);
            break;
        case CarDataType_ArrayOf:
            args->AssignOutputArgumentOfCarArrayPtrPtr(index, mCarQuintet);
            break;
        case CarDataType_Interface:
            args->AssignOutputArgumentOfObjectPtrPtr(index, mObjectValue);
            break;
        default:
            Logger::E(TAG, "CarValue::AssignArgumentListOutput unimplemented index = %d, type = %d", index, mType);
            assert(0);
            break;
        }
    }
    else {
        assert(0);
    }

    return NOERROR;
}

AutoPtr<IInterface> MsgArg::CarValue::Convert()
{
    AutoPtr<IInterface> ret;
    switch (mType) {
    case CarDataType_Int16:
        ret = CoreUtils::Convert(mInt16Value);
        break;
    case CarDataType_Int32:
        ret = CoreUtils::Convert(mInt32Value);
        break;
    case CarDataType_Int64:
        ret = CoreUtils::Convert(mInt64Value);
        break;
    case CarDataType_Byte:
        ret = CoreUtils::ConvertByte(mByteValue);
        break;
    case CarDataType_Double:
        ret = CoreUtils::Convert(mDoubleValue);
        break;
    case CarDataType_String:
        ret = CoreUtils::Convert(mStringValue);
        break;
    case CarDataType_Boolean:
        ret = CoreUtils::Convert(mBooleanValue);
        break;
    case CarDataType_Enum:
        ret = CoreUtils::Convert(mEnumValue);
        break;
    case CarDataType_ArrayOf: {
        switch (mElementType) {
        case CarDataType_Int16:
            ret = CoreUtils::Convert((ArrayOf<Int16>*)mCarQuintet);
            break;
        case CarDataType_Int32:
            ret = CoreUtils::Convert((ArrayOf<Int32>*)mCarQuintet);
            break;
        case CarDataType_Int64:
            ret = CoreUtils::Convert((ArrayOf<Int64>*)mCarQuintet);
            break;
        case CarDataType_Byte:
            ret = CoreUtils::ConvertByteArray((ArrayOf<Byte>*)mCarQuintet);
            break;
        case CarDataType_Double:
            ret = CoreUtils::Convert((ArrayOf<Double>*)mCarQuintet);
            break;
        case CarDataType_Boolean:
            ret = CoreUtils::Convert((ArrayOf<Boolean>*)mCarQuintet);
            break;
        case CarDataType_Enum:
            ret = CoreUtils::Convert((ArrayOf<Int32>*)mCarQuintet);
            break;
        case CarDataType_String:
            ret = CoreUtils::Convert((ArrayOf<String>*)mCarQuintet);
            break;
        case CarDataType_Interface:
            ret = CoreUtils::Convert((ArrayOf<IInterface*>*)mCarQuintet);
            break;
        default:
            Logger::E(TAG, "CarValue::Convert unimplemented mElementType = %d", mElementType);
            assert(0);
            break;
        }
        break;
    }
    case CarDataType_Interface:
        ret = mObjectValue;
        break;
    default:
        Logger::E(TAG, "CarValue::Convert unimplemented type = %d", mType);
        assert(0);
        break;
    }

    return ret;
}

AutoPtr<MsgArg::CarValue> MsgArg::CarValue::Convert(
    /* [in] */ IInterface* object)
{
    AutoPtr<CarValue> value;

    if (IByte::Probe(object) != NULL) {
        value = new CarValue(CarDataType_Byte);
        IByte::Probe(object)->GetValue(&value->mByteValue);
    }
    else if (IBoolean::Probe(object) != NULL) {
        value = new CarValue(CarDataType_Boolean);
        IBoolean::Probe(object)->GetValue(&value->mBooleanValue);
    }
    else if (IInteger16::Probe(object) != NULL) {
        value = new CarValue(CarDataType_Int16);
        IInteger16::Probe(object)->GetValue(&value->mInt16Value);
    }
    else if (IInteger32::Probe(object) != NULL) {
        value = new CarValue(CarDataType_Int32);
        IInteger32::Probe(object)->GetValue(&value->mInt32Value);
    }
    else if (IInteger64::Probe(object) != NULL) {
        value = new CarValue(CarDataType_Int64);
        IInteger64::Probe(object)->GetValue(&value->mInt64Value);
    }
    else if (IDouble::Probe(object) != NULL) {
        value = new CarValue(CarDataType_Double);
        IDouble::Probe(object)->GetValue(&value->mDoubleValue);
    }
    else if (ICharSequence::Probe(object) != NULL) {
        value = new CarValue(CarDataType_String);
        ICharSequence::Probe(object)->ToString(&value->mStringValue);
    }
    else if (IArrayOf::Probe(object) != NULL) {
        value = new CarValue(CarDataType_ArrayOf);
        IArrayOf* iarray = IArrayOf::Probe(object);
        Int32 size;
        iarray->GetLength(&size);
        InterfaceID id;
        iarray->GetTypeId(&id);
        if (id == EIID_IByte) {
            AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                iarray->Get(i, (IInterface**)&item);
                IByte::Probe(item)->GetValue(&(*array)[i]);
            }
            value->mElementType = CarDataType_Byte;
            value->mCarQuintet = array;
            array->AddRef();
        }
        else if (id == EIID_IBoolean) {
            AutoPtr<ArrayOf<Boolean> > array = ArrayOf<Boolean>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                iarray->Get(i, (IInterface**)&item);
                IBoolean::Probe(item)->GetValue(&(*array)[i]);
            }
            value->mElementType = CarDataType_Boolean;
            value->mCarQuintet = array;
            array->AddRef();
        }
        else if (id == EIID_IInteger16) {
            AutoPtr<ArrayOf<Int16> > array = ArrayOf<Int16>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                iarray->Get(i, (IInterface**)&item);
                IInteger16::Probe(item)->GetValue(&(*array)[i]);
            }
            value->mElementType = CarDataType_Int16;
            value->mCarQuintet = array;
            array->AddRef();
        }
        else if (id == EIID_IInteger32) {
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                iarray->Get(i, (IInterface**)&item);
                IInteger32::Probe(item)->GetValue(&(*array)[i]);
            }
            value->mElementType = CarDataType_Int32;
            value->mCarQuintet = array;
            array->AddRef();
        }
        else if (id == EIID_IInteger64) {
            AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                iarray->Get(i, (IInterface**)&item);
                IInteger64::Probe(item)->GetValue(&(*array)[i]);
            }
            value->mElementType = CarDataType_Int64;
            value->mCarQuintet = array;
            array->AddRef();
        }
        else if (id == EIID_IDouble) {
            AutoPtr<ArrayOf<Double> > array = ArrayOf<Double>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                iarray->Get(i, (IInterface**)&item);
                IDouble::Probe(item)->GetValue(&(*array)[i]);
            }
            value->mElementType = CarDataType_Double;
            value->mCarQuintet = array;
            array->AddRef();
        }
        else if (id == EIID_ICharSequence) {
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                iarray->Get(i, (IInterface**)&item);
                ICharSequence::Probe(item)->ToString(&(*array)[i]);
            }
            value->mElementType = CarDataType_String;
            value->mCarQuintet = array;
            array->AddRef();
        }
        else {
            assert(0);
        }
    }
    else {
        Logger::E(TAG, "CarValue::Convert unimplemented object %s", TO_CSTR(object));
        assert(0);
    }

    return value;
}

const Int32 MsgArg::ALLJOYN_INVALID          =  0;
const Int32 MsgArg::ALLJOYN_ARRAY            = 'a';
const Int32 MsgArg::ALLJOYN_BOOLEAN          = 'b';
const Int32 MsgArg::ALLJOYN_DOUBLE           = 'd';
const Int32 MsgArg::ALLJOYN_DICT_ENTRY       = 'e';
const Int32 MsgArg::ALLJOYN_SIGNATURE        = 'g';
const Int32 MsgArg::ALLJOYN_INT32            = 'i';
const Int32 MsgArg::ALLJOYN_INT16            = 'n';
const Int32 MsgArg::ALLJOYN_OBJECT_PATH      = 'o';
const Int32 MsgArg::ALLJOYN_UINT16           = 'q';
const Int32 MsgArg::ALLJOYN_STRUCT           = 'r';
const Int32 MsgArg::ALLJOYN_STRING           = 's';
const Int32 MsgArg::ALLJOYN_UINT64           = 't';
const Int32 MsgArg::ALLJOYN_UINT32           = 'u';
const Int32 MsgArg::ALLJOYN_VARIANT          = 'v';
const Int32 MsgArg::ALLJOYN_INT64            = 'x';
const Int32 MsgArg::ALLJOYN_BYTE             = 'y';

const Int32 MsgArg::ALLJOYN_STRUCT_OPEN      = '(';
const Int32 MsgArg::ALLJOYN_STRUCT_CLOSE     = ')';
const Int32 MsgArg::ALLJOYN_DICT_ENTRY_OPEN  = '{';
const Int32 MsgArg::ALLJOYN_DICT_ENTRY_CLOSE = '}';

const Int32 MsgArg::ALLJOYN_BOOLEAN_ARRAY    = ('b' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_DOUBLE_ARRAY     = ('d' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_INT32_ARRAY      = ('i' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_INT16_ARRAY      = ('n' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_UINT16_ARRAY     = ('q' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_UINT64_ARRAY     = ('t' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_UINT32_ARRAY     = ('u' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_INT64_ARRAY      = ('x' << 8) | 'a';
const Int32 MsgArg::ALLJOYN_BYTE_ARRAY       = ('y' << 8) | 'a';

Map<AutoPtr<IArgumentList>, AutoPtr<ArrayOf<MsgArg::CarValue*> > > MsgArg::sRecords;
Object MsgArg::sLock;

Int32 MsgArg::GetTypeId(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    return msgArg->typeId;
}

Byte MsgArg::GetByte(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_BYTE == msgArg->typeId);
    return msgArg->v_byte;
}

Int16 MsgArg::GetInt16(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_INT16 == msgArg->typeId);
    return msgArg->v_int16;
}

Int16 MsgArg::GetUint16(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_UINT16 == msgArg->typeId);
    return msgArg->v_uint16;
}

Boolean MsgArg::GetBool(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_BOOLEAN == msgArg->typeId);
    return msgArg->v_bool;
}

Int32 MsgArg::GetUint32(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_UINT32 == msgArg->typeId);
    return msgArg->v_uint32;
}

Int32 MsgArg::GetInt32(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_INT32 == msgArg->typeId);
    return msgArg->v_int32;
}

Int64 MsgArg::GetInt64(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_INT64 == msgArg->typeId);
    return msgArg->v_int64;
}

Int64 MsgArg::GetUint64(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_UINT64 == msgArg->typeId);
    return msgArg->v_uint64;
}

Double MsgArg::GetDouble(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_DOUBLE == msgArg->typeId);
    return msgArg->v_double;
}

String MsgArg::GetString(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_STRING == msgArg->typeId);

    String str(msgArg->v_string.str, msgArg->v_string.len);
    return str;
}

String MsgArg::GetObjPath(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_OBJECT_PATH == msgArg->typeId);

    String str(msgArg->v_objPath.str, msgArg->v_objPath.len);
    return str;
}

String MsgArg::GetSignature(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_SIGNATURE == msgArg->typeId);

    String str(msgArg->v_signature.sig, msgArg->v_signature.len);
    return str;
}

AutoPtr<ArrayOf<Byte> > MsgArg::GetByteArray(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_BYTE_ARRAY == msgArg->typeId);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_byte[i];
    }

    return array;
}

AutoPtr<ArrayOf<Int16> > MsgArg::GetInt16Array(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_INT16_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Int16> > array = ArrayOf<Int16>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_int16[i];
    }

    return array;
}

AutoPtr<ArrayOf<Int16> > MsgArg::GetUint16Array(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_UINT16_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Int16> > array = ArrayOf<Int16>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_uint16[i];
    }

    return array;
}

AutoPtr<ArrayOf<Boolean> > MsgArg::GetBoolArray(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_BOOLEAN_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Boolean> > array = ArrayOf<Boolean>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_bool[i];
    }

    return array;
}

AutoPtr<ArrayOf<Int32> > MsgArg::GetUint32Array(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_UINT32_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_uint32[i];
    }

    return array;
}

AutoPtr<ArrayOf<Int32> > MsgArg::GetInt32Array(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_INT32_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_int32[i];
    }

    return array;
}

AutoPtr<ArrayOf<Int64> > MsgArg::GetInt64Array(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_INT64_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_int64[i];
    }

    return array;
}

AutoPtr<ArrayOf<Int64> > MsgArg::GetUint64Array(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_UINT64_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_uint64[i];
    }

    return array;
}

AutoPtr<ArrayOf<Double> > MsgArg::GetDoubleArray(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_DOUBLE_ARRAY == msgArg->typeId);

    AutoPtr<ArrayOf<Double> > array = ArrayOf<Double>::Alloc(msgArg->v_scalarArray.numElements);
    if (!array) {
        return NULL;
    }

    for (size_t i = 0; i < msgArg->v_scalarArray.numElements; ++i) {
        (*array)[i] = msgArg->v_scalarArray.v_double[i];
    }

    return array;
}

Int64 MsgArg::GetKey(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_DICT_ENTRY == msgArg->typeId);
    return (Int64)msgArg->v_dictEntry.key;
}

Int64 MsgArg::GetVal(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    switch (msgArg->typeId) {
    case ALLJOYN_VARIANT:
        return (Int64)msgArg->v_variant.val;

    case ALLJOYN_DICT_ENTRY:
        return (Int64)msgArg->v_dictEntry.val;

    default:
        assert(0);
        return 0;
    }
}

Int32 MsgArg::GetNumElements(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_ARRAY == msgArg->typeId);
    return msgArg->v_array.GetNumElements();
}

Int64 MsgArg::GetElement(
    /* [in] */ Int64 _msgArg,
    /* [in] */ Int32 index)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_ARRAY == msgArg->typeId);
    assert(index < (Int32)msgArg->v_array.GetNumElements());
    return (Int64) & msgArg->v_array.GetElements()[index];
}

String MsgArg::GetElemSig(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_ARRAY == msgArg->typeId);
    return String(msgArg->v_array.GetElemSig());
}

Int32 MsgArg::GetNumMembers(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_STRUCT == msgArg->typeId);
    return msgArg->v_struct.numMembers;
}

Int64 MsgArg::GetMember(
    /* [in] */ Int64 _msgArg,
    /* [in] */ Int32 index)
{
    ajn::MsgArg* msgArg = (ajn::MsgArg*)_msgArg;
    assert(ajn::ALLJOYN_STRUCT == msgArg->typeId);
    assert(index < (Int32)msgArg->v_struct.numMembers);
    return (Int64) & msgArg->v_struct.members[index];
}

/**
 * Calls MsgArgUtils::SetV() to set the values of a MsgArg.
 *
 * @param arg the arg to set
 * @param jsignature the signature of the arg
 * @param ... the values to set
 * @return the @param arg passed in or NULL if an error occurred
 * @throws BusException if an error occurs
 */
static ECode SetV(
    /* [in] */ ajn::MsgArg* arg,
    /* [in] */ const String& signature, ...)
{
    va_list argp;
    va_start(argp, signature);
    size_t one = 1;
    QStatus status = ajn::MsgArgUtils::SetV(arg, one, signature.string(), &argp);
    va_end(argp);
    return status;
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ Byte arg)
{
    return SetV((ajn::MsgArg*)_msgArg, signature, arg);
}

ECode MsgArg::SetBoolean(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ Boolean arg)
{
    return SetV((ajn::MsgArg*)_msgArg, signature, arg);
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ Int16 arg)
{
    return SetV((ajn::MsgArg*)_msgArg, signature, arg);
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ Int32 arg)
{
    return SetV((ajn::MsgArg*)_msgArg, signature, arg);
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ Int64 arg)
{
    return SetV((ajn::MsgArg*)_msgArg, signature, arg);
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ Double arg)
{
    return SetV((ajn::MsgArg*)_msgArg, signature, arg);
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ const String& value)
{
    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature, value.string());
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->Stabilize();
    }
    return NOERROR;
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ ArrayOf<Byte>* arg)
{
    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature, arg->GetLength(), arg->GetPayload());
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->Stabilize();
    }

    return NOERROR;
}

ECode MsgArg::SetBooleanArray(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ ArrayOf<Boolean>* arg)
{
    /* Booleans are different sizes in Java and MsgArg, so can't just do a straight copy. */
    size_t numElements = arg->GetLength();
    bool* v_bool = new bool[numElements];
    if (!v_bool) {
        return E_STATUS_OUT_OF_MEMORY;
    }

    for (size_t i = 0; i < numElements; ++i) {
        v_bool[i] = (*arg)[i];
    }

    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature, numElements, v_bool);
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->SetOwnershipFlags(ajn::MsgArg::OwnsData);
    }
    else {
        delete [] v_bool;
    }

    return NOERROR;
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ ArrayOf<Int16>* arg)
{
    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature,  arg->GetLength(), arg->GetPayload());
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->Stabilize();
    }

    return NOERROR;
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ ArrayOf<Int32>* arg)
{
    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature,  arg->GetLength(), arg->GetPayload());
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->Stabilize();
    }

    return NOERROR;
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ ArrayOf<Int64>* arg)
{
    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature,  arg->GetLength(), arg->GetPayload());
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->Stabilize();
    }

    return NOERROR;
}

ECode MsgArg::Set(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ ArrayOf<Double>* arg)
{
    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature,  arg->GetLength(), arg->GetPayload());
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->Stabilize();
    }

    return NOERROR;
}

ECode MsgArg::SetArray(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& elemSig,
    /* [in] */ Int32 numElements)
{
    ajn::MsgArg* arg = (ajn::MsgArg*)_msgArg;

    ajn::MsgArg* elements = new ajn::MsgArg[numElements];
    if (!elements) {
        return E_STATUS_OUT_OF_MEMORY;
    }

    QStatus status = arg->v_array.SetElements(elemSig.string(), numElements, elements);
    if (ER_OK != status) {
        delete [] elements;
        return status;
    }
    arg->SetOwnershipFlags(ajn::MsgArg::OwnsArgs);
    arg->typeId = ajn::ALLJOYN_ARRAY;
    return NOERROR;
}

ECode MsgArg::SetStruct(
    /* [in] */ Int64 _msgArg,
    /* [in] */ Int32 numMembers)
{
    ajn::MsgArg* arg = (ajn::MsgArg*)_msgArg;

    ajn::MsgArg* members = new ajn::MsgArg[numMembers];
    if (!members) {
        return E_STATUS_OUT_OF_MEMORY;
    }

    arg->v_struct.numMembers = numMembers;
    arg->v_struct.members = members;
    arg->SetOwnershipFlags(ajn::MsgArg::OwnsArgs);
    arg->typeId = ajn::ALLJOYN_STRUCT;
    return NOERROR;
}

ECode MsgArg::SetDictEntry(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* arg = (ajn::MsgArg*)_msgArg;
    ajn::MsgArg* key = new ajn::MsgArg;
    ajn::MsgArg* val = new ajn::MsgArg;
    if (!key || !val) {
        delete val;
        delete key;
        return E_STATUS_OUT_OF_MEMORY;
    }
    arg->v_dictEntry.key = key;
    arg->v_dictEntry.val = val;
    arg->SetOwnershipFlags(ajn::MsgArg::OwnsArgs);
    arg->typeId = ajn::ALLJOYN_DICT_ENTRY;
    return NOERROR;
}

ECode MsgArg::SetVariant(
    /* [in] */ Int64 _msgArg,
    /* [in] */ const String& signature,
    /* [in] */ Int64 val)
{
    ajn::MsgArg* value = new ajn::MsgArg(*(ajn::MsgArg*)val);
    if (!value) {
        return E_STATUS_OUT_OF_MEMORY;
    }

    ECode ec = SetV((ajn::MsgArg*)_msgArg, signature, value);
    if (ec == E_STATUS_OK) {
        ((ajn::MsgArg*)_msgArg)->SetOwnershipFlags(ajn::MsgArg::OwnsArgs);
    }

    return NOERROR;
}

ECode MsgArg::SetVariant(
    /* [in] */ Int64 _msgArg)
{
    ajn::MsgArg* arg = (ajn::MsgArg*)_msgArg;

    ajn::MsgArg* val = new ajn::MsgArg;
    if (!val) {
        delete val;
        return E_STATUS_OUT_OF_MEMORY;
    }

    arg->v_variant.val = val;
    arg->SetOwnershipFlags(ajn::MsgArg::OwnsArgs);
    arg->typeId = ajn::ALLJOYN_VARIANT;
    return NOERROR;
}

ECode MsgArg::Set(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& signature,
    /* [in] */ ArrayOf<IBoolean*>* arg)
{
    Int32 len = arg->GetLength();
    AutoPtr<ArrayOf<Boolean> > ab = ArrayOf<Boolean>::Alloc(len);
    for (Int32 i = 0; i < len; ++i) {
        (*arg)[i]->GetValue(&(*ab)[i]);
    }
    return Set(msgArg, signature, ab);
}

String MsgArg::GetSignature(
    /* [in] */ ArrayOf<Int64>* msgArgs)
{
    ajn::MsgArg* values = NULL;
    size_t numValues = msgArgs ? msgArgs->GetLength() : 0;
    if (numValues) {
        values = new ajn::MsgArg[numValues];
        if (!values) {
            assert(0 && "E_STATUS_OUT_OF_MEMORY");
            return String(NULL);
        }

        for (size_t i = 0; i < numValues; ++i) {
            values[i] = *(ajn::MsgArg*)((*msgArgs)[i]);
        }
    }

    String signature(ajn::MsgArg::Signature(values, numValues).c_str());
    delete [] values;
    return signature;
}

template<typename ElementType, typename InterfaceType>
static AutoPtr<ArrayOf<ElementType> > ConvertArrayOfObject(
    /* [in] */ IArrayOf* arrayOf)
{
    assert(arrayOf != NULL);
    Int32 length;
    arrayOf->GetLength(&length);
    AutoPtr<ArrayOf<ElementType> > array = ArrayOf<ElementType>::Alloc(length);
    InterfaceID iid;
    arrayOf->GetTypeId(&iid);

    assert(iid == EIID_IByte
        || iid == EIID_IBoolean
        || iid == EIID_IInteger16
        || iid == EIID_IInteger32
        || iid == EIID_IInteger64
        || iid == EIID_IDouble);

    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> obj;
        arrayOf->Get(i, (IInterface**)&obj);
        InterfaceType* elementObj = (InterfaceType*)obj->Probe(iid);
        ElementType element;
        elementObj->GetValue(&element);
        array->Set(i, element);
    }
    return array;
}

static AutoPtr<IArrayOf> ConvertArrayOf(
    /* [in] */ ArrayOf<Byte>* arrayOf)
{
    Int32 length = arrayOf->GetLength();
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, length, (IArrayOf**)&array);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IByte> obj = CoreUtils::ConvertByte((*arrayOf)[i]);
        array->Set(i, obj);
    }
    return array;
}

template<typename ElementType, typename InterfaceType>
static AutoPtr<IArrayOf> ConvertArrayOf(
    /* [in] */ ArrayOf<ElementType>* arrayOf,
    /* [in] */ const InterfaceID& iid)
{
    assert(arrayOf != NULL);
    assert(iid == EIID_IBoolean
        || iid == EIID_IInteger16
        || iid == EIID_IInteger32
        || iid == EIID_IInteger64
        || iid == EIID_IDouble
        || iid == EIID_ICharSequence);

    Int32 length = arrayOf->GetLength();
    AutoPtr<IArrayOf> array;
    CArrayOf::New(iid, length, (IArrayOf**)&array);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<InterfaceType> obj = CoreUtils::Convert((*arrayOf)[i]);
        array->Set(i, obj);
    }
    return array;
}

InterfaceID MsgArg::GetInterfaceIDByTypeId(
    /* [in] */ Char32 typeId)
{
    switch (typeId) {
        case ALLJOYN_BYTE:      return EIID_IByte;
        case ALLJOYN_BOOLEAN:   return EIID_IBoolean;
        case ALLJOYN_INT16:     return EIID_IInteger16;
        case ALLJOYN_UINT16:    return EIID_IInteger16;
        case ALLJOYN_INT32:     return EIID_IInteger32;
        case ALLJOYN_UINT32:    return EIID_IInteger32;
        case ALLJOYN_INT64:     return EIID_IInteger64;
        case ALLJOYN_UINT64:    return EIID_IInteger64;
        case ALLJOYN_DOUBLE:    return EIID_IDouble;
        case ALLJOYN_STRING:    return EIID_ICharSequence;
        default: {
            Logger::I("MsgArg", "GetInterfaceIDByTypeId: invalid typeId: %c", typeId);
            assert(0 && "invalid typeId");
            break;
        }
    }
    return EIID_IInterface;
}

ECode MsgArg::MarshalInterface(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ IInterface* arg)
{
    if (DEBUG_MAP) {
        Logger::D(TAG, "MarshalInterface: signature: '%s', arg: %s", sig.string(), TO_CSTR(arg));
    }

    switch (sig.GetChar(0)) {
        case ALLJOYN_BYTE: {
            IByte* obj = IByte::Probe(arg);
            if (!obj) {
                Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                assert(0 && "invalid signature or argument");
                return E_FAIL;
            }
            Byte value;
            obj->GetValue(&value);
            Set(msgArg, sig, value);
            break;
        }
        case ALLJOYN_BOOLEAN: {
            IBoolean* obj = IBoolean::Probe(arg);
            if (!obj) {
                Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                assert(0 && "invalid signature or argument");
                return E_FAIL;
            }
            Boolean value;
            obj->GetValue(&value);
            SetBoolean(msgArg, sig, value);
            break;
        }
        case ALLJOYN_INT16:
        case ALLJOYN_UINT16: {
            IInteger16* obj = IInteger16::Probe(arg);
            if (!obj) {
                Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                assert(0 && "invalid signature or argument");
                return E_FAIL;
            }
            Int16 value;
            obj->GetValue(&value);
            Set(msgArg, sig, value);
            break;
        }
        case ALLJOYN_INT32:
        case ALLJOYN_UINT32: {
            IInteger32* obj = IInteger32::Probe(arg);
            if (!obj) {
                Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                assert(0 && "invalid signature or argument");
                return E_FAIL;
            }
            Int32 value;
            obj->GetValue(&value);
            Set(msgArg, sig, value);
            break;
        }
        case ALLJOYN_INT64:
        case ALLJOYN_UINT64: {
            IInteger64* obj = IInteger64::Probe(arg);
            if (!obj) {
                Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                assert(0 && "invalid signature or argument");
                return E_FAIL;
            }
            Int64 value;
            obj->GetValue(&value);
            Set(msgArg, sig, value);
            break;
        }
        case ALLJOYN_DOUBLE: {
            IDouble* obj = IDouble::Probe(arg);
            if (!obj) {
                Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                assert(0 && "invalid signature or argument");
                return E_FAIL;
            }
            Double value;
            obj->GetValue(&value);
            Set(msgArg, sig, value);
            break;
        }
        case ALLJOYN_STRING:
        case ALLJOYN_SIGNATURE:
        case ALLJOYN_OBJECT_PATH: {
            ICharSequence* obj = ICharSequence::Probe(arg);
            if (!obj) {
                Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                assert(0 && "invalid signature or argument");
                return E_FAIL;
            }
            String value;
            obj->ToString(&value);
            Set(msgArg, sig, value);
            break;
        }

        case ALLJOYN_ARRAY: {
            String elemSig = sig.Substring(1);
            Char32 elementTypeId = elemSig.GetChar(0);

            IArrayOf* arrayOf = IArrayOf::Probe(arg);
            if (elementTypeId != ALLJOYN_DICT_ENTRY_OPEN) {
                if (arrayOf == NULL) {
                    Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                    assert(0 && "invalid signature or argument");
                    return E_FAIL;
                }
            }

            if (DEBUG_MAP) {
                Logger::I(TAG, " marshal array of %c with sig: %s", elementTypeId, sig.string());
            }

            switch (elementTypeId) {
                case ALLJOYN_BYTE: {
                    AutoPtr<ArrayOf<Byte> > array = ConvertArrayOfObject<Byte, IByte>(arrayOf);
                    Set(msgArg, sig, array);
                    break;
                }
                case ALLJOYN_BOOLEAN: {
                    AutoPtr<ArrayOf<Boolean> > array = ConvertArrayOfObject<Boolean, IBoolean>(arrayOf);
                    Set(msgArg, sig, array);
                    break;
                }
                case ALLJOYN_INT16:
                case ALLJOYN_UINT16: {
                    AutoPtr<ArrayOf<Int16> > array = ConvertArrayOfObject<Int16, IInteger16>(arrayOf);
                    Set(msgArg, sig, array);
                    break;
                }
                case ALLJOYN_INT32:
                case ALLJOYN_UINT32: {
                    AutoPtr<ArrayOf<Int32> > array = ConvertArrayOfObject<Int32, IInteger32>(arrayOf);
                    Set(msgArg, sig, array);
                    break;
                }
                case ALLJOYN_INT64:
                case ALLJOYN_UINT64: {
                    AutoPtr<ArrayOf<Int64> > array = ConvertArrayOfObject<Int64, IInteger64>(arrayOf);
                    Set(msgArg, sig, array);
                    break;
                }
                case ALLJOYN_DOUBLE: {
                    AutoPtr<ArrayOf<Double> > array = ConvertArrayOfObject<Double, IDouble>(arrayOf);
                    Set(msgArg, sig, array);
                    break;
                }

                case ALLJOYN_STRING: {
                    Int32 length;
                    arrayOf->GetLength(&length);
                    SetArray(msgArg, elemSig, length);
                    length = GetNumElements(msgArg);
                    for (Int32 i = 0; i < length; ++i) {
                        AutoPtr<IInterface> obj;
                        arrayOf->Get(i, (IInterface**)&obj);
                        MarshalInterface(GetElement(msgArg, i), elemSig, obj);
                    }
                    break;
                }

                case ALLJOYN_DICT_ENTRY_OPEN: {
                    AutoPtr<ArrayOf<String> > sigs =  Signature::Split(elemSig.Substring(1, elemSig.GetLength() - 1));
                    if (sigs == NULL || sigs->GetLength() < 2) {
                        Logger::E(TAG, "MarshalBusException: cannot marshal map entry into '%s'", elemSig.string());
                        return E_FAIL;
                    }

                    AutoPtr<IMap> map = IMap::Probe(arg);
                    if (map == NULL) {
                        Logger::E(TAG, "invalid signature '%s' or argument '%s'", sig.string(), TO_CSTR(arg));
                        assert(0 && "invalid signature or argument");
                        return E_FAIL;
                    }

                    Int32 length;
                    map->GetSize(&length);
                    if (DEBUG_MAP) {
                        Logger::I(TAG, " >> Marshal map with %d elements: %s", length, TO_CSTR(map));
                    }

                    SetArray(msgArg, elemSig, length);

                    AutoPtr<ISet> set;
                    map->GetEntrySet((ISet**)&set);
                    AutoPtr<IIterator> it;
                    set->GetIterator((IIterator**)&it);
                    length = GetNumElements(msgArg);
                    Int32 i = 0;
                    Boolean hasNext;
                    IMapEntry* entry;
                    while ((it->HasNext(&hasNext), hasNext) && i < length) {
                        AutoPtr<IInterface> obj, key, value;;
                        it->GetNext((IInterface**)&obj);
                        entry = IMapEntry::Probe(obj);
                        entry->GetKey((IInterface**)&key);
                        entry->GetValue((IInterface**)&value);

                        Int64 elementMsgArg = GetElement(msgArg, i++);
                        SetDictEntry(elementMsgArg);
                        MarshalInterface(GetKey(elementMsgArg), (*sigs)[0], key);
                        MarshalInterface(GetVal(elementMsgArg), (*sigs)[1], value);
                    }
                    break;
                }

                default: {
                    Logger::D(TAG,"unimplemented ALLJOYN_ARRAY with signature : %s", sig.string());
                    assert(0 && "TODO");
                    break;
                }
            }
            break;
        }

        case ALLJOYN_STRUCT_OPEN: {
            Logger::D(TAG, "unimplemented '%s'", sig.string());
            assert(0 && "TODO");
            // Object[] args = Signature.structArgs(arg);
            // String[] memberSigs = Signature.split(sig.substring(1, sig.length() - 1));
            // if (memberSigs == null) {
            //     throw new MarshalBusException("cannot marshal " + arg.getClass() + " into '"
            //                                   + sig + "'");
            // }
            // SetStruct(msgArg, memberSigs.length);
            // for (int i = 0; i < getNumMembers(msgArg); ++i) {
            //     marshal(getMember(msgArg, i), memberSigs[i], args[i]);
            break;
        }

        case ALLJOYN_VARIANT: {
            Logger::D(TAG, "unimplemented '%s'", sig.string());
            assert(0 && "TODO");

            // CVariant* variant = (CVariant*)IVariant::Probe(arg);
            // assert(variant);
            // if (variant->GetMsgArg() != 0) {
            //     SetVariant(msgArg, sig, variant->GetMsgArg());
            // }
            // else {
            //     SetVariant(msgArg);
            //     String signature;
            //     variant->GetSignature(&signature);
            //     AutoPtr<CarValue> value = CarValue::Convert(variant->GetValue());
            //     Marshal(GetVal(msgArg), signature, value->ToValuePtr());
            // }
            break;
        }

        case ALLJOYN_DICT_ENTRY_OPEN: {
            assert(0 && "ALLJOYN_DICT_ENTRY_OPEN Should never happen!");
            break;
        }

        default: {
            Logger::D(TAG, "MarshalInterface unimplemented '%s'", sig.string());
            assert(0);
            break;
        }
    }
    return NOERROR;
}

ECode MsgArg::UnmarshalInterface(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Char32 typeId = GetTypeId(msgArg);
    if (DEBUG_MAP) {
        Logger::D(TAG, " >> UnmarshalInterface with signature: %s, typeId: %c", sig.string(), typeId);
    }

    switch (typeId) {
        case ALLJOYN_BYTE: {
            AutoPtr<IByte> value = CoreUtils::ConvertByte(GetByte(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_BOOLEAN: {
            AutoPtr<IBoolean> value = CoreUtils::Convert(GetBool(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_INT16:
        case ALLJOYN_UINT16: {
            AutoPtr<IInteger16> value = CoreUtils::Convert(GetInt16(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_INT32:
        case ALLJOYN_UINT32: {
            AutoPtr<IInteger32> value = CoreUtils::Convert(GetInt32(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_INT64:
        case ALLJOYN_UINT64: {
            AutoPtr<IInteger64> value = CoreUtils::Convert(GetInt64(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_DOUBLE: {
            AutoPtr<IDouble> value = CoreUtils::Convert(GetDouble(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_BOOLEAN_ARRAY:{
            AutoPtr<ArrayOf<Boolean> > array = GetBoolArray(msgArg);
            AutoPtr<IArrayOf> value = ConvertArrayOf<Boolean, IBoolean>(array, EIID_IBoolean);
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_BYTE_ARRAY: {
            AutoPtr<ArrayOf<Byte> > array = GetByteArray(msgArg);
            AutoPtr<IArrayOf> value = ConvertArrayOf(array);
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_INT16_ARRAY: {
            AutoPtr<ArrayOf<Int16> > array = GetInt16Array(msgArg);
            AutoPtr<IArrayOf> value = ConvertArrayOf<Int16, IInteger16>(array, EIID_IInteger16);
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_INT32_ARRAY: {
            AutoPtr<ArrayOf<Int32> > array = GetInt32Array(msgArg);
            AutoPtr<IArrayOf> value = ConvertArrayOf<Int32, IInteger32>(array, EIID_IInteger32);
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_INT64_ARRAY: {
            AutoPtr<ArrayOf<Int64> > array = GetInt64Array(msgArg);
            AutoPtr<IArrayOf> value = ConvertArrayOf<Int64, IInteger64>(array, EIID_IInteger64);
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_DOUBLE_ARRAY: {
            AutoPtr<ArrayOf<Double> > array = GetDoubleArray(msgArg);
            AutoPtr<IArrayOf> value = ConvertArrayOf<Double, IDouble>(array, EIID_IDouble);
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_STRING: {
            AutoPtr<ICharSequence> value = CoreUtils::Convert(GetString(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }
        case ALLJOYN_SIGNATURE: {
            AutoPtr<ICharSequence> value = CoreUtils::Convert(GetSignature(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }
        case ALLJOYN_OBJECT_PATH: {
            AutoPtr<ICharSequence> value = CoreUtils::Convert(GetObjPath(msgArg));
            *result = value;
            REFCOUNT_ADD(*result)
            break;
        }

        case ALLJOYN_ARRAY: {
            String eleSig = GetElemSig(msgArg);
            assert(eleSig.IsNullOrEmpty() == FALSE && "signature of Array cannot be null!");

            if (DEBUG_MAP) {
                Logger::I(TAG, " >> Unmarshal array: eleSig: %s", eleSig.string());
            }

            Char32 elementTypeId = eleSig.GetChar(0);
            if (elementTypeId ==ALLJOYN_STRING) {
                Int32 size = GetNumElements(msgArg);
                AutoPtr<IArrayOf> array;
                CArrayOf::New(EIID_ICharSequence, size, (IArrayOf**)&array);
                for (Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> obj;
                    UnmarshalInterface(GetElement(msgArg, i), eleSig, (IInterface**)&obj);
                    array->Set(i, obj);
                }

                *result = array.Get();
                REFCOUNT_ADD(*result)
            }
            else if (elementTypeId == ALLJOYN_DICT_ENTRY_OPEN) {
                AutoPtr<ArrayOf<String> > sigs =  Signature::Split(eleSig.Substring(1, eleSig.GetLength() - 1));
                if (sigs == NULL || sigs->GetLength() < 2) {
                    Logger::E(TAG, "Unmarshal map: cannot Unmarshal from '%s'", eleSig.string());
                    return E_FAIL;
                }

                Int32 size = GetNumElements(msgArg);
                if (DEBUG_MAP) {
                    Logger::I(TAG, " >> Unmarshal map with %d elements, key: '%s', value: '%s'",
                        size, (*sigs)[0].string(), (*sigs)[1].string());
                }

                AutoPtr<IMap> map;
                CHashMap::New(size, (IMap**)&map);
                for (Int32 i = 0; i < size; ++i) {
                    Int64 element  = GetElement(msgArg, i);
                    AutoPtr<IInterface> key, value;
                    UnmarshalInterface(GetKey(element), (*sigs)[0], (IInterface**)&key);
                    UnmarshalInterface(GetVal(element), (*sigs)[1], (IInterface**)&value);
                    if (DEBUG_MAP) {
                        Logger::I(TAG, "    > unmarshal item %d: key: %s, value: %s", i, TO_CSTR(key), TO_CSTR(value));
                    }
                    map->Put(key, value);
                }

                *result = map.Get();
                REFCOUNT_ADD(*result)
            }
            else {
                Logger::E(TAG, "Unmarshal unimplemented ALLJOYN_ARRAY item is %s", eleSig.string());
                assert(0);
            }
            break;
        }

        case ALLJOYN_STRUCT_OPEN: {
            assert(0 && "TODO");
            Logger::D(TAG, "unimplemented '%s'", sig.string());
            // Type[] types = Signature.structTypes((Class<?>) type);
            // if (types.length != getNumMembers(msgArg)) {
            //     throw new MarshalBusException(
            //         "cannot marshal '" + getSignature(new Int64[] { msgArg }) + "' with "
            //         + getNumMembers(msgArg) + " members into " + type + " with "
            //         + types.length + " fields");
            // }
            // object = ((Class<?>) type).newInstance();
            // Field[] fields = Signature.structFields((Class<?>) type);
            // for (int i = 0; i < getNumMembers(msgArg); ++i) {
            //     Object value = unmarshal(getMember(msgArg, i), types[i]);
            //     fields[i].set(object, value);
            // }
            // return object;
            Logger::E(TAG, "unimplemented ALLJOYN_STRUCT '%s'", GetSignature(msgArg).string());
            assert(0);
            break;
        }

        case ALLJOYN_VARIANT: {
            Logger::D(TAG, "unimplemented '%s'", sig.string());
            assert(0);

            // AutoPtr<CVariant> variant;
            // CVariant::NewByFriend((CVariant**)&variant);
            // variant->SetMsgArg(msgArg);
            // *(PInterface*)object = (IVariant*)variant;
            // variant->AddRef();
            break;
        }

        case ALLJOYN_DICT_ENTRY_OPEN: {
            assert(0 && "ALLJOYN_DICT_ENTRY_OPEN Should never happen!");
            break;
        }

        default: {
            Logger::E(TAG, "UnmarshalInterface unimplemented signature: '%s', typeId: %c", sig.string(), typeId);
            assert(0);
            break;
        }
    }

    if (DEBUG_MAP) {
        Logger::D(TAG, "UnmarshalInterface: signature: '%s', value: %s", sig.string(), TO_CSTR(*result));
    }
    return NOERROR;
}

ECode MsgArg::Unmarshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ CarDataType type,
    /* [out] */ PVoid object)
{
    VALIDATE_NOT_NULL(object)

    Char32 typeId = GetTypeId(msgArg);
    if (DEBUG_MAP) {
        Logger::D(TAG, "==== Marshal: %c ====", typeId);
    }

    switch (typeId) {
        case ALLJOYN_BOOLEAN:
            *(Boolean*)object = GetBool(msgArg);
            break;
        case ALLJOYN_BOOLEAN_ARRAY:{
                AutoPtr<ArrayOf<Boolean> > b = GetBoolArray(msgArg);
                *(PCarQuintet*)object = b;
                b->AddRef();
                break;
            }

        case ALLJOYN_BYTE:
            *(Byte*)object = GetByte(msgArg);
            break;
        case ALLJOYN_BYTE_ARRAY: {
            AutoPtr<ArrayOf<Byte> > b = GetByteArray(msgArg);
            *(PCarQuintet*)object = b;
            b->AddRef();
            break;
        }

        case ALLJOYN_INT16:
            *(Int16*)object = GetInt16(msgArg);
            break;
        case ALLJOYN_INT16_ARRAY: {
            AutoPtr<ArrayOf<Int16> > array = GetInt16Array(msgArg);
            *(PCarQuintet*)object = array;
            array->AddRef();
            break;
        }

        case ALLJOYN_INT32:
            *(Int32*)object = GetInt32(msgArg);
            break;
        case ALLJOYN_INT32_ARRAY: {
            AutoPtr<ArrayOf<Int32> > array = GetInt32Array(msgArg);
            *(PCarQuintet*)object = array;
            array->AddRef();
            break;
        }

        case ALLJOYN_INT64:
            *(Int64*)object = GetInt64(msgArg);
            break;
        case ALLJOYN_INT64_ARRAY: {
            AutoPtr<ArrayOf<Int64> > array = GetInt64Array(msgArg);
            *(PCarQuintet*)object = array;
            array->AddRef();
            break;
        }

        case ALLJOYN_UINT16:
            *(Int16*)object = GetUint16(msgArg);
            break;
        case ALLJOYN_UINT16_ARRAY: {
            AutoPtr<ArrayOf<Int16> > array = GetUint16Array(msgArg);
            *(PCarQuintet*)object = array;
            array->AddRef();
            break;
        }

        case ALLJOYN_UINT32:
            *(Int32*)object = GetUint32(msgArg);
            break;
        case ALLJOYN_UINT32_ARRAY: {
            AutoPtr<ArrayOf<Int32> > array = GetUint32Array(msgArg);
            *(PCarQuintet*)object = array;
            array->AddRef();
            break;
        }

        case ALLJOYN_UINT64:
            *(Int64*)object = GetUint64(msgArg);
            break;
        case ALLJOYN_UINT64_ARRAY: {
            AutoPtr<ArrayOf<Int64> > array = GetUint64Array(msgArg);
            *(PCarQuintet*)object = array;
            array->AddRef();
        }
        break;

        case ALLJOYN_DOUBLE:
            *(Double*)object = GetDouble(msgArg);
            break;
        case ALLJOYN_DOUBLE_ARRAY: {
            AutoPtr<ArrayOf<Double> > array = GetDoubleArray(msgArg);
            *(PCarQuintet*)object = array;
            array->AddRef();
            break;
        }

        case ALLJOYN_OBJECT_PATH:
            *(String*)object = GetObjPath(msgArg);
            break;
        case ALLJOYN_SIGNATURE:
            *(String*)object = GetSignature(msgArg);
            break;
        case ALLJOYN_STRING:
            *(String*)object = GetString(msgArg);
            break;

        case ALLJOYN_ARRAY: {
            String sig = GetElemSig(msgArg);
            assert(!sig.IsNullOrEmpty() && "signature of Array cannot be null!");

            Char32 elementTypeId = sig.GetChar(0);
            if (elementTypeId == ALLJOYN_DICT_ENTRY_OPEN) {
                if (DEBUG_MAP) {
                    Logger::I(TAG, " >> Unmarshal map: sig: %s", sig.string());
                }
                AutoPtr<ArrayOf<String> > sigs =  Signature::Split(sig.Substring(1, sig.GetLength() - 1));
                if (sigs == NULL || sigs->GetLength() < 2) {
                    Logger::E(TAG, "Unmarshal map: cannot Unmarshal from '%s'", sig.string());
                    return E_FAIL;
                }

                Int32 size = GetNumElements(msgArg);
                if (DEBUG_MAP) {
                    Logger::I(TAG, " >> Unmarshal map with %d elements, key: '%s', value: '%s'",
                        size, (*sigs)[0].string(), (*sigs)[1].string());
                }

                AutoPtr<IMap> map;
                CHashMap::New(size, (IMap**)&map);
                for (Int32 i = 0; i < size; ++i) {
                    Int64 element  = GetElement(msgArg, i);
                    AutoPtr<IInterface> key, value;
                    UnmarshalInterface(GetKey(element), (*sigs)[0], (IInterface**)&key);
                    UnmarshalInterface(GetVal(element), (*sigs)[1], (IInterface**)&value);
                    if (DEBUG_MAP) {
                        Logger::I(TAG, "    > unmarshal item %d: key: %s, value: %s", i, TO_CSTR(key), TO_CSTR(value));
                    }
                    map->Put(key, value);
                }

                *(PInterface*)object = map.Get();
                map->AddRef();

                if (DEBUG_MAP) {
                    Logger::I(TAG, " >> Unmarshal map: %s", TO_CSTR(map));
                }
                return NOERROR;
            }
            else if (elementTypeId == ALLJOYN_STRING) {
                Int32 size = GetNumElements(msgArg);
                AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(size);
                for (Int32 i = 0; i < size; ++i) {
                    Unmarshal(GetElement(msgArg, i), CarDataType_String, (PVoid)&(*array)[i]);
                }
                *(PCarQuintet*)object = array;
                array->AddRef();
            }
            else {
                Logger::E(TAG, "Unmarshal unimplemented ALLJOYN_ARRAY item is %s", sig.string());
                assert(0);
            }
            break;
        }

        case ALLJOYN_STRUCT: {
            // Type[] types = Signature.structTypes((Class<?>) type);
            // if (types.length != getNumMembers(msgArg)) {
            //     throw new MarshalBusException(
            //         "cannot marshal '" + getSignature(new Int64[] { msgArg }) + "' with "
            //         + getNumMembers(msgArg) + " members into " + type + " with "
            //         + types.length + " fields");
            // }
            // object = ((Class<?>) type).newInstance();
            // Field[] fields = Signature.structFields((Class<?>) type);
            // for (int i = 0; i < getNumMembers(msgArg); ++i) {
            //     Object value = unmarshal(getMember(msgArg, i), types[i]);
            //     fields[i].set(object, value);
            // }
            // return object;
            Logger::E(TAG, "unimplemented ALLJOYN_STRUCT '%s'", GetSignature(msgArg).string());
            assert(0);
            break;
        }

        case ALLJOYN_VARIANT: {
            AutoPtr<CVariant> variant;
            CVariant::NewByFriend((CVariant**)&variant);
            variant->SetMsgArg(msgArg);
            *(PInterface*)object = (IVariant*)variant;
            variant->AddRef();
            break;
        }

        case ALLJOYN_DICT_ENTRY_OPEN:
            assert(0 && "ALLJOYN_DICT_ENTRY_OPEN Should never happen!");
            break;

        default: {
            Logger::E(TAG, "unimplemented '%s'", GetSignature(msgArg).string());
            assert(0);
            break;
        }
    }

    return NOERROR;
}

ECode MsgArg::UnmarshalIn(
    /* [in] */ IMethodInfo* method,
    /* [in] */ Int64 msgArgs,
    /* [out] */ IArgumentList** args)
{
    Int32 count = 0;
    method->GetParamCount(&count);
    AutoPtr<ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(count);
    method->GetAllParamInfos(paramInfos);
    Int32 inCount = 0;
    for (Int32 i = 0; i < count; i++) {
        ParamIOAttribute ioAttr = ParamIOAttribute_In;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        if (ioAttr == ParamIOAttribute_In)
            inCount++;
    }
    Boolean isStruct = ((ajn::MsgArg*)msgArgs)->typeId == ajn::ALLJOYN_STRUCT;
    Int32 numArgs = isStruct ? GetNumMembers(msgArgs) : 1;
    if (inCount != numArgs) {
        Logger::E(TAG, "cannot marshal %d args into %d parameters", numArgs, count);
        assert(0);
        // throw new MarshalBusException(
        //     "cannot marshal " + numArgs + " args into " + types.length + " parameters");
        return NOERROR;
    }
    AutoPtr<ArrayOf<CarValue*> > array = ArrayOf<CarValue*>::Alloc(count);
    method->CreateArgumentList(args);
    Int32 inIndex = 0;
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IDataTypeInfo> typeInfo;
        (*paramInfos)[i]->GetTypeInfo((IDataTypeInfo**)&typeInfo);
        CarDataType type;
        typeInfo->GetDataType(&type);
        ParamIOAttribute ioAttr;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        AutoPtr<CarValue> value = new CarValue(type);
        value->mIOAttribute = ioAttr;
        if (type == CarDataType_ArrayOf) {
            AutoPtr<IDataTypeInfo> elementTypeInfo;
            ICarArrayInfo::Probe(typeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementTypeInfo);
            elementTypeInfo->GetDataType(&value->mElementType);
        }
        if (ioAttr == ParamIOAttribute_In) {
            PVoid arg = value->ToValuePtr();
            if (FAILED(Unmarshal(isStruct ? GetMember(msgArgs, inIndex++) : msgArgs, type, arg)))
                continue;
        }
        array->Set(i, value);
        value->SetToArgumentList(*args, i);
    }
    AutoLock lock(sLock);
    sRecords[*args] = array;
    return NOERROR;
}

ECode MsgArg::UnmarshalOut(
    /* [in] */ Int64 msgArgs,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    Int32 count = 0;
    method->GetParamCount(&count);
    AutoPtr<ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(count);
    method->GetAllParamInfos(paramInfos);
    Int32 typeId = GetTypeId(msgArgs);
    Int32 outCount = 0;
    for (Int32 i = 0; i < count; i++) {
        ParamIOAttribute ioAttr = ParamIOAttribute_In;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        if (ioAttr != ParamIOAttribute_In)
            outCount++;
    }
    if (typeId == ALLJOYN_STRUCT) {
        Int32 numArgs = GetNumMembers(msgArgs);
        if (outCount != numArgs) {
            Logger::E(TAG, "UnmarshalOut: cannot marshal %d args into %d parameters", numArgs, outCount);
            assert(0);
            // throw new MarshalBusException(
            //     "cannot marshal " + numArgs + " args into " + types.length + " parameters");
            return NOERROR;
        }
    }
    else if (outCount != 1) {
        assert(0);
    }

    Int32 outIndex = 0;
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IDataTypeInfo> typeInfo;
        (*paramInfos)[i]->GetTypeInfo((IDataTypeInfo**)&typeInfo);
        ParamIOAttribute ioAttr;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        if (ioAttr == ParamIOAttribute_In)
            continue;

        Boolean isNull = FALSE;
        args->IsOutputArgumentNullPtr(i, &isNull);
        if (isNull) {
            outIndex++;
            continue;
        }

        CarDataType type;
        typeInfo->GetDataType(&type);
        AutoPtr<CarValue> value = new CarValue(type);
        value->mIOAttribute = ioAttr;
        if (type == CarDataType_ArrayOf) {
            AutoPtr<IDataTypeInfo> elementTypeInfo;
            ICarArrayInfo::Probe(typeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementTypeInfo);
            elementTypeInfo->GetDataType(&value->mElementType);
        }
        PVoid arg = value->ToValuePtr();
        Int32 msgArg = typeId == ALLJOYN_STRUCT ? GetMember(msgArgs, outIndex) : msgArgs;
        if (SUCCEEDED(Unmarshal(msgArg, type, arg)))
            value->AssignArgumentListOutput(args, i);
        outIndex++;
    }
    return NOERROR;
}

ECode MsgArg::Marshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ PVoid arg)
{
    if (DEBUG_MAP) {
        Logger::D(TAG, "==== Marshal: %s ====", sig.string());
    }

    switch (sig.GetChar(0)) {
        case ALLJOYN_BYTE:
            Set(msgArg, sig, *(Byte*)arg);
            break;
        case ALLJOYN_BOOLEAN:
            SetBoolean(msgArg, sig, *(Boolean*)arg);
            break;
        case ALLJOYN_INT16:
        case ALLJOYN_UINT16:
            Set(msgArg, sig, *(Int16*)arg);
            break;
        case ALLJOYN_INT32:
        case ALLJOYN_UINT32:
            Set(msgArg, sig, *(Int32*)arg);
            break;
        case ALLJOYN_INT64:
        case ALLJOYN_UINT64:
            Set(msgArg, sig, *(Int64*)arg);
            break;
        case ALLJOYN_DOUBLE:
            Set(msgArg, sig, *(Double*)arg);
            break;

        case ALLJOYN_STRING:
        case ALLJOYN_SIGNATURE:
        case ALLJOYN_OBJECT_PATH: {
            if (arg == NULL) {
                Logger::E(TAG, "line %d: cannot marshal NULL into '%s'", __LINE__, sig.string());
                assert(0);
                break;
            }
            Set(msgArg, sig, *(String*)arg);
            break;
        }

        case ALLJOYN_ARRAY: {
            if (arg == NULL) {
                Logger::E(TAG, "line %d: cannot marshal NULL into '%s'", __LINE__, sig.string());
                assert(0);
                break;
            }

            Char32 elementTypeId = sig.GetChar(1);
            switch (elementTypeId) {
                case ALLJOYN_BYTE:
                    Set(msgArg, sig, *(ArrayOf<Byte>**)arg);
                    break;
                case ALLJOYN_BOOLEAN:
                    Set(msgArg, sig, *(ArrayOf<Boolean>**)arg);
                    break;
                case ALLJOYN_INT16:
                case ALLJOYN_UINT16:
                    Set(msgArg, sig, *(ArrayOf<Int16>**)arg);
                    break;
                case ALLJOYN_INT32:
                case ALLJOYN_UINT32:
                    Set(msgArg, sig, *(ArrayOf<Int32>**)arg);
                    break;
                case ALLJOYN_INT64:
                case ALLJOYN_UINT64:
                    Set(msgArg, sig, *(ArrayOf<Int64>**)arg);
                    break;
                case ALLJOYN_DOUBLE:
                    Set(msgArg, sig, *(ArrayOf<Double>**)arg);
                    break;

                case ALLJOYN_STRING: {
                    String elemSig = sig.Substring(1);
                    AutoPtr<ArrayOf<String> > args = *(ArrayOf<String>**)arg;
                    SetArray(msgArg, elemSig, args->GetLength());
                    for (Int32 i = 0; i < GetNumElements(msgArg); ++i) {
                        Marshal(GetElement(msgArg, i), elemSig, (PVoid)&(*args)[i]);
                    }
                    break;
                }

                default: {
                    if (elementTypeId == ALLJOYN_DICT_ENTRY_OPEN) {
                        if (DEBUG_MAP) {
                            Logger::D(TAG," >> Marshal map with signature : %s", sig.string());
                        }

                        AutoPtr<IMap> map = IMap::Probe(*(IInterface**)arg);
                        Int32 length;
                        map->GetSize(&length);
                        if (DEBUG_MAP) {
                            Logger::I(TAG, " >> Marshal map with %d elements: %s", length, TO_CSTR(map));
                        }

                        String elemSig = sig.Substring(1);
                        AutoPtr<ArrayOf<String> > sigs =  Signature::Split(elemSig.Substring(1, elemSig.GetLength() - 1));
                        if (sigs == NULL || sigs->GetLength() < 2) {
                            Logger::E(TAG, "MarshalBusException: cannot marshal map entry into '%s'", elemSig.string());
                            return E_FAIL;
                        }

                        SetArray(msgArg, elemSig, length);

                        AutoPtr<ISet> set;
                        map->GetEntrySet((ISet**)&set);
                        AutoPtr<IIterator> it;
                        set->GetIterator((IIterator**)&it);
                        length = GetNumElements(msgArg);
                        Int32 i = 0;
                        Boolean hasNext;
                        IMapEntry* entry;
                        while ((it->HasNext(&hasNext), hasNext) && i < length) {
                            AutoPtr<IInterface> obj, key, value;
                            it->GetNext((IInterface**)&obj);
                            entry = IMapEntry::Probe(obj);
                            entry->GetKey((IInterface**)&key);
                            entry->GetValue((IInterface**)&value);

                            Int64 elementMsgArg = GetElement(msgArg, i++);
                            SetDictEntry(elementMsgArg);
                            MarshalInterface(GetKey(elementMsgArg), (*sigs)[0], key);
                            MarshalInterface(GetVal(elementMsgArg), (*sigs)[1], value);
                        }
                    }
                    else {
                        Logger::D(TAG,"Marshal unimplemented ALLJOYN_ARRAY with signature : %s", sig.string());
                        assert(0 && "TODO");
                    }
                }
                break;
            }
            break;
        }

        case ALLJOYN_STRUCT_OPEN: {
            Logger::D(TAG,"Marshal unimplemented ALLJOYN_STRUCT_OPEN with signature : %s", sig.string());
            assert(0 && "TODO");
            // Object[] args = Signature.structArgs(arg);
            // String[] memberSigs = Signature.split(sig.substring(1, sig.length() - 1));
            // if (memberSigs == null) {
            //     throw new MarshalBusException("cannot marshal " + arg.getClass() + " into '"
            //                                   + sig + "'");
            // }
            // SetStruct(msgArg, memberSigs.length);
            // for (int i = 0; i < getNumMembers(msgArg); ++i) {
            //     marshal(getMember(msgArg, i), memberSigs[i], args[i]);
            // }
            break;
        }
        case ALLJOYN_VARIANT: {
            CVariant* variant = (CVariant*)IVariant::Probe(*(PInterface*)arg);
            assert(variant);
            if (variant->GetMsgArg() != 0) {
                SetVariant(msgArg, sig, variant->GetMsgArg());
            }
            else {
                SetVariant(msgArg);
                String signature;
                variant->GetSignature(&signature);
                AutoPtr<CarValue> value = CarValue::Convert(variant->GetValue());
                Marshal(GetVal(msgArg), signature, value->ToValuePtr());
            }
            break;
        }

        case ALLJOYN_DICT_ENTRY_OPEN: {
            assert(0 && "ALLJOYN_DICT_ENTRY_OPEN Should never happen!");
            break;
        }

        default: {
            Logger::D(TAG, "unimplemented '%s'", sig.string());
            assert(0);
            break;
        }
    }

    return NOERROR;
}

ECode MsgArg::MarshalIn(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    AutoPtr<ArrayOf<String> > sigs = Signature::Split(sig);
    if (sigs == NULL) {
        Logger::E(TAG, "cannot marshal args into '%s', bad signature", sig.string());
        assert(0);
        // throw new MarshalBusException("cannot marshal args into '" + sig + "', bad signature");
    }

    Int32 count = 0;
    method->GetParamCount(&count);
    AutoPtr<ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(count);
    method->GetAllParamInfos(paramInfos);
    Int32 inCount = 0;
    for (Int32 i = 0; i < count; i++) {
        ParamIOAttribute ioAttr = ParamIOAttribute_In;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        if (ioAttr == ParamIOAttribute_In)
            inCount++;
    }
    SetStruct(msgArg, inCount);
    Int32 inIndex = 0;
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IDataTypeInfo> typeInfo;
        (*paramInfos)[i]->GetTypeInfo((IDataTypeInfo**)&typeInfo);
        ParamIOAttribute ioAttr;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        if (ioAttr != ParamIOAttribute_In)
            continue;
        CarDataType type;
        typeInfo->GetDataType(&type);
        AutoPtr<CarValue> value = new CarValue(type);
        if (type == CarDataType_ArrayOf) {
            AutoPtr<IDataTypeInfo> elementTypeInfo;
            ICarArrayInfo::Probe(typeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementTypeInfo);
            elementTypeInfo->GetDataType(&value->mElementType);
        }
        value->GetFromArgumentList(args, i);
        PVoid arg = value->ToValuePtr();
        Marshal(GetMember(msgArg, inIndex), (*sigs)[inIndex], arg);
        inIndex++;
    }
    return NOERROR;
}

ECode MsgArg::MarshalOut(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    AutoPtr<ArrayOf<String> > sigs = Signature::Split(sig);
    if (sigs == NULL) {
        Logger::E(TAG, "cannot marshal args into '%s', bad signature", sig.string());
        assert(0);
        // throw new MarshalBusException("cannot marshal args into '" + sig + "', bad signature");
        return NOERROR;
    }
    Int32 count = 0;
    method->GetParamCount(&count);
    AutoPtr<ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(count);
    method->GetAllParamInfos(paramInfos);
    Int32 outCount = 0;
    for (Int32 i = 0; i < count; i++) {
        ParamIOAttribute ioAttr = ParamIOAttribute_In;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        if (ioAttr != ParamIOAttribute_In)
            outCount++;
    }
    AutoPtr<ArrayOf<CarValue*> > array;
    if (outCount > 0) {
        AutoLock lock(sLock);
        RecordMap::Iterator it = sRecords.Find(args);
        if (it == sRecords.End()) {
            Logger::E(TAG, "MarshalOut() cannot find record");
            assert(0);
            return NOERROR;
        }
        array = it->mSecond;
    }
    SetStruct(msgArg, outCount);
    Int32 outIndex = 0;
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IDataTypeInfo> typeInfo;
        (*paramInfos)[i]->GetTypeInfo((IDataTypeInfo**)&typeInfo);
        ParamIOAttribute ioAttr;
        (*paramInfos)[i]->GetIOAttribute(&ioAttr);
        if (ioAttr == ParamIOAttribute_In)
            continue;
        AutoPtr<CarValue> value = (*array)[i];
        assert(value);
        PVoid arg = value->ToValuePtr();
        Marshal(GetMember(msgArg, outIndex), (*sigs)[outIndex], arg);
        outIndex++;
    }

    return NOERROR;
}

ECode MsgArg::ReleaseRecord(
    /* [in] */ IArgumentList* args)
{
    if (args == NULL)
        return NOERROR;

    AutoLock lock(sLock);
    sRecords.Erase(args);
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
