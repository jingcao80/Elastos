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

#include "Elastos.CoreLibrary.IO.h"
#include "CDBDump.h"
#include "CTableResult.h"
#include "StringBuffer.h"
#include "StringUtils.h"

using Elastos::IO::IFlushable;
using Elastos::IO::EIID_IFlushable;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CDBDump)
CAR_INTERFACE_IMPL_2(CDBDump, Object, IDBDump, ICallback)

ECode CDBDump::Columns(
    /* [in] */ ArrayOf<String> * coldata)
{
    return NOERROR;
}

ECode CDBDump::Types(
    /* [in] */ ArrayOf<String> * types)
{
    return NOERROR;
}

ECode CDBDump::Newrow(
    /* [in] */ ArrayOf<String> * args,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    VALIDATE_NOT_NULL(args)

    if (args->GetLength() != 3) {
        *value = TRUE;
    }
    mShell->mPw->Print((*args)[2] + String(";"));
    if ((*args)[1].Equals("table") == 0) {
        CShell s2 ;
        mShell->Clone(s2);
        s2.mMode = CShell::MODE_Insert;
        s2.SetTableName((*args)[0]);
        AutoPtr<ArrayOf<String> > qargs = ArrayOf<String>::Alloc(1);
        (*qargs)[0] = (*args)[0];
        Boolean isflag = FALSE;
        s2.mDb->Is3(&isflag);
        ECode ec = NOERROR;
        if (isflag) {
            AutoPtr<ITableResult> t ;
            s2.mDb->GetTable(String("PRAGMA table_info('%q')")  , qargs,(ITableResult**)&t);
            String query;
            if (t != NULL) {
                StringBuffer sb;
                String sep = String("");

                sb.Append("SELECT ");
                for (Int32 i = 0; i < ((CTableResult *)t.Get())->mNrows; i++) {
                    String col = (*(((CTableResult *)t.Get())->mRows)[i])[1];
                    sb.Append(sep + "quote(" + CShell::SqlQuoteDbl(col) + ")");
                    sep = ",";
                }
                sb.Append(" from '%q'");
                query = sb.ToString();
                s2.mMode = CShell::MODE_Insert2;
            }
            else {
                query = String("SELECT * from '%q'");
            }
            ec = s2.mDb->Exec(query, &s2, qargs);
        }
        else {
            ec = s2.mDb->Exec(String("SELECT * from '%q'"), &s2, qargs);
        }
        if (ec != NOERROR) {
            mShell->mErr->Print(String("SQL Error: ")+StringUtils::ToString(ec,16));
            AutoPtr<IFlushable> iflush = (IFlushable*)mShell->mErr->Probe(EIID_IFlushable);
            iflush->Flush();
            *value = TRUE;
        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode CDBDump::constructor(
    /* [in] */ IShell * ms,
    /* [in] */ ArrayOf<String> * tables)
{
    mShell = (CShell *)ms;
    mShell->mPw->Print(String("BEGIN TRANSACTION;"));
    AutoPtr<IFlushable> iflush = (IFlushable*)mShell->mErr->Probe(EIID_IFlushable);
    if (tables == NULL || tables->GetLength() == 0) {
        ECode  ec = mShell->mDb->Exec(String("SELECT name, type, sql FROM sqlite_master ") +
                        String("WHERE type!='meta' AND sql NOT NULL ") +
                        String("ORDER BY substr(type,2,1), name"), (ICallback *)this);
        if (ec != NOERROR)
        {
            mShell->mErr->Print(String("SQL Error: ")+StringUtils::ToString(ec,16));
            iflush->Flush();
        }
    } else {
        AutoPtr<ArrayOf<String> > arg = ArrayOf<String>::Alloc(1);
        for (Int32 i = 0; i < tables->GetLength(); i++) {
            (*arg)[0] = (*tables)[i];
            ECode ec = mShell->mDb->Exec(String("SELECT name, type, sql FROM sqlite_master ") +
                                   String("WHERE tbl_name LIKE '%q' AND type!='meta' ") +
                                   String(" AND sql NOT NULL ") +
                                   String(" ORDER BY substr(type,2,1), name"),
                                   (ICallback *)this, arg);
         if(ec != NOERROR) {
            mShell->mErr->Print(String("SQL Error: ")+StringUtils::ToString(ec,16));
            iflush->Flush();
        }
        }
    }
    mShell->mPw->Print(String("COMMIT;"));
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
