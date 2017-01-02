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

#ifndef __ELASTOS_SQL_SQLITE_CFUNCTIONCONTEXT_H__
#define __ELASTOS_SQL_SQLITE_CFUNCTIONCONTEXT_H__

#include "_Elastos_Sql_SQLite_CFunctionContext.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CFunctionContext)
    , public Object
    , public IFunctionContext
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetResult(
        /* [in] */ const String& ret);

    CARAPI SetResult(
        /* [in] */ Int32 i);

    CARAPI SetResult(
        /* [in] */ Double d);

    CARAPI SetError(
        /* [in] */ const String& err);

    CARAPI SetResult(
        /* [in] */ const ArrayOf<Byte>& b);

    CARAPI SetResultZeroblob(
        /* [in] */ Int32 n);

    CARAPI Count(
        /* [out] */ Int32* count);
    CFunctionContext();

public:
    Int64 mHandle;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CFUNCTIONCONTEXT_H__
