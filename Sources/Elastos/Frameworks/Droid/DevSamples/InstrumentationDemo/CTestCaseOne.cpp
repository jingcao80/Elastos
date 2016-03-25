
#include "CTestCaseOne.h"
#include <elastos/core/StringBuilder.h>

#include <cutils/log.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::StringBuilder;
using Elastos::Core::CInteger32;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContacts;
using Elastos::Droid::Provider::IContactsPeople;
using Elastos::Droid::Provider::CContactsPeople;
using Elastos::Droid::Provider::IContactsGroups;
using Elastos::Droid::Provider::CContactsGroups;
using Elastos::Droid::Provider::IContactsPeopleColumns;
using Elastos::Droid::Provider::IContactsGroupsColumns;
using Eunit::Framework::EIID_ITest;
using Eunit::Framework::EIID_ITestCase;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace InstrumentationDemo {

AutoPtr< ArrayOf<String> > CTestCaseOne::PEOPLE_PROJECTION;
const Int32 CTestCaseOne::PEOPLE_ID_INDEX = 0;
const Int32 CTestCaseOne::PEOPLE_LAST_CONTACTED_INDEX = 1;

const Int32 CTestCaseOne::MEMBERSHIP_PERSON_ID_INDEX = 1;
const Int32 CTestCaseOne::MEMBERSHIP_GROUP_ID_INDEX = 5;

AutoPtr< ArrayOf<String> > CTestCaseOne::GROUPS_PROJECTION;
const Int32 CTestCaseOne::GROUPS_ID_INDEX = 0;
const Int32 CTestCaseOne::GROUPS_NAME_INDEX = 1;

AutoPtr<ICharSequence> CTestCaseOne::StringToICharSequence(
    /* [in] */ const String& str)
{
    AutoPtr<ICharSequence> csq;
    CStringWrapper::New(str, (ICharSequence**)&csq);
    return csq;
}

AutoPtr<IInteger32> CTestCaseOne::Int32ToIInteger32(
    /* [in] */ Int32 value)
{
    AutoPtr<IInteger32> obj;
    CInteger32::New(value, (IInteger32**)&obj);
    return obj;
}

PInterface CTestCaseOne::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    else if (riid == EIID_IMyTestCase) {
        return this;
    }
    return InstrumentationTestCase::Probe(riid);
}

UInt32 CTestCaseOne::AddRef()
{
    return InstrumentationTestCase::AddRef();
}

UInt32 CTestCaseOne::Release()
{
    return InstrumentationTestCase::Release();
}

ECode CTestCaseOne::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    assert(0);
    return E_INVALID_ARGUMENT;
}

ECode CTestCaseOne::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    VALIDATE_NOT_NULL(pCLSID);
    *pCLSID = ECLSID_CTestCaseOne;
    return NOERROR;
}

