
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
