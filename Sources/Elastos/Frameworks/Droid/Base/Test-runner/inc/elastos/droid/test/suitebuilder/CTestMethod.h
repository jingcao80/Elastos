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

#ifndef __CTESTMETHOD_H__
#define __CTESTMETHOD_H__

#include "_Elastos_Droid_Test_SuiteBuilder_CTestMethod.h"
#include <elastos/core/Object.h>

using Eunit::Framework::ITestCase;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

/**
 * Represents a test to be run. Can be constructed without instantiating the TestCase or even
 * loading the class.
 */
CarClass(CTestMethod)
    , public Object
    , public ITestMethod
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IClassInfo* enclosingClass);

    CARAPI constructor(
        /* [in] */ const String& methodName,
        /* [in] */ IClassInfo* enclosingClass);

    CARAPI constructor(
        /* [in] */ ITestCase* testCase);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetEnclosingClassname(
        /* [out] */ String* classname);

    // public <T extends Annotation> T getAnnotation(Class<T> annotationClass);

    CARAPI GetEnclosingClass(
        /* [out] */ IClassInfo** clazz);

    CARAPI CreateTest(
        /* [out] */ ITestCase** test);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    CARAPI InstantiateTest(
        /* [in] */ IClassInfo* testCaseClass,
        /* [in] */ const String& testName,
        /* [out] */ ITestCase** test);

    CARAPI_(Boolean) SingleStringConstructor(
        /* [in] */ ArrayOf<IParamInfo*>* params);

    CARAPI_(Boolean) NoargsConstructor(
        /* [in] */ ArrayOf<IParamInfo*>* params);

private:
    String mEnclosingClassname;
    String mTestMethodName;
    AutoPtr<IClassInfo> mEnclosingClass;
};

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__CTESTMETHOD_H__
