
#include "Elastos.CoreLibrary.IO.h"
#include "CDriverManager.h"
#include "CSystem.h"
#include "StringUtils.h"
#include "CProperties.h"
#include "AutoLock.h"
#include "Collections.h"
#include "CArrayList.h"

using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::IProperties;
using Elastos::Utility::CProperties;
using Elastos::Utility::Collections;
using Elastos::Utility::ICollection;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Sql {

List<AutoPtr<IDriver> > CDriverManager::mTheDrivers;
AutoPtr<IPrintStream> CDriverManager::mThePrintStream;
AutoPtr<IPrintWriter> CDriverManager::mThePrintWriter;
Int32 CDriverManager::mLoginTimeout = 0;
Boolean CDriverManager::mIsInitflag = FALSE;

CAR_INTERFACE_IMPL(CDriverManager, Object, IDriverManager);

CAR_SINGLETON_IMPL(CDriverManager);

CDriverManager::CDriverManager()
{
    if (mIsInitflag == FALSE) {
        LoadInitialDrivers();
        mIsInitflag = TRUE;
    }
}

Boolean CDriverManager::LoadInitialDrivers()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String theDriverList;
    system->GetProperty(String("jdbc.drivers"), String(NULL), &theDriverList);
    if (theDriverList.IsNull()) {
        return FALSE;
    }

    /*
     * Get the names of the drivers as an array of Strings from the system
     * property by splitting the property at the separator character ':'
     */
    AutoPtr<ArrayOf<String> > theDriverNames;
    StringUtils::Split(theDriverList, String(":"), (ArrayOf<String> **)&theDriverNames);

    if (theDriverNames) {
        for (Int32 i = 0; i < theDriverNames->GetLength(); ++i) {
            String element = (*theDriverNames)[i];
            // try {
                // Load the driver class
            //    Class.forName(element, true, ClassLoader.getSystemClassLoader());
            // } catch (Throwable t) {
            //     // Ignored
            // }
        }
    }
    return TRUE;
}

ECode CDriverManager::DeregisterDriver(
    /* [in] */ IDriver * driver)
{
    if (driver == NULL) {
        return NOERROR;
    }
    AutoPtr<IClassLoader> callerClassLoader ;// = VMStack.getCallingClassLoader();
    assert(0 && "TODO");
    if (!IsClassFromClassLoader(driver, callerClassLoader)) {
        return E_SECURITY_EXCEPTION;
    }

    synchronized(mTheDriversLock) {
        AutoPtr<IDriver> d = driver;
        mTheDrivers.Remove(d);
    }

    return NOERROR;
}

ECode CDriverManager::GetConnection(
    /* [in] */ const String& url,
    /* [out] */ IConnection ** conn)
{
    VALIDATE_NOT_NULL(conn);

    AutoPtr<IProperties> mprope;
    CProperties::New((IProperties **)&mprope);
    return GetConnection(url, mprope , conn);
}

ECode CDriverManager::GetConnection(
    /* [in] */ const String& url,
    /* [in] */ IProperties * info,
    /* [out] */ IConnection ** conn)
{
    VALIDATE_NOT_NULL(conn)
    *conn = NULL;

    // 08 - connection exception
    // 001 - SQL-client unable to establish SQL-connection
    String sqlState = String("08001");
    if (url.IsNull()) {
        return E_SQL_EXCEPTION;
    }

    synchronized(mTheDriversLock) {
        /*
         * Loop over the drivers in the DriverSet checking to see if one can
         * open a connection to the supplied URL - return the first
         * connection which is returned
         */
        List<AutoPtr<IDriver> >::Iterator iter = mTheDrivers.Begin();
        while (iter != mTheDrivers.End()) {
            AutoPtr<IConnection> theConnection;
            (*iter)->Connect(url, info, (IConnection **)&theConnection);
            if (theConnection != NULL) {
                *conn = theConnection;
                REFCOUNT_ADD(*conn)
                return NOERROR;
            }
        }
    }
    // If we get here, none of the drivers are able to resolve the URL
    return E_SQL_EXCEPTION;
}

ECode CDriverManager::GetConnection(
    /* [in] */ const String& url,
    /* [in] */ const String& user,
    /* [in] */ const String& password,
    /* [out] */ IConnection ** conn)
{
    VALIDATE_NOT_NULL(conn)
    *conn = NULL;

    AutoPtr<IProperties> theProperties;
    CProperties::New((IProperties **)&theProperties);

    String temp;
    if (!user.IsNull()) {
        theProperties->SetProperty(String("user"), user, &temp);
    }
    if (!password.IsNull()) {
        theProperties->SetProperty(String("password"), password, &temp);
    }
    return GetConnection(url, theProperties,conn);
}

