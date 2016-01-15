
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/database/CDefaultDatabaseErrorHandler.h"
#include "elastos/droid/database/sqlite/SQLiteDatabase.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Database::Sqlite::SQLiteDatabase;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(CDefaultDatabaseErrorHandler, Object, IDatabaseErrorHandler);

CAR_OBJECT_IMPL(CDefaultDatabaseErrorHandler)

const String CDefaultDatabaseErrorHandler::TAG("DefaultDatabaseErrorHandler");

ECode CDefaultDatabaseErrorHandler::OnCorruption(
    /* [in] */ ISQLiteDatabase* dbObj)
{
    String path;
    dbObj->GetPath(&path);
    Slogger::E(TAG, "Corruption reported by sqlite on database: %s", path.string());

    // is the corruption detected even before database could be 'opened'?
    Boolean isOpened;
    if (dbObj->IsOpen(&isOpened), !isOpened) {
        // database files are not even openable. delete this database file.
        // NOTE if the database has attached databases, then any of them could be corrupt.
        // and not deleting all of them could cause corrupted database file to remain and
        // make the application crash on database open operation. To avoid this problem,
        // the application should provide its own {@link DatabaseErrorHandler} impl class
        // to delete ALL files of the database (including the attached databases).
        dbObj->GetPath(&path);
        DeleteDatabaseFile(path);
        return NOERROR;
    }

    AutoPtr<IMap> attachedDbs;
    //try {
    // Close the database, which will cause subsequent operations to fail.
    // before that, get the attached database list first.
    //try {
    dbObj->GetAttachedDbs((IMap**)&attachedDbs);
    //} catch (SQLiteException e) {
        /* ignore */
    //}
    //try {
    ICloseable::Probe(dbObj)->Close();
    //} catch (SQLiteException e) {
        /* ignore */
    //}
    //} finally {
    // Delete all files of this corrupt database and/or attached databases
    if (attachedDbs != NULL) {
        AutoPtr<ISet> keys;
        attachedDbs->GetKeySet((ISet**)&keys);
        AutoPtr<IIterator> it;
        keys->GetIterator((IIterator**)&it);
        Boolean hasNext = FALSE;
        String name;
        while ((it->HasNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
            AutoPtr<IInterface> obj;
            entry->GetKey((IInterface**)&obj);
            assert(ICharSequence::Probe(obj) != NULL);
            ICharSequence::Probe(obj)->ToString(&name);

            AutoPtr<ICharSequence> value;
            attachedDbs->Get(obj, (IInterface**)&value);
            String second;
            value->ToString(&second);
            DeleteDatabaseFile(second);
        }
    }
    else {
        // attachedDbs = null is possible when the database is so corrupt that even
        // "PRAGMA database_list;" also fails. delete the main database file
        dbObj->GetPath(&path);
        DeleteDatabaseFile(path);
    }
    //}
    return NOERROR;
}

void CDefaultDatabaseErrorHandler::DeleteDatabaseFile(
    /* [in] */ const String& fileName)
{
    if (fileName.EqualsIgnoreCase(":memory:") || fileName.Trim().GetLength() == 0) {
        return;
    }
    Slogger::E(TAG, "deleting the database file: %s", fileName.string());
    //try {
    AutoPtr<IFile> mfile;
    CFile::New(fileName, (IFile**)&mfile);
    Boolean result;
    SQLiteDatabase::DeleteDatabase(mfile, &result);
    //} catch (Exception e) {
        /* print warning and ignore exception */
        // Log.w(TAG, "delete failed: " + e.getMessage());
    //}
}

} //Database
} //Droid
} //Elastos
