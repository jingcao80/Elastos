//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_ALLJOYN_BUS_MSGARG_H__
#define __ORG_ALLJOYN_BUS_MSGARG_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Core.h>
#include "_Org.Alljoyn.Bus.h"
#include <elastos/utility/etl/Map.h>
#include <elastos/core/Object.h>

using Elastos::Core::IBoolean;
using Elastos::Core::IArrayOf;
using Elastos::Utility::Etl::Map;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * MsgArg provides methods for marshalling from Java types to native types and
 * unmarshalling from native types to Java types.  The methods use a {@code
 * Int64} as the handle to the native type (representing a {@code MsgArg} in
 * native code).  No unnecessary Java objects are created.
 */
class MsgArg
{
public:
    class CarValue : public ElLightRefBase
    {
    public:
        CarValue(
            /* [in] */ CarDataType type);

        ~CarValue();

        CARAPI_(PVoid) ToValuePtr();

        CARAPI SetToArgumentList(
            /* [in] */ IArgumentList* args,
            /* [in] */ Int32 index);

        CARAPI GetFromArgumentList(
            /* [in] */ IArgumentList* args,
            /* [in] */ Int32 index);

        CARAPI AssignArgumentListOutput(
            /* [in] */ IArgumentList* args,
            /* [in] */ Int32 index);

        CARAPI_(AutoPtr<IInterface>) Convert();

        static AutoPtr<CarValue> Convert(
            /* [in] */ IInterface* object);

    public:
        ParamIOAttribute mIOAttribute;
        CarDataType mType;
        Byte mByteValue;
        Boolean mBooleanValue;
        Int16 mInt16Value;
        Int32 mInt32Value;
        Int64 mInt64Value;
        Double mDoubleValue;
        String mStringValue;
        Int32 mEnumValue;
        PCarQuintet mCarQuintet;
        CarDataType mElementType;
        AutoPtr<IInterface> mObjectValue;
    };

public:
    /*
     * Accessor functions for talking to the native MsgArgs.  The msgArg
     * parameter is a native (MsgArg *).
     */
    static CARAPI_(Int32) GetTypeId(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Byte) GetByte(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int16) GetInt16(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int16) GetUint16(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Boolean) GetBool(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int32) GetUint32(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int32) GetInt32(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int64) GetInt64(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int64) GetUint64(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Double) GetDouble(
        /* [in] */ Int64 msgArg);

    static CARAPI_(String) GetString(
        /* [in] */ Int64 msgArg);

    static CARAPI_(String) GetObjPath(
        /* [in] */ Int64 msgArg);