ECode CDriverManager::GetDriver(
    /* [in] */ const String& url,
    /* [out] */ IDriver ** driver)
{
    VALIDATE_NOT_NULL(driver)
    *driver = NULL;

    AutoPtr<IClassLoader> callerClassLoader  ;//= VMStack.getCallingClassLoader();
    assert(0 && "TODO");
    synchronized(mTheDriversLock) {
        /*
         * Loop over the drivers in the DriverSet checking to see if one
         * does understand the supplied URL - return the first driver which
         * does understand the URL
         */
        List<AutoPtr<IDriver> >::Iterator iter = mTheDrivers.Begin();
        while (iter != mTheDrivers.End()) {
            Boolean isiterflag = FALSE;
            (*iter)->AcceptsURL(url,&isiterflag);
            if (isiterflag &&
                IsClassFromClassLoader(*iter, callerClassLoader)) {
                *driver = *iter;
                REFCOUNT_ADD(*driver)
                return NOERROR;
            }
        }
    }
    // If no drivers understand the URL, throw an SQLException
    // SQLState: 08 - connection exception
    // 001 - SQL-client unable to establish SQL-connection
    return E_SQL_EXCEPTION;
}

ECode CDriverManager::GetDrivers(
    /* [out] */ IEnumeration ** enumeration)
{
    VALIDATE_NOT_NULL(enumeration);
    *enumeration = NULL;

    /*
     * Synchronize to avoid clashes with additions and removals of drivers
     * in the DriverSet
     */
    AutoPtr<IClassLoader> callerClassLoader ; //= VMStack.getCallingClassLoader();
    assert(0 && "TODO");
    synchronized(mTheDriversLock) {
        AutoPtr<ICollection> result;
        CArrayList::New((ICollection**)&result);
        List<AutoPtr<IDriver> >::Iterator iter = mTheDrivers.Begin();
        while (iter != mTheDrivers.End()) {
            if (IsClassFromClassLoader(*iter, callerClassLoader)) {
                result->Add((*iter)->Probe(EIID_IInterface));
            }
        }
        return Collections::Enumeration(result, enumeration);
    }
    return NOERROR;
}

ECode CDriverManager::GetLoginTimeout(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value);
    *value = mLoginTimeout;
    return NOERROR;
}

ECode CDriverManager::GetLogStream(
    /* [out] */ IPrintStream ** pntstream)
{
    VALIDATE_NOT_NULL(pntstream);
    *pntstream = mThePrintStream;
    REFCOUNT_ADD(*pntstream)
    return NOERROR;
}

ECode CDriverManager::GetLogWriter(
    /* [out] */ IPrintWriter ** pntwriter)
{
    VALIDATE_NOT_NULL(pntwriter);
    *pntwriter = mThePrintWriter;
    REFCOUNT_ADD(*pntwriter);
    return NOERROR;
}

ECode CDriverManager::Println(
    /* [in] */ const String& message)
{
    if (mThePrintWriter != NULL) {
        mThePrintWriter->Print(message);
        IFlushable::Probe(mThePrintWriter)->Flush();
    }
    else if (mThePrintStream != NULL) {
        mThePrintStream->Print(message);
        IFlushable::Probe(mThePrintStream)->Flush();
    }
    /*
     * If neither the PrintWriter not the PrintStream are set, then silently
     * do nothing the message is not recorded and no exception is generated.
     */
    return NOERROR;
}

ECode CDriverManager::RegisterDriver(
    /* [in] */ IDriver * driver)
{
    if (driver == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    synchronized(mTheDriversLock) {
        mTheDrivers.PushBack(driver);
    }
    return NOERROR;
}

ECode CDriverManager::SetLoginTimeout(
    /* [in] */ Int32 seconds)
{
    mLoginTimeout = seconds;
    return NOERROR;
}

ECode CDriverManager::SetLogStream(
    /* [in] */ IPrintStream * pntstream)
{
    mThePrintStream = pntstream;
    return NOERROR;
}

ECode CDriverManager::SetLogWriter(
    /* [in] */ IPrintWriter * pntwriter)
{
    mThePrintWriter = pntwriter;
    return NOERROR;
}

Boolean CDriverManager::IsClassFromClassLoader(
    /* [in] */ IInterface * theObject,
    /* [in] */ IClassLoader * theClassLoader)
{
    if ((theObject == NULL) || (theClassLoader == NULL)) {
        return FALSE;
    }

    // Class<?> objectClass = theObject.getClass();

    // try {
    //     Class<?> checkClass = Class.forName(objectClass.getName(), true,
    //             theClassLoader);
    //     if (checkClass == objectClass) {
    //         return true;
    //     }
    // } catch (Throwable t) {
    //     // Empty
    // }
    return FALSE;
}

} // namespace Sql
} // namespace Elastos
