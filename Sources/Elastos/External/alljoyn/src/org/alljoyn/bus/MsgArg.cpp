
#include "org/alljoyn/bus/MsgArg.h"
#include "_Org.Alljoyn.Bus.h"
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/MsgArg.h>
#include <alljoyn/MsgArgUtils.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

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

/**
 * Unmarshals a native MsgArg into a Java object.
 *
 * @param msgArg the native MsgArg pointer
 * @param type the Java type to unmarshal into
 * @return the unmarshalled Java object
 * @throws MarshalBusException if the unmarshalling fails
 */
// @SuppressWarnings("unchecked")
ECode MsgArg::Unmarshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ CarDataType type,
    /* [out] */ PVoid object)
{
    // try {
        switch (GetTypeId(msgArg)) {
        case ALLJOYN_ARRAY:
            // if (GetElemSig(msgArg).GetChar(0) == ALLJOYN_DICT_ENTRY_OPEN) {
            //     Type rawType = ((ParameterizedType) type).getRawType();
            //     rawType = (rawType == Map.class) ? HashMap.class : rawType;
            //     object = ((Class<?>) rawType).newInstance();
            //     for (int i = 0; i < getNumElements(msgArg); ++i) {
            //         Int64 element  = getElement(msgArg, i);
            //         Type[] typeArgs = ((ParameterizedType) type).getActualTypeArguments();
            //         // TODO Can't seem to get it to suppress the warning here...
            //         ((Map<Object, Object>) object).put(unmarshal(getKey(element), typeArgs[0]),
            //                                            unmarshal(getVal(element), typeArgs[1]));
            //     }
            //     return object;
            // } else {
            //     Type componentType = (type instanceof GenericArrayType)
            //         ? ((GenericArrayType) type).getGenericComponentType()
            //         : ((Class<?>) type).getComponentType();
            //     Class<?> componentClass;
            //     if (componentType instanceof ParameterizedType) {
            //         Type rawType = ((ParameterizedType) componentType).getRawType();
            //         rawType = (rawType == Map.class) ? HashMap.class : rawType;
            //         componentClass = (Class<?>) rawType;
            //     } else {
            //         componentClass = (Class<?>) componentType;
            //     }
            //     object = Array.newInstance(componentClass, getNumElements(msgArg));
            //     for (int i = 0; i < getNumElements(msgArg); ++i) {
            //         /*
            //          * Under Sun the Array.set() is sufficient to check the
            //          * type.  Under Android that is not the case.
            //          */
            //         Object component = unmarshal(getElement(msgArg, i), componentType);
            //         if (!componentClass.isInstance(component)) {
            //             throw new IllegalArgumentException("argument type mismatch");
            //         }
            //         Array.set(object, i, component);
            //     }
            //     return object;
            // }
            {
                Int32 size = GetNumElements(msgArg);
                AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
                for (Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> component;
                    Unmarshal(GetElement(msgArg, i), CarDataType_Interface, (PVoid)&component);
                    array->Set(i, component);
                }
                *(ArrayOf<IInterface*>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_BOOLEAN:
            *(Boolean*)object = GetBool(msgArg);
            break;
        case ALLJOYN_BOOLEAN_ARRAY:
            {
                AutoPtr<ArrayOf<Boolean> > b = GetBoolArray(msgArg);
                // Class<?> componentClass = ((Class<?>) type).getComponentType();
                // if (componentClass == Boolean.class) {
                //     Boolean[] B = new Boolean[b.length];
                //     for (int i = 0; i < b.length; ++i) {
                //         B[i] = b[i];
                //     }
                //     return B;
                // }
                *(ArrayOf<Boolean>**)object = b;
                b->AddRef();
            }
            break;
        case ALLJOYN_BYTE:
            {
                // object = getEnumObject(type, (int) getByte(msgArg));
                // if (object == null) {
                    *(Byte*)object = GetByte(msgArg);
                // }
            }
            break;
        case ALLJOYN_BYTE_ARRAY:
            {
                AutoPtr<ArrayOf<Byte> > b = GetByteArray(msgArg);
                *(ArrayOf<Byte>**)object = b;
                b->AddRef();
            }
            break;
        case ALLJOYN_DOUBLE:
            *(Double*)object = GetBool(msgArg);
            break;
        case ALLJOYN_DOUBLE_ARRAY:
            {
                AutoPtr<ArrayOf<Double> > array = GetDoubleArray(msgArg);
                *(ArrayOf<Double>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_INT16:
            {
                // object = getEnumObject(type, (int) getInt16(msgArg));
                // if (object == null) {
                    *(Int16*)object = GetInt16(msgArg);
                // }
            }
            break;
        case ALLJOYN_INT16_ARRAY:
            {
                AutoPtr<ArrayOf<Int16> > array = GetInt16Array(msgArg);
                *(ArrayOf<Int16>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_INT32:
            {
                // object = getEnumObject(type, getInt32(msgArg));
                // if (object == null) {
                    *(Int32*)object = GetInt32(msgArg);
                // }
            }
            break;
        case ALLJOYN_INT32_ARRAY:
            {
                AutoPtr<ArrayOf<Int32> > array = GetInt32Array(msgArg);
                *(ArrayOf<Int32>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_INT64:
            {
                // object = getEnumObject(type, (int) getInt64(msgArg));
                // if (object == null) {
                    *(Int64*)object = GetInt64(msgArg);
                // }
            }
            break;
        case ALLJOYN_INT64_ARRAY:
            {
                AutoPtr<ArrayOf<Int64> > array = GetInt64Array(msgArg);
                *(ArrayOf<Int64>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_OBJECT_PATH:
            *(String*)object = GetObjPath(msgArg);
            break;
        case ALLJOYN_SIGNATURE:
            *(String*)object = GetSignature(msgArg);
            break;
        case ALLJOYN_STRING:
            *(String*)object = GetString(msgArg);
            break;
        case ALLJOYN_STRUCT:
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
            assert(0);
            break;
        case ALLJOYN_UINT16:
            {
                // object = getEnumObject(type, (int) getUint16(msgArg));
                // if (object == null) {
                    *(Int32*)object = GetUint16(msgArg);
                // }
            }
            break;
        case ALLJOYN_UINT16_ARRAY:
            {
                AutoPtr<ArrayOf<Int16> > array = GetUint16Array(msgArg);
                *(ArrayOf<Int16>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_UINT32:
            {
                // object = getEnumObject(type, getUint32(msgArg));
                // if (object == null) {
                    *(Int32*)object = GetUint32(msgArg);
                // }
            }
            break;
        case ALLJOYN_UINT32_ARRAY:
            {
                AutoPtr<ArrayOf<Int32> > array = GetUint32Array(msgArg);
                *(ArrayOf<Int32>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_UINT64:
            {
                // object = getEnumObject(type, (int) getUint64(msgArg));
                // if (object == null) {
                    *(Int64*)object = GetUint64(msgArg);
                // }
            }
            break;
        case ALLJOYN_UINT64_ARRAY:
            {
                AutoPtr<ArrayOf<Int64> > array = GetUint64Array(msgArg);
                *(ArrayOf<Int64>**)object = array;
                array->AddRef();
            }
            break;
        case ALLJOYN_VARIANT:
            // Variant variant = new Variant();
            // variant.setMsgArg(msgArg);
            // return variant;
        default:
            Logger::E("MsgArg", "unimplemented '%s'", GetSignature(msgArg).string());
            assert(0);
            // throw new MarshalBusException("unimplemented '"
            //                               + getSignature(new Int64[] { msgArg }) + "'");
            break;
        }
    // } catch (Throwable th) {
    //     throw new MarshalBusException("cannot marshal '" + getSignature(new Int64[] { msgArg })
    //                                   + "' into " + type, th);
    // }

    return NOERROR;
}

ECode MsgArg::UnmarshalInternal(
    /* [in] */ Int64 msgArg,
    /* [in] */ CarDataType type,
    /* [in] */ IArgumentList* args,
    /* [in] */ Int32 index)
{
    assert(0);
    return NOERROR;
}

/**
 * Unmarshals an array of native MsgArgs into an array of Java objects.  The
 * MsgArgs are unmarshalled into the parameter types of the specified Java
 * method.
 *
 * @param method the method specifying the parameter types to unmarshal into
 * @param msgArgs an ALLJOYN_STRUCT containing the native MsgArgs as members
 * @return the unmarshalled Java objects
 * @throws MarshalBusException if the unmarshalling fails
 */
ECode MsgArg::Unmarshal(
    /* [in] */ IMethodInfo* method,
    /* [in] */ Int64 msgArgs,
    /* [out] */ IArgumentList** args)
{
    Int32 count;
    method->GetParamCount(&count);
    AutoPtr<ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(count);
    method->GetAllParamInfos(paramInfos);
    Int32 numArgs = GetNumMembers(msgArgs);
    if (count != numArgs) {
        ParamIOAttribute ioAttr = ParamIOAttribute_In;
        if (count - 1 == numArgs) { // count-1 may be out param
            AutoPtr<IParamInfo> paramInfo;
            method->GetParamInfoByIndex(count - 1, (IParamInfo**)&paramInfo);
            paramInfo->GetIOAttribute(&ioAttr);
        }
        if (ioAttr == ParamIOAttribute_In) {
            Logger::E("MsgArg", "cannot marshal %d args into %d parameters", numArgs, count);
            assert(0);
            // throw new MarshalBusException(
            //     "cannot marshal " + numArgs + " args into " + types.length + " parameters");
            return NOERROR;
        }
    }
    method->CreateArgumentList(args);
    for (Int32 i = 0; i < numArgs; ++i) {
        AutoPtr<IDataTypeInfo> typeInfo;
        (*paramInfos)[i]->GetTypeInfo((IDataTypeInfo**)&typeInfo);
        CarDataType type;
        typeInfo->GetDataType(&type);
        UnmarshalInternal(GetMember(msgArgs, i), type, *args, i);
    }
    return NOERROR;
}

/**
 * Marshals a Java object into a native MsgArg.
 *
 * @param msgArg the MsgArg to marshal the Java object into.
 * @param sig the desired signature of the marshalled MsgArg
 * @param arg the Java object
 * @throws MarshalBusException if the marshalling fails
 */
ECode MsgArg::Marshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ PVoid arg)
{
    // try {
        // Int32 value = GetEnumValue(arg);
        switch (sig.GetChar(0)) {
        case ALLJOYN_BYTE:
            // if (value != -1) {
            //     Set(msgArg, sig, (Byte) value);
            // }
            // else {
            //     Set(msgArg, sig, ((Number) arg).byteValue());
            // }
            Set(msgArg, sig, *(Byte*)arg);
            break;
        case ALLJOYN_BOOLEAN:
            // SetBoolean(msgArg, sig, ((Boolean) arg).booleanValue());
            SetBoolean(msgArg, sig, *(Boolean*)arg);
            break;
        case ALLJOYN_INT16:
        case ALLJOYN_UINT16:
            // if (value != -1) {
            //     Set(msgArg, sig, (Int16) value);
            // }
            // else {
            //     Set(msgArg, sig, ((Number) arg).shortValue());
            // }
            Set(msgArg, sig, *(Int16*)arg);
            break;
        case ALLJOYN_INT32:
        case ALLJOYN_UINT32:
            // if (value != -1) {
            //     Set(msgArg, sig, value);
            // }
            // else {
            //     Set(msgArg, sig, ((Number) arg).intValue());
            // }
            Set(msgArg, sig, *(Int32*)arg);
            break;
        case ALLJOYN_INT64:
        case ALLJOYN_UINT64:
            // if (value != -1) {
            //     Set(msgArg, sig, (Int64) value);
            // }
            // else {
            //     Set(msgArg, sig, ((Number) arg).longValue());
            // }
            Set(msgArg, sig, *(Int64*)arg);
            break;
        case ALLJOYN_DOUBLE:
            // Set(msgArg, sig, ((Number) arg).doubleValue());
            Set(msgArg, sig, *(Double*)arg);
            break;
        case ALLJOYN_STRING:
        case ALLJOYN_SIGNATURE:
        case ALLJOYN_OBJECT_PATH:
            if (arg == NULL) {
                Logger::E("MsgArg", "cannot marshal NULL into '%s'", sig.string());
                assert(0);
                // throw new MarshalBusException("cannot marshal NULL into '" + sig + "'");
            }
            Set(msgArg, sig, *(String*)arg);
            break;
        case ALLJOYN_ARRAY:
            {
                if (arg == NULL) {
                    Logger::E("MsgArg", "cannot marshal NULL into '%s'", sig.string());
                    assert(0);
                    // throw new MarshalBusException("cannot marshal NULL into '" + sig + "'");
                }
                Char32 elementTypeId = sig.GetChar(1);
                // if (ALLJOYN_DICT_ENTRY_OPEN == elementTypeId) {
                //     arg = ((Map<?, ?>) arg).entrySet().toArray();
                // }
                switch (elementTypeId) {
                case ALLJOYN_BYTE:
                    Set(msgArg, sig, (ArrayOf<Byte>*)arg);
                    break;
                case ALLJOYN_BOOLEAN:
                    // if (arg instanceof ArrayOf<Boolean>*){
                        Set(msgArg, sig, (ArrayOf<Boolean>*)arg);
                    // } else {
                    //     Set(msgArg, sig, (ArrayOf<Boolean>*)arg);
                    // }
                    break;
                case ALLJOYN_INT16:
                case ALLJOYN_UINT16:
                    Set(msgArg, sig, (ArrayOf<Int16>*)arg);
                    break;
                case ALLJOYN_INT32:
                case ALLJOYN_UINT32:
                    Set(msgArg, sig, (ArrayOf<Int32>*)arg);
                    break;
                case ALLJOYN_INT64:
                case ALLJOYN_UINT64:
                    Set(msgArg, sig, (ArrayOf<Int64>*)arg);
                    break;
                case ALLJOYN_DOUBLE:
                    Set(msgArg, sig, (ArrayOf<Double>*)arg);
                    break;
                default:
                    {
                        String elemSig = sig.Substring(1);
                        AutoPtr<ArrayOf<IInterface*> > args = (ArrayOf<IInterface*>*)arg;
                        SetArray(msgArg, elemSig, args->GetLength());
                        for (Int32 i = 0; i < GetNumElements(msgArg); ++i) {
                            Marshal(GetElement(msgArg, i), elemSig, (PVoid)(*args)[i]);
                        }
                    }
                    break;
                }
            }
            break;
        case ALLJOYN_STRUCT_OPEN:
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
            // break;
        case ALLJOYN_VARIANT:
            // Variant variant = (Variant) arg;
            // if (variant.getMsgArg() != 0) {
            //     SetVariant(msgArg, sig, variant.getMsgArg());
            // } else {
            //     SetVariant(msgArg);
            //     marshal(getVal(msgArg), variant.getSignature(), variant.getValue());
            // }
            // break;
        case ALLJOYN_DICT_ENTRY_OPEN:
            // Map.Entry<?, ?> entry = (Map.Entry<?, ?>) arg;
            // String[] sigs = Signature.split(sig.substring(1, sig.length() - 1));
            // if (sigs == null) {
            //     throw new MarshalBusException("cannot marshal " + arg.getClass() + " into '"
            //                                   + sig + "'");
            // }
            // SetDictEntry(msgArg);
            // marshal(getKey(msgArg), sigs[0], entry.getKey());
            // marshal(getVal(msgArg), sigs[1], entry.getValue());
            // break;
        default:
            Logger::D("MsgArg", "unimplemented '%s'", sig.string());
            assert(0);
            // throw new MarshalBusException("unimplemented '" + sig + "'");
        }
    // } catch (Throwable th) {
    //     throw new MarshalBusException("cannot marshal " +
    //                                   ((arg == null) ? "null" : arg.getClass()) +
    //                                   " into '" + sig + "'", th);
    // }
    return NOERROR;
}

/**
 * Marshals an array of Java objects into a ALLJOYN_STRUCT containing the Java
 * objects as members.
 *
 * @param msgArg the MsgArg to marshal the Java object into.
 * @param sig the desired signature of the marshalled MsgArgs
 * @param args the Java objects
 * @throws MarshalBusException if the marshalling fails
 */
ECode MsgArg::Marshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    AutoPtr<ArrayOf<String> > sigs;// = Signature.split(sig);
    if (sigs == NULL) {
        Logger::E("MsgArg", "cannot marshal args into '%s', bad signature", sig.string());
        assert(0);
        // throw new MarshalBusException("cannot marshal args into '" + sig + "', bad signature");
    }
    SetStruct(msgArg, (args == NULL) ? 0 : args->GetLength());
    for (Int32 i = 0; i < GetNumMembers(msgArg); ++i) {
        Marshal(GetMember(msgArg, i), (*sigs)[i], (PVoid)(*args)[i]);
    }
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
