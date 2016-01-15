#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDERHELPER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDERHELPER_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteQueryBuilderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IStringBuilder;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteQueryBuilderHelper)
    , public Singleton
    , public ISQLiteQueryBuilderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI BuildQueryString(
        /* [in] */ Boolean distinct,
        /* [in] */ const String& tables,
        /* [in] */ ArrayOf<String>* columns,
        /* [in] */ const String& where,
        /* [in] */ const String& groupBy,
        /* [in] */ const String& having,
        /* [in] */ const String& orderBy,
        /* [in] */ const String& limit,
        /* [out] */ String* queryStr);

    CARAPI AppendColumns(
        /* [in] */ IStringBuilder* s,
        /* [in] */ const ArrayOf<String>& columns);
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDERHELPER_H__
