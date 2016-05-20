#ifndef __ELASTOS_SQL_SQLITE_DATABASE_H__
#define __ELASTOS_SQL_SQLITE_DATABASE_H__

#include "Elastos.CoreLibrary.Sql.h"
#include "sqlitejni.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {
namespace SQLite {

class Database
    : public Object
    , public IDatabase
{
public:
    CAR_INTERFACE_DECL()

    Database();
    ~Database();

    CARAPI Open(
        /* [in] */ const String& filename,
        /* [in] */ Int32 mode);

    CARAPI Open(
        /* [in] */ const String& filename,
        /* [in] */ Int32 mode,
        /* [in] */ const String& vfs);

    CARAPI Open(
        /* [in] */ const String& filename,
        /* [in] */ Int32 mode,
        /* [in] */ const String& vfs,
        /* [in] */ Boolean ver2);

    CARAPI OpenAuxFile(
        /* [in] */ const String& filename);

    CARAPI Finalize();

    CARAPI Close();

    CARAPI Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback* cb);

    CARAPI Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback* cb,
        /* [in] */ ArrayOf<String> * argsstr);

    CARAPI LastInsertRowid(
        /* [out] */ Int64* id);

    CARAPI Interrupt();

    CARAPI Changes(
        /* [out] */ Int64* id);

    CARAPI BusyHandler(
        /* [in] */ IBusyHandler* bh);

    CARAPI BusyTimeout(
        /* [in] */ Int32 ms);

    virtual CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ Int32 maxrows,
        /* [out] */ ITableResult ** tableresult);

    virtual CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [out] */ ITableResult ** tableresult);

    virtual CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ Int32 maxrows,
        /* [in] */ ArrayOf<String> * args,
        /* [out] */ ITableResult ** tableresult);

    virtual CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<String> * args,
        /* [out] */ ITableResult ** tableresult);

    virtual CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<String> * args,
        /* [in] */ ITableResult* tbl);

    virtual CARAPI Dbversion(
        /* [out] */ String * ver);

    CARAPI CreateFunction(
        /* [in] */ const String& name,
        /* [in] */ Int32 nargs,
        /* [in] */ IFunction* f);

    CARAPI CreateAggregate(
        /* [in] */ const String& name,
        /* [in] */ Int32 nargs,
        /* [in] */ IFunction* f);

    CARAPI FunctionType(
        /* [in] */ const String& name,
        /* [in] */ Int32 type);

    virtual CARAPI LastError(
        /* [out] */ Int32 * err);

    CARAPI SetLastError(
        /* [in] */ Int32 error_code);

    virtual CARAPI ErrorMessage(
        /* [out] */ String * str);

    CARAPI SetEncoding(
        /* [in] */ const String& enc);

    CARAPI SetAuthorizer(
        /* [in] */ IAuthorizer* auth);

    CARAPI Trace(
        /* [in] */ ITrace* tr);

    virtual CARAPI Backup(
        /* [in] */ IDatabase* dest,
        /* [in] */ const String& destName,
        /* [in] */ const String& srcName,
        /* [out] */ IBackup ** backup);

    CARAPI Profile(
        /* [in] */ IProfile* pr);

    virtual CARAPI DbStatus(
        /* [in] */ Int32 op,
        /* [in] */ ArrayOf<Int32> * info,
        /* [in] */ Boolean flag,
        /* [out] */ Int32 * status);

    virtual CARAPI Compile(
        /* [in] */ const String& sql,
        /* [out] */ IVm ** ivm);

    virtual CARAPI Compile(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<String> * args,
        /* [out] */ IVm ** ivm);

    virtual CARAPI Prepare(
        /* [in] */ const String& sql,
        /* [out] */ IStmt ** st);

    virtual CARAPI OpenBlob(
        /* [in] */ const String& db,
        /* [in] */ const String& table,
        /* [in] */ const String& column,
        /* [in] */ Int64 row,
        /* [in] */ Boolean rw,
        /* [out] */ IBlob ** blob);

    virtual CARAPI Is3(
        /* [out] */ Boolean * value);

    CARAPI ProgressHandler(
        /* [in] */ Int32 n,
        /* [in] */ IProgressHandler* p);

    CARAPI Key(
        /* [in] */ ArrayOf<Byte> * ekey);

    CARAPI Key(
        /* [in] */ const String& skey);

    static CARAPI_(Boolean) Complete(
        /* [in] */ const String& sql);

    static CARAPI_(String) Version();

    static CARAPI_(String) ErrorString(
        /* [in] */ Int32 error_code);

    static CARAPI_(Int32) Status(
        /* [in] */ Int32 op,
        /* [in] */ ArrayOf<Int32>* info,
        /* [in] */ Boolean flag);

    static CARAPI_(Int64) LongFromJulian(
        /* [in] */ Double d);

    static CARAPI_(Int64) LongFromJulian(
        /* [in] */ const String& s);

    static CARAPI_(Double) JulianFromLong(
        /* [in] */ Int64 ms);

