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

#ifndef __ELASTOS_SQL_SQLITE_CSHELL_H__
#define __ELASTOS_SQL_SQLITE_CSHELL_H__

#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_Sql_SQLite_CShell.h"
#include "CDatabase.h"

using Elastos::IO::IPrintWriter;
using Elastos::IO::IPrintStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CShell)
    , public Object
    , public IShell
    , public ICallback
{
public:
    CShell();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Columns(
        /* [in] */ ArrayOf<String> * args);

    CARAPI Types(
        /* [in] */ ArrayOf<String> * types);

    CARAPI Newrow(
        /* [in] */ ArrayOf<String> * args,
        /* [out] */ Boolean * pValue);

    CARAPI constructor(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IPrintWriter* err);

    CARAPI constructor(
        /* [in] */ IPrintStream* pw,
        /* [in] */ IPrintStream* errs);

public:
    CARAPI_(void) Clone(
        /* [out] */ CShell& s);

    CARAPI_(void) SetTableName(
        /* [out] */ String str);

    static CARAPI_(String) HtmlQuote(
        /* [in] */ const String& str);

    static CARAPI_(Boolean) IsNumeric(
        /* [in] */ const String& str);

    static CARAPI_(String) SqlQuoteDbl(
        /* [in] */ const String& str);

    static CARAPI_(String) SqlQuote(
        /* [in] */ const String& str);

public:
    AutoPtr<CDatabase> mDb;
    Boolean mEcho;
    Int32 mCount;
    Int32 mMode;
    Boolean mShowHeader;
    String mTableName;
    String mSep;
    AutoPtr<ArrayOf<String> > mCols;
    AutoPtr<ArrayOf<Int32> > mColwidth;
    String mDestTable;
    AutoPtr<IPrintWriter> mPw;
    AutoPtr<IPrintWriter> mErr;

    const static Int32 MODE_Line = 0;
    const static Int32 MODE_Column = 1;
    const static Int32 MODE_List = 2;
    const static Int32 MODE_Semi = 3;
    const static Int32 MODE_Html = 4;
    const static Int32 MODE_Insert = 5;
    const static Int32 MODE_Insert2 = 6;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CSHELL_H__
