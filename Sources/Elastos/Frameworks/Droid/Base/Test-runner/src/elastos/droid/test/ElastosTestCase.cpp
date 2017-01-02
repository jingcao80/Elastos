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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/test/ElastosTestCase.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Database::ICursor;

namespace Elastos {
namespace Droid {
namespace Test {

CAR_INTERFACE_IMPL(ElastosTestCase, TestCase, IElastosTestCase)

ECode ElastosTestCase::SetUp()
{
    return TestCase::SetUp();
}

ECode ElastosTestCase::TearDown()
{
    return TestCase::TearDown();
}

ECode ElastosTestCase::TestElastosTestCaseSetupProperly()
{
    return AssertNotNull(String("Context is null. setContext "
            "should be called before tests are run"), mContext);
}

ECode ElastosTestCase::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode ElastosTestCase::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = mContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode ElastosTestCase::SetTestContext(
    /* [in] */ IContext* context)
{
    mTestContext = context;
    return NOERROR;
}

ECode ElastosTestCase::GetTestContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = mTestContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode ElastosTestCase::AssertActivityRequiresPermission(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ const String& permission)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(packageName, className);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

    // try {
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    ECode ec = context->StartActivity(intent);
    if (SUCCEEDED(ec)) {
        return Fail(String("expected security exception for ") + permission);
    }
    return AssertTrue(String("security exception's error message."), ec == (ECode)E_SECURITY_EXCEPTION);
    // } catch (SecurityException expected) {
    //     assertNotNull("security exception's error message.", expected.getMessage());
    //     assertTrue("error message should contain " + permission + ".",
    //             expected.getMessage().contains(permission));
    // }
}

ECode ElastosTestCase::AssertReadingContentUriRequiresPermission(
    /* [in] */ IUri* uri,
    /* [in] */ const String& permission)
{
    // try {
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    ECode ec = resolver->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (SUCCEEDED(ec)) {
        return Fail(String("expected SecurityException requiring ") + permission);
    }
    return AssertTrue(String("security exception's error message."), ec == (ECode)E_SECURITY_EXCEPTION);
    // } catch (SecurityException expected) {
    //     assertNotNull("security exception's error message.", expected.getMessage());
    //     assertTrue("error message should contain " + permission + ".",
    //             expected.getMessage().contains(permission));
    // }
}

ECode ElastosTestCase::AssertWritingContentUriRequiresPermission(
    /* [in] */ IUri* uri,
    /* [in] */ const String& permission)
{
    // try {
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> retUri;
    ECode ec = resolver->Insert(uri, values, (IUri**)&retUri);
    if (SUCCEEDED(ec)) {
        return Fail(String("expected SecurityException requiring ") + permission);
    }
    return AssertTrue(String("security exception's error message."), ec == (ECode)E_SECURITY_EXCEPTION);
    // } catch (SecurityException expected) {
    //     assertNotNull("security exception's error message.", expected.getMessage());
    //     assertTrue("error message should contain \"" + permission + "\". Got: \""
    //             + expected.getMessage() + "\".",
    //             expected.getMessage().contains(permission));
    // }
}

void ElastosTestCase::ScrubClass(
    /* [in] */ IClassInfo* testCaseClass)
{
    assert(0);
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
