
#include "CJDBCDatabaseMetaData.h"
#include "Database.h"
#include <StringBuffer.h>
#include <StringUtils.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Pair;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Sql::ITypes;
using Elastos::Sql::SQLite::CTableResult;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CAR_OBJECT_IMPL(CJDBCDatabaseMetaData)

CAR_INTERFACE_IMPL_3(CJDBCDatabaseMetaData, Object, IJDBCDatabaseMetaData, IDatabaseMetaData, IWrapper);

ECode CJDBCDatabaseMetaData::AllProceduresAreCallable(
    /* [out] */ Boolean * callable)
{
    *callable = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::AllTablesAreSelectable(
    /* [out] */ Boolean * selectable)
{
    *selectable = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::DataDefinitionCausesTransactionCommit(
    /* [out] */ Boolean * commit)
{
    *commit = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::DataDefinitionIgnoredInTransactions(
    /* [out] */ Boolean * ignored)
{
    *ignored = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::DeletesAreDetected(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * detected)
{
    *detected = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::DoesMaxRowSizeIncludeBlobs(
    /* [out] */ Boolean * doesmax)
{
    *doesmax = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetAttributes(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& typeNamePattern,
    /* [in] */ const String& attributeNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    // throw new SQLException("not supported");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetBestRowIdentifier(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schema,
    /* [in] */ const String& table,
    /* [in] */ Int32 scope,
    /* [in] */ Boolean nullable,
    /* [out] */ IResultSet ** resultSet)
{
    VALIDATE_NOT_NULL(resultSet);
    *resultSet = NULL;

    AutoPtr<IJDBCStatement> s0Obj, s1Obj;
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s0Obj));
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s1Obj));
    CJDBCStatement* s0 = (CJDBCStatement*)s0Obj.Get();
    CJDBCStatement* s1 = (CJDBCStatement*)s1Obj.Get();
    AutoPtr<IResultSet> rs0Obj, rs1Obj;

    // try {
    //     try {
        conn->mDb->Exec(String("SELECT 1 FROM sqlite_master LIMIT 1"), NULL);
    //     }
    //     catch (SQLite.Exception se) {
    //     throw new SQLException("schema reload failed");
    //     }
        s0->ExecuteQuery(String("PRAGMA index_list(") +
                         Elastos::Sql::SQLite::CShell::SqlQuote(table) +
                         String(")"),(IResultSet**)&rs0Obj);
        s1->ExecuteQuery(String("PRAGMA table_info(") +
                         Elastos::Sql::SQLite::CShell::SqlQuote(table) +
                         String(")"),(IResultSet**)&rs1Obj);
    CJDBCResultSet* rs0 = (CJDBCResultSet*)rs0Obj.Get();
    CJDBCResultSet* rs1 = (CJDBCResultSet*)rs1Obj.Get();
    // } catch (SQLException e) {
    //     throw e;
    // } finally {
    //     s0.close();
    //     s1.close();
    // }
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(8);
    (*cols)[0] = String("SCOPE");
    (*cols)[1] = String("COLUMN_NAME");
    (*cols)[2] = String("DATA_TYPE");
    (*cols)[3] = String("TYPE_NAME");
    (*cols)[4] = String("COLUMN_SIZE");
    (*cols)[5] = String("BUFFER_LENGTH");
    (*cols)[6] = String("DECIMAL_DIGITS");
    (*cols)[7] = String("PSEUDO_COLUMN");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(8);
    (*types)[0] = ITypes::SMALLINT;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::SMALLINT;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::INTEGER;
    (*types)[5] = ITypes::INTEGER;
    (*types)[6] = ITypes::SMALLINT;
    (*types)[7] = ITypes::SMALLINT;
    AutoPtr<ITableResultX> trObj;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&trObj));
    CTableResultX* tr = (CTableResultX*)trObj.Get();
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<IJDBCResultSet> rs ;
    FAIL_RETURN(CJDBCResultSet::New(ITableResult::Probe(trObj), NULL,(IJDBCResultSet**)&rs));
    if (rs0 != NULL && rs0->tr != NULL && rs0->tr->mNrows > 0 &&
        rs1 != NULL && rs1->tr != NULL && rs1->tr->mNrows > 0) {

        HashMap<String, Int32> h0;
        for (Int32 i = 0; i < rs0->tr->mNcolumns; i++) {
            h0.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[i], i));
        }
        HashMap<String, Int32> h1;
        for (Int32 i = 0; i < rs1->tr->mNcolumns; i++) {
            h1.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[i], i)); // android-changed
        }
        for (Int32 i = 0; i < rs0->tr->mNrows; i++) {
            AutoPtr<ArrayOf<String> > r0 = rs0->tr->mRows[i];
            Int32 col = h0[String("unique")];
            String uniq = (*r0)[col];
            col = h0[String("name")];
            String iname = (*r0)[col];
            if (uniq.GetChar(0) == '0') {
                continue;
            }
            AutoPtr<IJDBCStatement> s2Obj;
            FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s2Obj));
            CJDBCStatement* s2 = (CJDBCStatement*)s2Obj.Get();
            AutoPtr<IResultSet> rs2Obj;
            // try {
                s2->ExecuteQuery(String("PRAGMA index_info(") +
                                 Elastos::Sql::SQLite::CShell::SqlQuote(iname) +
                                 String(")"),(IResultSet**)&rs2Obj);
                CJDBCResultSet* rs2 = (CJDBCResultSet*)rs2Obj.Get();
            // } catch (SQLException e) {
            // } finally {
            //     s2.close();
            // }
            if (rs2 == NULL || rs2->tr == NULL || rs2->tr->mNrows <= 0) {
                continue;
            }
            HashMap<String, Int32> h2 ;
            for (Int32 k = 0; k < rs2->tr->mNcolumns; k++) {
                h2.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[k], k));
            }
            for (Int32 k = 0; k < rs2->tr->mNrows; k++) {
                AutoPtr<ArrayOf<String> > r2 = rs2->tr->mRows[k];
                col = h2[String("name")];
                String cname = (*r2)[col];
                for (Int32 m = 0; m < rs1->tr->mNrows; m++) {
                    AutoPtr<ArrayOf<String> > r1 = rs1->tr->mRows[m];
                    col = h1[String("name")];
                    if (cname.Compare((*r1)[col]) == 0) {
                        AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(cols->GetLength());
                        (*row)[0] = String("") + StringUtils::ToString(scope);
                        (*row)[1] = cname;
                        (*row)[2] = String("") + StringUtils::ToString(ITypes::VARCHAR);
                        (*row)[3] = String("VARCHAR");
                        (*row)[4] = String("65536");
                        (*row)[5] = String("0");
                        (*row)[6] = String("0");
                        (*row)[7] = String("") + StringUtils::ToString(IDatabaseMetaData::bestRowNotPseudo);
                        Boolean rowflag = FALSE;
                        tr->Newrow(row, &rowflag);
                    }
                }
            }
        }
    }
    if (tr->mNrows <= 0) {
        AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(cols->GetLength());
        (*row)[0] = String("") + StringUtils::ToString(scope);
        (*row)[1] = String("_ROWID_");
        (*row)[2] = String("") + StringUtils::ToString(ITypes::INTEGER);
        (*row)[3] = String("INTEGER");
        (*row)[4] = String("10");
        (*row)[5] = String("0");
        (*row)[6] = String("0");
        (*row)[7] = String("") + StringUtils::ToString(IDatabaseMetaData::bestRowPseudo);
        Boolean rowflag = FALSE;
        tr->Newrow(row, &rowflag);
    }
    *resultSet = IResultSet::Probe(rs);
    REFCOUNT_ADD(*resultSet)
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetCatalogs(
    /* [out] */ IResultSet ** resultSet)
{
    VALIDATE_NOT_NULL(resultSet)
    *resultSet = NULL;

    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(1);
    (*cols)[0] = String("TABLE_CAT");
    AutoPtr<CTableResult> tr;
    FAIL_RETURN(CTableResult::New((ITableResult**)&tr));
    tr->Columns(cols);
    AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(1) ;
    (*row)[0] = String("");
    Boolean flag = FALSE;
    tr->Newrow(row, &flag);
    AutoPtr<IJDBCResultSet> jrs;
    FAIL_RETURN(CJDBCResultSet::New(tr,NULL,(IJDBCResultSet**)&jrs));
    *resultSet = IResultSet::Probe(jrs);
    REFCOUNT_ADD(*resultSet)
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetCatalogSeparator(
    /* [out] */ String * str)
{
    *str = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetCatalogTerm(
    /* [out] */ String * str)
{
    *str = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetColumnPrivileges(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schema,
    /* [in] */ const String& table,
    /* [in] */ const String& columnNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    VALIDATE_NOT_NULL(resultSet);
    *resultSet = NULL;

    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(8);
    (*cols)[0] = String("TABLE_CAT");
    (*cols)[1] = String("TABLE_SCHEM");
    (*cols)[2] = String("TABLE_NAME");
    (*cols)[3] = String("COLUMN_NAME");
    (*cols)[4] = String("GRANTOR");
    (*cols)[5] = String("GRANTEE");
    (*cols)[6] = String("PRIVILEGE");
    (*cols)[7] = String("IS_GRANTABLE");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(8);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::VARCHAR;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::VARCHAR;
    (*types)[7] = ITypes::VARCHAR;

    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<IJDBCResultSet> rs;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL ,(IJDBCResultSet**)&rs));
    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetColumns(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& tableNamePattern,
    /* [in] */ const String& inColumnNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    String columnNamePattern = inColumnNamePattern;
    if (conn->mDb == NULL) {
        // throw new SQLException("connection closed.");
    }
    AutoPtr<IJDBCStatement> sObj ;
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&sObj));
    CJDBCStatement* s = (CJDBCStatement*)sObj.Get();
    AutoPtr<IResultSet> rs0Obj ;
    // try {
    //     try {
             conn->mDb->Exec(String("SELECT 1 FROM sqlite_master LIMIT 1"), NULL);
    //     } catch (SQLite.Exception se) {
    //     throw new SQLException("schema reload failed");
    //     }
             s->ExecuteQuery(String("PRAGMA table_info(") +
                     SQLite::CShell::SqlQuote(tableNamePattern) +
                     String(")"),(IResultSet**)&rs0Obj);
             s->Close();
            CJDBCResultSet* rs0 = (CJDBCResultSet*)rs0Obj.Get();
    // } catch (SQLException e) {
    //     throw e;
    // } finally {
    //     s.close();
    // }
    if (rs0->tr->mNrows < 1) {
        //throw new SQLException("no such table: " + tableNamePattern);
    }
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(18);
    (*cols)[0] = String("TABLE_CAT");
    (*cols)[1] = String("TABLE_SCHEM");
    (*cols)[2] = String("TABLE_NAME");
    (*cols)[3] = String("COLUMN_NAME");
    (*cols)[4] = String("DATA_TYPE");
    (*cols)[5] = String("TYPE_NAME");
    (*cols)[6] = String("COLUMN_SIZE");
    (*cols)[7] = String("BUFFER_LENGTH");
    (*cols)[8] = String("DECIMAL_DIGITS");
    (*cols)[9] = String("NUM_PREC_RADIX");
    (*cols)[10] = String("NULLABLE");
    (*cols)[11] = String("REMARKS");
    (*cols)[12] = String("COLUMN_DEF");
    (*cols)[13] = String("SQL_DATA_TYPE");
    (*cols)[14] = String("SQL_DATETIME_SUB");
    (*cols)[15] = String("CHAR_OCTET_LENGTH");
    (*cols)[16] = String("ORDINAL_POSITION");
    (*cols)[17] = String("IS_NULLABLE");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(18);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::SMALLINT;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::INTEGER;
    (*types)[7] = ITypes::INTEGER;
    (*types)[8] = ITypes::INTEGER;
    (*types)[9] = ITypes::INTEGER;
    (*types)[10] = ITypes::INTEGER;
    (*types)[11] = ITypes::VARCHAR;
    (*types)[12] = ITypes::VARCHAR;
    (*types)[13] = ITypes::INTEGER;
    (*types)[14] = ITypes::INTEGER;
    (*types)[15] = ITypes::INTEGER;
    (*types)[16] = ITypes::INTEGER;
    (*types)[17] = ITypes::VARCHAR;
    AutoPtr<ITableResultX> trObj;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&trObj));
    CTableResultX* tr = (CTableResultX*)trObj.Get();
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<IJDBCResultSet> rsObj;
    FAIL_RETURN(CJDBCResultSet::New(ITableResult::Probe(tr),NULL,(IJDBCResultSet**)&rsObj));
    CJDBCResultSet* rs = (CJDBCResultSet*)rsObj.Get();
    if (rs0 != NULL && rs0->tr != NULL && rs0->tr->mNrows > 0) {
        HashMap<String, Int32> h ;
        for (Int32 i = 0; i < rs0->tr->mNcolumns; i++) {
            h.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[i], i)); // android-changed
        }
        if (columnNamePattern != NULL && columnNamePattern.GetChar(0) == '%') {
            columnNamePattern = NULL;
        }
        for (Int32 i = 0; i < rs0->tr->mNrows; i++) {
            AutoPtr<ArrayOf<String> > r0 = rs0->tr->mRows[i];
            Int32 col =  h[String("name")];
            if (columnNamePattern != NULL) {
                if ((*r0)[col].Compare(columnNamePattern) != 0) {
                    continue;
                }
            }
            AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(cols->GetLength());
            (*row)[0]  = String("");
            (*row)[1]  = String("");
            (*row)[2]  = tableNamePattern;
            (*row)[3]  = (*r0)[col];
            col = h[String("type")];
            String typeStr = (*r0)[col];
            Int32 type = MapSqlType(typeStr);
            (*row)[4]  = StringUtils::ToString(type);
            (*row)[5]  = MapTypeName(type);
            (*row)[6]  = StringUtils::ToString(GetD(typeStr, type));
            (*row)[7]  = StringUtils::ToString(GetM(typeStr, type));
            (*row)[8]  = String("10");
            (*row)[9]  = String("0");
            (*row)[11] = String(NULL);
            col = h[String("dflt_value")];
            (*row)[12] = (*r0)[col];
            (*row)[13] = String("0");
            (*row)[14] = String("0");
            (*row)[15] = String("65536");
            col = h[String("cid")];
            (*row)[16] = StringUtils::ToString(StringUtils::ParseInt32((*r0)[col]) + 1); // android-changed
            col = h[String("notnull")];
            (*row)[17] = ((*r0)[col].GetChar(0) == '0') ? String("YES") : String("NO");
            (*row)[10] = ((*r0)[col].GetChar(0) == '0') ?
                        String("") + StringUtils::ToString(Elastos::Sql::IDatabaseMetaData::columnNullable) :
                        String("") + StringUtils::ToString(Elastos::Sql::IDatabaseMetaData::columnNoNulls);
            Boolean rowflag = FALSE;
            tr->Newrow(row,&rowflag);
        }
    }
    *resultSet = IResultSet::Probe(rs);
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetConnection(
    /* [out] */ IConnection ** connection)
{
    *connection = IConnection::Probe(conn);
    REFCOUNT_ADD(*connection)
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetCrossReference(
    /* [in] */ const String& primaryCatalog,
    /* [in] */ const String& primarySchema,
    /* [in] */ const String& primaryTable,
    /* [in] */ const String& foreignCatalog,
    /* [in] */ const String& foreignSchema,
    /* [in] */ const String& foreignTable,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<CJDBCResultSet> rs0 ;
    if (!foreignTable.IsNull() && foreignTable.GetChar(0) != '%') {
        AutoPtr<CJDBCStatement> s0;
        FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s0));
        // try {
        // try {
            conn->mDb->Exec(String("SELECT 1 FROM sqlite_master LIMIT 1"), NULL);
        // } catch (SQLite.Exception se) {
        //     throw new SQLException("schema reload failed");
        // }
            s0->ExecuteQuery(String("PRAGMA foreign_key_list(") +
                         Elastos::Sql::SQLite::CShell::SqlQuote(foreignTable) +
                         String(")"),(IResultSet**)&rs0);
        // } catch (SQLException e) {
        // throw e;
        // } finally {
        // s0.close();
        // }
    }
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(14);
    (*cols)[0] = String("PKTABLE_CAT");
    (*cols)[1] = String("PKTABLE_SCHEM");
    (*cols)[2] = String("PKTABLE_NAME");
    (*cols)[3] = String("PKCOLUMN_NAME");
    (*cols)[4] = String("FKTABLE_CAT");
    (*cols)[5] = String("FKTABLE_SCHEM");
    (*cols)[6] = String("FKTABLE_NAME");
    (*cols)[7] = String("FKCOLUMN_NAME");
    (*cols)[8] = String("KEY_SEQ");
    (*cols)[9] = String("UPDATE_RULE");
    (*cols)[10] = String("DELETE_RULE");
    (*cols)[11] = String("FK_NAME");
    (*cols)[12] = String("PK_NAME");
    (*cols)[13] = String("DEFERRABILITY");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(14);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::VARCHAR;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::VARCHAR;
    (*types)[7] = ITypes::VARCHAR;
    (*types)[8] = ITypes::SMALLINT;
    (*types)[9] = ITypes::SMALLINT;
    (*types)[10] = ITypes::SMALLINT;
    (*types)[11] = ITypes::VARCHAR;
    (*types)[12] = ITypes::VARCHAR;
    (*types)[13] = ITypes::SMALLINT;
    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<CJDBCResultSet> rs ;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL,(IJDBCResultSet**)&rs));
    if (rs0 != NULL && rs0->tr != NULL && rs0->tr->mNrows > 0) {
        String pktable = String(NULL);
        if (!primaryTable.IsNull() && primaryTable.GetChar(0) != '%') {
            pktable = primaryTable;
        }
        InternalImportedKeys(foreignTable, pktable, rs0, tr);
    }
    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDatabaseMajorVersion(
    /* [out] */ Int32 * ver)
{
    *ver = SQLite::CJDBCDriver::MAJORVERSION;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDatabaseMinorVersion(
    /* [out] */ Int32 * ver)
{
    *ver = SQLite::IConstants::drv_minor;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDatabaseProductName(
    /* [out] */ String * name)
{
    *name = String("SQLite");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDatabaseProductVersion(
    /* [out] */ String * ver)
{
    *ver = SQLite::Database::Version();
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDefaultTransactionIsolation(
    /* [out] */ Int32 * level)
{
    *level = IConnection::TRANSACTION_SERIALIZABLE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDriverMajorVersion(
    /* [out] */ Int32 * ver)
{
    *ver = SQLite::CJDBCDriver::MAJORVERSION;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDriverMinorVersion(
    /* [out] */ Int32 * ver)
{
    *ver = SQLite::IConstants::drv_minor;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDriverName(
    /* [out] */ String * name)
{
    *name = String("SQLite/JDBC");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetDriverVersion(
    /* [out] */ String * ver)
{
    *ver = String("") + StringUtils::ToString(SQLite::CJDBCDriver::MAJORVERSION) +
           String(".") + StringUtils::ToString(SQLite::IConstants::drv_minor);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetExportedKeys(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schema,
    /* [in] */ const String& table,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(14);
    (*cols)[0] = String("PKTABLE_CAT");
    (*cols)[1] = String("PKTABLE_SCHEM");
    (*cols)[2] = String("PKTABLE_NAME");
    (*cols)[3] = String("PKCOLUMN_NAME");
    (*cols)[4] = String("FKTABLE_CAT");
    (*cols)[5] = String("FKTABLE_SCHEM");
    (*cols)[6] = String("FKTABLE_NAME");
    (*cols)[7] = String("FKCOLUMN_NAME");
    (*cols)[8] = String("KEY_SEQ");
    (*cols)[9] = String("UPDATE_RULE");
    (*cols)[10] = String("DELETE_RULE");
    (*cols)[11] = String("FK_NAME");
    (*cols)[12] = String("PK_NAME");
    (*cols)[13] = String("DEFERRABILITY");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(14);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::VARCHAR;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::VARCHAR;
    (*types)[7] = ITypes::VARCHAR;
    (*types)[8] = ITypes::SMALLINT;
    (*types)[9] = ITypes::SMALLINT;
    (*types)[10] = ITypes::SMALLINT;
    (*types)[11] = ITypes::VARCHAR;
    (*types)[12] = ITypes::VARCHAR;
    (*types)[13] = ITypes::SMALLINT;
    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<CJDBCResultSet> rs ;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL,(IJDBCResultSet**)&rs));
    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetExtraNameCharacters(
    /* [out] */ String * name)
{
    *name = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetIdentifierQuoteString(
    /* [out] */ String * quote)
{
    *quote = String("\"");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetImportedKeys(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schema,
    /* [in] */ const String& table,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<CJDBCStatement> s0;
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s0));
    AutoPtr<CJDBCResultSet> rs0;
    // try {
    //     try {
            conn->mDb->Exec(String("SELECT 1 FROM sqlite_master LIMIT 1"), NULL);
    //     }
    //     catch (SQLite.Exception se) {
    //     throw new SQLException("schema reload failed");
    //     }
            s0->ExecuteQuery(String("PRAGMA foreign_key_list(") +
                         Elastos::Sql::SQLite::CShell::SqlQuote(table) +
                         String(")"),(IResultSet**)&rs0);
    // } catch (SQLException e) {
    //     throw e;
    // } finally {
    //     s0.close();
    // }
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(14);
    (*cols)[0] = String("PKTABLE_CAT");
    (*cols)[1] = String("PKTABLE_SCHEM");
    (*cols)[2] = String("PKTABLE_NAME");
    (*cols)[3] = String("PKCOLUMN_NAME");
    (*cols)[4] = String("FKTABLE_CAT");
    (*cols)[5] = String("FKTABLE_SCHEM");
    (*cols)[6] = String("FKTABLE_NAME");
    (*cols)[7] = String("FKCOLUMN_NAME");
    (*cols)[8] = String("KEY_SEQ");
    (*cols)[9] = String("UPDATE_RULE");
    (*cols)[10] = String("DELETE_RULE");
    (*cols)[11] = String("FK_NAME");
    (*cols)[12] = String("PK_NAME");
    (*cols)[13] = String("DEFERRABILITY");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(14);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::VARCHAR;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::VARCHAR;
    (*types)[7] = ITypes::VARCHAR;
    (*types)[8] = ITypes::SMALLINT;
    (*types)[9] = ITypes::SMALLINT;
    (*types)[10] = ITypes::SMALLINT;
    (*types)[11] = ITypes::VARCHAR;
    (*types)[12] = ITypes::VARCHAR;
    (*types)[13] = ITypes::SMALLINT;
    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<CJDBCResultSet> rs ;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL,(IJDBCResultSet**)&rs));
    if (rs0 != NULL && rs0->tr != NULL && rs0->tr->mNrows > 0) {
        InternalImportedKeys(table, String(NULL), rs0, tr);
    }
    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetIndexInfo(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schema,
    /* [in] */ const String& table,
    /* [in] */ Boolean unique,
    /* [in] */ Boolean approximate,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<CJDBCStatement> s0;
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s0));
    AutoPtr<CJDBCResultSet> rs0;
    // try {
    //     try {
            conn->mDb->Exec(String("SELECT 1 FROM sqlite_master LIMIT 1"), NULL);
    //     } catch (SQLite.Exception se) {
    //     throw new SQLException("schema reload failed");
    //     }
            s0->ExecuteQuery(String("PRAGMA index_list(") +
                         Elastos::Sql::SQLite::CShell::SqlQuote(table) +
                         String(")"),(IResultSet**)&rs0);
    // } catch (SQLException e) {
    //     throw e;
    // } finally {
    //     s0.close();
    // }
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(13);
    (*cols)[0] = String("TABLE_CAT");
    (*cols)[1] = String("TABLE_SCHEM");
    (*cols)[2] = String("TABLE_NAME");
    (*cols)[3] = String("NON_UNIQUE");
    (*cols)[4] = String("INDEX_QUALIFIER");
    (*cols)[5] = String("INDEX_NAME");
    (*cols)[6] = String("TYPE");
    (*cols)[7] = String("ORDINAL_POSITION");
    (*cols)[8] = String("COLUMN_NAME");
    (*cols)[9] = String("ASC_OR_DESC");
    (*cols)[10] = String("CARDINALITY");
    (*cols)[11] = String("PAGES");
    (*cols)[12] = String("FILTER_CONDITION");
    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(13);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::BIT;
    (*types)[4] = ITypes::VARCHAR;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::SMALLINT;
    (*types)[7] = ITypes::SMALLINT;
    (*types)[8] = ITypes::VARCHAR;
    (*types)[9] = ITypes::VARCHAR;
    (*types)[10] = ITypes::INTEGER;
    (*types)[11] = ITypes::INTEGER;
    (*types)[12] = ITypes::VARCHAR;
    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<CJDBCResultSet> rs ;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL,(IJDBCResultSet**)&rs));
    if (rs0 != NULL && rs0->tr != NULL && rs0->tr->mNrows > 0) {
        HashMap<String, Int32> h0 ;
        for (Int32 i = 0; i < rs0->tr->mNcolumns; i++) {
            h0.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[i], i)); // android-changed
        }
        for (Int32 i = 0; i < rs0->tr->mNrows; i++) {
            AutoPtr<ArrayOf<String> > r0 = rs0->tr->mRows[i];
            Int32 col = h0[String("unique")];
            String uniq = (*r0)[col];
            col = h0[String("name")];
            String iname = (*r0)[col];
            if (unique && uniq.GetChar(0) == '0') {
                continue;
            }
            AutoPtr<CJDBCStatement> s1;
            FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s1));
            AutoPtr<CJDBCResultSet> rs1;
            // try {
                s1->ExecuteQuery(String("PRAGMA index_info(") +
                         Elastos::Sql::SQLite::CShell::SqlQuote(iname) +
                         String(")"),(IResultSet**)&rs1);
            // } catch (SQLException e) {
            // } finally {
            //     s1.close();
            // }
            if (rs1 == NULL || rs1->tr == NULL || rs1->tr->mNrows <= 0) {
                continue;
            }
            HashMap<String, Int32> h1;
            for (Int32 k = 0; k < rs1->tr->mNcolumns; k++) {
                h1.Insert(Pair<String,Int32>((*rs1->tr->mColumn)[k], k)); // android-changed
            }
            for (Int32 k = 0; k < rs1->tr->mNrows; k++) {
                AutoPtr<ArrayOf<String> > r1 = rs1->tr->mRows[k];
                AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(cols->GetLength());
                (*row)[0]  = String("");
                (*row)[1]  = String("");
                (*row)[2]  = table;
                (*row)[3]  = (uniq.GetChar(0) != '0' ||
                             (iname.GetChar(0) == '(' &&
                             iname.IndexOf(" autoindex ") > 0)) ? String("0") : String("1");
                (*row)[4]  = String("");
                (*row)[5]  = iname;
                (*row)[6]  = String("") + StringUtils::ToString(IDatabaseMetaData::tableIndexOther);
                col = h1[String("seqno")];
                (*row)[7]  = StringUtils::ToString(StringUtils::ParseInt32((*r1)[col]) + 1);
                col = h1[String("name")];
                (*row)[8]  = (*r1)[col];
                (*row)[9]  = String("A");
                (*row)[10] = String("0");
                (*row)[11] = String("0");
                (*row)[12] = String(NULL);
                Boolean rowflag = FALSE;
                tr->Newrow(row,&rowflag);
            }
        }
    }
    resultSet = (IResultSet**)&rs;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetJDBCMajorVersion(
    /* [out] */ Int32 * ver)
{
    *ver = 1;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetJDBCMinorVersion(
    /* [out] */ Int32 * ver)
{
    *ver = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxBinaryLiteralLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxCatalogNameLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxCharLiteralLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxColumnNameLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxColumnsInGroupBy(
    /* [out] */ Int32 * number)
{
    *number = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxColumnsInIndex(
    /* [out] */ Int32 * number)
{
    *number = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxColumnsInOrderBy(
    /* [out] */ Int32 * number)
{
    *number = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxColumnsInSelect(
    /* [out] */ Int32 * number)
{
    *number = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxColumnsInTable(
    /* [out] */ Int32 * number)
{
    *number = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxConnections(
    /* [out] */ Int32 * connections)
{
    *connections = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxCursorNameLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxIndexLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxProcedureNameLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxRowSize(
    /* [out] */ Int32 * number)
{
    *number = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxSchemaNameLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxStatementLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxStatements(
    /* [out] */ Int32 * number)
{
    *number = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxTableNameLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxTablesInSelect(
    /* [out] */ Int32 * number)
{
    *number = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetMaxUserNameLength(
    /* [out] */ Int32 * len)
{
    *len = 0;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetNumericFunctions(
    /* [out] */ String * len)
{
    *len = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetPrimaryKeys(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schema,
    /* [in] */ const String& table,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<CJDBCStatement> s0;
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s0));
    AutoPtr<CJDBCResultSet> rs0;
    // try {
    //     try {
            conn->mDb->Exec(String("SELECT 1 FROM sqlite_master LIMIT 1"), NULL);
    //     } catch (SQLite.Exception se) {
    //     throw new SQLException("schema reload failed");
    //     }
            s0->ExecuteQuery(String("PRAGMA index_list(") +
                         Elastos::Sql::SQLite::CShell::SqlQuote(table) +
                         String(")"),(IResultSet**)&rs0);
    // } catch (SQLException e) {
    //     throw e;
    // } finally {
    //     s0.close();
    // }
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(6);
    (*cols)[0] = String("TABLE_CAT");
    (*cols)[1] = String("TABLE_SCHEM");
    (*cols)[2] = String("TABLE_NAME");
    (*cols)[3] = String("COLUMN_NAME");
    (*cols)[4] = String("KEY_SEQ");
    (*cols)[5] = String("PK_NAME");
    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(6);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::SMALLINT;
    (*types)[5] = ITypes::VARCHAR;
    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);

    AutoPtr<CJDBCResultSet> rs ;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL,(IJDBCResultSet**)&rs));
    if (rs0 != NULL && rs0->tr != NULL && rs0->tr->mNrows > 0) {
        HashMap<String, Int32> h0;
        for (Int32 i = 0; i < rs0->tr->mNcolumns; i++) {
            h0.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[i], i));
        }
        for (Int32 i = 0; i < rs0->tr->mNrows; i++) {
            AutoPtr<ArrayOf<String> > r0 = rs0->tr->mRows[i];
            Int32 col = h0[String("unique")];
            String uniq = (*r0)[col];
            col = h0[String("name")];
            String iname = (*r0)[col];
            if (uniq.GetChar(0) == '0') {
                continue;
            }
            AutoPtr<CJDBCStatement> s1;
            FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s1));
            AutoPtr<CJDBCResultSet> rs1;
            // try {
                s1->ExecuteQuery(String("PRAGMA index_info(") +
                                 Elastos::Sql::SQLite::CShell::SqlQuote(iname) +
                                 String(")"),(IResultSet**)&rs1);
            // } catch (SQLException e) {
            // } finally {
            //     s2.close();
            // }
            if (rs1 == NULL || rs1->tr == NULL || rs1->tr->mNrows <= 0) {
                continue;
            }
            HashMap<String, Int32> h1 ;
            for (Int32 k = 0; k < rs1->tr->mNcolumns; k++) {
                h1.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[k], k));
            }
            for (Int32 k = 0; k < rs1->tr->mNrows; k++) {
                AutoPtr<ArrayOf<String> > r1 = rs1->tr->mRows[k];
                AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(cols->GetLength());
                (*row)[0]  = String("");
                (*row)[1]  = String("");
                (*row)[2]  = table;
                col = h1[String("name")];
                (*row)[3] = (*r1)[col];
                col = h1[String("seqno")];
                (*row)[4]  = StringUtils::ToString(StringUtils::ParseInt32((*r1)[col]) + 1);
                (*row)[5]  = iname;
                Boolean rowflag = FALSE;
                        tr->Newrow(row,&rowflag);
            }
        }
    }
    if (tr->mNrows > 0) {

        *resultSet = (IResultSet *)rs.Get();
        REFCOUNT_ADD(*resultSet);
        return NOERROR;
    }
    AutoPtr<CJDBCStatement> s1;
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s1));
    // try {
        s1->ExecuteQuery(String("PRAGMA table_info(") +
                                 Elastos::Sql::SQLite::CShell::SqlQuote(table) +
                                 String(")"),(IResultSet**)&rs0);
    // } catch (SQLException e) {
    //     throw e;
    // } finally {
    //     s1.close();
    // }
    if (rs0 != NULL && rs0->tr != NULL && rs0->tr->mNrows > 0) {
        HashMap<String, Int32> h0 ;
        for (Int32 i = 0; i < rs0->tr->mNcolumns; i++) {
            h0.Insert(Pair<String,Int32>((*rs0->tr->mColumn)[i], i));
        }
        for (Int32 i = 0; i < rs0->tr->mNrows; i++) {
            AutoPtr<ArrayOf<String> > r0 = rs0->tr->mRows[i];
            Int32 col = h0[String("type")];
            String type = (*r0)[col];
            if (!type.EqualsIgnoreCase("integer")) {
                continue;
            }
            col = h0[String("pk")];
            String pk = (*r0)[col];
            if (pk.GetChar(0) == '0') {
                continue;
            }
            AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(cols->GetLength());
            (*row)[0]  = String("");
            (*row)[1]  = String("");
            (*row)[2]  = table;
            col = h0[String("name")];
            (*row)[3] = (*r0)[col];
            col = h0[String("cid")];
            (*row)[4] = StringUtils::ToString(StringUtils::ParseInt32((*r0)[col]) + 1);
            (*row)[5] = String("");
            Boolean rowflag = FALSE;
                        tr->Newrow(row,&rowflag);
        }
    }
    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetProcedureColumns(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& procedureNamePattern,
    /* [in] */ const String& columnNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    *resultSet = NULL;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetProcedures(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& procedureNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    *resultSet = NULL;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetProcedureTerm(
    /* [out] */ String * prefer)
{
    *prefer = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetResultSetHoldability(
    /* [out] */ Int32 * code)
{
    *code = IResultSet::HOLD_CURSORS_OVER_COMMIT;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSchemas(
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(1) ;
    (*cols)[0] = String("TABLE_SCHEM");
    AutoPtr<CTableResult> tr;
    FAIL_RETURN(CTableResult::New((ITableResult**)&tr));
    tr->Columns(cols);
    AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(1) ;
    (*row)[0] = String("");
    Boolean flag = FALSE;
    tr->Newrow(row,&flag);
    FAIL_RETURN(CJDBCResultSet::New(tr,NULL,(IJDBCResultSet **)resultSet));
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSchemaTerm(
    /* [out] */ String * term)
{
    *term = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSearchStringEscape(
    /* [out] */ String * escape)
{
    *escape = String("\\");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SetSQLKeywords(
    /* [out] */ String * key)
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSQLStateType(
    /* [out] */ Int32 * type)
{
    *type = IDatabaseMetaData::sqlStateXOpen;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetStringFunctions(
    /* [out] */ String * str)
{
    *str = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSuperTables(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& tableNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    // throw new SQLException("not supported");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSuperTypes(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& typeNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    // throw new SQLException("not supported");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSystemFunctions(
    /* [out] */ String * str)
{
    *str = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetTablePrivileges(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& tableNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(8);
    (*cols)[0] = String("TABLE_CAT");
    (*cols)[1] = String("TABLE_SCHEM");
    (*cols)[2] = String("TABLE_NAME");
    (*cols)[3] = String("COLUMN_NAME");
    (*cols)[4] = String("GRANTOR");
    (*cols)[5] = String("GRANTEE");
    (*cols)[6] = String("PRIVILEGE");
    (*cols)[7] = String("IS_GRANTABLE");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(8);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::VARCHAR;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::VARCHAR;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::VARCHAR;
    (*types)[7] = ITypes::VARCHAR;

    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<IJDBCResultSet> rs;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL ,(IJDBCResultSet**)&rs));
    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetTables(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& tableNamePattern,
    /* [in] */ const ArrayOf<String> & types,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<CJDBCStatement> s;
    FAIL_RETURN(CJDBCStatement::New(conn,(IJDBCStatement**)&s));
    StringBuffer sb;
    sb += "SELECT '' AS 'TABLE_CAT', ";
    sb += "'' AS 'TABLE_SCHEM', ";
    sb += "tbl_name AS 'TABLE_NAME', ";
    sb += "upper(type) AS 'TABLE_TYPE', ";
    sb += "'' AS REMARKS FROM sqlite_master ";
    sb += "WHERE tbl_name like ";
    if (tableNamePattern.IsNull()) {
        sb += SQLite::CShell::SqlQuote(tableNamePattern);
    } else {
        sb += "'%'";
    }
    sb += (" AND ");
    if (types.GetLength() == 0) {
        sb+= ("(type = 'table' or type = 'view')");
    }
    else {
        sb+= ("(");
        String sep = String("");
        for (Int32 i = 0; i < types.GetLength(); i++) {
            sb += (sep);
            sb += ("type = ");
            sb += (SQLite::CShell::SqlQuote(types[i]));
            sep = " or ";
        }
        sb += (")");
    }

    // try {
        // s->ExecuteQuery(sb->ToString(),resultSet);
        // s->Close();
    // } catch (SQLException e) {
    //     throw e;
    // } finally {
    //     s.close();
    // }
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetTableTypes(
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(1) ;
    (*cols)[0] = String("TABLE_TYPE");
    AutoPtr<CTableResult> tr;
    FAIL_RETURN(CTableResult::New((ITableResult**)&tr));
    tr->Columns(cols);
    AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(1) ;
    (*row)[0] = String("TABLE");
    Boolean flag = FALSE;
    tr->Newrow(row,&flag);
    (*row)[0] = String("VIEW");
    tr->Newrow(row,&flag);
    FAIL_RETURN(CJDBCResultSet::New(tr,NULL,(IJDBCResultSet **)resultSet));
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetTimeDateFunctions(
    /* [out] */ String * timedate)
{
    *timedate = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetTypeInfo(
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(18);
    (*cols)[0] = String("TYPE_NAME");
    (*cols)[1] = String("DATA_TYPE");
    (*cols)[2] = String("PRECISION");
    (*cols)[3] = String("LITERAL_PREFIX");
    (*cols)[4] = String("LITERAL_SUFFIX");
    (*cols)[5] = String("CREATE_PARAMS");
    (*cols)[6] = String("NULLABLE");
    (*cols)[7] = String("CASE_SENSITIVE");
    (*cols)[8] = String("SEARCHABLE");
    (*cols)[9] = String("UNSIGNED_ATTRIBUTE");
    (*cols)[10] = String("FIXED_PREC_SCALE");
    (*cols)[11] = String("AUTO_INCREMENT");
    (*cols)[12] = String("LOCAL_TYPE_NAME");
    (*cols)[13] = String("MINIMUM_SCALE");
    (*cols)[14] = String("MAXIMUM_SCALE");
    (*cols)[15] = String("SQL_DATA_TYPE");
    (*cols)[16] = String("SQL_DATETIME_SUB");
    (*cols)[17] = String("NUM_PREC_RADIX");
    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(18);
    (*types)[0] = ITypes::VARCHAR;
    (*types)[1] = ITypes::SMALLINT;
    (*types)[2] = ITypes::INTEGER;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::VARCHAR;
    (*types)[5] = ITypes::VARCHAR;
    (*types)[6] = ITypes::SMALLINT;
    (*types)[7] = ITypes::BIT;
    (*types)[8] = ITypes::SMALLINT;
    (*types)[9] = ITypes::BIT;
    (*types)[10] = ITypes::BIT;
    (*types)[11] = ITypes::BIT;
    (*types)[12] = ITypes::VARCHAR;
    (*types)[13] = ITypes::SMALLINT;
    (*types)[14] = ITypes::SMALLINT;
    (*types)[15] = ITypes::INTEGER;
    (*types)[16] = ITypes::INTEGER;
    (*types)[17] = ITypes::INTEGER;
    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<CJDBCResultSet> rs ;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL,(IJDBCResultSet**)&rs));
    AutoPtr<ArrayOf<String> > row1 = ArrayOf<String>::Alloc(18);
    (*row1)[0] = String("VARCHAR");
    (*row1)[1] = String("") + StringUtils::ToString(ITypes::VARCHAR);
    (*row1)[2] = String("65536");
    (*row1)[3] = String("'");
    (*row1)[4] = String("'");
    (*row1)[5] = String(NULL);
    (*row1)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row1)[7] = String("1");
    (*row1)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row1)[9] = String("0");
    (*row1)[10] = String("0");
    (*row1)[11] = String("0");
    (*row1)[12] = String(NULL);
    (*row1)[13] = String("0");
    (*row1)[14] = String("0");
    (*row1)[15] = String("0");
    (*row1)[16] = String("0");
    (*row1)[17] = String("0");
    Boolean rowflag = FALSE;
    tr->Newrow(row1,&rowflag);
    AutoPtr<ArrayOf<String> > row2 = ArrayOf<String>::Alloc(18);
    (*row2)[0] = String("INTEGER");
    (*row2)[1] = String("") + StringUtils::ToString(ITypes::INTEGER);
    (*row2)[2] = String("32");
    (*row2)[3] = String(NULL);
    (*row2)[4] = String(NULL);
    (*row2)[5] = String(NULL);
    (*row2)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row2)[7] = String("0");
    (*row2)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row2)[9] = String("0");
    (*row2)[10] = String("0");
    (*row2)[11] = String("1");
    (*row2)[12] = String(NULL);
    (*row2)[13] = String("0");
    (*row2)[14] = String("0");
    (*row2)[15] = String("0");
    (*row2)[16] = String("0");
    (*row2)[17] = String("2");
    tr->Newrow(row2,&rowflag);
    AutoPtr<ArrayOf<String> > row3 = ArrayOf<String>::Alloc(18);
    (*row3)[0] = String("DOUBLE");
    (*row3)[1] = String("") + StringUtils::ToString(ITypes::DOUBLE);
    (*row3)[2] = String("16");
    (*row3)[3] = String(NULL);
    (*row3)[4] = String(NULL);
    (*row3)[5] = String(NULL);
    (*row3)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row3)[7] = String("0");
    (*row3)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row3)[9] = String("0");
    (*row3)[10] = String("0");
    (*row3)[11] = String("1");
    (*row3)[12] = String(NULL);
    (*row3)[13] = String("0");
    (*row3)[14] = String("0");
    (*row3)[15] = String("0");
    (*row3)[16] = String("0");
    (*row3)[17] = String("10");
    tr->Newrow(row3,&rowflag);
    AutoPtr<ArrayOf<String> > row4 = ArrayOf<String>::Alloc(18);
    (*row4)[0] = String("FLOAT");
    (*row4)[1] = String("") + StringUtils::ToString(ITypes::FLOAT);
    (*row4)[2] = String("7");
    (*row4)[3] = String(NULL);
    (*row4)[4] = String(NULL);
    (*row4)[5] = String(NULL);
    (*row4)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row4)[7] = String("0");
    (*row4)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row4)[9] = String("0");
    (*row4)[10] = String("0");
    (*row4)[11] = String("1");
    (*row4)[12] = String(NULL);
    (*row4)[13] = String("0");
    (*row4)[14] = String("0");
    (*row4)[15] = String("0");
    (*row4)[16] = String("0");
    (*row4)[17] = String("10");
    tr->Newrow(row4,&rowflag);
    AutoPtr<ArrayOf<String> > row5 = ArrayOf<String>::Alloc(18);
    (*row5)[0] = String("SMALLINT");
    (*row5)[1] = String("") + StringUtils::ToString(ITypes::SMALLINT);
    (*row5)[2] = String("16");
    (*row5)[3] = String(NULL);
    (*row5)[4] = String(NULL);
    (*row5)[5] = String(NULL);
    (*row5)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row5)[7] = String("0");
    (*row5)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row5)[9] = String("0");
    (*row5)[10] = String("0");
    (*row5)[11] = String("1");
    (*row5)[12] = String(NULL);
    (*row5)[13] = String("0");
    (*row5)[14] = String("0");
    (*row5)[15] = String("0");
    (*row5)[16] = String("0");
    (*row5)[17] = String("2");
    tr->Newrow(row5,&rowflag);
    AutoPtr<ArrayOf<String> > row6 = ArrayOf<String>::Alloc(18);
    (*row6)[0] = String("BIT");
    (*row6)[1] = String("") + StringUtils::ToString(ITypes::BIT);
    (*row6)[2] = String("1");
    (*row6)[3] = String(NULL);
    (*row6)[4] = String(NULL);
    (*row6)[5] = String(NULL);
    (*row6)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row6)[7] = String("0");
    (*row6)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row6)[9] = String("0");
    (*row6)[10] = String("0");
    (*row6)[11] = String("1");
    (*row6)[12] = String(NULL);
    (*row6)[13] = String("0");
    (*row6)[14] = String("0");
    (*row6)[15] = String("0");
    (*row6)[16] = String("0");
    (*row6)[17] = String("2");
    tr->Newrow(row6,&rowflag);
    AutoPtr<ArrayOf<String> > row7 = ArrayOf<String>::Alloc(18);
    (*row7)[0] = String("TIMESTAMP");
    (*row7)[1] = String("") + StringUtils::ToString(ITypes::TIMESTAMP);
    (*row7)[2] = String("30");
    (*row7)[3] = String(NULL);
    (*row7)[4] = String(NULL);
    (*row7)[5] = String(NULL);
    (*row7)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row7)[7] = String("0");
    (*row7)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row7)[9] = String("0");
    (*row7)[10] = String("0");
    (*row7)[11] = String("1");
    (*row7)[12] = String(NULL);
    (*row7)[13] = String("0");
    (*row7)[14] = String("0");
    (*row7)[15] = String("0");
    (*row7)[16] = String("0");
    (*row7)[17] = String("0");
    tr->Newrow(row7,&rowflag);
    AutoPtr<ArrayOf<String> > row8 = ArrayOf<String>::Alloc(18);
    (*row8)[0] = String("DATE");
    (*row8)[1] = String("") + StringUtils::ToString(ITypes::DATE);
    (*row8)[2] = String("10");
    (*row8)[3] = String(NULL);
    (*row8)[4] = String(NULL);
    (*row8)[5] = String(NULL);
    (*row8)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row8)[7] = String("0");
    (*row8)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row8)[9] = String("0");
    (*row8)[10] = String("0");
    (*row8)[11] = String("1");
    (*row8)[12] = String(NULL);
    (*row8)[13] = String("0");
    (*row8)[14] = String("0");
    (*row8)[15] = String("0");
    (*row8)[16] = String("0");
    (*row8)[17] = String("0");
    tr->Newrow(row8,&rowflag);
    AutoPtr<ArrayOf<String> > row9 = ArrayOf<String>::Alloc(18);
    (*row9)[0] = String("TIME");
    (*row9)[1] = String("") + StringUtils::ToString(ITypes::TIME);
    (*row9)[2] = String("8");
    (*row9)[3] = String(NULL);
    (*row9)[4] = String(NULL);
    (*row9)[5] = String(NULL);
    (*row9)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row9)[7] = String("0");
    (*row9)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row9)[9] = String("0");
    (*row9)[10] = String("0");
    (*row9)[11] = String("1");
    (*row9)[12] = String(NULL);
    (*row9)[13] = String("0");
    (*row9)[14] = String("0");
    (*row9)[15] = String("0");
    (*row9)[16] = String("0");
    (*row9)[17] = String("0");
    tr->Newrow(row9,&rowflag);
    AutoPtr<ArrayOf<String> > row10 = ArrayOf<String>::Alloc(18);
    (*row10)[0] = String("BINARY");
    (*row10)[1] = String("") + StringUtils::ToString(ITypes::BINARY);
    (*row10)[2] = String("65536");
    (*row10)[3] = String(NULL);
    (*row10)[4] = String(NULL);
    (*row10)[5] = String(NULL);
    (*row10)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row10)[7] = String("0");
    (*row10)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row10)[9] = String("0");
    (*row10)[10] = String("0");
    (*row10)[11] = String("1");
    (*row10)[12] = String(NULL);
    (*row10)[13] = String("0");
    (*row10)[14] = String("0");
    (*row10)[15] = String("0");
    (*row10)[16] = String("0");
    (*row10)[17] = String("0");
    tr->Newrow(row10,&rowflag);
    AutoPtr<ArrayOf<String> > row11 = ArrayOf<String>::Alloc(18);
    (*row11)[0] = String("VARBINARY");
    (*row11)[1] = String("") + StringUtils::ToString(ITypes::VARBINARY);
    (*row11)[2] = String("65536");
    (*row11)[3] = String(NULL);
    (*row11)[4] = String(NULL);
    (*row11)[5] = String(NULL);
    (*row11)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row11)[7] = String("0");
    (*row11)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row11)[9] = String("0");
    (*row11)[10] = String("0");
    (*row11)[11] = String("1");
    (*row11)[12] = String(NULL);
    (*row11)[13] = String("0");
    (*row11)[14] = String("0");
    (*row11)[15] = String("0");
    (*row11)[16] = String("0");
    (*row11)[17] = String("0");
    tr->Newrow(row11,&rowflag);
    AutoPtr<ArrayOf<String> > row12 = ArrayOf<String>::Alloc(18);
    (*row12)[0] = String("REAL");
    (*row12)[1] = String("") + StringUtils::ToString(ITypes::REAL);
    (*row12)[2] = String("16");
    (*row12)[3] = String(NULL);
    (*row12)[4] = String(NULL);
    (*row12)[5] = String(NULL);
    (*row12)[6] = String("") + StringUtils::ToString(IDatabaseMetaData::typeNullable);
    (*row12)[7] = String("0");
    (*row12)[8] = String("") + StringUtils::ToString(IDatabaseMetaData::typeSearchable);
    (*row12)[9] = String("0");
    (*row12)[10] = String("0");
    (*row12)[11] = String("1");
    (*row12)[12] = String(NULL);
    (*row12)[13] = String("0");
    (*row12)[14] = String("0");
    (*row12)[15] = String("0");
    (*row12)[16] = String("0");
    (*row12)[17] = String("10");
    tr->Newrow(row12,&rowflag);

    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetUDTs(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& typeNamePattern,
    /* [in] */ const ArrayOf<Int32> & types,
    /* [out] */ IResultSet ** resultSet)
{
    *resultSet = NULL;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetURL(
    /* [out] */ String * url)
{
    *url = conn->mUrl;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetUserName(
    /* [out] */ String * name)
{
    *name = String("");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetVersionColumns(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schema,
    /* [in] */ const String& table,
    /* [out] */ IResultSet ** resultSet)
{
    AutoPtr<ArrayOf<String> > cols = ArrayOf<String>::Alloc(8);
    (*cols)[0] = String("SCOPE");
    (*cols)[1] = String("COLUMN_NAME");
    (*cols)[2] = String("DATA_TYPE");
    (*cols)[3] = String("TYPE_NAME");
    (*cols)[4] = String("COLUMN_SIZE");
    (*cols)[5] = String("BUFFER_LENGTH");
    (*cols)[6] = String("DECIMAL_DIGITS");
    (*cols)[7] = String("PSEUDO_COLUMN");

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(8);
    (*types)[0] = ITypes::SMALLINT;
    (*types)[1] = ITypes::VARCHAR;
    (*types)[2] = ITypes::SMALLINT;
    (*types)[3] = ITypes::VARCHAR;
    (*types)[4] = ITypes::INTEGER;
    (*types)[5] = ITypes::INTEGER;
    (*types)[6] = ITypes::SMALLINT;
    (*types)[7] = ITypes::SMALLINT;

    AutoPtr<CTableResultX> tr;
    FAIL_RETURN(CTableResultX::New((ITableResultX**)&tr));
    tr->Columns(cols);
    tr->Sql_types(types);
    AutoPtr<IJDBCResultSet> rs;
    FAIL_RETURN(CJDBCResultSet::New((ITableResult *)tr, NULL ,(IJDBCResultSet**)&rs));
    *resultSet = (IResultSet *)rs.Get();
    REFCOUNT_ADD(*resultSet);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::InsertsAreDetected(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * detected)
{
    *detected = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::IsCatalogAtStart(
    /* [out] */ Boolean * isCatalog)
{
    *isCatalog = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::IsReadOnly(
    /* [out] */ Boolean * isReadOnly)
{
    *isReadOnly = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::LocatorsUpdateCopy(
    /* [out] */ Boolean * isCopy)
{
    //  throw new SQLException("not supported");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::NullPlusNonNullIsNull(
    /* [out] */ Boolean * isNull)
{
    *isNull = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::NullsAreSortedAtEnd(
    /* [out] */ Boolean * isNull)
{
    isNull = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::NullsAreSortedAtStart(
    /* [out] */ Boolean * isNull)
{
    isNull = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::NullsAreSortedHigh(
    /* [out] */ Boolean * isNull)
{
    isNull = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::NullsAreSortedLow(
    /* [out] */ Boolean * isNull)
{
    isNull = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::OthersDeletesAreVisible(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isVisible)
{
    *isVisible = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::OthersInsertsAreVisible(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isVisible)
{
    *isVisible = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::OthersUpdatesAreVisible(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isVisible)
{
    *isVisible = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::OwnDeletesAreVisible(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isVisible)
{
    if (type == IResultSet::TYPE_FORWARD_ONLY ||
        type == IResultSet::TYPE_SCROLL_INSENSITIVE ||
        type == IResultSet::TYPE_SCROLL_SENSITIVE) {
        *isVisible = TRUE;
    }
    *isVisible = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::OwnInsertsAreVisible(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isVisible)
{
    if (type == IResultSet::TYPE_FORWARD_ONLY ||
        type == IResultSet::TYPE_SCROLL_INSENSITIVE ||
        type == IResultSet::TYPE_SCROLL_SENSITIVE) {
        *isVisible = TRUE;
    }
    *isVisible = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::OwnUpdatesAreVisible(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isVisible)
{
    if (type == IResultSet::TYPE_FORWARD_ONLY ||
        type == IResultSet::TYPE_SCROLL_INSENSITIVE ||
        type == IResultSet::TYPE_SCROLL_SENSITIVE) {
        *isVisible = TRUE;
    }
    *isVisible = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::StoresLowerCaseIdentifiers(
    /* [out] */ Boolean * isStored)
{
    *isStored = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::StoresLowerCaseQuotedIdentifiers(
    /* [out] */ Boolean * isStored)
{
    *isStored = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::StoresMixedCaseIdentifiers(
    /* [out] */ Boolean * isStored)
{
    *isStored = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::StoresMixedCaseQuotedIdentifiers(
    /* [out] */ Boolean * isStored)
{
    *isStored = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::StoresUpperCaseIdentifiers(
    /* [out] */ Boolean * isStored)
{
    *isStored = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::StoresUpperCaseQuotedIdentifiers(
    /* [out] */ Boolean * isStored)
{
    *isStored = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsAlterTableWithAddColumn(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsAlterTableWithDropColumn(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsANSI92EntryLevelSQL(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsANSI92FullSQL(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsANSI92IntermediateSQL(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsBatchUpdates(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsCatalogsInDataManipulation(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsCatalogsInIndexDefinitions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsCatalogsInPrivilegeDefinitions(
    /* [out] */ Boolean * isSupported)
{
   *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsCatalogsInProcedureCalls(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsCatalogsInTableDefinitions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsColumnAliasing(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsConvert(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsConvert(
    /* [in] */ Int32 fromType,
    /* [in] */ Int32 toType,
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsCoreSQLGrammar(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsCorrelatedSubqueries(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsDataDefinitionAndDataManipulationTransactions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsDataManipulationTransactionsOnly(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsDifferentTableCorrelationNames(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsExpressionsInOrderBy(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsExtendedSQLGrammar(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsFullOuterJoins(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsGetGeneratedKeys(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsGroupBy(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsGroupByBeyondSelect(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsGroupByUnrelated(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsIntegrityEnhancementFacility(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsLikeEscapeClause(
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsLimitedOuterJoins(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsMinimumSQLGrammar(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsMixedCaseIdentifiers(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsMixedCaseQuotedIdentifiers(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsMultipleOpenResults(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsMultipleResultSets(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsMultipleTransactions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsNamedParameters(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsNonNullableColumns(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsOpenCursorsAcrossCommit(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsOpenCursorsAcrossRollback(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsOpenStatementsAcrossCommit(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsOpenStatementsAcrossRollback(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsOrderByUnrelated(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsOuterJoins(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsPositionedDelete(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsPositionedUpdate(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsResultSetConcurrency(
    /* [in] */ Int32 type,
    /* [in] */ Int32 concurrency,
    /* [out] */ Boolean * isSupported)
{
    if (type == IResultSet::TYPE_FORWARD_ONLY ||
        type == IResultSet::TYPE_SCROLL_INSENSITIVE ||
        type == IResultSet::TYPE_SCROLL_SENSITIVE) {

        *isSupported = concurrency == IResultSet::CONCUR_READ_ONLY ||
                       concurrency == IResultSet::CONCUR_UPDATABLE;
    }
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsResultSetHoldability(
    /* [in] */ Int32 holdability,
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsResultSetType(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isSupported)
{
    *isSupported = type == IResultSet::TYPE_FORWARD_ONLY ||
                   type == IResultSet::TYPE_SCROLL_INSENSITIVE ||
                   type == IResultSet::TYPE_SCROLL_SENSITIVE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSavepoints(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSchemasInDataManipulation(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSchemasInIndexDefinitions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSchemasInPrivilegeDefinitions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSchemasInProcedureCalls(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSchemasInTableDefinitions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSelectForUpdate(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsStatementPooling(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsStoredProcedures(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSubqueriesInComparisons(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSubqueriesInExists(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSubqueriesInIns(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsSubqueriesInQuantifieds(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsTableCorrelationNames(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsTransactionIsolationLevel(
    /* [in] */ Int32 level,
    /* [out] */ Boolean * isSupported)
{
    *isSupported = (level == IConnection::TRANSACTION_SERIALIZABLE);
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsTransactions(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsUnion(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsUnionAll(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::UpdatesAreDetected(
    /* [in] */ Int32 type,
    /* [out] */ Boolean * isUpdate)
{
    *isUpdate = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::UsesLocalFilePerTable(
    /* [out] */ Boolean * isUsed)
{
    isUsed = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::UsesLocalFiles(
    /* [out] */ Boolean * isStored)
{
    *isStored = TRUE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::AutoCommitFailureClosesAllResultSets(
    /* [out] */ Boolean * isClosed)
{
    *isClosed = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetClientInfoProperties(
    /* [out] */ IResultSet ** resultSet)
{
    // throw new SQLException("unsupported");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetFunctionColumns(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& functionNamePattern,
    /* [in] */ const String& columnNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    // throw new SQLException("unsupported");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetFunctions(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [in] */ const String& functionNamePattern,
    /* [out] */ IResultSet ** resultSet)
{
    // throw new SQLException("unsupported");
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetRowIdLifetime(
    /* [out] */ RowIdLifeTime * rowIdLifeTime)
{
    *rowIdLifeTime = Elastos::Sql::RowIdLifeTime_UNSUPPORTED;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SetSchemas(
    /* [in] */ const String& catalog,
    /* [in] */ const String& schemaPattern,
    /* [out] */ IResultSet ** resultSet)
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::SupportsStoredFunctionsUsingCallSyntax(
    /* [out] */ Boolean * isSupported)
{
    *isSupported = FALSE;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::constructor(
    /* [in] */ IJDBCConnection * pConn)
{
    conn = (CJDBCConnection*)pConn;
    return NOERROR;
}

ECode CJDBCDatabaseMetaData::GetSQLKeywords(
    /* [out] */ String* str)
{
    *str = String("SELECT,UPDATE,CREATE,TABLE,VIEW,DELETE,FROM,WHERE") +
           String(",COMMIT,ROLLBACK,TRIGGER");
    return NOERROR;
}


ECode CJDBCDatabaseMetaData::IsWrapperFor(
    /* [in] */ IInterface * iface,
    /* [out] */ Boolean * value)
{
    return NOERROR;
}

String CJDBCDatabaseMetaData::MapTypeName(Int32 type)
{
    switch (type) {
        case ITypes::INTEGER:     return String("integer");
        case ITypes::SMALLINT:    return String("smallint");
        case ITypes::FLOAT:       return String("float");
        case ITypes::DOUBLE:      return String("double");
        case ITypes::TIMESTAMP:   return String("timestamp");
        case ITypes::DATE:        return String("date");
        case ITypes::TIME:        return String("time");
        case ITypes::BINARY:      return String("binary");
        case ITypes::VARBINARY:   return String("varbinary");
        case ITypes::REAL:        return String("real");
    }
    return String("varchar");
}

Int32 CJDBCDatabaseMetaData::MapSqlType(
    const String& inType)
{
    if (inType.IsNull()) {
        return ITypes::VARCHAR;
    }
    String type = inType.ToLowerCase();
    if (type.StartWith("inter")) {
        return ITypes::VARCHAR;
    }
    if (type.StartWith("numeric") ||
        type.StartWith("int")) {
        return ITypes::INTEGER;
    }
    if (type.StartWith("tinyint") ||
        type.StartWith("smallint")) {
        return ITypes::SMALLINT;
    }
    if (type.StartWith("float")) {
        return ITypes::FLOAT;
    }
    if (type.StartWith("double")) {
        return ITypes::DOUBLE;
    }
    if (type.StartWith("datetime") ||
        type.StartWith("timestamp")) {
        return ITypes::TIMESTAMP;
    }
    if (type.StartWith("date")) {
        return ITypes::DATE;
    }
    if (type.StartWith("time")) {
        return ITypes::TIME;
    }
    if (type.StartWith("blob")) {
        return ITypes::BINARY;
    }
    if (type.StartWith("binary")) {
        return ITypes::BINARY;
    }
    if (type.StartWith("varbinary")) {
        return ITypes::VARBINARY;
    }
    if (type.StartWith("real")) {
        return ITypes::REAL;
    }
    return ITypes::VARCHAR;
}

Int32 CJDBCDatabaseMetaData::GetM(
    const String& inTypeStr,
    Int32 type)
{
    Int32 m = 65536;
    switch (type) {
        case ITypes::INTEGER:       m = 11; break;
        case ITypes::SMALLINT:      m = 6;  break;
        case ITypes::FLOAT:         m = 25; break;
        case ITypes::REAL:
        case ITypes::DOUBLE:        m = 54; break;
        case ITypes::TIMESTAMP:     return 30;
        case ITypes::DATE:          return 10;
        case ITypes::TIME:          return 8;
    }
    String typeStr = inTypeStr.ToLowerCase();
    Int32 i1 = typeStr.IndexOf('(');
    if (i1 > 0) {
        ++i1;
        Int32 i2 = typeStr.IndexOf(',', i1);
        if (i2 < 0) {
        i2 = typeStr.IndexOf(')', i1);
        }
        if (i2 - i1 > 0) {
            String num = typeStr.Substring(i1, i2);
            m = StringUtils::ParseInt32(num);
        }
    }
    return m;
}

Int32 CJDBCDatabaseMetaData::GetD(
    const String& typeStr,
    Int32 type)
{
    Int32 d = 0;
    switch (type) {
        case ITypes::INTEGER:       d = 10; break;
        case ITypes::SMALLINT:      d = 5;  break;
        case ITypes::FLOAT:         d = 24; break;
        case ITypes::REAL:
        case ITypes::DOUBLE:        d = 53; break;
        default:                    return GetM(typeStr, type);
    }
    String lcTypeStr = typeStr.ToLowerCase();
    Int32 i1 = lcTypeStr.IndexOf('(');
    if (i1 > 0) {
        ++i1;
        Int32 i2 = lcTypeStr.IndexOf(',', i1);
        if (i2 < 0) {
        return GetM(lcTypeStr, type);
        }
        i1 = i2;
        i2 = lcTypeStr.IndexOf(')', i1);
        if (i2 - i1 > 0) {
            String num = lcTypeStr.Substring(i1, i2);
            d = StringUtils::ParseInt32(num);
        }
    }
    return d;
}

void CJDBCDatabaseMetaData::InternalImportedKeys(
    const String& table,
    const String& pktable,
    CJDBCResultSet * in,
    CTableResultX * out)
{
    HashMap<String, Int32> h0 ;
    for (Int32 i = 0; i < in->tr->mNcolumns; i++) {
        h0.Insert(Pair<String,Int32>((*in->tr->mColumn)[i], i)); // android-changed
    }
    for (Int32 i = 0; i < in->tr->mNrows; i++) {
        AutoPtr<ArrayOf<String> > r0 = in->tr->mRows[i];
        Int32 col = h0[String("table")];
        String pktab = (*r0)[col];
        if (pktable != NULL && !pktable.EqualsIgnoreCase(pktab)) {
            continue;
        }
        col = h0[String("from")];
        String fkcol = (*r0)[col];
        col = h0[String("to")];
        String pkcol = (*r0)[col];
        col = h0[String("seq")];
        String seq = (*r0)[col];
        AutoPtr<ArrayOf<String> > row = ArrayOf<String>::Alloc(14);
        (*row)[0]  = String("");
        (*row)[1]  = String("");
        (*row)[2]  = pktab;
        (*row)[3]  = pkcol;
        (*row)[4]  = String("");
        (*row)[5]  = String("");
        (*row)[6]  = table;
        (*row)[7]  = fkcol == NULL ? pkcol : fkcol;
        (*row)[8]  = StringUtils::ToString(StringUtils::ParseInt32(seq) + 1);
        (*row)[9]  = String("") + StringUtils::ToString(IDatabaseMetaData::importedKeyNoAction);
        (*row)[10] = String("") + StringUtils::ToString(IDatabaseMetaData::importedKeyNoAction);
        (*row)[11] = String(NULL);
        (*row)[12] = String(NULL);
        (*row)[13] = String("") + StringUtils::ToString(IDatabaseMetaData::importedKeyNotDeferrable);
        Boolean rowflag =FALSE;
        out->Newrow(row, &rowflag);
    }
}

AutoPtr<IResultSet> CJDBCDatabaseMetaData::GetSchemas(
    const String& cat,
    const String& schema)
{
    assert(0);
    //throw new SQLException("not supported");
    return NULL;
}

Boolean CJDBCDatabaseMetaData::IsWrapperFor(IInterface* iface)
{
    return FALSE;
}

ECode CJDBCDatabaseMetaData::Unwrap(
    /* [in] */ PInterface iface,
    /* [out] */ IInterface ** oface)
{
    return E_SQL_EXCEPTION;
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos
