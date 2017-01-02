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

#include "ObjectStreamConstants.h"

namespace Elastos {
namespace IO {

static AutoPtr<SerializablePermission> InitMember(
    /* [in] */ const String& v)
{
    AutoPtr<SerializablePermission> s = new SerializablePermission();
    s->constructor(v);
    return s;
}

const Int16 ObjectStreamConstants::STREAM_MAGIC = (Int16) 0xaced;
const Int16 ObjectStreamConstants::STREAM_VERSION = 5;
const Byte ObjectStreamConstants::TC_BASE = 0x70;
const Byte ObjectStreamConstants::TC_NULL = (byte) 0x70;
const Byte ObjectStreamConstants::TC_REFERENCE = (byte) 0x71;
const Byte ObjectStreamConstants::TC_CLASSDESC = (byte) 0x72;
const Byte ObjectStreamConstants::TC_OBJECT = (byte) 0x73;
const Byte ObjectStreamConstants::TC_STRING = (byte) 0x74;
const Byte ObjectStreamConstants::TC_ARRAY = (byte) 0x75;
const Byte ObjectStreamConstants::TC_CLASS = (byte) 0x76;
const Byte ObjectStreamConstants::TC_BLOCKDATA = (byte) 0x77;
const Byte ObjectStreamConstants::TC_ENDBLOCKDATA = (byte) 0x78;
const Byte ObjectStreamConstants::TC_RESET = (byte) 0x79;
const Byte ObjectStreamConstants::TC_BLOCKDATALONG = (byte) 0x7A;
const Byte ObjectStreamConstants::TC_EXCEPTION = (byte) 0x7B;
const Byte ObjectStreamConstants::TC_LONGSTRING = (byte) 0x7C;
const Byte ObjectStreamConstants::TC_PROXYCLASSDESC = (byte) 0x7D;
const Byte ObjectStreamConstants::TC_MAX = 0x7E;
const Int32 ObjectStreamConstants::baseWireHandle = 0x007e0000;
const Int32 ObjectStreamConstants::PROTOCOL_VERSION_1 = 1;
const Int32 ObjectStreamConstants::PROTOCOL_VERSION_2 = 2;
const AutoPtr<SerializablePermission> ObjectStreamConstants::SUBCLASS_IMPLEMENTATION_PERMISSION =
        InitMember(String("enableSubclassImplementation"));
const AutoPtr<SerializablePermission> ObjectStreamConstants::SUBSTITUTION_PERMISSION =
        InitMember(String("enableSubstitution"));
const Byte ObjectStreamConstants::SC_WRITE_METHOD = 0x01; // If SC_SERIALIZABLE
const Byte ObjectStreamConstants::SC_SERIALIZABLE = 0x02;
const Byte ObjectStreamConstants::SC_EXTERNALIZABLE = 0x04;
const Byte ObjectStreamConstants::SC_BLOCK_DATA = 0x08; // If SC_EXTERNALIZABLE
const Byte ObjectStreamConstants::TC_ENUM = 0x7E;
const Byte ObjectStreamConstants::SC_ENUM = 0x10;

} // namespace IO
} // namespace Elastos
