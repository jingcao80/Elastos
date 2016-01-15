
#include "CDataTruncation.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Sql {

const String CDataTruncation::THE_REASON("Data truncation");
const String CDataTruncation::THE_SQLSTATE_READ("01004");
const String CDataTruncation::THE_SQLSTATE_WRITE("22001");

const Int32 CDataTruncation::THE_ERROR_CODE = 0;

CAR_OBJECT_IMPL(CDataTruncation);

CAR_INTERFACE_IMPL_3(CDataTruncation, Object, IDataTruncation, ISQLWarning, ISerializable);

CDataTruncation::CDataTruncation()
    : mIndex(0)
    , mDataSize(0)
    , mTransferSize(0)
    , mParameter(0)
    , mRead(0)
{

}

ECode CDataTruncation::constructor(
    /* [in] */ Int32 index,
    /* [in] */ Boolean parameter,
    /* [in] */ Boolean read,
    /* [in] */ Int32 dataSize,
    /* [in] */ Int32 transferSize)
{
    //super(THE_REASON, THE_SQLSTATE_READ, THE_ERROR_CODE);
    mIndex = index;
    mParameter = parameter;
    mRead = read;
    mDataSize = dataSize;
    mTransferSize = transferSize;
    return NOERROR;
}

ECode CDataTruncation::constructor(
    /* [in] */ Int32 index,
    /* [in] */ Boolean parameter,
    /* [in] */ Boolean read,
    /* [in] */ Int32 dataSize,
    /* [in] */ Int32 transferSize,
    /* [in] */ IThrowable * cause)
{
    //super(THE_REASON, read ? THE_SQLSTATE_READ : THE_SQLSTATE_WRITE, THE_ERROR_CODE, cause);
    mIndex = index;
    mParameter = parameter;
    mRead = read;
    mDataSize = dataSize;
    mTransferSize = transferSize;
    return NOERROR;
}

ECode CDataTruncation::GetDataSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mDataSize;
    return NOERROR;
}

ECode CDataTruncation::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    *index = mIndex;
    return NOERROR;
}

ECode CDataTruncation::GetParameter(
    /* [out] */ Boolean* isPara)
{
    VALIDATE_NOT_NULL(isPara)
    *isPara = mParameter;
    return NOERROR;
}

ECode CDataTruncation::GetRead(
    /* [out] */ Boolean* isRead)
{
    VALIDATE_NOT_NULL(isRead)
    *isRead = mRead;
    return NOERROR;
}

ECode CDataTruncation::GetTransferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mTransferSize;
    return NOERROR;
}

ECode CDataTruncation::GetNextWarning(
    /* [out] */ ISQLWarning ** sqlwar)
{
    // ISQLException next = super.getNextException();
    // if (next == null) {
    //     return null;
    // }
    // if (next instanceof SQLWarning) {
    //     return (SQLWarning) next;
    // }
    // throw new Error("SQLWarning chain holds value that is not a SQLWarning");
    return NOERROR;
}

ECode CDataTruncation::SetNextWarning(
    /* [in] */ ISQLWarning * w)
{
    // super.setNextException(w);
    return NOERROR;
}

} // namespace Sql
} // namespace Elastos
