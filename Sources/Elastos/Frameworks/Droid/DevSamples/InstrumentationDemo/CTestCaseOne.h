
#ifndef __CTESTCASEONE_H__
#define __CTESTCASEONE_H__

#include <ext/frameworkdef.h>
#include "_CTestCaseOne.h"
#include <droid/test/InstrumentationTestCase.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Test::InstrumentationTestCase;
using Eunit::Framework::ITestResult;
using Eunit::Framework::ITestAnnotation;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace InstrumentationDemo {

CarClass(CTestCaseOne)
    , public InstrumentationTestCase
    , public IMyTestCase
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    CARAPI GetTestAnnotation(
        /* [out] */ ITestAnnotation** annotation);

    CARAPI TestAddToGroup();

protected:
    CARAPI SetUp();

    CARAPI TearDown();

private:
    static CARAPI_(AutoPtr<ICharSequence>) StringToICharSequence(
        /* [in] */ const String& str);

    static CARAPI_(AutoPtr<IInteger32>) Int32ToIInteger32(
        /* [in] */ Int32 value);

private:
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IContentProviderClient> mProvider;

    AutoPtr<IArrayList> mPeopleRowsAdded;
    AutoPtr<IArrayList> mGroupRowsAdded;
    AutoPtr<IArrayList> mRowsAdded;

    static AutoPtr< ArrayOf<String> > PEOPLE_PROJECTION;
    static const Int32 PEOPLE_ID_INDEX;
    static const Int32 PEOPLE_LAST_CONTACTED_INDEX;

    static const Int32 MEMBERSHIP_PERSON_ID_INDEX;
    static const Int32 MEMBERSHIP_GROUP_ID_INDEX;

    static AutoPtr< ArrayOf<String> > GROUPS_PROJECTION;
    static const Int32 GROUPS_ID_INDEX;
    static const Int32 GROUPS_NAME_INDEX;
};

}
}
}
}

#endif // __CTESTCASEONE_H__
