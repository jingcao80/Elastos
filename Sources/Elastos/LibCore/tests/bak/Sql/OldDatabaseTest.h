/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <elastos.h>
#include <cmdef.h>
#include <elautoptr.h>
#include <eltypes.h>
#include <Elastos.Sql.h>
#include <Elastos.CoreLibrary.h>
#include <stdio.h>
#include <elastos/Mutex.h>
#include <elastos/Math.h>
#include <unistd.h>

using namespace Elastos;
using namespace Elastos::Sql::SQLite;
using namespace Elastos::Sql::SQLite::JDBC;
using namespace Elastos::Sql;
using namespace Elastos::IO;
using namespace Elastos::Core;

using Elastos::Sql::SQLite::EIID_ICallback;

class OldSQLiteTest {

public:
    static AutoPtr<IConnection> conn;

    static AutoPtr<IFile> dbFile ;

    static AutoPtr<IFileHelper> dbFileHelper;

    static AutoPtr<IDriverManager> driverManager;

    static String filepath;

    void setUp() {
        String tmp("/data/data/com.elastos.runtime/elastos");
        AutoPtr<IFile> tmpDir;
        CFile::New(tmp,(IFile**)&(tmpDir));
        CFileHelper::AcquireSingleton((IFileHelper**)&dbFileHelper);

        CDriverManager::AcquireSingleton((IDriverManager**)&driverManager);
        Boolean isflag = FALSE;
        tmpDir->IsDirectory(&isflag);
        if (isflag) {
            // dbFileHelper->CreateTempFileEx(String("sqliteTest"), String(".db"), tmpDir,(IFile**)&dbFile);
            // dbFile.deleteOnExit();
        } else {
            printf("ctsdir does not exist \n");
        }
        isflag = TRUE;
        // dbFile->Exists(&isflag);
        if (!isflag) {
            printf("DB file could not be created. Tests can not be executed. \n");
        } else {
            // dbFile->GetPath(&filepath);
            filepath = tmp + String("/sqliteTest.db");
            printf("filepath==>%s\n", filepath.string());
            driverManager->GetConnection(String("jdbc:sqlite:/") + filepath,(IConnection**)&conn);
        }
    }

    void tearDown() {
        Boolean icoonflag(FALSE);
        conn->IsClosed(&icoonflag);
        if (!icoonflag) {
            conn->Close();
        }
        // super.tearDown();
    }
};

class OldDatabaseTest :  public OldSQLiteTest
{
public:
    AutoPtr<IStatement> statement;

    AutoPtr<IDatabase> db;

    AutoPtr<IDatabaseHelper> dbhelper;

    const static Int32 numThreads = 10;

    const static Int32 numOfRecords = 30;

    const static String CREATE_TABLE_SIMPLE1;

    ECode setUp() {
        OldSQLiteTest::setUp();
        // VALIDATE_NOT_NULL(conn);

        // conn->CreateStatement((IStatement**)&statement);
        // VALIDATE_NOT_NULL(statement);
        // // Cleanup tables if necessary
        // AutoPtr<IDatabaseMetaData> meta;
        // conn->GetMetaData((IDatabaseMetaData**)&meta);
        // VALIDATE_NOT_NULL(meta);
        // AutoPtr<IResultSet> userTab;
        // AutoPtr<ArrayOf<String> > types = NULL;
        // meta->GetTables(String(NULL), String(NULL), String(NULL), *types,(IResultSet**)&userTab);
        // Boolean tabflag(FALSE);
        // userTab->Next(&tabflag);
        // while (tabflag) {
        //     String tableName;
        //     userTab->GetStringEx(String("TABLE_NAME"),&tableName);
        //     statement->Execute(String("drop table ") + tableName,&tabflag);
        //     userTab->Next(&tabflag);
        // }
        // // Create default test table
        // statement->Execute(CREATE_TABLE_SIMPLE1,&tabflag);
        // statement->Close();

        ECode ec = CDatabase::New((IDatabase** )&db);
        if (!db) {
            PRINT_FILE_LINE_EX("dbhelper: %p, ec: 0x%08x", db.Get(), ec);
        }
        db->Open(filepath, 0);
        // db->Exec(CREATE_TABLE_SIMPLE1,NULL);
        db->BusyHandler(NULL);
        ec = CDatabaseHelper::AcquireSingleton((IDatabaseHelper**)&dbhelper);
        if (!dbhelper) {
            PRINT_FILE_LINE_EX("dbhelper: %p, ec: 0x%08x", dbhelper.Get(), ec);
        }
    }

    void tearDown() {
        if (db) {
            db->Close();
        } else {
            printf("tearDown===> db already closed!\n");
        }
        // tracker.reset();
        // OldSQLiteTest::tearDown();
    }

    void testDatabase() {
        // db closed
        AutoPtr<IDatabase> db2;
        CDatabase::New((IDatabase**)&db2);
        if (db)
        {
            db->Close();
            printf("db close\n");
        }

        db2->Open(filepath, 0);
        db2->Close();
        db->Open(filepath, 0);
        //db is open
        db2->Open(filepath, 0);

        db2->Close();
    }

    void testOpen() {
        db->Close();
        db->Open(filepath, 0);
        // open second db while db1 still open
        AutoPtr<IDatabase> db2;
        CDatabase::New((IDatabase**)&db2);
        db2->Open(filepath, 0);
        db2->Open(filepath, 0);
        db2->Close();
        // open non db file

            // URL file = OldDatabaseTest.class.getResource("/blob.c");
        ECode ec = db2->Open(String("blob.c"), 0);
        if (ec != NOERROR)
        {
            printf("Should not be able to open non db file \n");
        }

    }

