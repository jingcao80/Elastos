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

#include "eunit/framework/Assert.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <Elastos.CoreLibrary.h>
#include <utils/CallStack.h>

using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::IChar32;
using Elastos::Core::CChar32;
using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::StringBuilder;

namespace Eunit {
namespace Framework {

AutoPtr<ITest> Assert::sCurrentTest;

ECode Assert::AssertTrue(
    /* [in] */ const String& message,
    /* [in] */ Boolean condition)
{
    if (!condition) {
        return Fail(message);
    }
    return NOERROR;
}

ECode Assert::AssertTrue(
    /* [in] */ Boolean condition)
{
    String message("condition is FALSE");
    return AssertTrue(message, condition);
}

ECode Assert::AssertFalse(
    /* [in] */ const String& message,
    /* [in] */ Boolean condition)
{
    return AssertTrue(message, !condition);
}

ECode Assert::AssertFalse(
    /* [in] */ Boolean condition)
{
    String message("condition is TRUE");
    return AssertFalse(message, condition);
}

ECode Assert::Fail(
    /* [in] */ const String& message)
{
    android::CallStack stack;
    stack.update();
    assert(sCurrentTest != NULL);
    String backtrace(stack.toString("").string());
    sCurrentTest->AddFailure(E_ASSERTION_FAILED_ERROR, message, backtrace);
    return E_ASSERTION_FAILED_ERROR;
}

ECode Assert::Fail()
{
    String message("failed");
    return Fail(message);
}

ECode Assert::AssertObjectEquals(
    /* [in] */ const String& message,
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    if (expected == NULL && actual == NULL) {
        return NOERROR;
    }
    Boolean isEqual;
    if (expected != NULL && IObject::Probe(expected) != NULL
            && (IObject::Probe(expected)->Equals(actual, &isEqual), isEqual)) {
        return NOERROR;
    }
    return FailNotEquals(message, expected, actual);
}

ECode Assert::AssertObjectEquals(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    return AssertObjectEquals(String(NULL), expected, actual);
}

ECode Assert::AssertStringEquals(
    /* [in] */ const String& message,
    /* [in] */ const String& expected,
    /* [in] */ const String& actual)
{
    if (expected.IsNull() && actual.IsNull()) {
        return NOERROR;
    }
    if (!expected.IsNull() && expected.Equals(actual)) {
        return NOERROR;
    }
    String cleanMessage = message.IsNull() ? String("") : message;
    return Fail(cleanMessage);
}

ECode Assert::AssertStringEquals(
    /* [in] */ const String& expected,
    /* [in] */ const String& actual)
{
    StringBuilder sb;
    sb += "expected:<";
    sb += expected;
    sb += "> but was:<";
    sb += actual;
    sb += ">";
    return AssertStringEquals(sb.ToString(), expected, actual);
}

ECode Assert::AssertDoubleEquals(
    /* [in] */ const String& message,
    /* [in] */ Double expected,
    /* [in] */ Double actual,
    /* [in] */ Double delta)
{
    using Elastos::Core::Math;
    if (Math::Compare(expected, actual) == 0) {
        return NOERROR;
    }
    if (!(Math::Abs(expected - actual) <= delta)) {
        AutoPtr<IDouble> expObj, actObj;
        CDouble::New(expected, (IDouble**)&expObj);
        CDouble::New(actual, (IDouble**)&actObj);
        return FailNotEquals(message, expObj, actObj);
    }
    return NOERROR;
}

ECode Assert::AssertDoubleEquals(
    /* [in] */ Double expected,
    /* [in] */ Double actual,
    /* [in] */ Double delta)
{
    return AssertDoubleEquals(String(NULL), expected, actual, delta);
}

ECode Assert::AssertFloatEquals(
    /* [in] */ const String& message,
    /* [in] */ Float expected,
    /* [in] */ Float actual,
    /* [in] */ Float delta)
{
    using Elastos::Core::Math;
    if (Math::Compare(expected, actual) == 0) {
        return NOERROR;
    }
    if (!(Math::Abs(expected - actual) <= delta)) {
        AutoPtr<IFloat> expObj, actObj;
        CFloat::New(expected, (IFloat**)&expObj);
        CFloat::New(actual, (IFloat**)&actObj);
        return FailNotEquals(message, expObj, actObj);
    }
    return NOERROR;
}

ECode Assert::AssertFloatEquals(
    /* [in] */ Float expected,
    /* [in] */ Float actual,
    /* [in] */ Float delta)
{
    return AssertFloatEquals(String(NULL), expected, actual, delta);
}

ECode Assert::AssertInt64Equals(
    /* [in] */ const String& message,
    /* [in] */ Int64 expected,
    /* [in] */ Int64 actual)
{
    AutoPtr<IInteger64> expObj, actObj;
    CInteger64::New(expected, (IInteger64**)&expObj);
    CInteger64::New(actual, (IInteger64**)&actObj);
    return AssertObjectEquals(message, expObj, actObj);
}

ECode Assert::AssertInt64Equals(
    /* [in] */ Int64 expected,
    /* [in] */ Int64 actual)
{
    return AssertInt64Equals(String(NULL), expected, actual);
}

ECode Assert::AssertBooleanEquals(
    /* [in] */ const String& message,
    /* [in] */ Boolean expected,
    /* [in] */ Boolean actual)
{
    AutoPtr<IBoolean> expObj, actObj;
    CBoolean::New(expected, (IBoolean**)&expObj);
    CBoolean::New(actual, (IBoolean**)&actObj);
    return AssertObjectEquals(message, expObj, actObj);
}

ECode Assert::AssertBooleanEquals(
    /* [in] */ Boolean expected,
    /* [in] */ Boolean actual)
{
    return AssertBooleanEquals(String(NULL), expected, actual);
}

ECode Assert::AssertByteEquals(
    /* [in] */ const String& message,
    /* [in] */ Byte expected,
    /* [in] */ Byte actual)
{
    AutoPtr<IByte> expObj, actObj;
    CByte::New(expected, (IByte**)&expObj);
    CByte::New(actual, (IByte**)&actObj);
    return AssertObjectEquals(message, expObj, actObj);
}

ECode Assert::AssertByteEquals(
    /* [in] */ Byte expected,
    /* [in] */ Byte actual)
{
    return AssertByteEquals(String(NULL), expected, actual);
}

ECode Assert::AssertCharEquals(
    /* [in] */ const String& message,
    /* [in] */ Char32 expected,
    /* [in] */ Char32 actual)
{
    AutoPtr<IChar32> expObj, actObj;
    CChar32::New(expected, (IChar32**)&expObj);
    CChar32::New(actual, (IChar32**)&actObj);
    return AssertObjectEquals(message, expObj, actObj);
}

ECode Assert::AssertCharEquals(
    /* [in] */ Char32 expected,
    /* [in] */ Char32 actual)
{
    return AssertCharEquals(String(NULL), expected, actual);
}

ECode Assert::AssertInt16Equals(
    /* [in] */ const String& message,
    /* [in] */ Int16 expected,
    /* [in] */ Int16 actual)
{
    AutoPtr<IInteger16> expObj, actObj;
    CInteger16::New(expected, (IInteger16**)&expObj);
    CInteger16::New(actual, (IInteger16**)&actObj);
    return AssertObjectEquals(message, expObj, actObj);
}

ECode Assert::AssertInt16Equals(
    /* [in] */ Int16 expected,
    /* [in] */ Int16 actual)
{
    return AssertInt16Equals(String(NULL), expected, actual);
}

ECode Assert::AssertInt32Equals(
    /* [in] */ const String& message,
    /* [in] */ Int32 expected,
    /* [in] */ Int32 actual)
{
    AutoPtr<IInteger32> expObj, actObj;
    CInteger32::New(expected, (IInteger32**)&expObj);
    CInteger32::New(actual, (IInteger32**)&actObj);
    return AssertObjectEquals(message, expObj, actObj);
}

ECode Assert::AssertInt32Equals(
    /* [in] */ Int32 expected,
    /* [in] */ Int32 actual)
{
    return AssertInt32Equals(String(NULL), expected, actual);
}

ECode Assert::AssertNotNull(
    /* [in] */ IInterface* object)
{
    String message("object is NULL");
    return AssertNotNull(message, object);
}

ECode Assert::AssertNotNull(
    /* [in] */ const String& message,
    /* [in] */ IInterface* object)
{
    return AssertTrue(message, object != NULL);
}

ECode Assert::AssertNull(
    /* [in] */ IInterface* object)
{
    String message;
    if (object == NULL) {
        message = "Expected: <null> but was: NULL";
    }
    else {
        assert(IObject::Probe(object) != NULL);
        String str;
        IObject::Probe(object)->ToString(&str);
        message = String("Expected: <null> but was: ") + str;
    }
    return AssertNull(message, object);
}

ECode Assert::AssertNull(
    /* [in] */ const String& message,
    /* [in] */ IInterface* object)
{
    return AssertTrue(message, object == NULL);
}

ECode Assert::AssertSame(
    /* [in] */ const String& message,
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    if (expected->Probe(EIID_IInterface) == actual->Probe(EIID_IInterface)) {
        return NOERROR;
    }
    return FailNotSame(message, expected, actual);
}

ECode Assert::AssertSame(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    return AssertSame(String(NULL), expected, actual);
}

ECode Assert::AssertNotSame(
    /* [in] */ const String& message,
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    if (expected->Probe(EIID_IInterface) == actual->Probe(EIID_IInterface)) {
        return FailSame(message);
    }
    return NOERROR;
}

ECode Assert::AssertNotSame(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    return AssertNotSame(String(NULL), expected, actual);
}

ECode Assert::FailSame(
    /* [in] */ const String& message)
{
    String formatted("");
    if (!message.IsNull()) {
        formatted = message + " ";
    }
    return Fail(formatted + "expected not same");
}

ECode Assert::FailNotSame(
    /* [in] */ const String& message,
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    StringBuilder formatted("");
    if (!message.IsNull()) {
        formatted.Append(message);
        formatted.Append(" ");
    }
    assert(IObject::Probe(expected) != NULL && IObject::Probe(actual) != NULL);
    String expStr, actStr;
    IObject::Probe(expected)->ToString(&expStr);
    IObject::Probe(actual)->ToString(&actStr);
    formatted.Append("expected same:<");
    formatted.Append(expStr);
    formatted.Append("> was not:<");
    formatted.Append(actStr);
    formatted.Append(">");
    return Fail(formatted.ToString());
}

ECode Assert::FailNotEquals(
    /* [in] */ const String& message,
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    return Fail(Format(message, expected, actual));
}

String Assert::Format(
    /* [in] */ const String& message,
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    StringBuilder formatted("");
    if (!message.IsNullOrEmpty()) {
        formatted.Append(message);
        formatted.Append(" ");
    }
    assert(IObject::Probe(expected) != NULL && IObject::Probe(actual) != NULL);
    String expStr, actStr;
    IObject::Probe(expected)->ToString(&expStr);
    IObject::Probe(actual)->ToString(&actStr);
    formatted.Append("expected:<");
    formatted.Append(expStr);
    formatted.Append("> but was:<");
    formatted.Append(actStr);
    formatted.Append(">");
    return formatted.ToString();
}

} // namespace Framework
} // namespace Eunit
