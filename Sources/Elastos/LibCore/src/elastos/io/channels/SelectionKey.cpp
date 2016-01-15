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