    void testOpenAuxFile() {
        AutoPtr<IFile> temp;

        ECode ec = db->OpenAuxFile(String(""));
        if (ec != NOERROR)
        {
            printf("open should fail \n");
        }
        AutoPtr<IFileHelper> temphelper;
        CFileHelper::AcquireSingleton((IFileHelper**)&temphelper);
        // if (temphelper)
        // {
        //     ECode ec = temphelper->CreateTempFileEx(String("openAuxMethod"), String(".db"),String(NULL),(IFile**)&temp);
        //     printf("ec: %d\n", ec);
        // } else {
        //     printf("temphelper failed\n");
        // }

        db->OpenAuxFile(String(""));
        db->Exec(String("create table AUX_TABLE"), NULL);
        db->Close();
        // catch (Exception e) {
        //     temp.delete();
        //     fail("Error handling temporary file "+e.getMessage());
        //     e.printStackTrace();
        // } catch (IOException e) {
        //     temp.delete();
        //     fail("Could not create temporary File");
        //     e.printStackTrace();
        // }

        db->Open(filepath,0);
        ec = db->Exec(String("select * from AUX_TABLE"), NULL);
        if (ec != NOERROR)
        {
            printf("Statement should fail\n");
        }
        // catch (Exception e) {
        //     // TODO Auto-generated catch block
        //     e.printStackTrace();
        // }
        Boolean isdelflag(FALSE);
        // temp->Delete(&isdelflag);
    }

    void testClose() {

        db->Close();
        AutoPtr<ITableResult> outtable;
        db->GetTableEx(String("test"),(ITableResult**)&outtable);
            // fail();
        // catch (Exception e) {
        //     assertTrue(e.getMessage().equals("database already closed"));
        //     try {
        //         db.open(filepath, 0);
        //     } catch (Exception e1) {
        //         fail("Database object could not be reopened after 'close': "
        //                 + e.getMessage());
        //         e1.printStackTrace();
        //     }
        // }


        db->Close();
        db->Close();
        // fail();
        // catch (Exception e) {
        //     assertTrue(e.getMessage().equals("database already closed"));
        //     db.open(filepath, 0);
        // }
    }

    void testExecStringCallback() {
        AutoPtr<ITableResult> res;
        CTableResult::New((ITableResult**)&res);

        db->Exec(String("insert into simple_table1 VALUES(1, 10, 20)"), NULL);
        db->Exec(String("select * from simple_table1 "),(ICallback *)res.Get());
        db->Exec(String("delete from simple_table1 where 1"), NULL);
        String outstr;
        res->ToString(&outstr);
        printf("testExecStringCallback-===>%s\n", outstr.string());
        // assertEquals(Integer.parseInt(row[0]), 1);
        // assertEquals(Integer.parseInt(row[1]), 10);
        // assertEquals(Integer.parseInt(row[2]), 20);
    }

    void testExecStringCallbackStringArray() {
        AutoPtr<ITableResult> res;
        CTableResult::New((ITableResult**)&res);
        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = String("table");
        db->ExecEx(String("select name from sqlite_master where type = '%q';"), (ICallback *)res.Get(), *args);
        String outstr;
        res->ToString(&outstr);
        printf("testExecStringCallbackStringArray===>%s\n", outstr.string());
        // String[] s = (String[]) res.rows.elementAt(0);
        // assertEquals(s[0], DatabaseCreator.SIMPLE_TABLE1);

        ECode ec = db->ExecEx(String("select name from sqlite_master where type = "), (ICallback *)res.Get(), *args);
        if (ec != NOERROR)
        {
            printf("Testmethod should fail\n");
        }

    }

    void testLast_insert_rowid() {
        Int64 rowid(0);
        db->LastInsertRowid(&rowid);
        printf("rowid 1==>%lld\n", rowid);

        db->Exec(String("create table TEST5(id integer, firstname text, lastname text);"), NULL);
        db->Exec(String("insert into TEST5 values (1,'James','Bond');"), NULL);
        db->Exec(String("insert into TEST5 values (2,'Fiona','Apple');"), NULL);
        db->LastInsertRowid(&rowid);
        printf("rowid 2 ?=%lld\n", rowid);

        db->Exec(String("drop table TEST5;"), NULL);
        db->LastInsertRowid(&rowid);
        printf("rowid 2 ?=%lld\n", rowid);
    }

    // /**
    //  * Reason for failure unknown: Database should be locked. Specification
    //  * of interrupt is scarce.
    //  */
    // void testInterrupt() {
    //     ThreadPool threadPool = new ThreadPool(numThreads);

    //     // initialization
    //     ResultSet userTabs;
    //     userTabs = conn.getMetaData().getTables(null, NULL, NULL, NULL);
    //     while (userTabs.next()) {
    //         String tableName = userTabs.getString("TABLE_NAME");
    //         if (tableName.equals(DatabaseCreator.TEST_TABLE1)) {
    //             statement.execute(DatabaseCreator.DROP_TABLE1);
    //         }
    //     }
    //     db->Exec(DatabaseCreator.CREATE_TABLE3, NULL);
    //     db->Exec(DatabaseCreator.CREATE_TABLE1, NULL);

    //     int id1 = numOfRecords - 3;
    //     threadPool.runTask(createTask1(id1, filepath, tracker));
    //     // should not be able to do any other insertions since task 1 holds lock
    //     int id2 = numOfRecords + 3;
    //     threadPool
    //             .runTask(createTask2Interrupt(id2, filepath, tracker));

    //     threadPool.join();

    //     List<String> errors = tracker.getErrors();
    //     System.out.println("Last error: " + db.error_message());
    //     if (errors.size() > 0) {
    //         assertEquals(errors.get(0), db.error_string(Constants.SQLITE_LOCKED));
    //         for (String s : errors) {
    //             Logger.global.info("INTERRUPT Error: " + s);
    //         }

    //     } else {
    //         fail("Should have one exception: database should be locked.");
    //     }

    //     // reset
    //     db->Exec("delete from " + DatabaseCreator.TEST_TABLE1 + " where 1", NULL);
    //     db->Exec("delete from " + DatabaseCreator.TEST_TABLE3 + " where 1", NULL);
    // }

