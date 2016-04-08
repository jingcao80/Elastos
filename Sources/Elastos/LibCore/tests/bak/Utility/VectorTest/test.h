#ifndef __VECTORTEST_H__
#define __VECTORTEST_H__

#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Core;
using Elastos::Utility::IVector;

class CTest
{
public:
	CTest();
	~CTest();

public:
	int test_Constructor(int argc, char* argv[]);
	int test_ConstructorI(int argc, char* argv[]);
	int test_ConstructorII(int argc, char* argv[]);
	int test_ConstructorLjava_util_Collection(int argc, char* argv[]);
	int test_addILjava_lang_Object(int argc, char* argv[]);
	int test_addLjava_lang_Object(int argc, char* argv[]);
	int test_addAllILjava_util_Collection(int argc, char* argv[]);
	int test_addAllLjava_util_Collection(int argc, char* argv[]);
	int test_addElementLjava_lang_Object(int argc, char* argv[]);
	int test_addElementLjava_lang_Object_subtest0(int argc, char* argv[]);
	int test_capacity(int argc, char* argv[]);
	int test_clear(int argc, char* argv[]);
	int test_clone(int argc, char* argv[]);
	int test_containsLjava_lang_Object(int argc, char* argv[]);
	int test_containsAllLjava_util_Collection(int argc, char* argv[]);
	int test_copyInto_Ljava_lang_Object(int argc, char* argv[]);
	int test_elementAtI(int argc, char* argv[]);
	int test_elements(int argc, char* argv[]);
	int test_ensureCapacityI(int argc, char* argv[]);
	int test_equalsLjava_lang_Object(int argc, char* argv[]);
	int test_firstElement(int argc, char* argv[]);
	int test_getI(int argc, char* argv[]);
	int test_hashCode(int argc, char* argv[]);
	int test_indexOfLjava_lang_Object(int argc, char* argv[]);
	int test_indexOfLjava_lang_ObjectI(int argc, char* argv[]);
	int test_insertElementAtLjava_lang_ObjectI(int argc, char* argv[]);
	int test_isEmpty(int argc, char* argv[]);
	int test_lastElement(int argc, char* argv[]);
	int test_lastIndexOfLjava_lang_Object(int argc, char* argv[]);
	int test_lastIndexOfLjava_lang_ObjectI(int argc, char* argv[]);
	int test_removeI(int argc, char* argv[]);
	int test_removeLjava_lang_Object(int argc, char* argv[]);
	int test_removeAllLjava_util_Collection(int argc, char* argv[]);
	int test_removeAllElements(int argc, char* argv[]);
	int test_removeElementLjava_lang_Object(int argc, char* argv[]);
	int test_removeElementAtI(int argc, char* argv[]);
	int test_retainAllLjava_util_Collection(int argc, char* argv[]);
	int test_setILjava_lang_Object(int argc, char* argv[]);
	int test_setElementAtLjava_lang_ObjectI(int argc, char* argv[]);
	int test_setSizeI(int argc, char* argv[]);
	int test_subListII(int argc, char* argv[]);
	int test_toArray(int argc, char* argv[]);
	int test_toArray_Ljava_lang_Object(int argc, char* argv[]);
	int test_toString(int argc, char* argv[]);
	int test_trimToSize(int argc, char* argv[]);
	int test_removeRangeII(int argc, char* argv[]);
	//int test_elements_subtest0(int argc, char* argv[]);
	//int test_isEmpty_subtest0(int argc, char* argv[]);
	//int test_size(int argc, char* argv[]);

protected:
	void setUp();
	void tearDown();

private:
	AutoPtr<IVector> tVector;
	AutoPtr<ArrayOf<IInterface*> > objArray;
	String vString;
};

#endif //__VECTORTEST_H__