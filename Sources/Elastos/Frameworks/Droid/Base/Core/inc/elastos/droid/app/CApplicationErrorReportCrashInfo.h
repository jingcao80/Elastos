
#ifndef __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTCRASHINFO_H__
#define __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTCRASHINFO_H__

#include "_Elastos_Droid_App_CApplicationErrorReportCrashInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Describes an application crash.
 */
CarClass(CApplicationErrorReportCrashInfo)
    , public Object
    , public IApplicationErrorReportCrashInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CApplicationErrorReportCrashInfo();

    /**
     * Create an uninitialized instance of CrashInfo.
     */
    CARAPI constructor();

    /**
     * Dump a CrashInfo instance to a Printer.
     */
    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /**
     * Class name of the exception that caused the crash.
     */
    CARAPI GetExceptionClassName(
        /* [out] */ String* className);

    CARAPI SetExceptionClassName(
        /* [in] */ const String& className);

    /**
     * Message stored in the exception.
     */
    CARAPI GetExceptionMessage(
        /* [out] */ String* exceptionMessage);

    CARAPI SetExceptionMessage(
        /* [in] */ const String& exceptionMessage);

    /**
     * File which the exception was thrown from.
     */
    CARAPI GetThrowFileName(
        /* [out] */ String* fileName);

    CARAPI SetThrowFileName(
        /* [in] */ const String& fileName);

    /**
     * Class which the exception was thrown from.
     */
    CARAPI GetThrowClassName(
        /* [out] */ String* className);

    CARAPI SetThrowClassName(
        /* [in] */ const String& className);

    /**
     * Method which the exception was thrown from.
     */
    CARAPI GetThrowMethodName(
        /* [out] */ String* methodName);

    CARAPI SetThrowMethodName(
        /* [in] */ const String& methodName);

    /**
     * Line number the exception was thrown from.
     */
    CARAPI GetThrowLineNumber(
        /* [out] */ Int32* lineNumber);

    CARAPI SetThrowLineNumber(
        /* [in] */ Int32 lineNumber);

    /**
     * Stack trace.
     */
    CARAPI GetStackTrace(
        /* [out] */ String* stackTrace);

    CARAPI SetStackTrace(
        /* [in] */ const String& stackTrace);

private:
    /**
    * Class name of the exception that caused the crash.
    */
    String mExceptionClassName;

    /**
     * Message stored in the exception.
     */
    String mExceptionMessage;

    /**
     * File which the exception was thrown from.
     */
    String mThrowFileName;

    /**
     * Class which the exception was thrown from.
     */
    String mThrowClassName;

    /**
     * Method which the exception was thrown from.
     */
    String mThrowMethodName;

    /**
     * Line number the exception was thrown from.
     */
    Int32 mThrowLineNumber;

    /**
     * Stack trace.
     */
    String mStackTrace;
};


} // namespace App
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTCRASHINFO_H__
