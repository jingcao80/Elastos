
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/database/sqlite/SQLiteGlobal.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/os/CStatFs.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include "elastos/droid/R.h"
#include <sqlite3.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::IStatFs;
using Elastos::Droid::Os::CStatFs;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

const String SQLiteGlobal::TAG("SQLiteGlobal");
Object SQLiteGlobal::mLock;
Int32 SQLiteGlobal::sDefaultPageSize;

// Limit heap to 8MB for now.  This is 4 times the maximum cursor window
// size, as has been used by the original code in SQLiteDatabase for
// a long time.
static const Int32 SOFT_HEAP_LIMIT = 8 * 1024 * 1024;

Int32 SQLiteGlobal::NativeReleaseMemory()
{
    return sqlite3_release_memory(SOFT_HEAP_LIMIT);
}

Int32 SQLiteGlobal::ReleaseMemory()
{
    return NativeReleaseMemory();
}

Int32 SQLiteGlobal::GetDefaultPageSize()
{
    Int32 result;
    synchronized(mLock) {
        if (sDefaultPageSize == 0) {
            AutoPtr<IStatFs> fs;
            CStatFs::New(String("/data"), (IStatFs**)&fs);
            fs->GetBlockSize(&sDefaultPageSize);
        }
        SystemProperties::GetInt32(String("ro.build.version.sdk"), sDefaultPageSize, &result);
    }
    return result;
}

String SQLiteGlobal::GetDefaultJournalMode()
{
    String str, result;
    AutoPtr<IResources> r = CResources::GetSystem();
    r->GetString(R::string::db_default_journal_mode, &str);
    SystemProperties::Get(String("debug.sqlite.journalmode"), str, &result);
    return result;
}

Int32 SQLiteGlobal::GetJournalSizeLimit()
{
    Int32 num, result;
    AutoPtr<IResources> r = CResources::GetSystem();
    r->GetInteger(R::integer::db_journal_size_limit, &num);
    SystemProperties::GetInt32(String("debug.sqlite.journalsizelimit"), num, &result);
    return result;
}

String SQLiteGlobal::GetDefaultSyncMode()
{
    String str, result;
    AutoPtr<IResources> r = CResources::GetSystem();
    r->GetString(R::string::db_default_sync_mode, &str);
    SystemProperties::Get(String("debug.sqlite.syncmode"), str, &result);
    return result;
}

String SQLiteGlobal::GetWALSyncMode()
{
    String str, result;
    AutoPtr<IResources> r = CResources::GetSystem();
    r->GetString(R::string::db_wal_sync_mode, &str);
    SystemProperties::Get(String("debug.sqlite.wal.syncmode"), str, &result);
    return result;
}

Int32 SQLiteGlobal::GetWALAutoCheckpoint()
{
    Int32 num, value;
    AutoPtr<IResources> r = CResources::GetSystem();
    r->GetInteger(R::integer::db_wal_autocheckpoint, &num);
    SystemProperties::GetInt32(String("debug.sqlite.wal.autocheckpoint"), num, &value);
    return Elastos::Core::Math::Max(1, value);
}

Int32 SQLiteGlobal::GetWALConnectionPoolSize()
{
    Int32 num, value;
    AutoPtr<IResources> r = CResources::GetSystem();
    r->GetInteger(R::integer::db_connection_pool_size, &num);
    SystemProperties::GetInt32(String("debug.sqlite.wal.poolsize"), num, &value);
    return Elastos::Core::Math::Max(2, value);
}

} //Sqlite
} //Database
} //Droid
} //Elastos