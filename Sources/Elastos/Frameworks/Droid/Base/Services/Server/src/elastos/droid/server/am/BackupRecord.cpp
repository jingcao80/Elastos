
#include "elastos/droid/server/am/BackupRecord.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 BackupRecord::BACKUP_NORMAL;
const Int32 BackupRecord::BACKUP_FULL;
const Int32 BackupRecord::RESTORE;
const Int32 BackupRecord::RESTORE_FULL;

String BackupRecord::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }
    StringBuilder sb(128);
    sb.Append("BackupRecord{");
    sb.Append(StringUtils::ToString((Int32)this, 16));
    sb.AppendChar(' ');
    String pkgName;
    IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&pkgName);
    sb.Append(pkgName);
    sb.AppendChar(' ');
    String name;
    IPackageItemInfo::Probe(mAppInfo)->GetName(&name);
    sb.Append(name);
    sb.AppendChar(' ');
    String bName;
    mAppInfo->GetBackupAgentName(&bName);
    sb.Append(bName);
    sb.AppendChar('}');
    return mStringName = sb.ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
