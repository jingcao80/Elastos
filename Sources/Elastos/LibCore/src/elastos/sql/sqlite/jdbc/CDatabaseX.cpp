//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    ec = Database::Exec(sql, cb);
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
    ec = Database::Exec(sql, cb, args);
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
    ec = Database::GetTable(sql, args, tbl);
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
    ec = Database::GetTable(sql, args, tbl);
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
