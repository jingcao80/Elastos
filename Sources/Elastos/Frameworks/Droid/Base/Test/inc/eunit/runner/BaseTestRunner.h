
#ifndef __EUNIT_RUNNER_BASETESTRUNNER_H__
#define __EUNIT_RUNNER_BASETESTRUNNER_H__

#include <elastos/coredef.h>
#include <_Eunit.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::Utility::IProperties;
using Eunit::Framework::ITest;
using Eunit::Framework::ITestListener;
using Eunit::Runner::IBaseTestRunner;

namespace Eunit {
namespace Runner {

class BaseTestRunner
    : public Object
    , public ITestListener
    , public IBaseTestRunner
{
public:
    CAR_INTERFACE_DECL()

    CARAPI StartTest(
        /* [in] */ ITest* test);

    static CARAPI_(void) SavePreferences();

    CARAPI SetPreference(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI EndTest(
        /* [in] */ ITest* test);

    CARAPI AddError(
        /* [in] */ ITest* test,
        /* [in] */ ECode ec);

    CARAPI AddFailure(
        /* [in] */ ITest* test,
        /* [in] */ ECode ec);

    CARAPI ElapsedTimeAsString(
        /* [in] */ Int64 runTime,
        /* [out] */ String* timeStr);

    CARAPI SetLoading(
        /* [in] */ Boolean bEnable);

    CARAPI ExtractClassName(
        /* [in] */ const String& className,
        /* [out] */ String* result);

    /**
     * Truncates a String to the maximum length.
     */
    static CARAPI_(String) Truncate(
        /* [in] */ const String& s);

    CARAPI GetLoader(
        /* [out] */ ITestSuiteLoader** loader);

    static CARAPI_(String) GetPreference(
        /* [in] */ const String& key);

    static CARAPI_(Int32) GetPreference(
        /* [in] */ const String& key,
        /* [in] */ Int32 dflt);

    /**
     * Returns a filtered stack trace
     */
    static CARAPI_(String) GetFilteredTrace(
        /* [in] */ ECode ec);

    // BEGIN android-changed - add back this method for API compatibility
    /** @deprecated not present in JUnit4.10 */
    // public static boolean inVAJava();

    /**
     * Filters stack frames from internal JUnit classes
     */
    static CARAPI_(String) GetFilteredTrace(
        /* [in] */ const String& stack);

    static CARAPI_(Boolean) FilterLine(
        /* [in] */ const String& line);

protected:
    static CARAPI_(void) SetPreferences(
        /* [in] */ IProperties* preferences);

    static CARAPI_(AutoPtr<IProperties>) GetPreferences();

    /**
     * Processes the command line arguments and
     * returns the name of the suite class to run or null
     */
    virtual CARAPI_(String) ProcessArguments(
        /* [in] */ const ArrayOf<String>& args);

    /**
     * Override to define how to handle a failed loading of
     * a test suite.
     */
    virtual CARAPI_(void) RunFailed(
        /* [in] */ const String& message);

    /**
     * Returns the loaded Class for a suite name.
     */
    virtual CARAPI LoadSuiteClass(
        /* [in] */ const String& suiteClassName,
        /* [out] */ IClassInfo** clazz);

    /**
     * Clears the status message.
     */
    virtual CARAPI_(void) ClearStatus();

    virtual CARAPI_(Boolean) UseReloadingTestSuiteLoader();

    static CARAPI_(Boolean) ShowStackRaw();

private:
    static CARAPI_(AutoPtr<IFile>) GetPreferencesFile();

    static CARAPI_(void) ReadPreferences();

public:
    static const String SUITE_METHODNAME;

    static Int32 sMaxMessageLength;
    static Boolean sFilterStack;
    Boolean mLoading;

private:
    static AutoPtr<IProperties> sPreferences;
    Object mLock;
};

} // namespace Runner
} // namespace Eunit

#endif //__EUNIT_RUNNER_BASETESTRUNNER_H__