    /**
     * Returns wrong number for updates: returns value > 1 for select.
     */
    void testChanges() {
        AutoPtr<ITableResult> res;
        CTableResult::New((ITableResult**)&res);
        Int64 value(0);
        db->Changes(&value);
        printf("value 0 ==> 0 ?= %lld \n", value);

        db->Exec(String("INSERT INTO SIMPLE_TABLE1 VALUES(2, 5, 7);"), NULL);
        db->Changes(&value);
        printf("value 1 ==> 1 ?= %lld \n", value);

        db->Exec(String("update SIMPLE_TABLE1 set speed = 7, size= 5 where id = 2;"), NULL);
        db->Changes(&value);
        printf("value 2 ==> 1 ?= %lld \n", value);

        db->Exec(String("select * from SIMPLE_TABLE1;"), (ICallback *)res.Get());
        db->Changes(&value);
        printf("value 3 ==> 0 ?= %lld \n", value);

        db->Exec(String("INSERT INTO SIMPLE_TABLE1 VALUES(8, 5, 7);"), NULL);
        db->Exec(String("Update SIMPLE_TABLE1 set speed = 10;"),NULL);
        db->Changes(&value);
        printf("value 4 ==> >2 ?%lld \n", value);
    }

//     /**
//      * method test fails once in a while. Cannot be sure that exception is
//      * thrown in every test execution.
//      */
//     void testBusy_handler() throws SQLException, Exception {
//         TestBusyHandler bh = new TestBusyHandler();
//         db.busy_handler(bh);
//         int counter = 0;
//         ThreadPool threadPool = new ThreadPool(numThreads);

//         // initialization
//         ResultSet userTabs;
//         userTabs = conn.getMetaData().getTables(null, NULL, NULL, NULL);
//         while (userTabs.next()) {
//             String tableName = userTabs.getString("TABLE_NAME");
//             if (tableName.equals(DatabaseCreator.TEST_TABLE1)) {
//                 statement.execute(DatabaseCreator.DROP_TABLE1);
//             }
//         }
//         db->Exec(DatabaseCreator.CREATE_TABLE3, NULL);
//         db->Exec(DatabaseCreator.CREATE_TABLE1, NULL);


//         try {
//             conn.setAutoCommit(false);
//             int id1 = numOfRecords - 3;
//             threadPool.runTask(createTask1(id1, filepath, tracker));
//             int id2 = numOfRecords + 3;
//             threadPool.runTask(createTask2(id2, filepath, tracker));
//             int oldID = 5;
//             int newID = 100;
//             threadPool.runTask(createTask3(oldID, filepath, newID,
//                     tracker));

//             threadPool.join();

//             List<String> errors = tracker.getErrors();
//             if (errors.size() > 0) {
// //                 assertEquals(errors.get(0),
// //                 db.error_string(Constants.SQLITE_LOCKED));
//                 for (String s: errors) {
//                   System.out.println("Round 2 Error: "+s);
//               }
//             } else {
//                 fail("No error happened");
//             }

//             // reset


//             db->Exec("delete from " + DatabaseCreator.TEST_TABLE1 + " where 1",
//                     null);
//             db->Exec("delete from " + DatabaseCreator.TEST_TABLE3 + " where 1",
//                             null);
// //
// //            // increase timeout for retry
// //            db.busy_timeout(1000);
// //            db.busy_handler(bh);
// //            tracker.reset();

// //            threadPool = new ThreadPool(numThreads);
// //
// //            threadPool.runTask(createTask1(id1, filepath, tracker));
// //            threadPool.runTask(createTask2(id2, filepath, tracker));
// //
// //            threadPool.join();
// //
// //            errors = tracker.getErrors();
// //            if (errors.size() > 0) {
// //                // assertEquals(errors.get(0),
// //                // db.error_string(Constants.SQLITE_LOCKED));
// //                for (String s: errors) {
// //                    System.out.println("Round 2 Error"+s);
// //                }
// //            } else {
// //                // ok
// //                System.out.println("BUSY: No Error!");
// //            }
// //
// //
//         } finally {
//             conn.setAutoCommit(true);
//             db->Exec(DatabaseCreator.DROP_TABLE1, NULL);
//             db->Exec(DatabaseCreator.DROP_TABLE3, NULL);
//         }
//     }

//     /**
//      * test fails. Cannot be sure that exception is thrown every time.
//      * Database does not lock values.
//      */
//     void testBusy_timeout() throws Exception, SQLException {
//         int counter = 0;
//         ThreadPool threadPool = new ThreadPool(numThreads);

//         // initialization
//         ResultSet userTabs = conn.getMetaData().getTables(null, NULL, NULL, NULL);
//         while (userTabs.next()) {
//             String tableName = userTabs.getString("TABLE_NAME");
//             if (tableName.equals(DatabaseCreator.TEST_TABLE1)) {
//                 statement.execute(DatabaseCreator.DROP_TABLE1);
//             }
//         }
//         db->Exec(DatabaseCreator.CREATE_TABLE3, NULL);
//         db->Exec(DatabaseCreator.CREATE_TABLE1, NULL);

//         // test run
//         try {
//             conn.setAutoCommit(false);

// //            DatabaseCreator.fillTestTable1(conn, numOfRecords);
//             // set to fail immediately if table is locked.
//             db.busy_handler(null);
//             db.busy_timeout(0);
//             int id1 = numOfRecords - 3;

//             threadPool.runTask(createTask2(id1, filepath, tracker));
//             int id2 = numOfRecords + 3;
//             threadPool.runTask(createTask1(id2, filepath, tracker));
//             int oldID = 5;
//             int newID = 100;
//             threadPool.runTask(createTask3(oldID, filepath, newID,
//                     tracker));

//             threadPool.join();

//             List<String> errors = tracker.getErrors();
//             assertTrue("No error occurred on DB but should have",errors.size() > 0);

//             assertEquals(errors.get(0),
//             db.error_string(Constants.SQLITE_LOCKED));
//             assertEquals(errors.get(0), "database is locked");

//             // reset

//             db->Exec("delete from " + DatabaseCreator.TEST_TABLE1 + " where 1",
//                     null);
//             db->Exec("delete from " + DatabaseCreator.TEST_TABLE3 + " where 1",
//                             null);

//             // increase timeout for retry
//             db.busy_timeout(10000);
//             db.busy_handler(null);
//             tracker.reset();
//             threadPool = new ThreadPool(numThreads);

//             threadPool.runTask(createTask1(id1, filepath, tracker));
//             threadPool.runTask(createTask2(id2, filepath, tracker));

//             threadPool.join();

//             errors = tracker.getErrors();
//             if (errors.size() > 0) {
//                 fail("busy timeout should prevent from lock exception!");
//                 for (String s: errors) {
//                     System.out.println("Round 2 Error"+s);
//                 }
//             } else {
//                 // ok
//             }
//         } finally {
//             conn.setAutoCommit(true);
//             // cleanup
//             db->Exec(DatabaseCreator.DROP_TABLE1, NULL);
//             db->Exec(DatabaseCreator.DROP_TABLE3, NULL);
//         }
//     }

    void testGetTableString() {
        AutoPtr<ITableResult> emptyTable;
        CTableResult::New((ITableResult**)&emptyTable);
        //select from empty table
        AutoPtr<ITableResult> res;
        db->GetTableEx(String("select * from SIMPLE_TABLE1 ;"),(ITableResult**)&res);
        String emptystr;
        String resstr;
        emptyTable->ToString(&emptystr);
        res->ToString(&resstr);
        printf("emptystr: %s === resstr : %s\n", emptystr.string(),resstr.string());
        //fill table-> t
//        DatabaseCreator.fillSimpleTable1(conn);
//        res = db.GetTable("select * from "
//                + DatabaseCreator.SIMPLE_TABLE1);
//        assertFalse(emptyTable.toString().equals(res.toString()));

        db->Exec(String("insert into SIMPLE_TABLE1 VALUES(1, 10, 20)"), NULL);
        db->GetTableEx(String("select * from SIMPLE_TABLE1 ;"),(ITableResult**)&res);
        db->Exec(String("delete from SIMPLE_TABLE1 where 1 ;"), NULL);
        String outstr;
        res->ToString(&outstr);
        printf("testGetTableString-===>%s\n", outstr.string());
    }

    void testGetTableStringStringArray() {
        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = String("table");
        AutoPtr<ArrayOf<String> > argsFail = ArrayOf<String>::Alloc(1);
        // ECode ec = db->GetTableEx3(String("select name from sqlite_master where type = "), *argsFail , NULL);
        // if (ec != NOERROR)
        // {
        //     printf("Testmethod should fail\n");
        // }
        AutoPtr<ITableResult> res;
        db->GetTableEx3( String("select name from sqlite_master where type = '%q'"), *args, (ITableResult**)&res);
        String outstr;
        res->ToString(&outstr);
        printf("testGetTableStringStringArray===> %s\n", outstr.string());
    }

    void testGetTableStringStringArrayTableResult() {
        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        AutoPtr<ArrayOf<String> > argsFail = ArrayOf<String>::Alloc(1);
        AutoPtr<ITableResult> res ;
        AutoPtr<ITableResult> defaultTableRes;
        CTableResult::New((ITableResult**)&defaultTableRes);
        (*args)[0] = String("table");
        db->GetTableEx3(String("select name from sqlite_master where type = '%q'"), *argsFail, (ITableResult**)&res);
        String resstr;
        String defaultStr;
        res->ToString(&resstr);
        defaultTableRes->ToString(&defaultStr);
        printf("resstr : %s, defaultStr : %s\n", resstr.string(),defaultStr.string());
        db->GetTableEx3(String("select name from sqlite_master where type = '%q'"), *args, (ITableResult**)&res);
        res->ToString(&resstr);
        printf("resstr : %s, defaultStr : %s\n", resstr.string(),defaultStr.string());

    }

    void testComplete() {
        Boolean comflag(FALSE);
        dbhelper->Complete(String("create"),&comflag);
        printf("comflag ==>%d\n", comflag);
        dbhelper->Complete(String("create table TEST (res double);"),&comflag);
        printf("comflag 2==>%d\n", comflag);
    }

    void testVersion() {
        String version;
        dbhelper->Version(&version);
        if (!version.IsNull()) {
            printf("versionsub==>%d\n", version.Substring(0,1).ToInt32());
            printf("version 1==>%s\n", version.string());
            dbhelper->Version(&version);
            printf("version 2==>%s\n", version.string());
        } else {
            printf("DB version info missing \n");
        }
    }

    void testDbversion() {
        String verNo("");
        String verNo1("");

        db->Dbversion(&verNo);
        db->Close();
        db->Dbversion(&verNo1);
        printf("unknown ?= %s\n", verNo1.string());
        db->Open(filepath, 0);
        db->Dbversion(&verNo1);
        printf("verNo1==>%s;verNo==>%s\n", verNo1.string() , verNo.string());
        // assertTrue(Integer.parseInt(verNo.substring(0, 1))>= 3 );
    }

    void testCreate_function() {
        Double input = 1.0;
        db->Exec(String("create table TEST (res double)"), NULL);
        db->Exec(String("insert into TEST values (") + StringUtils::DoubleToString(input) + String(")"), NULL);
        AutoPtr<ITableResult> res;
        CTableResult::New((ITableResult**)&res);
        AutoPtr<IFunction> sinFunc;
        CSinFunc::New((IFunction**)&sinFunc);
        if (!sinFunc)
        {
            PRINT_FILE_LINE_EX("sinFunc is null\n")
        }
        db->CreateFunction(String("sin"), 1, sinFunc);
        db->Exec(String("select sin(res) from TEST WHERE res = ") + StringUtils::DoubleToString(input), (ICallback *)res.Get());
        String row;
        res->ToString(&row);
        printf("row==>%s\n", row.string());
        Double funcVal = Elastos::Core::Math::Sin(input);
        printf("funcVal==>%f\n", funcVal);

        // assertTrue(Math.round(funcVal) == Math.round(sinusVal));
    }

