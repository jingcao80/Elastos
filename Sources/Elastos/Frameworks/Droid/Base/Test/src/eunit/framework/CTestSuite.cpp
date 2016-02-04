
#include <Elastos.CoreLibrary.Utility.h>
#include "eunit/framework/CTestSuite.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::CVector;

namespace Eunit {
namespace Framework {

ECode CTestSuite::MyTestCase::RunTest()
{
    return Fail(mMessage);
}

ECode CTestSuite::MyTestCase::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    assert(0);
    return NOERROR;
}

ECode CTestSuite::MyTestCase::GetTestAnnotation(
    /* [out] */ ITestAnnotation** annotation)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}


CTestSuite::CTestSuite()
{
    ASSERT_SUCCEEDED(CVector::New(10, (IVector**)&mTests));
}

AutoPtr<ITest> CTestSuite::CreateTest(
    /* [in] */ IClassInfo* theClass,
    /* [in] */ const String& name)
{
    AutoPtr<IConstructorInfo> constructor;
    // try {
    ECode ec = GetTestConstructor(theClass, (IConstructorInfo**)&constructor);
    if (FAILED(ec)) {
        StringBuilder sb("Class ");
        String name;
        theClass->GetName(&name);
        sb.Append(name);
        sb.Append(" has no public constructor TestCase(String name) or TestCase()");
        return Warning(sb.ToString());
    }
    // } catch (NoSuchMethodException e) {
    //     return warning("Class "+theClass.getName()+" has no public constructor TestCase(String name) or TestCase()");
    // }
    AutoPtr<IInterface> test;
    // try {
    Int32 count;
    constructor->GetParamCount(&count);
    if (count == 0) {
        ECode ec = constructor->CreateObject(NULL, (IInterface**)&test);
        if (FAILED(ec)) {
            StringBuilder sb("Cannot instantiate test case: ");
            sb.Append(name);
            return Warning(sb.ToString());
        }
        if (ITestCase::Probe(test) != NULL) {
            ITestCase::Probe(test)->SetName(name);
        }
    }
    else {
        AutoPtr<IArgumentList> args;
        constructor->CreateArgumentList((IArgumentList**)&args);
        args->SetInputArgumentOfString(0, name);
        ECode ec = constructor->CreateObject(args, (IInterface**)&test);
        if (FAILED(ec)) {
            StringBuilder sb("Cannot instantiate test case: ");
            sb.Append(name);
            return Warning(sb.ToString());
        }
    }
    // } catch (InstantiationException e) {
    //     return(warning("Cannot instantiate test case: "+name+" ("+exceptionToString(e)+")"));
    // } catch (InvocationTargetException e) {
    //     return(warning("Exception in constructor: "+name+" ("+exceptionToString(e.getTargetException())+")"));
    // } catch (IllegalAccessException e) {
    //     return(warning("Cannot access test case: "+name+" ("+exceptionToString(e)+")"));
    // }
    return ITest::Probe(test);
}

ECode CTestSuite::GetTestConstructor(
    /* [in] */ IClassInfo* theClass,
    /* [out] */ IConstructorInfo** constructor)
{
    VALIDATE_NOT_NULL(constructor);
    Int32 count;
    theClass->GetConstructorCount(&count);
    AutoPtr< ArrayOf<IConstructorInfo*> > constructors = ArrayOf<IConstructorInfo*>::Alloc(count);
    theClass->GetAllConstructorInfos(constructors);
    AutoPtr<IConstructorInfo> ctor0, ctor1;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IConstructorInfo> ctor = (*constructors)[i];
        Int32 paramCount;
        ctor->GetParamCount(&paramCount);
        if (paramCount == 0) ctor0 = ctor;
        else if (paramCount == 1) {
            AutoPtr<IParamInfo> param;
            ctor->GetParamInfoByIndex(0, (IParamInfo**)&param);
            AutoPtr<IDataTypeInfo> typeInfo;
            param->GetTypeInfo((IDataTypeInfo**)&typeInfo);
            CarDataType type;
            typeInfo->GetDataType(&type);
            if (type == CarDataType_String) {
                ctor1 = ctor;
                break;
            }
        }
    }

    if (ctor1 != NULL) {
        *constructor = ctor1;
        REFCOUNT_ADD(*constructor);
        return NOERROR;
    }
    if (ctor0 != NULL) {
        *constructor = ctor0;
        REFCOUNT_ADD(*constructor);
        return NOERROR;
    }
    return E_NO_SUCH_METHOD_EXCEPTION;
}

AutoPtr<ITest> CTestSuite::Warning(
    /* [in] */ const String& message)
{
    AutoPtr<ITest> test = new MyTestCase(message);
    return test;
}

String CTestSuite::ExceptionToString(
    /* [in] */ ECode ec)
{
    // StringWriter stringWriter= new StringWriter();
    // PrintWriter writer= new PrintWriter(stringWriter);
    // t.printStackTrace(writer);
    // return stringWriter.toString();
    assert(0);
    return String(NULL);
}

ECode CTestSuite::constructor()
{
    return NOERROR;
}

ECode CTestSuite::constructor(
    /* [in] */ IClassInfo* theClass)
{
    AddTestsFromTestCase(theClass);
    return NOERROR;
}

void CTestSuite::AddTestsFromTestCase(
    /* [in] */ IClassInfo* theClass)
{
    theClass->GetName(&mName);
    // try {
    AutoPtr<IConstructorInfo> constructor;
    ECode ec = GetTestConstructor(theClass, (IConstructorInfo**)&constructor); // Avoid generating multiple error messages
    if (FAILED(ec)) {
        StringBuilder sb("Class ");
        sb.Append(mName);
        sb.Append(" has no public constructor TestCase(String name) or TestCase()");
        AutoPtr<ITest> test = Warning(sb.ToString());
        AddTest(test);
        return;
    }
    // } catch (NoSuchMethodException e) {
    //     addTest(warning("Class "+theClass.getName()+" has no public constructor TestCase(String name) or TestCase()"));
    //     return;
    // }

    // if (!Modifier.isPublic(theClass.getModifiers())) {
    //     addTest(warning("Class "+theClass.getName()+" is not public"));
    //     return;
    // }

    List<String> names;
    AutoPtr<IInterfaceInfo> itfInfo;
    ec = theClass->GetInterfaceInfo(String("ITest"), (IInterfaceInfo**)&itfInfo);
    if (SUCCEEDED(ec) && itfInfo != NULL) {
        Int32 count;
        theClass->GetMethodCount(&count);
        AutoPtr< ArrayOf<IMethodInfo*> > methodInfos = ArrayOf<IMethodInfo*>::Alloc(count);
        theClass->GetAllMethodInfos(methodInfos);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IMethodInfo> method = (*methodInfos)[i];
            AddTestMethod(method, names, theClass);
        }
    }
    // Class<?> superClass= theClass;
    // List<String> names= new ArrayList<String>();
    // while (Test.class.isAssignableFrom(superClass)) {
    //     for (Method each : superClass.getDeclaredMethods())
    //         addTestMethod(each, names, theClass);
    //     superClass= superClass.getSuperclass();
    // }
    Int32 size;
    if (mTests->GetSize(&size), size == 0) {
        AutoPtr<ITest> test = Warning(String("No tests found in ") + mName);
        AddTest(test);
    }
}

ECode CTestSuite::constructor(
    /* [in] */ IClassInfo* theClass,
    /* [in] */ const String& name)
{
    constructor(theClass);
    return SetName(name);
}

ECode CTestSuite::constructor(
    /* [in] */ const String& name)
{
    return SetName(name);
}

ECode CTestSuite::constructor(
    /* [in] */ ArrayOf<IClassInfo*>* classes)
{
    if (classes != NULL) {
        for (Int32 i = 0; i < classes->GetLength(); i++) {
            AutoPtr<IClassInfo> each = (*classes)[i];
            AutoPtr<ITest> test = TestCaseForClass(each);
            AddTest(test);
        }
    }
    return NOERROR;
}

AutoPtr<ITest> CTestSuite::TestCaseForClass(
    /* [in] */ IClassInfo* each)
{
    AutoPtr<IInterfaceInfo> itfInfo;
    each->GetInterfaceInfo(String("ITestCase"), (IInterfaceInfo**)&itfInfo);
    if (itfInfo != NULL) {
        AutoPtr<ITest> test;
        CTestSuite::New(each, (ITest**)&test);
        return test;
    }
    else {
        String name;
        each->GetName(&name);
        return Warning(name + " does not extend TestCase");
    }
}

ECode CTestSuite::constructor(
    /* [in] */ ArrayOf<IClassInfo*>* classes,
    /* [in] */ const String& name)
{
    constructor(classes);
    return SetName(name);
}

ECode CTestSuite::Initialize()
{
    return NOERROR;
}

PInterface CTestSuite::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ITest*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    else if (riid == EIID_ITest) {
        return (ITest*)this;
    }
    else if (riid == EIID_ITestSuite) {
        return (ITestSuite*)this;
    }
    return NULL;
}

UInt32 CTestSuite::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CTestSuite::Release()
{
    return ElRefBase::Release();
}