private:
    CARAPI _Open(
        /* [in] */ const String& filename,
        /* [in] */ Int32 mode);

    CARAPI _Open4(
        /* [in] */ const String& filename,
        /* [in] */ Int32 mode,
        /* [in] */ const String& vfs,
        /* [in] */ Boolean ver2);

    CARAPI _OpenAuxFile(
        /* [in] */ const String& filename);

    CARAPI _Finalize();

    CARAPI _Close();

    CARAPI _Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback* cb);

    CARAPI _Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback* cb,
        /* [in] */ ArrayOf<String> * args);

    CARAPI_(Int64) _LastInsertRowid();

    CARAPI _Interrupt();

    CARAPI_(Int64) _Changes();

    CARAPI _BusyHandler(
        /* [in] */ IBusyHandler* bh);

    CARAPI _BusyTimeout(
        /* [in] */ Int32 ms);

    CARAPI _CreateFunction(
        /* [in] */ const String& name,
        /* [in] */ Int32 nargs,
        /* [in] */ IFunction* f);

    CARAPI _CreateAggregate(
        /* [in] */ const String& name,
        /* [in] */ Int32 nargs,
        /* [in] */ IFunction* f);

    CARAPI _FunctionType(
        /* [in] */ const String& name,
        /* [in] */ Int32 type);

    CARAPI_(String) _Errmsg();

    CARAPI _SetEncoding(
        /* [in] */ const String& enc);

    CARAPI _SetAuthorizer(
        /* [in] */ IAuthorizer* auth);

    CARAPI _Trace(
        /* [in] */ ITrace* tr);

    CARAPI _Profile(
        /* [in] */ IProfile* pr);

    CARAPI_(Int32) _DbStatus(
        /* [in] */ Int32 op,
        /* [in] */ ArrayOf<Int32> * info,
        /* [in] */ Boolean flag);

    CARAPI Vm_compile(
        /* [in] */ const String& sql,
        /* [in] */ IVm* vm);

    CARAPI Vm_compile_args(
        /* [in] */ const String& sql,
        /* [in] */ IVm* vm,
        /* [in] */ ArrayOf<String> * argsstr);

    CARAPI Stmt_prepare(
        /* [in] */ const String& sql,
        /* [in] */ IStmt* stmt);

    CARAPI _OpenBlob(
        /* [in] */ const String& db,
        /* [in] */ const String& table,
        /* [in] */ const String& column,
        /* [in] */ Int64 row,
        /* [in] */ Boolean rw,
        /* [in] */ IBlob* blob);

    CARAPI _ProgressHandler(
        /* [in] */ Int32 n,
        /* [in] */ IProgressHandler* ph);

    CARAPI _Key(
        /* [in] */ ArrayOf<Byte> * ekey);

    CARAPI _Rekey(
        /* [in] */ ArrayOf<Byte> * ekey);

    CARAPI doclose(
        /* [in] */ Int32 final);

    static CARAPI_(Boolean) _Complete(
        /* [in] */ const String& sql);

    static CARAPI _Backup(
        /* [in] */ IBackup* b,
        /* [in] */ IDatabase* dest,
        /* [in] */ const String& destName,
        /* [in] */ IDatabase* src,
        /* [in] */ const String& srcName);

    static CARAPI_(Int32) _Status(
        /* [in] */ Int32 op,
        /* [in] */ ArrayOf<Int32>* info,
        /* [in] */ Boolean flag);

    static CARAPI_(Boolean) _Enable_shared_cache(
        /* [in] */ Boolean onoff);

    static CARAPI_(void) Internal_init();

private:
    Int64 mHandle;
    Int32 mError_code;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif //__ELASTOS_SQL_SQLITE_DATABASE_H__