    /**
     * Aggregation function not called.
     */
    void testCreate_aggregate() {
        AutoPtr<ITrace> t;
        CTestTrace::New((ITrace**)&t);
        AutoPtr<IMockFunction> aggFunction;
        CMockFunction::New((IMockFunction**)&aggFunction);
        db->Exec(String("drop table TEST "), NULL);
        db->Exec(String("create table TEST(id integer, firstname text, lastname text)"), NULL);
        db->Exec(String("insert into TEST values(3, 'James', 'Bond'); "), NULL);
        db->Exec(String("insert into TEST values(4, 'Fiona', 'Apple'); "), NULL);
        db->Trace(t);
        db->CreateAggregate(String("myaggfunc"), 1, (IFunction *)aggFunction.Get());
        db->FunctionType(String("myaggfunc"), IConstants::SQLITE3_TEXT);
        db->Exec(String("PRAGMA show_datatypes = on"), NULL);
        aggFunction->PrintCalled();

        db->Exec(String("select myaggfunc(TEST.firstname) from TEST"), (ICallback *)t->Probe(EIID_ICallback));
        aggFunction->PrintCalled();

        String outstr;
        aggFunction->GetAggValue(&outstr);
        printf("James Fiona ?= %s\n",outstr.string());

        db->Exec(String("drop table TEST"), NULL);


        db->CreateAggregate(String("myaggfunc"), 0, NULL);
        // catch (Throwable e) {
        //     assertEquals("null SQLite.Function not allowed",e.getMessage());
        // }


        db->CreateAggregate(String("myaggfunc"), 0, aggFunction);
        // catch (Throwable e) {
        //     assertEquals("wrong number of arguments to function myaggfunc()",e.getMessage());
        // }
    }

    void testFunction_type() {
        Double input = 1.0;
        AutoPtr<ITableResult> res;
        AutoPtr<IFunction> sinFunc;
        CSinFunc::New((IFunction**)&sinFunc);

        db->Exec(String("PRAGMA show_datatypes = on"), NULL);
        db->Exec(String("create table TEST (res double)"), NULL);
        db->Exec(String("insert into TEST values (") + StringUtils::DoubleToString(input) + String(")"), NULL);

        db->CreateFunction(String("sin"), 1, sinFunc);
        db->FunctionType(String("sin"), IConstants::SQLITE_FLOAT);
        db->GetTableEx(String("select sin(res) from TEST WHERE res = ")
                        + StringUtils::DoubleToString(input),(ITableResult**)&res);

        // AutoPtr<ArrayOf<String> > row = res.rows.elementAt(0);
        // String val = row[0];
        // assertTrue("double".equalsIgnoreCase(res.types[0]));
        // assertSame(Math.round(Math.sin(input)), Math.round(Double.parseDouble(val)));
        String outstr;
        res->ToString(&outstr);
        printf("math.sin: %f ,outstr: %s\n", Elastos::Core::Math::Sin(input) , outstr.string());

        // function determines return type: test that Double type is returned.
        db->FunctionType(String("sin"), IConstants::SQLITE_BLOB);
        AutoPtr<IStmt> s;
        db->Prepare(String("select sin(res) from TEST WHERE res = ?"), (IStmt**)&s);
        s->BindEx2(1, input);
        Boolean stepflag(FALSE);
        s->Step(&stepflag);

        db->GetTableEx(String("select sin(res) from TEST WHERE res = ")
                        + StringUtils::DoubleToString(input), (ITableResult**)&res);
        // assertTrue("double".equalsIgnoreCase(res.types[0]));
        // row = (String[]) res.rows.elementAt(0);
        // val = row[0];
        // assertSame(Math.round(Math.sin(input)), Math.round(Double.parseDouble(val)));
        res->ToString(&outstr);
        printf("math.sin: %f ,outstr: %s\n", Elastos::Core::Math::Sin(input) , outstr.string());

        db->Exec(String("drop table TEST"), NULL);
    }

    void testLast_error() {
        // assertEquals(db.last_error(), Constants.SQLITE_OK);
        Int32 lasterr(0) ;
        Int32 lasterr2(0);
        db->LastError(&lasterr);
        printf("lasterr: %d , SQLITE_OK: %d\n", lasterr , IConstants::SQLITE_OK);

        db->Exec(String("create table TEST (res double)"),NULL);
        ECode ec = db->Exec(String("create table TEST (res double)"),NULL);
        if (ec != NOERROR)
        {
            printf("Error should have happened !\n");
        }
        db->LastError(&lasterr);
        db->LastError(&lasterr2);
        printf("lasterr: %d , lasterr2: %d , SQLITE_ERROR: %d\n", lasterr , lasterr2, IConstants::SQLITE_ERROR);

        db->Exec(String("drop table TEST"), NULL);
    }

    void testSet_last_error() {
        Int32 lasterr(0) ;
        db->LastError(&lasterr);
        printf("lasterr: %d , SQLITE_OK: %d\n", lasterr , IConstants::SQLITE_OK);

        db->Exec(String("sel from test;"), NULL);
        db->LastError(&lasterr);
        printf("lasterr: %d , SQLITE_ERROR: %d\n", lasterr , IConstants::SQLITE_ERROR);
    }

    void testError_message() {
        String statement("create table TEST (res double)");

        db->Exec(statement,NULL);
        ECode ec = db->Exec(statement,NULL);
        if (ec != NOERROR)
        {
            printf("DB Error expected !\n");
        }

        String dbError;
        db->ErrorMessage(&dbError);
        printf("error: %s\n", dbError.string());

        db->Exec(String("drop table TEST"), NULL);

    }

    void testError_string() {
        AutoPtr<ITrace> t;
        CTestTrace::New((ITrace**)&t);
        Int32 lasterr(0) ;
        db->LastError(&lasterr);
        printf("lasterr: %d , SQLITE_OK: %d\n", lasterr , IConstants::SQLITE_OK);
        String errorString;
        dbhelper->ErrorString(IConstants::SQLITE_ERROR,&errorString);

        db->Trace(t);
        db->Exec(String("create table TEST (res double)"), (ICallback *)t.Get());

        db->LastError(&lasterr);
        printf("lasterr: %d , SQLITE_ERROR: %d\n", lasterr , IConstants::SQLITE_ERROR);
        Boolean is3flag(FALSE);
        db->Is3(&is3flag);
        if (is3flag) {
            db->LastError(&lasterr);
            String ouerr;
            dbhelper->ErrorString(lasterr,&ouerr);
            printf("Unsupported Method (sqlite 3):ouerr: %s  error_string : %s \n", ouerr.string(), errorString.string());
        }
    }

