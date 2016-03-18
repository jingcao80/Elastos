
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CAssetFileDescriptor.h"
#include "elastos/droid/content/res/CAssetFileDescriptorAutoCloseInputStream.h"
#include "elastos/droid/content/res/CAssetFileDescriptorAutoCloseOutputStream.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseInputStream.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseOutputStream.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IParcelFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Os::IParcelFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::Content::Res::IAssetFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Content::Res::IAssetFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::Content::Res::CAssetFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Content::Res::CAssetFileDescriptorAutoCloseOutputStream;
using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL_3(CAssetFileDescriptor, Object, IAssetFileDescriptor, ICloseable, IParcelable)

CAR_OBJECT_IMPL(CAssetFileDescriptor)

CAssetFileDescriptor::CAssetFileDescriptor()
    : mStartOffset(0)
    , mLength(0)
{}

CAssetFileDescriptor::~CAssetFileDescriptor()
{}

ECode CAssetFileDescriptor::constructor()
{
    return NOERROR;
}

ECode CAssetFileDescriptor::constructor(
    /* [in] */ IParcelFileDescriptor * fd,
    /* [in] */ Int64 startOffset,
    /* [in] */ Int64 length)
{
    return constructor(fd, startOffset, length, NULL);
}

ECode CAssetFileDescriptor::constructor(
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ Int64 startOffset,
    /* [in] */ Int64 length,
    /* [in] */ IBundle* extras)
{
    if (fd == NULL) {
        //throw new IllegalArgumentException("fd must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (length < 0 && startOffset != 0) {
        //throw new IllegalArgumentException(
        //"startOffset must be 0 when using UNKNOWN_LENGTH");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mFd = fd;
    mStartOffset = startOffset;
    mLength = length;
    mExtras = extras;
    return NOERROR;
}

ECode CAssetFileDescriptor::GetParcelFileDescriptor(
    /* [out] */ IParcelFileDescriptor ** fd)
{
    VALIDATE_NOT_NULL(fd);
    *fd = mFd;
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

ECode CAssetFileDescriptor::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);
    return mFd->GetFileDescriptor(fd);
}

ECode CAssetFileDescriptor::GetStartOffset(
    /* [out] */Int64 * startOffset)
{
    VALIDATE_NOT_NULL(startOffset);
    *startOffset = mStartOffset;
    return NOERROR;
}

ECode CAssetFileDescriptor::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode CAssetFileDescriptor::GetLength(
    /* [out] */Int64* length)
{
    VALIDATE_NOT_NULL(length);
    if (mLength >= 0) {
        *length = mLength;
        return NOERROR;
    }
    Int64 len = 0;
    mFd->GetStatSize(&len);
    *length = len >= 0 ? len : UNKNOWN_LENGTH;
    return NOERROR;
}

ECode CAssetFileDescriptor::GetDeclaredLength(
    /* [out] */Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mLength;
    return NOERROR;
}

ECode CAssetFileDescriptor::Close()
{
    return mFd->Close();
}

ECode CAssetFileDescriptor::CreateInputStream(
    /* [out] */ IFileInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    if (mLength < 0) {
        AutoPtr<IParcelFileDescriptorAutoCloseInputStream> closeInputStream;
        FAIL_RETURN(CParcelFileDescriptorAutoCloseInputStream::New(mFd, (IParcelFileDescriptorAutoCloseInputStream**)&closeInputStream));
        *stream = IFileInputStream::Probe(closeInputStream);
        REFCOUNT_ADD(*stream);
        return NOERROR;
    }
    AutoPtr<IAssetFileDescriptorAutoCloseInputStream> closeInputStream;
    FAIL_RETURN(CAssetFileDescriptorAutoCloseInputStream::New(this, (IAssetFileDescriptorAutoCloseInputStream**)&closeInputStream));
    *stream = IFileInputStream::Probe(closeInputStream);
    REFCOUNT_ADD(*stream);
    return NOERROR;
}

ECode CAssetFileDescriptor::CreateOutputStream(
    /* [out] */ IFileOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    if (mLength < 0) {
        AutoPtr<IParcelFileDescriptorAutoCloseOutputStream> closeOutputStream;
        FAIL_RETURN(CParcelFileDescriptorAutoCloseOutputStream::New(mFd, (IParcelFileDescriptorAutoCloseOutputStream**)&closeOutputStream));
        *stream = IFileOutputStream::Probe(closeOutputStream);
        REFCOUNT_ADD(*stream);
        return NOERROR;
    }
    AutoPtr<IAssetFileDescriptorAutoCloseOutputStream> closeOutputStream;
    FAIL_RETURN(CAssetFileDescriptorAutoCloseOutputStream::New(this, (IAssetFileDescriptorAutoCloseOutputStream**)&closeOutputStream));
    *stream = IFileOutputStream::Probe(closeOutputStream);
    REFCOUNT_ADD(*stream);
    return NOERROR;
}

ECode CAssetFileDescriptor::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> fd;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&fd);
    mFd = IParcelFileDescriptor::Probe(fd);
    FAIL_RETURN(source->ReadInt64(&mStartOffset));
    FAIL_RETURN(source->ReadInt64(&mLength));
    Int32 ival;
    source->ReadInt32(&ival);
    if (ival != 0) {
        CBundle::New((IBundle**)&mExtras);
        IParcelable::Probe(mExtras)->ReadFromParcel(source);
    }
    else {
        mExtras = NULL;
    }
    return NOERROR;
}

ECode CAssetFileDescriptor::WriteToParcel(
    /* [in] */ IParcel * dest)
{
    dest->WriteInterfacePtr(mFd);
    FAIL_RETURN(dest->WriteInt64(mStartOffset));
    FAIL_RETURN(dest->WriteInt64(mLength));
    if (mExtras != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mExtras)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
