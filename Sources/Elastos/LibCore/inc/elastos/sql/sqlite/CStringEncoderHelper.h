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

#ifndef __ELASTOS_SQL_SQLITE_CSTRINGENCODERHELPER_H__
#define __ELASTOS_SQL_SQLITE_CSTRINGENCODERHELPER_H__

#include "_Elastos_Sql_SQLite_CStringEncoderHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CStringEncoderHelper)
    , public Singleton
    , public IStringEncoderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte> * a,
        /* [out] */ String * str);

    CARAPI Decode(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte> ** outarray);

    CARAPI EncodeX(
        /* [in] */ ArrayOf<Byte> * a,
        /* [out] */ String * str);
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CSTRINGENCODERHELPER_H__