    /**
     * ASCII encoding does not work: a UTF encoded val is returned. Spec is not
     * sufficient. Might be that test impl is wrong or String constructor for
     * the ASCII encoding.
     */
    void testSet_encoding() {
        String input("\u00bfMa\u00f1ana\u003f"); // ?Manana?
        AutoPtr<ITableResult> res;
        CTableResult::New((ITableResult**)&res);
        String refOutput(NULL);
        AutoPtr<IStmt> stat;

        // DB setup
        db->Exec(String("create table encodingTest (encoded text DEFAULT NULL);"),NULL);
        ECode ec = db->Prepare(String("insert into encodingTest(encoded) values(:one);"),(IStmt**)&stat);
        ec = stat->BindEx4(1, input);
        Boolean stepflag(FALSE);
        ec = stat->Step(&stepflag);

        // stat.close();
        db->Exec(String("select * from encodingTest;"), (ICallback *)res.Get());
        String output;
        res->ToString(&output);
        printf("input:%s ?= output: %s\n", input.string(),output.string());
        // db->Exec("delete from encodingTest where 1", NULL);

        // tests for different encoding schemes
        AutoPtr<ArrayOf<String> > charsetNames = ArrayOf<String>::Alloc(4);
        (*charsetNames)[0] = String("UTF-8");
        (*charsetNames)[1] = String("UTF-16");
        (*charsetNames)[2] = String("UTF-16BE");
        (*charsetNames)[3] = String("UTF-16LE");
        for (UInt32 i = 0; i < charsetNames->GetLength(); i++) {
            // byte[] encInputBytes = input.getBytes(charsetNames[i]);
            db->SetEncoding((*charsetNames)[i]);
            db->Exec(String("select * from encodingTest;"), (ICallback *)res);
            String encOutput;
            // String inputAsString = new String(encInputBytes,charsetNames[i]);
            res->ToString(&encOutput);
            printf("encOutput: %s\n",encOutput.string());
            // assertEquals(inputAsString, encOutput[0]);
        }

        // Default tests
        db->SetEncoding(String("UTF-16"));
        db->Exec(String("select * from encodingTest;"), (ICallback *)res);
        String encOutput1;
        res->ToString(&encOutput1);
        printf("encOutput1: %s\n",encOutput1.string());
        // assertEquals("Got "+encOutput1[0]+" as UTF-16",input,encOutput1[0]);

        db->SetEncoding(String("US-ASCII"));
        db->Exec(String("select * from encodingTest;"), (ICallback *)res);
        String encOutput2;
        res->ToString(&encOutput2);
        printf("encOutput2: %s\n",encOutput2.string());
        // assertEquals(new String(input.getBytes(),"US-ASCII"),encOutput2[0]);

        // DB teardown
        // stat->Close();
        db->Exec(String("delete from encodingTest"), NULL);

        ec = db->SetEncoding(String(""));
        if (ec != NOERROR)
        {
            printf("invalid input should fail \n");
        }

        db->Exec(String("drop table encodingTest"), NULL);
    }

    /**
     * Callback never made for authorization. Results of private table are
     * returned withouth furhter checks.
     *
     * Test fails -> implemented correctly?
     */
    void testSet_authorizer() {
        AutoPtr<ITableResult> resPriv;
        AutoPtr<ITableResult> resPub ;
        AutoPtr<ITableResult> emptyTable;
        CTableResult::New((ITableResult**)&emptyTable);
        String insertPublic("insert into public_table values(1,2)");
        String insertPrivate("insert into private_table values(1,2)");
        // prepare, authorizer is not activated yet
        db->Exec(String("create table public_table(c1 integer, c2 integer);"), NULL);
        db->Exec(String("create table private_table(c1 integer, c2 integer);"), NULL);
        // inserts
        db->Exec(insertPublic, NULL);
        db->Exec(insertPrivate, NULL);
        // selects
        db->GetTableEx(String("select * from private_table"), (ITableResult**)&resPriv);
        db->GetTableEx(String("select * from public_table"), (ITableResult**)&resPub);

//        db->Exec("delete from public_table where 1", NULL);
//        TableResult emptyPubTable = db->Exec("select * from public");

        // set Authorizer (positive case): denies private table
        AutoPtr<IAuthorizer> cb;
        CAuthorizerCallback::New((IAuthorizer**)&cb);
        db->SetAuthorizer(cb);
        //select
        db->Exec(String("select * from private_table"), (ICallback *)cb->Probe(EIID_ICallback));
        // assertTrue(cb.wasCalled());

       /*
        TableResult res = db.GetTable("select * from private_table");
        assertEquals(emptyTable.toString(),res.toString());
        assertFalse(emptyTable.equals(resPriv));

        res = db.GetTable("select * from public_table");
        assertEquals(resPub,res);
        */

        // Try insert

        ECode ec = db->Exec(insertPublic, NULL);
        if (ec != NOERROR)
        {
            printf("authorization failed\n");
        }

        ec = db->Exec(insertPrivate, NULL);
        if (ec != NOERROR)
        {
            printf("authorization failed\n");
        }

        db->Exec(String("drop table public_table"), NULL);
        db->Exec(String("drop table private_table"), NULL);
    }

    void testTrace() {
        String stmt("create table TEST (res double);");
        AutoPtr<ITrace> t;
        CTestTrace::New((ITrace**)&t);
        // assertFalse(t.traceCalled);
        Int32 lasterr(0);
        db->LastError(&lasterr);
        if (lasterr == IConstants::SQLITE_OK)
        {
            printf("NOERROR !\n");
        } else {
            printf("error: %d\n", lasterr);
        }
        db->Trace(t);
        db->Exec(stmt,(ICallback *)t.Get());
        // assertTrue(t.traceCalled);
        // assertEquals(t.getTrace(),stmt);

        db->Close();
        ECode ec = db->Exec(stmt,(ICallback *)t.Get());
        if (ec != NOERROR)
        {
            printf("Exception Expected\n");
        }
    }

    void testCompileString() {
        db->Compile(String("select name from sqlite_master;"),NULL);
        ECode ec = db->Compile(String("test"),NULL);
        if (ec != NOERROR)
        {
            printf("Compiling of inaccurate statement does not fail.\n");
        }
    }

    void testCompileStringStringArray() {
        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = String("table");
        db->CompileEx(String("select name from sqlite_master where type = '%q';"), *args, NULL);

        ECode ec = db->Compile(String("test"),NULL);
        if (ec != NOERROR)
        {
            printf("Compiling of inaccurate statement does not fail.\n");
        }
    }

