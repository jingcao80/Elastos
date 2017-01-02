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

#ifndef __ELASTOS_IO_OBJECTSTREAMCONSTANTS_H__
#define __ELASTOS_IO_OBJECTSTREAMCONSTANTS_H__

#include "SerializablePermission.h"

namespace Elastos {
namespace IO {

/**
 * A helper interface with constants used by the serialization implementation.
 */
class ObjectStreamConstants
{
public:
    /**
     * The stream header's magic number.
     */
    static const Int16 STREAM_MAGIC;

    /**
     * The stream header's version number.
     */
    static const Int16 STREAM_VERSION;

    // These are tags to indicate the stream contents

    /**
     * The minimum tag value.
     */
    static const Byte TC_BASE;

    /**
     * Tag to mark a {@code null} object reference.
     */
    static const Byte TC_NULL;

    /**
     * Tag to mark a reference to an object that has already been written to the
     * stream.
     */
    static const Byte TC_REFERENCE;

    /**
     * Tag to mark a new class descriptor.
     */
    static const Byte TC_CLASSDESC;

    /**
     * Tag to mark a new object.
     */
    static const Byte TC_OBJECT;

    /**
     * Tag to mark a new string.
     */
    static const Byte TC_STRING;

    /**
     * Tag to mark a new array.
     */
    static const Byte TC_ARRAY;

    /**
     * Tag to mark a reference to a class.
     */
    static const Byte TC_CLASS;

    /**
     * Tag to mark a block of optional data. The byte following this tag
     * indicates the size of the block.
     */
    static const Byte TC_BLOCKDATA;

    /**
     * Tag to mark the end of block data blocks for an object.
     */
    static const Byte TC_ENDBLOCKDATA;

    /**
     * Tag to mark a stream reset.
     */
    static const Byte TC_RESET;

    /**
     * Tag to mark a long block of data. The long following this tag
     * indicates the size of the block.
     */
    static const Byte TC_BLOCKDATALONG;

    /**
     * Tag to mark an exception.
     */
    static const Byte TC_EXCEPTION;

    /**
     * Tag to mark a long string.
     */
    static const Byte TC_LONGSTRING;

    /**
     * Tag to mark a new proxy class descriptor.
     */
    static const Byte TC_PROXYCLASSDESC;

    /**
     * The maximum tag value.
     */
    static const Byte TC_MAX;

    /**
     * Handle for the first object that gets serialized.
     */
    static const Int32 baseWireHandle;

    /**
     * Stream protocol version 1.
     */
    static const Int32 PROTOCOL_VERSION_1;

    /**
     * Stream protocol version 2.
     */
    static const Int32 PROTOCOL_VERSION_2;

    /**
     * Permission constant to enable subclassing of ObjectInputStream and
     * ObjectOutputStream.
     */
    static const AutoPtr<SerializablePermission> SUBCLASS_IMPLEMENTATION_PERMISSION;

    /**
     * Permission constant to enable object substitution during serialization
     * and deserialization.
     */
    static const AutoPtr<SerializablePermission> SUBSTITUTION_PERMISSION;

    // Flags that indicate if the object was serializable, externalizable
    // and had a writeObject method when dumped.
    /**
     * Bit mask for the {@code flag} field in {@link ObjectStreamClass}. Indicates
     * that a {@link Serializable} class has its own {@code writeObject} method.
     */
    static const Byte SC_WRITE_METHOD; // If SC_SERIALIZABLE

    /**
     * Bit mask for the {@code flag} field in {@link ObjectStreamClass}. Indicates
     * that a class implements {@link Serializable} but not {@link Externalizable}.
     */
    static const Byte SC_SERIALIZABLE;

    /**
     * Bit mask for the {@code flag} field in {@link ObjectStreamClass}. Indicates
     * that a class implements {@link Externalizable}.
     */
    static const Byte SC_EXTERNALIZABLE;

    /**
     * Bit mask for the {@code flag} field in {@link ObjectStreamClass}. Indicates
     * that an externalizable class is written in block data mode.
     */
    static const Byte SC_BLOCK_DATA; // If SC_EXTERNALIZABLE

    /**
     * Tag to mark a new enum.
     */
    static const Byte TC_ENUM;

    /**
     * Bit mask for the {@code flag} field in {@link ObjectStreamClass}. Indicates
     * that a class is an enum type.
     */
    static const Byte SC_ENUM;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_OBJECTSTREAMCONSTANTS_H__