ECode CTestCaseOne::GetTestAnnotation(
    /* [out] */ ITestAnnotation** annotation)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCaseOne::SetUp()
{
    ALOGD("==== File: %s, Line: %d ====", __FILE__, __LINE__);
    if (PEOPLE_PROJECTION == NULL) {
        PEOPLE_PROJECTION = ArrayOf<String>::Alloc(2);
        (*PEOPLE_PROJECTION)[0] = IBaseColumns::ID;
        (*PEOPLE_PROJECTION)[1] = IContactsPeopleColumns::LAST_TIME_CONTACTED;
    }

    if (GROUPS_PROJECTION == NULL) {
        GROUPS_PROJECTION = ArrayOf<String>::Alloc(2);
        (*GROUPS_PROJECTION)[0] = IBaseColumns::ID;
        (*GROUPS_PROJECTION)[1] = IContactsGroupsColumns::NAME;
    }

    InstrumentationTestCase::SetUp();
    AutoPtr<IInstrumentation> instrumentation;
    GetInstrumentation((IInstrumentation**)&instrumentation);
    AutoPtr<IContext> targetContext;
    instrumentation->GetTargetContext((IContext**)&targetContext);
    targetContext->GetContentResolver((IContentResolver**)&mContentResolver);
    mContentResolver->AcquireContentProviderClient(IContacts::AUTHORITY,
            (IContentProviderClient**)&mProvider);

    CArrayList::New((IArrayList**)&mPeopleRowsAdded);
    CArrayList::New((IArrayList**)&mGroupRowsAdded);
    CArrayList::New((IArrayList**)&mRowsAdded);

    ECode ec;

    for (Int32 i = 0; i < 3; i++) {
        AutoPtr<IContentValues> value;
        CContentValues::New((IContentValues**)&value);

        StringBuilder builder("test_people_");
        builder += i;
        String str = builder.ToString();
        value->PutString(IContactsPeopleColumns::NAME,
                StringToICharSequence(str));
        value->PutInt32(IContactsPeopleColumns::TIMES_CONTACTED,
                Int32ToIInteger32(0));
        value->PutInt32(IContactsPeopleColumns::LAST_TIME_CONTACTED,
                Int32ToIInteger32(0));

        AutoPtr<IContactsPeople> people;
        CContactsPeople::AcquireSingleton((IContactsPeople**)&people);
        AutoPtr<IUri> baseUriPeople;
        people->GetCONTENT_URI((IUri**)&baseUriPeople);

        AutoPtr<IUri> insertedUri;
        ec = mProvider->Insert(baseUriPeople, value, (IUri**)&insertedUri);
        assert(SUCCEEDED(ec));
        AutoPtr<IInterface> object = (IInterface*)insertedUri;
        Boolean result;
        mPeopleRowsAdded->Add(object, &result);
    }

    AutoPtr<IContentValues> value;
    CContentValues::New((IContentValues**)&value);
    value->PutString(IContactsGroupsColumns::NAME, StringToICharSequence(String("test_group_0")));

    AutoPtr<IContactsGroups> group;
    CContactsGroups::AcquireSingleton((IContactsGroups**)&group);
    AutoPtr<IUri> baseUriGroup;
    group->GetCONTENT_URI((IUri**)&baseUriGroup);

    AutoPtr<IUri> insertedUri;
    ec = mProvider->Insert(baseUriGroup, value, (IUri**)&insertedUri);
    assert(SUCCEEDED(ec));
    AutoPtr<IInterface> object = (IInterface*)insertedUri;
    Boolean res;
    mGroupRowsAdded->Add(object, &res);

    value->PutString(IContactsGroupsColumns::NAME, StringToICharSequence(String("test_group_1")));
    insertedUri = NULL;
    ec = mProvider->Insert(baseUriGroup, value, (IUri**)&insertedUri);
    assert(SUCCEEDED(ec));
    object = (IInterface*)insertedUri;
    mGroupRowsAdded->Add(object, &res);

    return NOERROR;
}

ECode CTestCaseOne::TearDown()
{
    ALOGD("==== File: %s, Line: %d ====", __FILE__, __LINE__);
    // remove the lines we inserted in setup and added in test cases.
    Int32 N;
    mRowsAdded->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mRowsAdded->Get(i, (IInterface**)&obj);
        AutoPtr<IUri> row = IUri::Probe(obj);
        Int32 rowsAffected;
        mProvider->Delete(row, String(NULL), NULL, &rowsAffected);
    }
    mRowsAdded->Clear();

    mPeopleRowsAdded->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mPeopleRowsAdded->Get(i, (IInterface**)&obj);
        AutoPtr<IUri> row = IUri::Probe(obj);
        Int32 rowsAffected;
        mProvider->Delete(row, String(NULL), NULL, &rowsAffected);
    }
    mPeopleRowsAdded->Clear();

    mGroupRowsAdded->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mGroupRowsAdded->Get(i, (IInterface**)&obj);
        AutoPtr<IUri> row = IUri::Probe(obj);
        Int32 rowsAffected;
        mProvider->Delete(row, String(NULL), NULL, &rowsAffected);
    }
    mGroupRowsAdded->Clear();

    return InstrumentationTestCase::TearDown();
}

