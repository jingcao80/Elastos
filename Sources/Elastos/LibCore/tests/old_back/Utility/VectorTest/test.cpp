#include "test.h"

using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::CVector;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Arrays;

CTest::CTest()
:vString("[Test 0, Test 1, Test 2, Test 3, Test 4, Test 5, Test 6, Test 7, Test 8, Test 9, Test 10, Test 11, Test 12, Test 13, Test 14, Test 15, Test 16, Test 17, Test 18, Test 19, Test 20, Test 21, Test 22, Test 23, Test 24, Test 25, Test 26, Test 27, Test 28, Test 29, Test 30, Test 31, Test 32, Test 33, Test 34, Test 35, Test 36, Test 37, Test 38, Test 39, Test 40, Test 41, Test 42, Test 43, Test 44, Test 45, Test 46, Test 47, Test 48, Test 49, Test 50, Test 51, Test 52, Test 53, Test 54, Test 55, Test 56, Test 57, Test 58, Test 59, Test 60, Test 61, Test 62, Test 63, Test 64, Test 65, Test 66, Test 67, Test 68, Test 69, Test 70, Test 71, Test 72, Test 73, Test 74, Test 75, Test 76, Test 77, Test 78, Test 79, Test 80, Test 81, Test 82, Test 83, Test 84, Test 85, Test 86, Test 87, Test 88, Test 89, Test 90, Test 91, Test 92, Test 93, Test 94, Test 95, Test 96, Test 97, Test 98, Test 99]")
{
	setUp();
}

CTest::~CTest() {
	tearDown();
}

void CTest::setUp() {
	CVector::New((IVector**)&tVector);
	objArray = ArrayOf<IInterface*>::Alloc(100);
	AutoPtr<ICharSequence> cs;
	String str;
	Char32 c;
	for (Int32 i = 0; i < 100; ++i) {
		str = String("Test ");
		Char32 c;
		if (i >= 0 && i < 10) {
	        c = 48 + i;
	        str += c;
	    } else {
	        Int32 iVal = i / 10;
	        c = 48 + iVal;
	        str += c;
	        iVal = i % 10;
	        c = 48 + iVal;
	        str += c;
	    }
	    CString::New(str, (ICharSequence**)&cs);
		tVector->AddElement(cs);
		objArray->Set(i, cs);
	}
}

void CTest::tearDown() {}

//Num: Test1
int CTest::test_Constructor(int argc, char* argv[]) {
	AutoPtr<IVector> tv;
	CVector::New((IVector**)&tv);
	Int32 size;
	tv->GetSize(&size);
	assert(0 == size);

	Int32 capacity;
	tv->Capacity(&capacity);
	assert(10 == capacity);
}