    static CARAPI_(String) GetSignature(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetByteArray(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Int16> >) GetInt16Array(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Int16> >) GetUint16Array(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Boolean> >) GetBoolArray(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetUint32Array(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetInt32Array(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Int64> >) GetInt64Array(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Int64> >) GetUint64Array(
        /* [in] */ Int64 msgArg);

    static CARAPI_(AutoPtr<ArrayOf<Double> >) GetDoubleArray(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int64) GetKey(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int64) GetVal(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int32) GetNumElements(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int64) GetElement(
        /* [in] */ Int64 msgArg,
        /* [in] */ Int32 index);

    static CARAPI_(String) GetElemSig(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int32) GetNumMembers(
        /* [in] */ Int64 msgArg);

    static CARAPI_(Int64) GetMember(
        /* [in] */ Int64 msgArg,
        /* [in] */ Int32 index);

    /*
     * Accessor functions for setting native MsgArgs.  The msgArg
     * parameter is a native (MsgArg *).
     */
    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ Byte arg);

    static CARAPI SetBoolean(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ Boolean arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ Int16 arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ Int32 arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ Int64 arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ Double arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ const String& arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ ArrayOf<Byte>* arg);

    static CARAPI SetBooleanArray(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ ArrayOf<Boolean>* arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ ArrayOf<Int16>* arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ ArrayOf<Int32>* arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ ArrayOf<Int64>* arg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ ArrayOf<Double>* arg);

    static CARAPI SetArray(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& elemSig,
        /* [in] */ Int32 numElements);

    static CARAPI SetStruct(
        /* [in] */ Int64 msgArg,
        /* [in] */ Int32 numMembers);

    static CARAPI SetDictEntry(
        /* [in] */ Int64 msgArg);

    static CARAPI SetVariant(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ Int64 val);

    static CARAPI SetVariant(
        /* [in] */ Int64 msgArg);

    static CARAPI Set(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ ArrayOf<IBoolean*>* arg);

    /**
     * Returns a string representation of the signature of an array of message
     * args.
     *
     * @param msgArgs an array of message arg values
     * @return the signature string for the message args
     */
    static CARAPI_(String) GetSignature(
        /* [in] */ ArrayOf<Int64>* msgArgs);

    static CARAPI MarshalInterface(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& sig,
        /* [in] */ IInterface* arg);

    static CARAPI UnmarshalInterface(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& sig,
        /* [out] */ IInterface** arg);

    /**
     * Unmarshals a native MsgArg into a Java object.
     *
     * @param msgArg the native MsgArg pointer
     * @param type the Java type to unmarshal into
     * @return the unmarshalled Java object
     * @throws MarshalBusException if the unmarshalling fails
     */
    // @SuppressWarnings("unchecked")
    static CARAPI Unmarshal(
        /* [in] */ Int64 msgArg,
        /* [out] */ PVoid object);

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
    static CARAPI UnmarshalIn(
        /* [in] */ IMethodInfo* method,
        /* [in] */ Int64 msgArgs,
        /* [out] */ IArgumentList** args);

    static CARAPI UnmarshalOut(
        /* [in] */ Int64 msgArgs,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

    /**
     * Marshals a Java object into a native MsgArg.
     *
     * @param msgArg the MsgArg to marshal the Java object into.
     * @param sig the desired signature of the marshalled MsgArg
     * @param arg the Java object
     * @throws MarshalBusException if the marshalling fails
     */
    static CARAPI Marshal(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& sig,
        /* [in] */ PVoid arg);

    /**
     * Marshals an array of Java objects into a ALLJOYN_STRUCT containing the Java
     * objects as members.
     *
     * @param msgArg the MsgArg to marshal the Java object into.
     * @param sig the desired signature of the marshalled MsgArgs
     * @param args the Java objects
     * @throws MarshalBusException if the marshalling fails
     */
    static CARAPI MarshalIn(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& sig,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

    static CARAPI MarshalOut(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& sig,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

    static CARAPI ReleaseRecord(
        /* [in] */ IArgumentList* args);

    static CARAPI AssignOutputPropery(
        /* [in] */ const String& signature,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args,
        /* [in] */ IVariant* property);

    static CARAPI GetInputPropery(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& signature,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

private:
    MsgArg();

    static InterfaceID GetInterfaceIDByTypeId(
        /* [in] */ Char32 typeId);

    static CARAPI GetInputPropery(
        /* [in] */ const String& signature,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args,
        /* [in] */ CarDataType elementType,
        /* [in] */ PCarQuintet carQuintet,
        /* [out] */ IVariant** property);

    static CARAPI AssignOutputPropery(
        /* [in] */ const String& signature,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args,
        /* [in] */ CarDataType elementType,
        /* [in] */ IArrayOf* arrayOf);

private:
    /*
     * The AllJoyn data type IDs.
     */
    static const Int32 ALLJOYN_INVALID;
    static const Int32 ALLJOYN_ARRAY;
    static const Int32 ALLJOYN_BOOLEAN;
    static const Int32 ALLJOYN_DOUBLE;
    static const Int32 ALLJOYN_DICT_ENTRY;
    static const Int32 ALLJOYN_SIGNATURE;
    static const Int32 ALLJOYN_INT32;
    static const Int32 ALLJOYN_INT16;
    static const Int32 ALLJOYN_OBJECT_PATH;
    static const Int32 ALLJOYN_UINT16;
    static const Int32 ALLJOYN_STRUCT;
    static const Int32 ALLJOYN_STRING;
    static const Int32 ALLJOYN_UINT64;
    static const Int32 ALLJOYN_UINT32;
    static const Int32 ALLJOYN_VARIANT;
    static const Int32 ALLJOYN_INT64;
    static const Int32 ALLJOYN_BYTE;

    static const Int32 ALLJOYN_STRUCT_OPEN;
    static const Int32 ALLJOYN_STRUCT_CLOSE;
    static const Int32 ALLJOYN_DICT_ENTRY_OPEN;
    static const Int32 ALLJOYN_DICT_ENTRY_CLOSE;

    static const Int32 ALLJOYN_BOOLEAN_ARRAY;
    static const Int32 ALLJOYN_DOUBLE_ARRAY;
    static const Int32 ALLJOYN_INT32_ARRAY;
    static const Int32 ALLJOYN_INT16_ARRAY;
    static const Int32 ALLJOYN_UINT16_ARRAY;
    static const Int32 ALLJOYN_UINT64_ARRAY;
    static const Int32 ALLJOYN_UINT32_ARRAY;
    static const Int32 ALLJOYN_INT64_ARRAY;
    static const Int32 ALLJOYN_BYTE_ARRAY;

    typedef Map<AutoPtr<IArgumentList>, AutoPtr<ArrayOf<CarValue*> > > RecordMap;
    static RecordMap sRecords;
    static Object sLock;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_MSGARG_H__
