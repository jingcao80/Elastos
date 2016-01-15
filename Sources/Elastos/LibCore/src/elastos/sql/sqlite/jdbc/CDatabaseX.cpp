
#include "CDatabaseX.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

Object CDatabaseX::mLock;

CAR_OBJECT_IMPL(CDatabaseX)

CDatabaseX::CDatabaseX()
{
}

ECode CDatabaseX::Exec(
    /* [in] */ const String& sql,
    /* [in] */ ICallback * cb)
{
    ECode ec = NOERROR;
    ec = Exec(sql, cb);
    mLock.Lock();
    mLock.NotifyAll();
    mLock.Unlock();
    return ec;
}

ECode CDatabaseX::Exec(
    /* [in] */ const String& sql,
    /* [in] */ ICallback * cb,
    /* [in] */ ArrayOf<String> * args)
{
    ECode ec = NOERROR;
    ec = Exec(sql, cb, args);
    mLock.Lock();
    mLock.NotifyAll();
    mLock.Unlock();
    return ec;
}

ECode CDatabaseX::GetTable(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String> * args,
    /* [out] */ ITableResult ** tbl)
{
    ECode ec = NOERROR;
    ec = GetTable(sql, args, tbl);
    mLock.Lock();
    mLock.NotifyAll();
    mLock.Unlock();
    return ec;
}

ECode CDatabaseX::GetTable(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String> * args,
    /* [in] */ ITableResult * tbl)
{
    ECode ec = NOERROR;
    ec = GetTable(sql, args, tbl);
    mLock.Lock();
    mLock.NotifyAll();
    mLock.Unlock();
    return ec;
}

void CDatabaseX::Wait(Int32 ms)
{
    mLock.Lock();
    mLock.Wait(ms);
    mLock.Unlock();
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos
