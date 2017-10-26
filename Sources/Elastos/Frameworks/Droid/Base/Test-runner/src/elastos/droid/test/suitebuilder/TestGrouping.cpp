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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/test/suitebuilder/TestGrouping.h"
#include "elastos/droid/test/suitebuilder/CTestMethod.h"
#include "elastos/droid/test/PackageInfoSources.h"
#include "elastos/droid/test/ClassPathPackageInfoSource.h"
#include "elastos/droid/test/ClassPathPackageInfo.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::EIID_IComparator;
using Elastos::Utility::INavigableSet;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Internal::Utility::EIID_IPredicate;

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

//==================================================================
//     TestGrouping::SortBySimpleName
//==================================================================
CAR_INTERFACE_IMPL(TestGrouping::SortBySimpleName, Object, IComparator)

ECode TestGrouping::SortBySimpleName::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IClassInfo> class1, class2;
    class1 = IClassInfo::Probe(lhs);
    class2 = IClassInfo::Probe(rhs);
    assert(class1 != NULL && class2 != NULL);
    String name1, name2;
    class1->GetName(&name1);
    class2->GetName(&name2);
    *result = name1.Compare(name2);
    return NOERROR;
}


//==================================================================
//     TestGrouping::SortByFullyQualifiedName
//==================================================================
CAR_INTERFACE_IMPL(TestGrouping::SortByFullyQualifiedName, Object, IComparator)

ECode TestGrouping::SortByFullyQualifiedName::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IClassInfo> class1, class2;
    class1 = IClassInfo::Probe(lhs);
    class2 = IClassInfo::Probe(rhs);
    assert(class1 != NULL && class2 != NULL);
    String name1, name2;
    class1->GetName(&name1);
    class2->GetName(&name2);
    *result = name1.Compare(name2);
    return NOERROR;
}


//==================================================================
//     TestGrouping::TestCasePredicate
//==================================================================
CAR_INTERFACE_IMPL(TestGrouping::TestCasePredicate, Object, IPredicate)

ECode TestGrouping::TestCasePredicate::Apply(
    /* [in] */ IInterface* t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // int modifiers = ((Class<?>) aClass).getModifiers();
    // return TestCase.class.isAssignableFrom((Class<?>) aClass)
    //         && Modifier.isPublic(modifiers)
    //         && !Modifier.isAbstract(modifiers)
    //         && hasValidConstructor((Class<?>) aClass);
    AutoPtr<IClassInfo> aClass = IClassInfo::Probe(t);
    assert(aClass != NULL);
    AutoPtr<IInterfaceInfo> itfInfo;
    aClass->GetInterfaceInfo(String("ITestCase"), (IInterfaceInfo**)&itfInfo);
    if (itfInfo == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    *result = HasValidConstructor(aClass);
    return NOERROR;
}

Boolean TestGrouping::TestCasePredicate::HasValidConstructor(
    /* [in] */ IClassInfo* aClass)
{
    Int32 N;
    aClass->GetConstructorCount(&N);
    AutoPtr< ArrayOf<IConstructorInfo*> > constructors = ArrayOf<IConstructorInfo*>::Alloc(N);
    aClass->GetAllConstructorInfos(constructors);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IConstructorInfo> constructor = (*constructors)[i];
        Int32 count;
        constructor->GetParamCount(&count);
        AutoPtr< ArrayOf<IParamInfo*> > params = ArrayOf<IParamInfo*>::Alloc(count);
        constructor->GetAllParamInfos(params);
        if (params->GetLength() == 0) return TRUE;
        if (params->GetLength() == 1) {
            AutoPtr<IParamInfo> param = (*params)[0];
            AutoPtr<IDataTypeInfo> typeInfo;
            param->GetTypeInfo((IDataTypeInfo**)&typeInfo);
            CarDataType type;
            typeInfo->GetDataType(&type);
            if (type == CarDataType_String) return TRUE;
        }
    }
    String name;
    aClass->GetName(&name);
    Logger::I(TAG, "TestCase class %s is missing a public constructor with no parameters "
            "or a single String parameter - skipping", name.string());
    return FALSE;
    // The cast below is not necessary with the Java 5 compiler, but necessary with the Java 6 compiler,
    // where the return type of Class.getDeclaredConstructors() was changed
    // from Constructor<T>[] to Constructor<?>[]
    // Constructor<? extends TestCase>[] constructors
    //         = (Constructor<? extends TestCase>[]) aClass.getConstructors();
    // for (Constructor<? extends TestCase> constructor : constructors) {
    //     if (Modifier.isPublic(constructor.getModifiers())) {
    //         java.lang.Class[] parameterTypes = constructor.getParameterTypes();
    //         if (parameterTypes.length == 0 ||
    //                 (parameterTypes.length == 1 && parameterTypes[0] == String.class)) {
    //             return true;
    //         }
    //     }
    // }
    // Log.i(LOG_TAG, String.format(
    //         "TestCase class %s is missing a public constructor with no parameters " +
    //         "or a single String parameter - skipping",
    //         aClass.getName()));
    // return false;
}


//==================================================================
//     TestGrouping::TestMethodPredicate
//==================================================================
CAR_INTERFACE_IMPL(TestGrouping::TestMethodPredicate, Object, IPredicate)

ECode TestGrouping::TestMethodPredicate::Apply(
    /* [in] */ IInterface* t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMethodInfo> method = IMethodInfo::Probe(t);
    assert(method != NULL);
    Int32 count;
    method->GetParamCount(&count);
    String name;
    method->GetName(&name);
    *result = ((count == 0) && name.StartWith("Test"));
    return NOERROR;
    // return ((method.getParameterTypes().length == 0) &&
    //         (method.getName().startsWith("test")) &&
    //         (method.getReturnType().getSimpleName().equals("void")));
}


//==================================================================
//     TestGrouping
//==================================================================
const String TestGrouping::TAG("TestGrouping");
AutoPtr<IComparator> TestGrouping::SORT_BY_SIMPLE_NAME = new TestGrouping::SortBySimpleName();

AutoPtr<IComparator> TestGrouping::SORT_BY_FULLY_QUALIFIED_NAME = new TestGrouping::SortByFullyQualifiedName();

TestGrouping::TestGrouping(
    /* [in] */ IComparator* comparator)
{
    AutoPtr<INavigableSet> set;
    ASSERT_SUCCEEDED(CTreeSet::New(comparator, (INavigableSet**)&set));
    mTestCaseClasses = ISortedSet::Probe(set);
}

AutoPtr<IList> TestGrouping::GetTests()
{
    AutoPtr<IArrayList> testMethods;
    CArrayList::New((IArrayList**)&testMethods);
    AutoPtr<IIterator> it;
    mTestCaseClasses->GetIterator((IIterator**)&it);
    Boolean result;
    while(it->HasNext(&result), result) {
        AutoPtr<IInterface> elem;
        it->GetNext((IInterface**)&elem);
        IClassInfo* testCase = IClassInfo::Probe(elem);
        AutoPtr<IList> testMethodList = GetTestMethods(testCase);
        Int32 N;
        testMethodList->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            testMethodList->Get(i, (IInterface**)&obj);
            IMethodInfo* testMethod = IMethodInfo::Probe(obj);
            AutoPtr<ITestMethod> method;
            CTestMethod::New(testMethod, testCase, (ITestMethod**)&method);
            Boolean result;
            testMethods->Add(method, &result);
        }
    }
    return IList::Probe(testMethods);
}

AutoPtr<IList> TestGrouping::GetTestMethods(
    /* [in] */ IClassInfo* testCaseClass)
{
    AutoPtr<IArrayList> methods;
    CArrayList::New((IArrayList**)&methods);
    Int32 N;
    testCaseClass->GetMethodCount(&N);
    AutoPtr< ArrayOf<IMethodInfo*> > methodInfos = ArrayOf<IMethodInfo*>::Alloc(N);
    testCaseClass->GetAllMethodInfos(methodInfos);
    for (Int32 i = 0; i < N; i++) {
        Boolean result;
        methods->Add((*methodInfos)[i], &result);
    }
    AutoPtr<IPredicate> predicate = new TestMethodPredicate();
    return Select(ICollection::Probe(methods), predicate);
}

AutoPtr<ISortedSet> TestGrouping::GetTestCaseClasses()
{
    return mTestCaseClasses;
}

ECode TestGrouping::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // if (this == o) {
    //     return true;
    // }
    // if (o == null || getClass() != o.getClass()) {
    //     return false;
    // }
    // TestGrouping other = (TestGrouping) o;
    // if (!this.testCaseClasses.equals(other.testCaseClasses)) {
    //     return false;
    // }
    // return this.testCaseClasses.comparator().equals(other.testCaseClasses.comparator());
    assert(0);
    *result = FALSE;
    return NOERROR;
}

ECode TestGrouping::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    // return testCaseClasses.hashCode();
    assert(0);
    *hashCode = 0;
    return NOERROR;
}

AutoPtr<TestGrouping> TestGrouping::AddPackagesRecursive(
    /* [in] */ ArrayOf<String>* packageNames)
{
    if (packageNames != NULL) {
        for (Int32 i = 0; i < packageNames->GetLength(); i++) {
            String packageName = (*packageNames)[i];
            AutoPtr<IList> addedClasses = TestCaseClassesInPackage(packageName);
            Boolean result;
            if (addedClasses->IsEmpty(&result), result) {
                Logger::W(TAG, "Invalid Package: '%s' could not be found or has no tests",
                        packageName.string());
            }
            mTestCaseClasses->AddAll(ICollection::Probe(addedClasses), &result);
            if (mFirstIncludedPackage.IsNull()) {
                mFirstIncludedPackage = packageName;
            }
        }
    }
    return this;
}

AutoPtr<TestGrouping> TestGrouping::RemovePackagesRecursive(
    /* [in] */ ArrayOf<String>* packageNames)
{
    if (packageNames != NULL) {
        for (Int32 i = 0; i < packageNames->GetLength(); i++) {
            String packageName = (*packageNames)[i];
            AutoPtr<IList> removedClasses = TestCaseClassesInPackage(packageName);
            Boolean result;
            mTestCaseClasses->RemoveAll(ICollection::Probe(removedClasses), &result);
        }
    }
    return this;
}

String TestGrouping::GetFirstIncludedPackage()
{
    return mFirstIncludedPackage;
}

AutoPtr<IList> TestGrouping::TestCaseClassesInPackage(
    /* [in] */ const String& packageName)
{
    AutoPtr<ClassPathPackageInfoSource> source = PackageInfoSources::ForClassPath(mClassLoader);
    AutoPtr<ClassPathPackageInfo> packageInfo = source->GetPackageInfo(packageName);

    AutoPtr<ISet> classes = packageInfo->GetTopLevelClassesRecursive();
    return SelectTestClasses(classes);
}

AutoPtr<IList> TestGrouping::SelectTestClasses(
    /* [in] */ ISet* allClasses)
{
    AutoPtr<IArrayList> testClasses;
    CArrayList::New((IArrayList**)&testClasses);
    AutoPtr<IPredicate> predicate = new TestCasePredicate();
    AutoPtr<IList> classes = Select(ICollection::Probe(allClasses), predicate);
    Int32 N;
    classes->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> testClass;
        classes->Get(i, (IInterface**)&testClass);
        Boolean result;
        testClasses->Add(testClass, &result);
    }
    return IList::Probe(testClasses);
}

AutoPtr<IList> TestGrouping::Select(
    /* [in] */ ICollection* items,
    /* [in] */ IPredicate* predicate)
{
    AutoPtr<IArrayList> selectedItems;
    CArrayList::New((IArrayList**)&selectedItems);
    if (items != NULL) {
        AutoPtr<IIterator> it;
        items->GetIterator((IIterator**)&it);
        Boolean result;
        while(it->HasNext(&result), result) {
            AutoPtr<IInterface> item;
            it->GetNext((IInterface**)&item);
            if (predicate->Apply(item, &result), result) {
                selectedItems->Add(item, &result);
            }
        }
    }
    return IList::Probe(selectedItems);
}

void TestGrouping::SetClassLoader(
    /* [in] */ IClassLoader* classLoader)
{
    mClassLoader = classLoader;
}

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos
