
#include "elastos/droid/contacts/common/list/DirectoryPartition.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Contacts::Common::List::EIID_IDirectoryPartition;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

DirectoryPartition::DirectoryPartition(
    /* [in] */ Boolean showIfEmpty,
    /* [in] */ Boolean hasHeader)
    : CompositeCursorAdapter::Partition(showIfEmpty, hasHeader)
    , mDirectoryId(0)
    , mStatus(0)
    , mPriorityDirectory(FALSE)
    , mPhotoSupported(FALSE)
    , mResultLimit(RESULT_LIMIT_DEFAULT)
    , mDisplayNumber(TRUE)
{}

CAR_INTERFACE_IMPL(DirectoryPartition, CompositeCursorAdapter::Partition, IDirectoryPartition)

Int64 DirectoryPartition::GetDirectoryId()
{
    return mDirectoryId;
}

void DirectoryPartition::SetDirectoryId(
    /* [in] */ Int64 directoryId)
{
    mDirectoryId = directoryId;
}

String DirectoryPartition::GetDirectoryType()
{
    return mDirectoryType;
}

void DirectoryPartition::SetDirectoryType(
    /* [in] */ const String& directoryType)
{
    mDirectoryType = directoryType;
}

String DirectoryPartition::GetDisplayName()
{
    return mDisplayName;
}

void DirectoryPartition::SetDisplayName(
    /* [in] */ const String& displayName)
{
    mDisplayName = displayName;
}

Int32 DirectoryPartition::GetStatus()
{
    return mStatus;
}

void DirectoryPartition::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
}

Boolean DirectoryPartition::IsLoading()
{
    return mStatus == STATUS_NOT_LOADED || mStatus == STATUS_LOADING;
}

Boolean DirectoryPartition::IsPriorityDirectory()
{
    return mPriorityDirectory;
}

void DirectoryPartition::SetPriorityDirectory(
    /* [in] */ Boolean priorityDirectory)
{
    mPriorityDirectory = priorityDirectory;
}

Boolean DirectoryPartition::IsPhotoSupported()
{
    return mPhotoSupported;
}

void DirectoryPartition::SetPhotoSupported(
    /* [in] */ Boolean flag)
{
    mPhotoSupported = flag;
}

Int32 DirectoryPartition::GetResultLimit()
{
    return mResultLimit;
}

void DirectoryPartition::SetResultLimit(
    /* [in] */ Int32 resultLimit)
{
    mResultLimit = resultLimit;
}

String DirectoryPartition::GetContentUri()
{
    return mContentUri;
}

void DirectoryPartition::SetContentUri(
    /* [in] */ const String& contentUri)
{
    mContentUri = contentUri;
}

String DirectoryPartition::GetLabel()
{
    return mLabel;
}

void DirectoryPartition::SetLabel(
    /* [in] */ const String& label)
{
    mLabel = label;
}

ECode DirectoryPartition::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("DirectoryPartition{") +
            "mDirectoryId=" + StringUtils::ToString(mDirectoryId) +
            ", mContentUri='" + mContentUri + "\'" +
            ", mDirectoryType='" + mDirectoryType + "\'" +
            ", mDisplayName='" + mDisplayName + "\'" +
            ", mStatus=" + StringUtils::ToString(mStatus) +
            ", mPriorityDirectory=" + StringUtils::ToString(mPriorityDirectory) +
            ", mPhotoSupported=" + StringUtils::ToString(mPhotoSupported) +
            ", mResultLimit=" + StringUtils::ToString(mResultLimit) +
            ", mLabel='" + mLabel + "\'" +
            '}';
    return NOERROR;
}

Boolean DirectoryPartition::IsDisplayNumber()
{
    return mDisplayNumber;
}

void DirectoryPartition::SetDisplayNumber(
    /* [in] */ Boolean displayNumber)
{
    mDisplayNumber = displayNumber;
}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
