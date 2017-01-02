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

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/CApplicationErrorReportCrashInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CApplicationErrorReportCrashInfo, Object, IApplicationErrorReportCrashInfo, IParcelable)

CAR_OBJECT_IMPL(CApplicationErrorReportCrashInfo)

CApplicationErrorReportCrashInfo::CApplicationErrorReportCrashInfo()
    : mThrowLineNumber(0)
{
}

/**
 * Create an uninitialized instance of CrashInfo.
 */
ECode CApplicationErrorReportCrashInfo::constructor()
{
    return NOERROR;
}

// /**
//  * Create an instance of CrashInfo initialized from an exception.
//  */
// constructor(Throwable tr) {
//     StringWriter sw = new StringWriter();
//     tr.printStackTrace(new PrintWriter(sw));
//     stackTrace = sw.toString();
//     exceptionMessage = tr.getMessage();

//     // Populate fields with the "root cause" exception
//     Throwable rootTr = tr;
//     while (tr.getCause() != null) {
//         tr = tr.getCause();
//         if (tr.getStackTrace() != null && tr.getStackTrace().length > 0) {
//             rootTr = tr;
//         }
//         String msg = tr.getMessage();
//         if (msg != null && msg.length() > 0) {
//             exceptionMessage = msg;
//         }
//     }

//     exceptionClassName = rootTr.getClass().getName();
//     if (rootTr.getStackTrace().length > 0) {
//         StackTraceElement trace = rootTr.getStackTrace()[0];
//         throwFileName = trace.getFileName();
//         throwClassName = trace.getClassName();
//         throwMethodName = trace.getMethodName();
//         throwLineNumber = trace.getLineNumber();
//     } else {
//         throwFileName = "unknown";
//         throwClassName = "unknown";
//         throwMethodName = "unknown";
//         throwLineNumber = 0;
//     }
// }

/**
 * Class name of the exception that caused the crash.
 */
ECode CApplicationErrorReportCrashInfo::GetExceptionClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className);
    *className = mExceptionClassName;
    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::SetExceptionClassName(
    /* [in] */ const String& className)
{
    mExceptionClassName = className;
    return NOERROR;
}

/**
 * Message stored in the exception.
 */
ECode CApplicationErrorReportCrashInfo::GetExceptionMessage(
    /* [out] */ String* exceptionMessage)
{
    VALIDATE_NOT_NULL(exceptionMessage);
    *exceptionMessage = mExceptionMessage;
    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::SetExceptionMessage(
    /* [in] */ const String& exceptionMessage)
{
    mExceptionMessage = exceptionMessage;
    return NOERROR;
}

/**
 * File which the exception was thrown from.
 */
ECode CApplicationErrorReportCrashInfo::GetThrowFileName(
    /* [out] */ String* fileName)
{
    VALIDATE_NOT_NULL(fileName);
    *fileName = mThrowFileName;
    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::SetThrowFileName(
    /* [in] */ const String& fileName)
{
    mThrowFileName = fileName;
    return NOERROR;
}

/**
 * Class which the exception was thrown from.
 */
ECode CApplicationErrorReportCrashInfo::GetThrowClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className);
    *className = mThrowClassName;
    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::SetThrowClassName(
    /* [in] */ const String& className)
{
    mThrowClassName = className;
    return NOERROR;
}

/**
 * Method which the exception was thrown from.
 */
ECode CApplicationErrorReportCrashInfo::GetThrowMethodName(
    /* [out] */ String* methodName)
{
    VALIDATE_NOT_NULL(methodName);
    *methodName = mThrowMethodName;
    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::SetThrowMethodName(
    /* [in] */ const String& methodName)
{
    mThrowMethodName = methodName;
    return NOERROR;
}

/**
 * Line number the exception was thrown from.
 */
ECode CApplicationErrorReportCrashInfo::GetThrowLineNumber(
    /* [out] */ Int32* lineNumber)
{
    VALIDATE_NOT_NULL(lineNumber);
    *lineNumber = mThrowLineNumber;
    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::SetThrowLineNumber(
    /* [in] */ Int32 lineNumber)
{
    mThrowLineNumber = lineNumber;
    return NOERROR;
}

/**
 * Stack trace.
 */
ECode CApplicationErrorReportCrashInfo::GetStackTrace(
    /* [out] */ String* stackTrace)
{
    VALIDATE_NOT_NULL(stackTrace);
    *stackTrace = mStackTrace;
    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::SetStackTrace(
    /* [in] */ const String& stackTrace)
{
    mStackTrace = stackTrace;
    return NOERROR;
}

/**
 * Dump a CrashInfo instance to a Printer.
 */
ECode CApplicationErrorReportCrashInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);

    StringBuilder sb(128);
    sb.Append(prefix);
    sb.Append("exceptionClassName:");
    sb.Append(mExceptionClassName);
    String str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("exceptionMessage:");
    sb.Append(mExceptionMessage);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("throwFileName:");
    sb.Append(mThrowFileName);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("throwClassName:");
    sb.Append(mThrowClassName);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("throwMethodName:");
    sb.Append(mThrowMethodName);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("throwLineNumber:");
    sb.Append(mThrowLineNumber);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("stackTrace:");
    sb.Append(mStackTrace);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    FAIL_RETURN(parcel->ReadString(&mExceptionClassName));
    FAIL_RETURN(parcel->ReadString(&mExceptionMessage));
    FAIL_RETURN(parcel->ReadString(&mThrowFileName));
    FAIL_RETURN(parcel->ReadString(&mThrowClassName));
    FAIL_RETURN(parcel->ReadString(&mThrowMethodName));
    FAIL_RETURN(parcel->ReadInt32(&mThrowLineNumber));
    FAIL_RETURN(parcel->ReadString(&mStackTrace));

    return NOERROR;
}

ECode CApplicationErrorReportCrashInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mExceptionClassName));
    FAIL_RETURN(dest->WriteString(mExceptionMessage));
    FAIL_RETURN(dest->WriteString(mThrowFileName));
    FAIL_RETURN(dest->WriteString(mThrowClassName));
    FAIL_RETURN(dest->WriteString(mThrowMethodName));
    FAIL_RETURN(dest->WriteInt32(mThrowLineNumber));
    FAIL_RETURN(dest->WriteString(mStackTrace));

    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