    void testPrepare() {
        AutoPtr<IStmt> st;
        AutoPtr<IStmt> st2;
        // test empty statement
        db->Prepare(String(""),(IStmt**)&st);
        Int32 outbind(0);
        st->BindParameterCount(&outbind);
        printf("outbind:%d\n", outbind);
        Boolean stepflag(FALSE);
        st->Step(&stepflag);
        if (!stepflag)
        {
            printf("stmt should not be prepared \n");
        }

        // test statement with unbound arguments
        db->Prepare(String("insert into SIMPLE_TABLE1 values (:one,:two,:three)"),(IStmt**)&st2);
        st2->BindParameterCount(&outbind);
        printf("outbind:%d\n", outbind);

        st2->BindParameterIndex(String(":three"),&outbind);
        printf("Index:%d\n", outbind);

        String namestr;
        st2->BindParameterName(2,&namestr);
        printf(":two ?= %s\n", namestr.string());
        st2->Close();

        ECode ec = db->Prepare(String("insert into SIMPLE_TABLE1 values(:one,:two,:three,:four);"),NULL);
        if (ec != NOERROR)
        {
            printf("1==> error should happened!!\n");
        }
        // } catch (Exception e) {
        //     assertEquals("table " + DatabaseCreator.SIMPLE_TABLE1
        //             + " has 3 columns but 4 values were supplied", e
        //             .getMessage());
        // }

        ec = db->Prepare(String("insert into SIMPLE_TABLE1 values(5, '10, 20);"),NULL);
        if (ec != NOERROR)
        {
            printf("2==> error should happened!!\n");
        }
        // } catch (Exception e) {
        //     assertEquals("unrecognized token: \"'10, 20);\"", e.getMessage());
        // }

        ec = db->Prepare(String("insert into SIMPLE_TABLE1 values(5, 10 20);"),NULL);
        if (ec != NOERROR)
        {
            printf("3==> error should happened!!\n");
        }
        // } catch (Exception e) {
        //     assertEquals("near \"20\": syntax error", e.getMessage());
        // }

    }

    /**
     * Not supported.
     */
    void testOpen_blob() {
        AutoPtr<IStmt> statement2;
        AutoPtr<Elastos::Sql::SQLite::IBlob> blobInput;
        CBlob::New((Elastos::Sql::SQLite::IBlob **)&blobInput);

        // Create test input Blob
        // AutoPtr<IInputStream> inStream;
        AutoPtr<ArrayOf<Byte> > in = ArrayOf<Byte>::Alloc(4);
        (*in)[0] = 1;
        (*in)[1] = 2;
        (*in)[2] = 3;
        (*in)[3] = 4;

        // setup test input
        db->Exec(String("create table TEST (res blob)"),NULL);
        // inStream = Class.forName(this.getClass().getName()).getResourceAsStream("/blob.c");
        // assertNotNull(inStream);

        // insert byte array in db
        db->Prepare(String("insert into TEST(res) values (?)"),(IStmt**)&statement2);
        ECode ec = statement2->BindEx3(1, *in);
        if (ec != NOERROR)
        {
            PRINT_FILE_LINE_EX("BindEx3 error:ec = %p",ec)
        }
        Boolean stepflag(FALSE);
        ec = statement2->Step(&stepflag);
        if (ec != NOERROR)
        {
            PRINT_FILE_LINE_EX("Step error:ec = %p",ec)
        }
        PRINT_FILE_LINE_EX("stepflag:%d",stepflag)
        statement2->Close();

        // read from db
        AutoPtr<Elastos::Sql::SQLite::IBlob> blob;
        ec = db->OpenBlob(filepath, String("TEST"), String("res"), 1, TRUE, (Elastos::Sql::SQLite::IBlob **)&blob);
        if (ec != NOERROR)
        {
            PRINT_FILE_LINE_EX("OpenBlob error:ec = %p",ec)
        }
        if (blob == NULL) {
            printf("Blob could not be retrieved \n");
        }
        //read from blob and compare values (positive case)
        AutoPtr<IInputStream> is;
        blob->GetInputStream((IInputStream**)&is);

        Int32 i = 0;
        Int32 outByte = 0;
        AutoPtr<ArrayOf<Byte> > out = ArrayOf<Byte>::Alloc(4);
        is->Read(&outByte);
        while ( outByte > -1) {
            (*out)[i] = outByte;
            i++;
            is->Read(&outByte);
        }
        is->Close();

        blob->Close();

        // assertTrue(Arrays.equals(in, out));

        //read from blob and compare values (default blob)
        db->Exec(String("insert into TEST values(zeroblob(128))"), NULL);
        AutoPtr<Elastos::Sql::SQLite::IBlob> blob2 ;
        db->OpenBlob(filepath, String("TEST"), String("res"), 2, TRUE , (Elastos::Sql::SQLite::IBlob **)&blob2);
        blob2->GetInputStream((IInputStream**)&is);
        for (i = 0; i < 128; i++)  {
            is->Read(&outByte);
            if (outByte)
            {
                printf("i: %d : blob has no zero~~\n",i);
            }
        }
        is->Close();

        db->Exec(String("drop table TEST ;"),NULL);
    }

    void testIs3() {
        Int32 ver(0);
        String verstr;
        dbhelper->Version(&verstr);
        ver = verstr.Substring(0,1).ToInt32();
        Boolean is3flag(FALSE);
        db->Is3(&is3flag);
        if (is3flag) {
            printf("is3==>%d \n", ver == 3);
        } else {
            printf("notis3==>%d \n", ver != 3);
        }
    }

