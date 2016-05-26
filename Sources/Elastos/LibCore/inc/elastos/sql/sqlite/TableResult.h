
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