ECode CTestSuite::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(ITest *)this) {
        *pIID = EIID_ITest;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(ITestSuite *)this) {
        *pIID = EIID_ITestSuite;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode CTestSuite::Aggregate(
    /* [in] */ AggrType aggrType,
    /* [in] */ PInterface pObject)
{
    assert(0);
    return NOERROR;
}

ECode CTestSuite::GetDomain(
    /* [out] */ PInterface *ppObject)
{
    assert(0);
    return NOERROR;
}

ECode CTestSuite::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    VALIDATE_NOT_NULL(pCLSID);
    *pCLSID = ECLSID_CTestSuite;
    return NOERROR;
}

ECode CTestSuite::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    assert(0);
    return NOERROR;
}

ECode CTestSuite::GetHashCode(
    /* [out] */ Int32* hash)
{
    assert(0);
    return NOERROR;
}

ECode CTestSuite::AddTest(
    /* [in] */ ITest* test)
{
    Boolean result;
    mTests->Add(test, &result);
    return NOERROR;
}

ECode CTestSuite::AddTestSuite(
    /* [in] */ IClassInfo* testClass)
{
    AutoPtr<ITest> test;
    CTestSuite::New(testClass, (ITest**)&test);
    return AddTest(test);
}

ECode CTestSuite::CountTestCases(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    Int32 count = 0;
    Int32 N;
    mTests->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ITest> each;
        mTests->ElementAt(i, (IInterface**)&each);
        Int32 n;
        each->CountTestCases(&n);
        count += n;
    }
    *number = count;
    return NOERROR;
}

ECode CTestSuite::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CTestSuite::Run(
    /* [in] */ ITestResult* result)
{
    Int32 N;
    mTests->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ITest> each;
        mTests->ElementAt(i, (IInterface**)&each);
        Boolean shouldStop;
        if (result->ShouldStop(&shouldStop), shouldStop) {
            break;
        }
        FAIL_RETURN(RunTest(each, result));
    }
    return NOERROR;
}

ECode CTestSuite::RunTest(
    /* [in] */ ITest* test,
    /* [in] */ ITestResult* result)
{
    return test->Run(result);
}

ECode CTestSuite::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CTestSuite::TestAt(
    /* [in] */ Int32 index,
    /* [out] */ ITest** test)
{
    VALIDATE_NOT_NULL(test);
    AutoPtr<IInterface> elem;
    mTests->Get(index, (IInterface**)&elem);
    *test = ITest::Probe(elem);
    REFCOUNT_ADD(*test);
    return NOERROR;
}

ECode CTestSuite::TestCount(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    mTests->GetSize(number);
    return NOERROR;
}

ECode CTestSuite::Tests(
    /* [out] */ IEnumeration** tests)
{
    VALIDATE_NOT_NULL(tests);
    return mTests->GetElements(tests);
}

ECode CTestSuite::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    if (GetName(str), !str->IsNull()) return NOERROR;
    *str = "CTestSuite";
    return NOERROR;
    // return super.toString();
}

void CTestSuite::AddTestMethod(
    /* [in] */ IMethodInfo* m,
    /* [in] */ List<String>& names,
    /* [in] */ IClassInfo* theClass)
{
    String name;
    m->GetName(&name);
    if (Find(names.Begin(), names.End(), name) != names.End()) {
        return;
    }
    // if (! isPublicTestMethod(m)) {
    //     if (isTestMethod(m))
    //         addTest(warning("Test method isn't public: "+ m.getName() + "(" + theClass.getCanonicalName() + ")"));
    //     return;
    // }
    names.PushBack(name);
    AutoPtr<ITest> test = CreateTest(theClass, name);
    AddTest(test);
}

Boolean CTestSuite::IsPublicTestMethod(
    /* [in] */ IMethodInfo* m)
{
    // return isTestMethod(m) && Modifier.isPublic(m.getModifiers());
    return FALSE;
}

Boolean CTestSuite::IsTestMethod(
    /* [in] */ IMethodInfo* m)
{
    // return
    //         m.getParameterTypes().length == 0 &&
    //         m.getName().startsWith("test") &&
    //         m.getReturnType().equals(Void.TYPE);
    return FALSE;
}

ECode CTestSuite::GetTestAnnotation(
    /* [out] */ ITestAnnotation** annotation)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CTestSuite::IsFailed(
    /* [out] */ Boolean* result)
{
    assert(0);
    return NOERROR;
}

ECode CTestSuite::AddFailure(
    /* [in] */ ECode ec,
    /* [in] */ const String& message,
    /* [in] */ const String& backtrace)
{
    assert(0);
    return NOERROR;
}

ECode CTestSuite::GetFailures(
    /* [out, callee] */ ArrayOf<ITestFailure*>** failures)
{
    assert(0);
    return NOERROR;
}

} // namespace Framework
} // namespace Eunit
