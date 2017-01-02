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

#ifndef __ELASTOS_SQL_SQLITE_TABLERESULT_H__
#define __ELASTOS_SQL_SQLITE_TABLERESULT_H__

#include "Elastos.CoreLibrary.Sql.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Sql {
namespace SQLite {

class TableResult
    : public Object
    , public ITableResult
    , public ICallback
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Columns(
        /* [in] */ ArrayOf<String> * coldata);

    CARAPI Types(
        /* [in] */ ArrayOf<String> * intypes);

    CARAPI Newrow(
        /* [in] */ ArrayOf<String> * rowdata,
        /* [out] */ Boolean* value);

    CARAPI ToString(
        /* [out] */ String* value);

    TableResult();

    TableResult(
        /* [in] */ Int32 maxrows);

    CARAPI Clear();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 maxrows);

public:
    Int32 mNcolumns;
    Int32 mNrows;
    AutoPtr<ArrayOf<String> > mColumn;
    AutoPtr<ArrayOf<String> > mTypes;
    Int32 mMaxRows;
    Vector<AutoPtr<ArrayOf<String> > > mRows;
    Boolean mAtMaxRows;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_TABLERESULT_H__