ECode CTestCaseOne::TestAddToGroup()
{
    ALOGD("==== File: %s, Line: %d ====", __FILE__, __LINE__);
    AutoPtr<ICursor> cursor;

    // Add the My Contacts group, since it is no longer automatically created.
    AutoPtr<IContentValues> testValues;
    CContentValues::New((IContentValues**)&testValues);
    testValues->PutString(IContactsGroupsColumns::SYSTEM_ID,
            StringToICharSequence(IContactsGroups::GROUP_MY_CONTACTS));

    AutoPtr<IContactsGroups> group;
    CContactsGroups::AcquireSingleton((IContactsGroups**)&group);
    AutoPtr<IUri> uriGroup;
    group->GetCONTENT_URI((IUri**)&uriGroup);

    AutoPtr<IUri> insertedUri;
    ECode ec = mProvider->Insert(uriGroup, testValues, (IUri**)&insertedUri);
    assert(SUCCEEDED(ec));

    // People: test_people_0, Group: Groups.GROUP_MY_CONTACTS
    AutoPtr<IInterface> object;
    mPeopleRowsAdded->Get(0, (IInterface**)&object);
    AutoPtr<IUri> uri = IUri::Probe(object);
    ec = mProvider->Query(uri, PEOPLE_PROJECTION, String(NULL), NULL, String(NULL), NULL, (ICursor**)&cursor);
    assert(SUCCEEDED(ec));
    Boolean succeeded;
    ec = cursor->MoveToFirst(&succeeded);
    assert(succeeded);
    Int32 personId;
    cursor->GetInt32(PEOPLE_ID_INDEX, &personId);
    cursor->Close();

    AutoPtr<IContactsPeople> people;
    CContactsPeople::AcquireSingleton((IContactsPeople**)&people);
    AutoPtr<IUri> uriPeople;
    people->GetCONTENT_URI((IUri**)&uriPeople);

    uri = NULL;
    ec = people->AddToMyContactsGroup(mContentResolver, personId, (IUri**)&uri);
    assert(SUCCEEDED(ec));
    object = (IInterface*)uri;
    Boolean modified;
    mRowsAdded->Add(object, &modified);
    cursor = NULL;
    ec = mProvider->Query(uriGroup, GROUPS_PROJECTION,
            IContactsGroupsColumns::SYSTEM_ID + String("='") + IContactsGroups::GROUP_MY_CONTACTS + String("'"),
            NULL, String(NULL), NULL, (ICursor**)&cursor);
    assert(SUCCEEDED(ec));
    ec = cursor->MoveToFirst(&succeeded);
    assert(succeeded);
    Int32 groupId;
    cursor->GetInt32(GROUPS_ID_INDEX, &groupId);
    cursor->Close();

    cursor = NULL;
    ec = people->QueryGroups(mContentResolver, personId, (ICursor**)&cursor);
    assert(SUCCEEDED(ec));
    ec = cursor->MoveToFirst(&succeeded);
    assert(succeeded);
    Int32 p1, g1;
    cursor->GetInt32(MEMBERSHIP_PERSON_ID_INDEX, &p1);
    cursor->GetInt32(MEMBERSHIP_GROUP_ID_INDEX, &g1);
    assert(personId == p1);
    assert(groupId == g1);
    cursor->Close();


    // People: test_people_create, Group: Groups.GROUP_MY_CONTACTS
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->PutString(IContactsPeopleColumns::NAME,
            StringToICharSequence(String("test_people_create")));
    values->PutInt32(IContactsPeopleColumns::TIMES_CONTACTED,
            Int32ToIInteger32(0));
    values->PutInt32(IContactsPeopleColumns::LAST_TIME_CONTACTED,
            Int32ToIInteger32(0));
    uri = NULL;
    people->CreatePersonInMyContactsGroup(mContentResolver, values, (IUri**)&uri);
    object = (IInterface*)uri;
    mRowsAdded->Add(object, &modified);
    cursor = NULL;
    ec = mProvider->Query(uriPeople, PEOPLE_PROJECTION,
            IContactsPeopleColumns::NAME + String(" = 'test_people_create'"), NULL, String(NULL), NULL, (ICursor**)&cursor);
    assert(SUCCEEDED(ec));

    cursor->MoveToFirst(&succeeded);
    cursor->GetInt32(PEOPLE_ID_INDEX, &personId);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<IUri> newUri;
    contentUris->WithAppendedId(uriPeople, personId, (IUri**)&newUri);
    object = (IInterface*)newUri;
    mRowsAdded->Add(object, &modified);
    cursor->Close();

    cursor = NULL;
    mProvider->Query(uriGroup, GROUPS_PROJECTION,
            IContactsGroupsColumns::SYSTEM_ID + String("='") + IContactsGroups::GROUP_MY_CONTACTS + String("'"),
            NULL, String(NULL), NULL, (ICursor**)&cursor);
    cursor->MoveToFirst(&succeeded);
    cursor->GetInt32(GROUPS_ID_INDEX, &groupId);
    cursor->Close();

    cursor = NULL;
    people->QueryGroups(mContentResolver, personId, (ICursor**)&cursor);
    cursor->MoveToFirst(&succeeded);
    cursor->GetInt32(MEMBERSHIP_PERSON_ID_INDEX, &p1);
    cursor->GetInt32(MEMBERSHIP_GROUP_ID_INDEX, &g1);
    assert(personId == p1);
    assert(groupId == g1);
    cursor->Close();

    // People: test_people_1, Group: test_group_0
    object = NULL;
    mPeopleRowsAdded->Get(1, (IInterface**)&object);
    uri = IUri::Probe(object);
    cursor = NULL;
    mProvider->Query(uri, PEOPLE_PROJECTION, String(NULL), NULL, String(NULL), NULL, (ICursor**)&cursor);
    cursor->MoveToFirst(&succeeded);
    cursor->GetInt32(PEOPLE_ID_INDEX, &personId);
    cursor->Close();

    object = NULL;
    mGroupRowsAdded->Get(0, (IInterface**)&object);
    uri = IUri::Probe(object);
    cursor = NULL;
    mProvider->Query(uri, GROUPS_PROJECTION, String(NULL), NULL, String(NULL), NULL, (ICursor**)&cursor);
    cursor->MoveToFirst(&succeeded);
    cursor->GetInt32(GROUPS_ID_INDEX, &groupId);
    cursor->Close();

    uri = NULL;
    people->AddToGroup(mContentResolver, personId, groupId, (IUri**)&uri);
    object = (IInterface*)uri;
    mRowsAdded->Add(object, &modified);
    cursor = NULL;
    people->QueryGroups(mContentResolver, personId, (ICursor**)&cursor);
    Boolean found = FALSE;
    Boolean result = FALSE;
    while (cursor->MoveToNext(&result), result) {
        Int32 i;
        cursor->GetInt32(MEMBERSHIP_PERSON_ID_INDEX, &i);
        assert(personId == i);
        Int32 j;
        cursor->GetInt32(MEMBERSHIP_GROUP_ID_INDEX, &j);
        if (j == groupId) {
            found = TRUE;
            break;
        }
    }
    assert(found);

    cursor->Close();

    // People: test_people_2, Group: test_group_1
    object = NULL;
    mPeopleRowsAdded->Get(2, (IInterface**)&object);
    uri = IUri::Probe(object);
    cursor = NULL;
    mProvider->Query(uri, PEOPLE_PROJECTION, String(NULL), NULL, String(NULL), NULL, (ICursor**)&cursor);
    cursor->MoveToFirst(&succeeded);
    cursor->GetInt32(PEOPLE_ID_INDEX, &personId);
    cursor->Close();
    uri = NULL;
    String groupName("test_group_1");
    people->AddToGroup(mContentResolver, personId, groupName, (IUri**)&uri);
    object = (IInterface*)uri;
    mRowsAdded->Add(object, &modified);
    cursor = NULL;
    people->QueryGroups(mContentResolver, personId, (ICursor**)&cursor);
    AutoPtr<IList> groupIds;
    CArrayList::New((IArrayList**)&groupIds);
    while (cursor->MoveToNext(&result), result) {
        Int32 i;
        cursor->GetInt32(MEMBERSHIP_PERSON_ID_INDEX, &i);
        assert(personId == i);
        Int32 j;
        cursor->GetInt32(MEMBERSHIP_GROUP_ID_INDEX, &j);
        AutoPtr<IInteger32> obj;
        CInteger32::New(j, (IInteger32**)&obj);
        Boolean res;
        groupIds->Add(obj, &res);
    }
    cursor->Close();

    found = FALSE;
    Int32 size;
    groupIds->GetSize(&size);
    for (Int32 i = 0 ; i < size; i++) {
        AutoPtr<IInterface> object;
        groupIds->Get(i, (IInterface**)&object);
        Int32 id;
        if (IInteger32::Probe(object) == NULL) {
            assert(0);
        } else {
            IInteger32::Probe(object)->GetValue(&id);
        }

        StringBuilder builder;
        builder += IBaseColumns::ID;
        builder += String("=");
        builder += id;
        String str = builder.ToString();
        cursor = NULL;
        mProvider->Query(uriGroup, GROUPS_PROJECTION, str, NULL, String(NULL), NULL, (ICursor**)&cursor);
        Boolean succeed;
        cursor->MoveToFirst(&succeed);
        String columnValue;
        cursor->GetString(GROUPS_NAME_INDEX, &columnValue);
        if (groupName.Equals(columnValue)) {
            found = TRUE;
            break;
        }
    }
    assert(found);
    cursor->Close();

    return NOERROR;
}

}
}
}
}

