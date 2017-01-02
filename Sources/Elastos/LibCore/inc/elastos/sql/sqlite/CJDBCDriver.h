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

#ifndef __ELASTOS_SQL_SQLITE_CJDBCDRIVER_H__
#define __ELASTOS_SQL_SQLITE_CJDBCDRIVER_H__

#include "_Elastos_Sql_SQLite_CJDBCDriver.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Sql::IConnection;
using Elastos::Sql::IDriverPropertyInfo;
using Elastos::Utility::IProperties;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CJDBCDriver)
    , public Object
    , public IDriver
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI AcceptsURL(
        /* [in] */ const String& url,
        /* [out] */ Boolean * canOpen);

    CARAPI GetMajorVersion(
        /* [out] */ Int32 * ver);

    CARAPI GetMinorVersion(
        /* [out] */ Int32 * ver);

    CARAPI JdbcCompliant(
        /* [out] */ Boolean * isCompliant);

    CARAPI constructor();

    CARAPI Connect(
        /* [in] */ const String& url,
        /* [in] */ IProperties* info,
        /* [out] */ IConnection** connection);

    CARAPI GetPropertyInfo(
        /* [in] */ const String& url,
        /* [in] */ IProperties* info,
        /* [out] */ ArrayOf<IDriverPropertyInfo*>** array);

public:
    const static Int32 MAJORVERSION;
    static Boolean sSharedCache;
    static String mVfs;
    AutoPtr<IConnection> mConn;

private:
    //static java.lang.reflect.Constructor makeConn = null;
    static CARAPI_(Boolean) Init();

private:
    static Boolean sIsinit;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CJDBCDRIVER_H__
