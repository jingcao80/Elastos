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

#ifndef __EUNIT_FRAMEWORK_ASSERT_H__
#define __EUNIT_FRAMEWORK_ASSERT_H__

#include <_Eunit.h>

using namespace Elastos;

namespace Eunit {
namespace Framework {

/**
 * A set of assert methods.  Messages are only displayed when an assert fails.
 */
class ECO_PUBLIC Assert
{
public:
    /**
     * Asserts that a condition is true. If it isn't it throws
     * an AssertionFailedError with the given message.
     */
    static CARAPI AssertTrue(
        /* [in] */ const String& message,
        /* [in] */ Boolean condition);

    /**
     * Asserts that a condition is true. If it isn't it throws
     * an AssertionFailedError.
     */
    static CARAPI AssertTrue(
        /* [in] */ Boolean condition);

    /**
     * Asserts that a condition is false. If it isn't it throws
     * an AssertionFailedError with the given message.
     */
    static CARAPI AssertFalse(
        /* [in] */ const String& message,
        /* [in] */ Boolean condition);

    /**
     * Asserts that a condition is false. If it isn't it throws
     * an AssertionFailedError.
     */
    static CARAPI AssertFalse(
        /* [in] */ Boolean condition);

    /**
     * Fails a test with the given message.
     */
    static CARAPI Fail(
        /* [in] */ const String& message);

    /**
     * Fails a test with no message.
     */
    static CARAPI Fail();

    /**
     * Asserts that two objects are equal. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertObjectEquals(
        /* [in] */ const String& message,
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    /**
     * Asserts that two objects are equal. If they are not
     * an AssertionFailedError is thrown.
     */
    static CARAPI AssertObjectEquals(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    /**
     * Asserts that two Strings are equal.
     */
    static CARAPI AssertStringEquals(
        /* [in] */ const String& message,
        /* [in] */ const String& expected,
        /* [in] */ const String& actual);

    /**
     * Asserts that two Strings are equal.
     */
    static CARAPI AssertStringEquals(
        /* [in] */ const String& expected,
        /* [in] */ const String& actual);

    /**
     * Asserts that two doubles are equal concerning a delta.  If they are not
     * an AssertionFailedError is thrown with the given message.  If the expected
     * value is infinity then the delta value is ignored.
     */
    static CARAPI AssertDoubleEquals(
        /* [in] */ const String& message,
        /* [in] */ Double expected,
        /* [in] */ Double actual,
        /* [in] */ Double delta);

    /**
     * Asserts that two doubles are equal concerning a delta. If the expected
     * value is infinity then the delta value is ignored.
     */
    static CARAPI AssertDoubleEquals(
        /* [in] */ Double expected,
        /* [in] */ Double actual,
        /* [in] */ Double delta);

    /**
     * Asserts that two floats are equal concerning a positive delta. If they
     * are not an AssertionFailedError is thrown with the given message. If the
     * expected value is infinity then the delta value is ignored.
     */
    static CARAPI AssertFloatEquals(
        /* [in] */ const String& message,
        /* [in] */ Float expected,
        /* [in] */ Float actual,
        /* [in] */ Float delta);

    /**
     * Asserts that two floats are equal concerning a delta. If the expected
     * value is infinity then the delta value is ignored.
     */
    static CARAPI AssertFloatEquals(
        /* [in] */ Float expected,
        /* [in] */ Float actual,
        /* [in] */ Float delta);

    /**
     * Asserts that two longs are equal. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertInt64Equals(
        /* [in] */ const String& message,
        /* [in] */ Int64 expected,
        /* [in] */ Int64 actual);

    /**
     * Asserts that two longs are equal.
     */
    static CARAPI AssertInt64Equals(
        /* [in] */ Int64 expected,
        /* [in] */ Int64 actual);

    /**
     * Asserts that two booleans are equal. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertBooleanEquals(
        /* [in] */ const String& message,
        /* [in] */ Boolean expected,
        /* [in] */ Boolean actual);

    /**
     * Asserts that two booleans are equal.
     */
    static CARAPI AssertBooleanEquals(
        /* [in] */ Boolean expected,
        /* [in] */ Boolean actual);

    /**
     * Asserts that two bytes are equal. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertByteEquals(
        /* [in] */ const String& message,
        /* [in] */ Byte expected,
        /* [in] */ Byte actual);

    /**
     * Asserts that two bytes are equal.
     */
    static CARAPI AssertByteEquals(
        /* [in] */ Byte expected,
        /* [in] */ Byte actual);

    /**
     * Asserts that two chars are equal. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertCharEquals(
        /* [in] */ const String& message,
        /* [in] */ Char32 expected,
        /* [in] */ Char32 actual);

    /**
     * Asserts that two chars are equal.
     */
    static CARAPI AssertCharEquals(
        /* [in] */ Char32 expected,
        /* [in] */ Char32 actual);

    /**
     * Asserts that two shorts are equal. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertInt16Equals(
        /* [in] */ const String& message,
        /* [in] */ Int16 expected,
        /* [in] */ Int16 actual);

    /**
     * Asserts that two shorts are equal.
     */
    static CARAPI AssertInt16Equals(
        /* [in] */ Int16 expected,
        /* [in] */ Int16 actual);

    /**
     * Asserts that two ints are equal. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertInt32Equals(
        /* [in] */ const String& message,
        /* [in] */ Int32 expected,
        /* [in] */ Int32 actual);

    /**
     * Asserts that two ints are equal.
     */
    static CARAPI AssertInt32Equals(
        /* [in] */ Int32 expected,
        /* [in] */ Int32 actual);

    /**
     * Asserts that an object isn't null.
     */
    static CARAPI AssertNotNull(
        /* [in] */ IInterface* object);

    /**
     * Asserts that an object isn't null. If it is
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertNotNull(
        /* [in] */ const String& message,
        /* [in] */ IInterface* object);

    /**
     * Asserts that an object is null. If it isn't an {@link AssertionError} is
     * thrown.
     * Message contains: Expected: <null> but was: object
     *
     * @param object
     *            Object to check or <code>null</code>
     */
    static CARAPI AssertNull(
        /* [in] */ IInterface* object);

    /**
     * Asserts that an object is null.  If it is not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertNull(
        /* [in] */ const String& message,
        /* [in] */ IInterface* object);

    /**
     * Asserts that two objects refer to the same object. If they are not
     * an AssertionFailedError is thrown with the given message.
     */
    static CARAPI AssertSame(
        /* [in] */ const String& message,
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    /**
     * Asserts that two objects refer to the same object. If they are not
     * the same an AssertionFailedError is thrown.
     */
    static CARAPI AssertSame(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    /**
     * Asserts that two objects do not refer to the same object. If they do
     * refer to the same object an AssertionFailedError is thrown with the
     * given message.
     */
    static CARAPI AssertNotSame(
        /* [in] */ const String& message,
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    /**
     * Asserts that two objects do not refer to the same object. If they do
     * refer to the same object an AssertionFailedError is thrown.
     */
    static CARAPI AssertNotSame(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    static CARAPI FailSame(
        /* [in] */ const String& message);

    static CARAPI FailNotSame(
        /* [in] */ const String& message,
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    static CARAPI FailNotEquals(
        /* [in] */ const String& message,
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    static CARAPI_(String) Format(
        /* [in] */ const String& message,
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

protected:
    /**
     * Protect constructor since it is a static only class
     */
    Assert() {}

public:
    static AutoPtr<ITest> sCurrentTest;
};

} // namespace Framework
} // namespace Eunit

#endif //__EUNIT_FRAMEWORK_ASSERT_H__