//Num: Test2
int CTest::test_ConstructorI(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New(100, (IVector**)&v);
	Int32 size;
	v->GetSize(&size);
	assert(0 == size);

	Int32 capacity;
	v->Capacity(&capacity);
	assert(100 == capacity);

	ECode ec = CVector::New(-1, (IVector**)&v);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num: Test3
int CTest::test_ConstructorII(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New(2, 10, (IVector**)&v);
	AutoPtr<IVector> object1;
	CVector::New((IVector**)&object1);
	AutoPtr<IVector> object2;
	CVector::New((IVector**)&object2);
	AutoPtr<IVector> object3;
	CVector::New((IVector**)&object3);
	v->AddElement(object1);
	v->AddElement(object2);
	v->AddElement(object3);
	Int32 capacity;
	v->Capacity(&capacity);
	assert(12 == capacity);

	AutoPtr<IVector> grow;
	CVector::New(3, -1, (IVector**)&grow);
	AutoPtr<ICharSequence> cs;
	String str = String("one");
	CString::New(str, (ICharSequence**)&cs);
	grow->AddElement(cs);
	str = String("two");
	CString::New(str, (ICharSequence**)&cs);
	grow->AddElement(cs);
	str  = String("three");
	grow->AddElement(cs);
	str = String("four");
	grow->AddElement(cs);

	Int32 size;
	grow->GetSize(&size);
	assert(4 == size);

	grow->Capacity(&capacity);
	assert(6 == capacity);

	ECode ec = CVector::New(-1, 1, (IVector**)&v);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num: Test4
int CTest::test_ConstructorLjava_util_Collection(int argc, char* argv[]) {
	AutoPtr<Elastos::Utility::ICollection> l;
	AutoPtr<ILinkedList> list;
	CLinkedList::New((ILinkedList**)&list);
	l = Elastos::Utility::ICollection::Probe(list);
	String str;
	AutoPtr<ICharSequence> cs;
	for (int i = 0; i < 100; ++i) {
		str = String("Test") + i;
		CString::New(str, (ICharSequence**)&cs);
		Boolean flag = FALSE;
		l->Add(cs, &flag);
	}
	AutoPtr<Elastos::Utility::IVector> myVector;
	CVector::New(l, (IVector**)&myVector);
	Int32 size;
	myVector->GetSize(&size);
	Int32 length = objArray->GetLength();
	assert(size == length);

	for (int counter = 0; counter < length; ++counter) {
		AutoPtr<IList> list = IList::Probe(l);
		AutoPtr<IInterface> out;
		list->Get(counter, (IInterface**)&out);
		Boolean flag = FALSE;
		myVector->Contains(out, &flag);
		assert(flag);
	}

	AutoPtr<Elastos::Utility::ICollection> last;
	ECode ec = CVector::New(last, (IVector**)&myVector);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num:Test5
int CTest::test_addILjava_lang_Object(int argc, char* argv[]) {
	AutoPtr<IInterface> o;
	AutoPtr<IInterface> prev;
	tVector->Get(45, (IInterface**)&prev);
	tVector->AddEx(45, o);
	AutoPtr<IInterface> ap;
	tVector->Get(45, (IInterface**)&ap);
	assert(ap == o);
	AutoPtr<IInterface> ap1;
	tVector->Get(46, (IInterface**)&ap1);
	assert(ap1 == prev);
	Int32 size;
	tVector->GetSize(&size);
	assert(101 == size);

	tVector->Get(50, (IInterface**)&prev);
	tVector->AddEx(50, NULL);
	AutoPtr<IInterface> ap2;
	tVector->Get(50, (IInterface**)&ap2);
	assert(NULL == ap2);

	AutoPtr<IInterface> ap3;
	tVector->Get(51, (IInterface**)&ap3);
	assert(ap3 == prev);

	tVector->GetSize(&size);
	assert(102 == size);

	ECode ec = tVector->AddEx(-5, NULL);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test6
int CTest::test_addLjava_lang_Object(int argc, char* argv[]) {
	AutoPtr<IInterface> o;
	Boolean flag = FALSE;
	tVector->Add(o, &flag);
	AutoPtr<IInterface> last;
	tVector->LastElement((IInterface**)&last);
	assert(o == last);
	Int32 size;
	tVector->GetSize(&size);
	assert(101 == size);

	tVector->Add(NULL, &flag);
	tVector->LastElement((IInterface**)&last);
	assert(NULL == last);

	tVector->GetSize(&size);
	assert(102 == size);
}

//Num:Test7
int CTest::test_addAllILjava_util_Collection(int argc, char* argv[]) {
	AutoPtr<ICollection> l;
	AutoPtr<ILinkedList> lkst;
	CLinkedList::New((ILinkedList**)&lkst);
	l = ICollection::Probe(lkst);
	String str;
	Boolean flag = FALSE;
	for (int i = 0; i < 100; ++i) {
		str = String("Test") + i;
		AutoPtr<ICharSequence> cs;
		CString::New(str, (ICharSequence**)&cs);
		l->Add(cs, &flag);
	}
	AutoPtr<IVector> v;
	CVector::New((IVector**)&v);
	tVector->AddAllEx(50, l, &flag);
	AutoPtr<IInterface> ap;
	AutoPtr<IInterface> ap1;
	for (int i = 50; i < 100; ++i) {
		AutoPtr<IList> ls = IList::Probe(l);
		ls->Get(i-50, (IInterface**)&ap);
		tVector->Get(i, (IInterface**)&ap1);
		assert(ap == ap1);
	}
	CVector::New((IVector**)&v);
	String str1 = String("one");
	AutoPtr<ICharSequence> cs;
	CString::New(str1, (ICharSequence**)&cs);
	v->Add(cs, &flag);
	AutoPtr<ILinkedList> lst;
	CLinkedList::New((ILinkedList**)&lst);
	l = ICollection::Probe(lst);
	l->Add(NULL, &flag);
	String str2 = String("gah");
	CString::New(str2, (ICharSequence**)&cs);
	l->Add(cs, &flag);
	l->Add(NULL, &flag);
	tVector->AddAllEx(50, l, &flag);
	tVector->Get(50, (IInterface**)&ap);
	assert(NULL == ap);
	tVector->Get(51, (IInterface**)&ap);
	assert(cs == ap);
	tVector->Get(52, (IInterface**)&ap);
	assert(NULL == ap);

	Int32 size;
	tVector->GetSize(&size);
	AutoPtr<ICollection> last;
	ECode ec = tVector->AddAllEx(size/2, last, &flag);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num:Test8
int CTest::test_addAllLjava_util_Collection(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New((IVector**)&v);
	AutoPtr<ILinkedList> lst;
	CLinkedList::New((ILinkedList**)&lst);
	AutoPtr<ICollection> l = ICollection::Probe(lst);

	AutoPtr<ICharSequence> cs;
	String str;
	Boolean flag = FALSE;
	for (int i = 0; i < 100; ++i) {
		str = String("Test ") + i;
		CString::New(str, (ICharSequence**)&cs);
		l->Add(cs, &flag);
	}
	v->AddAll(l, &flag);
	tVector->Equals(v->Probe(EIID_IInterface), &flag);
	assert(TRUE == flag);

	v->AddAll(l, &flag);
	Int32 vSize;
	tVector->GetSize(&vSize);
	AutoPtr<IInterface> ap;
	AutoPtr<IInterface> ap1;
	for (Int32 counter = vSize - 1; counter >= 0; counter--) {
		v->Get(counter, (IInterface**)&ap);
		v->Get(counter + vSize, (IInterface**)&ap1);
		assert(ap == ap1);
	}

	AutoPtr<ILinkedList> ll;
	CLinkedList::New((ILinkedList**)&ll);
	l = ICollection::Probe(ll);
	l->Add(NULL, &flag);
	String s1 = String("gah");
	CString::New(s1, (ICharSequence**)&cs);
	l->Add(cs, &flag);
	l->Add(NULL, &flag);
	tVector->AddAll(l, &flag);
	tVector->Get(vSize, (IInterface**)&ap);
	assert(NULL == ap);

	tVector->Get(vSize + 1, (IInterface**)&ap);
	assert(cs == ap);
	tVector->Get(vSize + 2, (IInterface**)&ap);
	assert(NULL == ap);

	tVector->GetSize(&vSize);
	AutoPtr<ICollection> last;
	ECode ec = tVector->AddAllEx(vSize / 2, last, &flag);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num:Test9
int CTest::test_addElementLjava_lang_Object(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	String str = String("Added Element");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	v->AddElement(cs);
	Boolean flag = FALSE;
	v->Contains(cs, &flag);
	assert(flag == TRUE);

	AutoPtr<IInterface> inter;
	v->ElementAt(100, (IInterface**)&inter);
	AutoPtr<ICharSequence> cs1;
	cs1 = ICharSequence::Probe(inter);
	String cs1Str;
	cs1->ToString(&cs1Str);
	assert(str.Equals(cs1Str));

	v->AddElement(NULL);
	v->Contains(NULL, &flag);
	assert(TRUE == flag);
	v->ElementAt(101, (IInterface**)&inter);
	assert(NULL == inter);
}

//Num:Test10
int CTest::test_addElementLjava_lang_Object_subtest0(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	String str = String("Added Element");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	v->AddElement(cs);
	Boolean flag = FALSE;
	v->Contains(cs, &flag);
	assert(TRUE == flag);
	AutoPtr<IInterface> ap;
	v->ElementAt(100, (IInterface**)&ap);
	AutoPtr<ICharSequence> cs1;
	cs1 = ICharSequence::Probe(ap);
	String cs1Str;
	cs1->ToString(&cs1Str);
	assert(str.Equals(cs1Str));

	v->AddElement(NULL);
	v->Contains(NULL, &flag);
	assert(TRUE == flag);

	AutoPtr<IInterface> ap1;
	v->ElementAt(101, (IInterface**)&ap1);
	assert(NULL == ap1);
}

//Num:Test11
int CTest::test_capacity(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New(9, (IVector**)&v);
	Int32 capacity;
	v->Capacity(&capacity);
	assert(9 == capacity);
}

//Num:Test12
int CTest::test_clear(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> orgVector = IVector::Probe(obj);
	tVector->Clear();
	Int32 size;
	tVector->GetSize(&size);
	assert(0 == size);

	AutoPtr<IEnumeration> e;
	orgVector->Elements((IEnumeration**)&e);
	Boolean flag = FALSE;
	while((e->HasMoreElements(&flag), flag)) {
		AutoPtr<IInterface> inter;
		e->NextElement((IInterface**)&inter);
		tVector->Contains(inter, &flag);
		assert(FALSE == flag);
	}

	tVector->Add(NULL, &flag);
	tVector->Clear();
	tVector->GetSize(&size);
	assert(0 == size);

	orgVector->Elements((IEnumeration**)&e);
	while((e->HasMoreElements(&flag), flag)) {
		AutoPtr<IInterface> inter;
		e->NextElement((IInterface**)&inter);
		tVector->Contains(inter, &flag);
		assert(FALSE == flag);
	}
}

//Num:Test13
int CTest::test_clone(int argc, char* argv[]) {
	tVector->AddEx(25, NULL);
	tVector->AddEx(75, NULL);
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	AutoPtr<IEnumeration> orgNum;
	tVector->Elements((IEnumeration**)&orgNum);
	AutoPtr<IEnumeration> cnum;
	v->Elements((IEnumeration**)&cnum);

	Boolean flag = FALSE;
	AutoPtr<IInterface> inter;
	AutoPtr<IInterface> inter1;
	while ((orgNum->HasMoreElements(&flag),flag)) {
		cnum->HasMoreElements(&flag);
		assert(TRUE == flag);
		orgNum->NextElement((IInterface**)&inter);
		cnum->NextElement((IInterface**)&inter1);
		assert(inter == inter1);
	}
	cnum->HasMoreElements(&flag);
	assert(flag == FALSE);
}

//Num:Test14
int CTest::test_containsLjava_lang_Object(int argc, char* argv[]) {
	String str = String("Test 42");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	Boolean flag = FALSE;
	tVector->Contains(cs, &flag);
	assert(TRUE == flag);

	str = String("Hello");
	CString::New(str, (ICharSequence**)&cs);
	tVector->Contains(cs, &flag);
	assert(FALSE == flag);

	tVector->Contains(NULL, &flag);
	assert(FALSE == flag);

	tVector->InsertElementAt(NULL, 20);
	tVector->Contains(NULL, &flag);
	assert(TRUE == flag);
}

//Num:Test15
int CTest::test_containsAllLjava_util_Collection(int argc, char* argv[]) {
	AutoPtr<ISet> set;
	CHashSet::New((ISet**)&set);
	AutoPtr<ICollection> s = ICollection::Probe(set);
	Boolean flag = FALSE;
	String str;
	AutoPtr<ICharSequence> cs;
	for (int i = 0; i < 100; ++i) {
		str = String("Test ") + i;
		CString::New(str, (ICharSequence**)&cs);
		s->Add(cs, &flag);
	}

	tVector->ContainsAll(s, &flag);
	assert(TRUE == flag);

/*	s->Add(NULL, &flag);
	tVector->ContainsAll(s, &flag);
	assert(FALSE == flag);

	tVector->AddEx(25, NULL);
	ICollection::Probe(tVector)->ContainsAll(s, &flag);
	assert(TRUE == flag);

	CHashSet::New((ISet**)&set);
	s = ICollection::Probe(set);

	AutoPtr<IInterface> inter;
	s->Add(inter, &flag);
	ICollection::Probe(tVector)->ContainsAll(s, &flag);
	assert(FALSE == flag);

	ECode ec = ICollection::Probe(tVector)->ContainsAll(NULL, &flag);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);*/
}

//Num:Test16
int CTest::test_copyInto_Ljava_lang_Object(int argc, char* argv[]) {
	AutoPtr<ArrayOf<IInterface*> > a = ArrayOf<IInterface*>::Alloc(100);
	tVector->SetElementAt(NULL, 20);
	tVector->CopyInto(a);

	AutoPtr<IInterface> inter;
	for (int i = 0; i < 100; ++i) {
		tVector->ElementAt(i, (IInterface**)&inter);
		assert((*a)[i] == inter);
	}

	ECode ec = tVector->CopyInto(NULL);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num:Test17
int CTest::test_elementAtI(int argc, char* argv[]) {
	AutoPtr<IInterface> inter;
	tVector->ElementAt(18, (IInterface**)&inter);
	AutoPtr<ICharSequence> cs;
	cs = ICharSequence::Probe(inter);
	String str1;
	cs->ToString(&str1);
	AutoPtr<IInterface> inter1;
	String str2 = String("Test 18");
	assert(str2.Equals(str1));

	tVector->SetElementAt(NULL, 20);
	tVector->ElementAt(20, (IInterface**)&inter);
	assert(NULL == inter);
	ECode ec = tVector->ElementAt(-5, (IInterface**)&inter);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test18
int CTest::test_elements(int argc, char* argv[]) {
	tVector->InsertElementAt(NULL, 20);
	AutoPtr<IEnumeration> e;
	tVector->Elements((IEnumeration**)&e);
	Int32 i = 0;
	Boolean flag = FALSE;
	AutoPtr<IInterface> ap;
	AutoPtr<IInterface> ap1;
	while((e->HasMoreElements(&flag), flag)) {
		e->NextElement((IInterface**)&ap);
		tVector->ElementAt(i, (IInterface**)&ap1);
		// assert(ap == ap1);
		i++;
	}
	Int32 size;
	tVector->GetSize(&size);
	assert(i == size);
}


/*int CTest::test_elements_subtest0(int argc, char* argv[]) {
	Int32 iterations = 10000;
	AutoPtr<IVector> v;
	CVector::New((IVector**)&v);

}
*/
//Num:Test19
int CTest::test_ensureCapacityI(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New(9, (IVector**)&v);
	v->EnsureCapacity(20);
	Int32 capacity;
	v->Capacity(&capacity);
	assert(20 == capacity);

	CVector::New(100, (IVector**)&v);
	v->Capacity(&capacity);
	assert(100 == capacity);
}

//Num:Test20
int CTest::test_equalsLjava_lang_Object(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New((IVector**)&v);
	String str;
	AutoPtr<ICharSequence> cs;
	for (int i = 0; i < 100; ++i) {
		str = String("Test ") + i;
		CString::New(str, (ICharSequence**)&cs);
		v->AddElement(cs);
	}

	Boolean flag = FALSE;
	tVector->Equals(v, &flag);
	assert(TRUE == flag);

	v->AddElement(NULL);
	tVector->Equals(v, &flag);
	assert(FALSE == flag);

	v->AddElement(NULL);
	tVector->Equals(v, &flag);
	tVector->RemoveElementAt(22);
	tVector->Equals(v, &flag);
	assert(FALSE == flag);
}

//Num:Test21
int CTest::test_firstElement(int argc, char* argv[]) {
	AutoPtr<IInterface> inter;
	tVector->FirstElement((IInterface**)&inter);
	AutoPtr<ICharSequence> cs;
	cs = ICharSequence::Probe(inter);
	String str;
	cs->ToString(&str);
	assert(str.Equals(String("Test 0")));

	tVector->InsertElementAt(NULL, 0);
	tVector->FirstElement((IInterface**)&inter);
	assert(NULL == inter);

	CVector::New(10, (IVector**)&tVector);
	ECode ec = tVector->FirstElement((IInterface**)&inter);
	assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
}

//Num:Test22
int CTest::test_getI(int argc, char* argv[]) {
	AutoPtr<IInterface> inter;
	tVector->Get(80, (IInterface**)&inter);
	AutoPtr<ICharSequence> cs;
	cs = ICharSequence::Probe(inter);
	String str;
	cs->ToString(&str);
	assert(String("Test 80").Equals(str));
	Boolean flag = FALSE;

	tVector->AddEx(25, NULL);
	tVector->Get(25, (IInterface**)&inter);
	assert(NULL == inter);

	AutoPtr<IInterface> inter1;
	ECode ec = tVector->Get(-5, (IInterface**)&inter1);
	assert(ec ==  E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);

	Int32 size;
	tVector->GetSize(&size);
	ec = tVector->Get(size + 1, (IInterface**)&inter1);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test23
int CTest::test_hashCode(int argc, char* argv[]) {
	Int32 hashCode = 1;
	tVector->InsertElementAt(NULL, 20);
	Int32 size;
	tVector->GetSize(&size);
	for (int i = 0; i < size; ++i) {
		AutoPtr<IInterface> obj;
		tVector->ElementAt(i, (IInterface**)&obj);
		Int32 hashCode1;
		if (obj != NULL) {
			hashCode1 = Object::GetHashCode(obj);
		}else {
			hashCode1 = 0;
		}

		hashCode = 31 * hashCode + hashCode1;
	}

	Int32 value = Object::GetHashCode(tVector);
	assert(value == hashCode);
}

//Num:Test24
int CTest::test_indexOfLjava_lang_Object(int argc, char* argv[]) {
	String str = String("Test 10");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	Int32 index;
	tVector->IndexOf(cs, &index);
	assert(10 == index);

	str = String("XXXXXXXXXXX");
	CString::New(str, (ICharSequence**)&cs);
	tVector->IndexOf(cs, &index);
	assert(-1 == index);

	tVector->SetElementAt(NULL, 20);
	tVector->SetElementAt(NULL, 40);
	tVector->IndexOf(NULL, &index);
	assert(20 == index);
}

//Num:Test25
int CTest::test_indexOfLjava_lang_ObjectI(int argc, char* argv[]) {
	AutoPtr<ICharSequence> cs;
	String str = String("Test 98");
	CString::New(str, (ICharSequence**)&cs);
	Int32 value;
	tVector->IndexOfEx(cs, 50, &value);
	assert(value == 98);

	str = String("Test 1001");
	CString::New(str, (ICharSequence**)&cs);
	tVector->IndexOfEx(cs, 50, &value);
	assert(value == -1);

	tVector->SetElementAt(NULL, 20);
	tVector->SetElementAt(NULL, 40);
	tVector->SetElementAt(NULL, 60);
	tVector->IndexOfEx(NULL, 25, &value);
	assert(value == 40);
	tVector->IndexOfEx(NULL, 20, &value);
	assert(value == 20);

	ECode ec = tVector->IndexOfEx(NULL, -1, &value);
	assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test26
int CTest::test_insertElementAtLjava_lang_ObjectI(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	AutoPtr<IInterface> inter;
	v->ElementAt(99, (IInterface**)&inter);
	AutoPtr<ICharSequence> cs;
	cs = ICharSequence::Probe(inter);
	String preElementAt;
	cs->ToString(&preElementAt);
	String str = String("Inserted Element");
	CString::New(str, (ICharSequence**)&cs);
	v->InsertElementAt(cs, 99);
	v->ElementAt(99, (IInterface**)&inter);
	AutoPtr<ICharSequence> cs1;
	cs1 = ICharSequence::Probe(inter);
	assert(cs == cs1);

	v->ElementAt(100, (IInterface**)&inter);
	cs1 = ICharSequence::Probe(inter);
	cs1->ToString(&str);
	assert(str.Equals(preElementAt));

	v->InsertElementAt(NULL, 20);
	v->ElementAt(20, (IInterface**)&inter);
	assert(NULL ==  inter);
}

//Num:Test27
int CTest::test_isEmpty(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New((IVector**)&v);
	Boolean flag = FALSE;
	v->IsEmpty(&flag);
	assert(TRUE == flag);

	AutoPtr<IInterface> obj;
	v->AddElement(obj);
	v->IsEmpty(&flag);
	assert(FALSE == flag);
}

//Num:Test28
// int CTest::test_isEmpty_subtest0(int argc, char* argv[]) {
// }

//Num:Test28
int CTest::test_lastElement(int argc, char* argv[]) {
	AutoPtr<IInterface> inter;
	tVector->LastElement((IInterface**)&inter);
	AutoPtr<ICharSequence> cs;
	cs = ICharSequence::Probe(inter);
	String str;
	cs->ToString(&str);
	assert(String("Test 99").Equals(str));

	tVector->AddElement(NULL);
	tVector->LastElement((IInterface**)&inter);
	assert(NULL ==  inter);

	CVector::New(10, (IVector**)&tVector);
	ECode ec = tVector->LastElement((IInterface**)&inter);
	assert(ec == E_NO_SUCH_ELEMENT_EXCEPTION);
}

//Num:Test29
int CTest::test_lastIndexOfLjava_lang_Object(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New(9, (IVector**)&v);
	String str = String("Test");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	for (int i = 0; i < 9; ++i) {
		v->AddElement(cs);
	}
	AutoPtr<ICharSequence> cs1;
	String str1 = String("z");
	CString::New(str1, (ICharSequence**)&cs1);
	v->AddElement(cs1);
	Int32 value;
	v->LastIndexOf(cs, &value);
	assert(8 ==  value);

	tVector->SetElementAt(NULL, 20);
	tVector->SetElementAt(NULL, 40);
	tVector->LastIndexOf(NULL, &value);
	assert(40 == value);
}

//Num:Test30
int CTest::test_lastIndexOfLjava_lang_ObjectI(int argc, char* argv[]) {
	String str = String("Test 0");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	Int32 value;
	tVector->LastIndexOfEx(cs, 0, &value);
	assert(0 ==  value);

	tVector->LastIndexOfEx(cs, 10, &value);
	assert(value > -1);

	tVector->SetElementAt(NULL, 20);
	tVector->SetElementAt(NULL, 40);
	tVector->SetElementAt(NULL, 60);
	tVector->LastIndexOfEx(NULL, 15, &value);
	assert(value == -1);

	tVector->LastIndexOfEx(NULL, 45, &value);
	assert(value == 40);

	Int32 size;
	tVector->GetSize(&size);
	ECode ec = tVector->LastIndexOfEx(NULL, size, &value);
	assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test31
int CTest::test_removeI(int argc, char* argv[]) {
	AutoPtr<IInterface> inter;
	tVector->RemoveEx(36, (IInterface**)&inter);
	String str = String("Test 36");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	Boolean flag = FALSE;
	tVector->Contains(cs, &flag);
	assert(FALSE == flag);

	Int32 size;
	tVector->GetSize(&size);
	assert(99 == size);

	tVector->AddEx(20, NULL);
	tVector->RemoveEx(19, (IInterface**)&inter);
	tVector->Get(19, (IInterface**)&inter);
	assert(NULL == inter);

	tVector->RemoveEx(19, (IInterface**)&inter);
	tVector->Get(19, (IInterface**)&inter);
	assert(NULL != inter);

	tVector->GetSize(&size);
	assert(98 == size);

	ECode ec = tVector->RemoveEx(-5, (IInterface**)&inter);
	assert(ec ==  E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);

	tVector->GetSize(&size);
	ec = tVector->RemoveEx(size + 1, (IInterface**)&inter);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test32
int CTest::test_removeLjava_lang_Object(int argc, char* argv[]) {
	Boolean flag = FALSE;
	String str = String("Test 0");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	tVector->Remove(cs, &flag); //because remove return false
	tVector->Contains(cs, &flag);
	assert(FALSE == flag);

	Int32 size;
	tVector->GetSize(&size);
	assert(99 == size);

	tVector->Add(NULL, &flag);
	tVector->Remove(NULL, &flag);
	tVector->Contains(NULL, &flag);
	assert(FALSE == flag);

	tVector->GetSize(&size);
	assert(99 == size);
}

//Num:Test33
int CTest::test_removeAllLjava_util_Collection(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New((IVector**)&v);
	AutoPtr<ILinkedList> lst;
	CLinkedList::New((ILinkedList**)&lst);
	AutoPtr<ICollection> l = ICollection::Probe(lst);
	String str;
	AutoPtr<ICharSequence> cs;
	Boolean flag = FALSE;
	for (int i = 0; i < 5; ++i) {
		str = String("Test ") + i;
		CString::New(str, (ICharSequence**)&cs);
		l->Add(cs, &flag);
	}
	v->AddElement(l);

	AutoPtr<ICollection> s;
	AutoPtr<ISet> set;
	CHashSet::New((ISet**)&set);
	s = ICollection::Probe(set);

	AutoPtr<IInterface> o;
	v->FirstElement((IInterface**)&o);
	s->Add(o, &flag);

	v->RemoveAll(s, &flag);
	v->Contains(o, &flag);
	assert(FALSE == flag);

	v->RemoveAll(l, &flag);
	v->IsEmpty(&flag);
	assert(TRUE == flag);

	v->Add(NULL, &flag);
	v->Add(NULL, &flag);
	str = String("Boom");
	CString::New(str, (ICharSequence**)&cs);
	v->Add(cs, &flag);
	v->RemoveAll(s, &flag);

	Int32 size;
	v->GetSize(&size);
	assert(3 == size);

	CLinkedList::New((ILinkedList**)&lst);
	l = ICollection::Probe(lst);
	l->Add(NULL, &flag);
	v->RemoveAll(l, &flag);
	v->GetSize(&size);
	assert(1 == size);

	v->FirstElement((IInterface**)&o);
	AutoPtr<ICharSequence> cs1;
	cs1 = ICharSequence::Probe(o);
	String cs1Str;
	cs1->ToString(&cs1Str);
	assert(cs1Str.Equals(str));

	ECode ec = v->RemoveAll(NULL, &flag);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num:Test34
int CTest::test_removeAllElements(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	v->RemoveAllElements();
	Int32 size;
	v->GetSize(&size);
	assert(0 == size);
}

//Num:Test35
int CTest::test_removeElementLjava_lang_Object(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	String str = String("Test 98");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	Boolean flag = FALSE;
	v->RemoveElement(cs, &flag);
	v->ElementAt(98, (IInterface**)&obj);
	cs = ICharSequence::Probe(obj);
	cs->ToString(&str);
	assert(String("Test 99").Equals(str));

	Int32 size;
	v->GetSize(&size);
	assert(99 == size);

	tVector->AddElement(NULL);
	v->RemoveElement(NULL, &flag);
	v->GetSize(&size);
	assert(99 == size);
}

//Num:Test 36
int CTest::test_removeElementAtI(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	v->RemoveElementAt(50);
	String str = String("Test 50");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	Int32 value;
	v->IndexOfEx(cs, 0, &value);
	tVector->InsertElementAt(NULL, 60);
	tVector->RemoveElementAt(60);
	tVector->ElementAt(60, (IInterface**)&obj);
	assert(NULL != obj);

	ECode ec = tVector->ElementAt(-5, (IInterface**)&obj);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);

	Int32 size;
	tVector->GetSize(&size);
	tVector->ElementAt(size + 1, (IInterface**)&obj);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test 37
int CTest::test_retainAllLjava_util_Collection(int argc, char* argv[]) {
	AutoPtr<IInterface> o;
	tVector->FirstElement((IInterface**)&o);
	Boolean flag = FALSE;
	tVector->Add(NULL, &flag);
	AutoPtr<ICollection> s;
	AutoPtr<ISet> set;
	CHashSet::New((ISet**)&set);
	s = ICollection::Probe(set);
	s->Add(o, &flag);
	s->Add(NULL, &flag);

	tVector->RetainAll(s, &flag);
	tVector->Contains(o, &flag);
	assert(TRUE == flag);
	tVector->Contains(NULL, &flag);
	assert(TRUE == flag);
	Int32 size;
	tVector->GetSize(&size);
	assert(2 == size);
}

//Num:Test38
int CTest::test_setILjava_lang_Object(int argc, char* argv[]) {
	AutoPtr<IInterface> o;
	AutoPtr<IInterface> out;
	tVector->Set(23, o, (IInterface**)&out);
	tVector->Get(23, (IInterface**)&out);
	assert(o == out);

	String str = String("Wrong position");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	ECode ec = tVector->Set(-5, cs, (IInterface**)&out);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);

	Int32 size;
	tVector->GetSize(&size);
	ec = tVector->Set(size + 1, cs, (IInterface**)&out);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test39
int CTest::test_setElementAtLjava_lang_ObjectI(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	String str = String("Inserted Element");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	v->SetElementAt(cs, 99);
	v->ElementAt(99, (IInterface**)&obj);
	AutoPtr<ICharSequence> cs1;
	cs1 = ICharSequence::Probe(obj);
	assert(cs == cs1);

	str = String("Wrong position");
	CString::New(str, (ICharSequence**)&cs);
	ECode ec = tVector->SetElementAt(cs, -5);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);

	Int32 size;
	tVector->GetSize(&size);
	ec = tVector->SetElementAt(cs, size + 1);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test40
int CTest::test_setSizeI(int argc, char* argv[]) {
	AutoPtr<IInterface> obj;
	ICloneable::Probe(tVector)->Clone((IInterface**)&obj);
	AutoPtr<IVector> v = IVector::Probe(obj);
	v->SetSize(10);
	Int32 size;
	v->GetSize(&size);
	assert(10 == size);

	ECode ec = tVector->SetSize(-5);
	assert(ec == E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION);
	// assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
}

//Num:Test42
// int CTest::test_size(int argc, char* argv[]) {

// }

//Num:Test41
int CTest::test_subListII(int argc, char* argv[]) {
	AutoPtr<IList> sl;
	tVector->SubList(10, 25, (IList**)&sl);
	Int32 size;
	sl->GetSize(&size);
	assert(15 == size);

	AutoPtr<IInterface> obj;
	Boolean flag = FALSE;
	for (int i = 10; i < 25; ++i) {
		tVector->Get(i, (IInterface**)&obj);
		sl->Contains(obj, &flag);
		assert(TRUE == flag);
	}

	ECode ec = tVector->SubList(-10, 25, (IList**)&sl);
	assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

	tVector->GetSize(&size);
	ec = tVector->SubList(10, size + 1, (IList**)&sl);
	assert(ec ==  E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

	ec = tVector->SubList(25, 10, (IList**)&sl);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num:Test42
int CTest::test_toArray(int argc, char* argv[]) {
	AutoPtr<ArrayOf<IInterface*> > array;
	tVector->ToArray((ArrayOf<IInterface*>**)&array);
	Boolean flag = FALSE;
	Arrays::Equals(array, objArray, &flag);
	assert(flag == TRUE);
}

//Num:Test43
int CTest::test_toArray_Ljava_lang_Object(int argc, char* argv[]) {
	AutoPtr<ArrayOf<IInterface*> > o = ArrayOf<IInterface*>::Alloc(1000);
	AutoPtr<IInterface> f;
	for (int i = 0; i < o->GetLength(); ++i) {
		(*o)[i] = f;
	}
	tVector->ToArray((ArrayOf<IInterface*>**)&o);
	// assert(NULL == (*o)[100]);

	Int32 size;
	tVector->GetSize(&size);
	for (int i = 0; i < size; ++i) {
		tVector->ElementAt(i, (IInterface**)&f);
		assert((*o)[i] == f);
	}

	ECode ec = tVector->ToArray(NULL);
	assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

	CVector::New((IVector**)&tVector);
	Boolean flag = FALSE;
	AutoPtr<IInteger32> num;
	CInteger32::New(1, (IInteger32**)&num);
	tVector->Add(num, &flag);
	CInteger32::New(2, (IInteger32**)&num);
	tVector->Add(num, &flag);
	CInteger32::New(3, (IInteger32**)&num);
	tVector->Add(num, &flag);

}

//Num:Test44
int CTest::test_toString(int argc, char* argv[]) {
	String str = Object::ToString(tVector);
	assert(str.Equals(vString));

	AutoPtr<IVector> v;
	CVector::New((IVector**)&v);
	str = String("one");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	v->AddElement(cs);
	v->AddElement(v);
	str = String("3");
	CString::New(str, (ICharSequence**)&cs);
	v->AddElement(v);
	String result = Object::ToString(v);
	Int32 value = result.IndexOf(String("this"));
	assert(value > -1);
}

//Num:Test45
int CTest::test_trimToSize(int argc, char* argv[]) {
	AutoPtr<IVector> v;
	CVector::New(10, (IVector**)&v);
	AutoPtr<IInterface> obj;
	v->AddElement(obj);
	v->TrimToSize();
	Int32 capacity;
	v->Capacity(&capacity);
	assert(1 == capacity);
}

//Num:Test 46
int CTest::test_removeRangeII(int argc, char* argv[]) {
	AutoPtr<IVector> mv;
	CVector::New((IVector**)&mv);
	String str = String("First");
	AutoPtr<ICharSequence> cs;
	CString::New(str, (ICharSequence**)&cs);
	Boolean flag = FALSE;
	mv->Add(cs, &flag);

	AutoPtr<ICharSequence> cs1;
	str = String("Second");
	CString::New(str, (ICharSequence**)&cs1);
	mv->Add(cs1, &flag);

	str = String("One more");
	AutoPtr<ICharSequence> cs2;
	CString::New(str, (ICharSequence**)&cs2);
	mv->Add(cs2, &flag);

	str = String("Last");
	AutoPtr<ICharSequence> cs3;
	CString::New(str, (ICharSequence**)&cs3);
	mv->Add(cs3, &flag);

	// mv->RemoveRange(1, 3); //need modify protected to public
	mv->Contains(cs, &flag);
	assert(TRUE == flag);
	mv->Contains(cs1, &flag);
	assert(FALSE == flag);

	mv->Contains(cs2, &flag);
	assert(FALSE == flag);
	mv->Contains(cs3, &flag);
	assert(TRUE == flag);
}










































