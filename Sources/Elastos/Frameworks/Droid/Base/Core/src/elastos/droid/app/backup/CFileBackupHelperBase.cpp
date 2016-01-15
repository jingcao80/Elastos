#include "elastos/droid/app/backup/CFileBackupHelperBase.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_OBJECT_IMPL(CFileBackupHelperBase)

CFileBackupHelperBase::~CFileBackupHelperBase()
{
}

ECode CFileBackupHelperBase::constructor(
    /* [in] */ IContext* constext)
{
    return FileBackupHelperBase::Init(constext);
}

ECode CFileBackupHelperBase::WriteNewStateDescription(
    /* [in] */ IParcelFileDescriptor* fd)
{

    return FileBackupHelperBase::WriteNewStateDescription(fd);
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
