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

#include "SelectionKey.h"

namespace Elastos {
namespace IO {
namespace Channels {

CAR_INTERFACE_IMPL(SelectionKey, Object, ISelectionKey)

Int32 SelectionKey::sOP_ACCEPT = 16;

Int32 SelectionKey::sOP_CONNECT = 8;

Int32 SelectionKey::sOP_READ = 1;

Int32 SelectionKey::sOP_WRITE = 4;

SelectionKey::SelectionKey()
    : mAttachment(NULL)
{
}

SelectionKey::~SelectionKey()
{
}

ECode SelectionKey::Attach(
    /* [in] */ IObject* anObject,
    /* [out] */ IObject** oldObject)
{
    VALIDATE_NOT_NULL(oldObject)
    *oldObject = mAttachment;
    REFCOUNT_ADD(*oldObject)

    mAttachment = anObject;
    return NOERROR;
}

ECode SelectionKey::Attachment(
    /* [out] */ IObject** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = mAttachment;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode SelectionKey::IsAcceptable(
    /* [out] */ Boolean* acceptable)
{
    VALIDATE_NOT_NULL(acceptable)
    *acceptable = FALSE;

    Int32 ops;
    ECode ec = GetReadyOps(&ops);
    if ((ops & sOP_ACCEPT) == sOP_ACCEPT)
        *acceptable = TRUE;

    return ec;
}

ECode SelectionKey::IsConnectable(
    /* [out] */ Boolean* connectable)
{
    VALIDATE_NOT_NULL(connectable)
    Int32 ops;
    ECode ec = GetReadyOps(&ops);
    *connectable = (ops & sOP_CONNECT) == sOP_CONNECT;
    return ec;
}

ECode SelectionKey::IsReadable(
    /* [out] */ Boolean* readable)
{
    VALIDATE_NOT_NULL(readable)
    *readable = FALSE;

    Int32 ops;
    ECode ec = GetReadyOps(&ops);
    if ((ops & sOP_READ) == sOP_READ)
        *readable = TRUE;
    return ec;
}

ECode SelectionKey::IsWritable(
    /* [out] */ Boolean* writable)
{
    VALIDATE_NOT_NULL(writable)
    Int32 ops;
    ECode ec = GetReadyOps(&ops);
    *writable = (ops & sOP_WRITE) == sOP_WRITE;
    return ec;
}

} // namespace Channels
} // namespace IO
} // snamespace Elastos