    void testProgress_handler() {
        Int32 inputVal = 3;
        AutoPtr<IProgressHandler> prog;
        CTestProgressHandler::New((IProgressHandler**)&prog);
        db->Exec(String("create table TEST5(id integer, firstname text, lastname text)"),NULL);
        AutoPtr<IVm> vm;
        ECode ec = db->Compile(String("select * from TEST5; ")
                    + String("insert into TEST5 values(3, 'James', 'Bond'); ")
                    + String("delete from TEST5 where id = 3; ")
                    + String("select * from TEST5"),
                    (IVm**)&vm);
        if (ec != NOERROR)
        {
            PRINT_FILE_LINE_EX("Compile error:ec = %p",ec)
        }
        Int32 stmt = 0;
        Boolean vmflag(TRUE);
        do {
            ++stmt;
            if (stmt > inputVal) {
                db->ProgressHandler(inputVal, prog);
            } else {
                // assertEquals(0, prog.getCounts());
            }
            Boolean progflag(TRUE);
            while (progflag) {
                ec = vm->Step((ICallback *)prog->Probe(EIID_ICallback),&progflag);
                if (ec != NOERROR)
                {
                    PRINT_FILE_LINE_EX("Step error:ec = %p",ec)
                }
            }
            vm->Compile(&vmflag);
        } while (vmflag);

        // assertEquals(inputVal,prog.getCounts());

        // Boundary value test
        inputVal = 0;
        AutoPtr<IProgressHandler> progBoundary;
        CTestProgressHandler::New((IProgressHandler**)&progBoundary);
        db->ProgressHandler(inputVal, progBoundary);
        AutoPtr<IVm> vm2;
        db->Compile(String("select * from TEST5; ")
                    + String("insert into TEST5 values(3, 'James', 'Bond'); ")
                    + String("delete from TEST5 where id = 3; ")
                    + String("select * from TEST5"),
                    (IVm**)&vm2);
        vmflag = TRUE;
        do {
            Boolean progflag(TRUE);
            vm2->Step((ICallback *)progBoundary->Probe(EIID_ICallback),&progflag);
            vm2->Compile(&vmflag);
        } while (vmflag);
        // assertEquals(inputVal, progBoundary.getCounts());
        ec = db->Exec(String("drop table TEST5"),NULL);

        db->Exec(String("drop table TEST5;"),NULL);

    }

    // /**
    //  * This method creates a Runnable that executes insert operation for the first table
    //  */
    // private static Runnable createTask2Interrupt(final int id,
    //         final String dbName, final ErrorTracker errorTracker) {
    //     return new Runnable() {
    //         void run() {
    //             Database db = new Database();
    //             try {
    //                 String value = DatabaseCreator.defaultString + id;

    //                 db.open(dbName, 0);
    //                 String insertQuery = "INSERT INTO "
    //                         + DatabaseCreator.TEST_TABLE1
    //                         + " (id, field1, field2, field3) VALUES(" + id
    //                         + ", '" + value + "', " + id + ", " + id + ")";
    //                 db->Exec(insertQuery, NULL);
    //             } catch (Exception e) {
    //                 errorTracker.registerException(this, e);
    //                 try {
    //                     db.interrupt();
    //                     db->Exec("DELETE FROM " + DatabaseCreator.SIMPLE_TABLE1
    //                             + " WHERE id=" + id, NULL);
    //                 } catch (Exception e1) {
    //                     errorTracker.registerException(this, e1);
    //                 }
    //             }
    //         }
    //     };
    // }

//     /**
//      * This method creates a Runnable that executes delete operation for the first table
//      */
//     private static Runnable createTask1(final int id, final String dbName,
//             final ErrorTracker errorTracker) {
//         return new Runnable() {
//             void run() {
//                 try {
//                     Database db = new Database();
//                     db.open(dbName, 0);
//                     db->Exec("DELETE FROM "
//                             + DatabaseCreator.SIMPLE_TABLE1 + " WHERE id=" + id, NULL);
//                 } catch (Exception e) {
//                     errorTracker.registerException(this, e);
//                 }
//             }
//         };
//     }

//     /**
//      * This method creates a Runnable that executes insert operation for the first table
//      */
//     private static Runnable createTask2(final int id, final String dbName,
//             final ErrorTracker errorTracker) {
//         return new Runnable() {
//             void run() {
//                 try {
//                     String value = DatabaseCreator.defaultString + id;
//                     Database db = new Database();
//                     db.open(dbName, 0);
//                     String insertQuery = "INSERT INTO "
//                             + DatabaseCreator.TEST_TABLE1
//                             + " (id, field1, field2, field3) VALUES(" + id
//                             + ", '" + value + "', " + id + ", " + id + ")";
//                     db->Exec(insertQuery, NULL);
//                 } catch (Exception e) {
//                     errorTracker.registerException(this, e);

//                 }
//             }
//         };
//     }

//     /**
//      * This method creates a Runnable that executes update operation for the one record of the first
//      * table
//      */
//     private static Runnable createTask3(final int oldID, final String dbName,
//             final int newID, final ErrorTracker errorTracker) {
//         return new Runnable() {
//             void run() {
//                 Database db = new Database();
//                 try {
//                     db.open(dbName, 0);
//                     String value = DatabaseCreator.defaultString + newID;
//                     String updateQuery = "UPDATE "
//                             + DatabaseCreator.TEST_TABLE1 + " SET id=" + newID
//                             + ", field1='" + value + "', field2=" + newID
//                             + ", field3=" + newID + " WHERE id=" + oldID;
//                     db->Exec(updateQuery, NULL);
//                 } catch (Exception e) {
//                     errorTracker.registerException(this, e);
//                 }
//             }
//         };
//     }

//     private class ErrorTracker {

//         private List<String> errors = new ArrayList<String>();

//         void registerException(Runnable runnable, Exception e) {
//             System.out.println("Registered: " + e.getMessage());
//             errors.add(e.getMessage());
//         }

//         public List<String> getErrors() {
//             return errors;
//         }

//         void reset() {
//             errors.clear();
//         }
//     };
};

AutoPtr<IConnection> OldSQLiteTest::conn;

AutoPtr<IFile> OldSQLiteTest::dbFile ;

AutoPtr<IFileHelper> OldSQLiteTest::dbFileHelper;

AutoPtr<IDriverManager> OldSQLiteTest::driverManager;

String OldSQLiteTest::filepath = String("/data/data/com.elastos.runtime/elastos/sqliteTest.db");

const String OldDatabaseTest::CREATE_TABLE_SIMPLE1 = String("CREATE TABLE simple_table1 (id INTEGER NOT NULL,speed INTEGER,size INTEGER)");
