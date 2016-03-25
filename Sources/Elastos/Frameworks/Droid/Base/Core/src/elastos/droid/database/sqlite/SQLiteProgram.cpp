
#include "elastos/droid/database/sqlite/SQLiteProgram.h"
#include "elastos/droid/database/sqlite/SQLiteStatementInfo.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

const AutoPtr< ArrayOf<String> > SQLiteProgram::EMPTY_STRING_ARRAY = ArrayOf<String>::Alloc(0);

CAR_INTERFACE_IMPL(SQLiteProgram, SQLiteClosable, ISQLiteProgram)

SQLiteProgram::SQLiteProgram()
    : mReadOnly(FALSE)
    , mNumParameters(0)
{}

ECode SQLiteProgram::Init(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICancellationSignal* cancellationSignalForPrepare)
{
    mDatabase = (SQLiteDatabase*)db;
    mSql = sql.Trim();
    Int32 n = DatabaseUtils::GetSqlStatementType(mSql);

    switch (n) {
        case DatabaseUtils_STATEMENT_BEGIN:
        case DatabaseUtils_STATEMENT_COMMIT:
        case DatabaseUtils_STATEMENT_ABORT:
            mReadOnly = FALSE;
            mColumnNames = EMPTY_STRING_ARRAY;
            mNumParameters = 0;
            break;

        default:
            Boolean assumeReadOnly = (n == DatabaseUtils_STATEMENT_SELECT);
            AutoPtr<SQLiteStatementInfo> info = new SQLiteStatementInfo();
            AutoPtr<SQLiteSession> session;
            FAIL_RETURN(mDatabase->GetThreadSession((SQLiteSession**)&session));
            Int32 flags = mDatabase->GetThreadDefaultConnectionFlags(assumeReadOnly);
            assert(session != NULL);
            FAIL_RETURN(session->Prepare(mSql, flags, cancellationSignalForPrepare, info));
            mReadOnly = info->mReadOnly;
            mColumnNames = info->mColumnNames;
            mNumParameters = info->mNumParameters;
            break;
    }

    if (bindArgs != NULL && bindArgs->GetLength() > mNumParameters) {
        //throw new IllegalArgumentException("Too many bind arguments.  "
        //        + bindArgs.length + " arguments were provided but the statement needs "
        //        + mNumParameters + " arguments.");
        Slogger::E("SQLiteProgram", "Too many bind arguments. %d arguments were provided"
            " but the statement needs %d arguments.", bindArgs->GetLength(), mNumParameters);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mNumParameters != 0) {
        mBindArgs = ArrayOf<IInterface*>::Alloc(mNumParameters);
        if (bindArgs != NULL) {
            for(Int32 i = 0; i < bindArgs->GetLength(); i++) {
                mBindArgs->Set(i, (*bindArgs)[i]);
            }

        }
    }
    else {
        mBindArgs = NULL;
    }
    return NOERROR;
}

AutoPtr<ISQLiteDatabase> SQLiteProgram::GetDatabase()
{
    return mDatabase;
}

String SQLiteProgram::GetSql()
{
    return mSql;
}

AutoPtr< ArrayOf<IInterface*> > SQLiteProgram::GetBindArgs()
{
    return mBindArgs;
}

AutoPtr< ArrayOf<String> > SQLiteProgram::GetColumnNames()
{
    return mColumnNames;
}

ECode SQLiteProgram::GetSession(
    /* [out] */ SQLiteSession** session)
{
    return mDatabase->GetThreadSession(session);
}

Int32 SQLiteProgram::GetConnectionFlags()
{
    return mDatabase->GetThreadDefaultConnectionFlags(mReadOnly);
}

ECode SQLiteProgram::OnCorruption()
{
    return mDatabase->OnCorruption();
}

ECode SQLiteProgram::GetUniqueId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = -1;
    return NOERROR;
}

ECode SQLiteProgram::BindNull(
    /* [in] */ Int32 index)
{
    return Bind(index, NULL);
}

ECode SQLiteProgram::BindInt64(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    AutoPtr<IInteger64> temp;
    CInteger64::New(value, (IInteger64**)&temp);
    return Bind(index, temp);
}

ECode SQLiteProgram::BindDouble(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    AutoPtr<IDouble> temp;
    CDouble::New(value, (IDouble**)&temp);
    return Bind(index, temp);
}

ECode SQLiteProgram::BindString(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    if (value.IsNullOrEmpty()) {
        // throw new IllegalArgumentException("the bind value at index " + index + " is null");
        Slogger::E(String("SQLiteProgram"), "the bind value at index %d is null", index);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> valueSequence;
    CString::New(value, (ICharSequence**)&valueSequence);
    return Bind(index, valueSequence);
}

ECode SQLiteProgram::BindBlob(
    /* [in] */ Int32 index,
    /* [in] */ const ArrayOf<Byte>& value)
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, value.GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < value.GetLength(); ++i) {
        AutoPtr<IByte> bv;
        CByte::New(value[i], (IByte**)&bv);
        array->Set(i, bv);
    }
    return Bind(index, array);
}

ECode SQLiteProgram::ClearBindings()
{
    if (mBindArgs != NULL) {
        for (Int32 i = 0; i < mBindArgs->GetLength(); ++i) {
            mBindArgs->Set(i, NULL);
        }
    }
    return NOERROR;
}

ECode SQLiteProgram::BindAllArgsAsStrings(
    /* [in] */ ArrayOf<String>* bindArgs)
{
    if (bindArgs != NULL) {
        for (Int32 i = bindArgs->GetLength(); i != 0; i--) {
            FAIL_RETURN(BindString(i, (*bindArgs)[i - 1]))
        }
    }
    return NOERROR;
}

void SQLiteProgram::OnAllReferencesReleased()
{
    ClearBindings();
}

ECode SQLiteProgram::Bind(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    if (index < 1 || index > mNumParameters) {
        //throw new IllegalArgumentException("Cannot bind argument at index "
        //        + index + " because the index is out of range.  "
        //        + "The statement has " + mNumParameters + " parameters.");
        Slogger::E(String("SQLiteProgram"), "Cannot bind argument at index %d because the index is out of range.  The statement has %d parameters."
                , index, mNumParameters);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBindArgs->Set(index - 1, value);
    return NOERROR;
}

} //Sqlite
} //Database
} //Droid
} //Elastos

