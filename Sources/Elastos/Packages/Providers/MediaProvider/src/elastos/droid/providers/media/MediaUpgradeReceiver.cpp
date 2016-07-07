#include "elastos/droid/providers/media/MediaUpgradeReceiver.h"
#include "elastos/droid/providers/media/CMediaProvider.h"
#include "elastos/droid/providers/media/MediaProvider.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.IO.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

const String MediaUpgradeReceiver::TAG("MediaUpgradeReceiver");
const String MediaUpgradeReceiver::PREF_DB_VERSION("db_version");

CAR_INTERFACE_IMPL(MediaUpgradeReceiver, BroadcastReceiver, IMediaUpgradeReceiver)

ECode MediaUpgradeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // We are now running with the system up, but no apps started,
    // so can do whatever cleanup after an upgrade that we want.

    // Lookup the last known database version
    AutoPtr<ISharedPreferences> prefs;
    context->GetSharedPreferences(TAG, IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    Int32 prefVersion;
    prefs->GetInt32(PREF_DB_VERSION, 0, &prefVersion);
    Int32 dbVersion = MediaProvider::GetDatabaseVersion(context);
    if (prefVersion == dbVersion) {
        return NOERROR;
    }

    AutoPtr<ISharedPreferencesEditor> e;
    prefs->Edit((ISharedPreferencesEditor**)&e);
    e->PutInt32(PREF_DB_VERSION, dbVersion);
    Boolean flag = FALSE;
    e->Commit(&flag);

    AutoPtr<IIActivityManager> am;
    AutoPtr<ISystem> sys;
    AutoPtr<ICharSequence> cs;

    AutoPtr<IFile> tmpFile;
    AutoPtr<IFile> dbDir;
    AutoPtr<ArrayOf<String> > files;
    String file;
    Int32 flength;
    Int64 now, startTime;

    ECode ec = context->GetDatabasePath(String("foo"), (IFile**)&tmpFile);
    FAIL_GOTO(ec, __EXIT__)
    ec = tmpFile->GetParentFile((IFile**)&dbDir);
    FAIL_GOTO(ec, __EXIT__)
    dbDir->List((ArrayOf<String>**)&files);
    if (files == NULL) return NOERROR;

    am = ActivityManagerNative::GetDefault();
    CSystem::AcquireSingleton((ISystem**)&sys);
    context->GetText(R::string::upgrade_msg, (ICharSequence**)&cs);

    flength = files->GetLength();
    for (Int32 i = 0; i < flength; i++) {
        file = (*files)[i];
        Logger::I(TAG, " file %d: %s", i, file.string());
        if (MediaProvider::IsMediaDatabaseName(file)) {
            am->ShowBootMessage(cs, TRUE);
            sys->GetCurrentTimeMillis(&startTime);

            Logger::I(TAG, "---> Start upgrade of media database %s", file.string());
            AutoPtr<ISQLiteDatabase> db;
            // try {
            AutoPtr<IMediaProvider> mp;
            CMediaProvider::New((IMediaProvider**)&mp);
            AutoPtr<MediaProvider::DatabaseHelper> helper = new MediaProvider::DatabaseHelper();
            helper->constructor((MediaProvider*)(mp.Get()), context, file,
                MediaProvider::IsInternalMediaDatabaseName(file), FALSE, NULL);
            helper->GetWritableDatabase((ISQLiteDatabase**)&db);
            // } catch (Throwable t) {
                // Log.wtf(TAG, "Error during upgrade of media db " + file, t);
            // } finally {
            if (db != NULL) {
                ICloseable::Probe(db)->Close();
            }
            // }
            sys->GetCurrentTimeMillis(&now);
            Logger::I(TAG, "<--- Finished upgrade of media database %s in %ld ms",
                file.string(), now - startTime);
        }
    }

    Logger::I(TAG, " >> OnReceive done: %s", TO_CSTR(intent));
    return NOERROR;

__EXIT__:
    // Something has gone terribly wrong.
    Logger::W(TAG, "Error during upgrade attempt. ec=%08x", ec);
    return ec;
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos