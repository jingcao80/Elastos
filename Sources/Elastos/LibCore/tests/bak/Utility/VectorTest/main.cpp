#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
   &CTest::test_Constructor, //1.ok
   &CTest::test_ConstructorI, //2.ok
   &CTest::test_ConstructorII, //3.ok
   &CTest::test_ConstructorLjava_util_Collection, //4.ok
   &CTest::test_addILjava_lang_Object, //5.ok
   &CTest::test_addLjava_lang_Object, //6.ok
   &CTest::test_addAllILjava_util_Collection, //7.ok
   &CTest::test_addAllLjava_util_Collection, //8.ok
   &CTest::test_addElementLjava_lang_Object,//9.ok
   &CTest::test_addElementLjava_lang_Object_subtest0, //10.ok
   &CTest::test_capacity, //11.ok
   &CTest::test_clear, //12.ok
   &CTest::test_clone, //13.ok
   &CTest::test_containsLjava_lang_Object, //14.ok
   &CTest::test_containsAllLjava_util_Collection, //15. not ok, need ICollection->Add(),,,,,
   &CTest::test_copyInto_Ljava_lang_Object, //16.ok
   &CTest::test_elementAtI, //17.ok
   &CTest::test_elements, //18.ok
   &CTest::test_ensureCapacityI, //19.ok
   &CTest::test_equalsLjava_lang_Object, //20.ok
   &CTest::test_firstElement, //21.ok
   &CTest::test_getI, //22.ok
   &CTest::test_hashCode, //23.ok
   &CTest::test_indexOfLjava_lang_Object, //24.ok
   &CTest::test_indexOfLjava_lang_ObjectI, //25.ok
   &CTest::test_insertElementAtLjava_lang_ObjectI, //26.ok
   &CTest::test_isEmpty, //27.ok
   &CTest::test_lastElement, //28.ok
   &CTest::test_lastIndexOfLjava_lang_Object, //29.ok
   &CTest::test_lastIndexOfLjava_lang_ObjectI, //30.ok
   &CTest::test_removeI, //31.ok
   &CTest::test_removeLjava_lang_Object, //32.ok
   &CTest::test_removeAllLjava_util_Collection, //33.ok
   &CTest::test_removeAllElements, //34. ok
   &CTest::test_removeElementLjava_lang_Object, //35.ok
   &CTest::test_removeElementAtI, //36.ok
   &CTest::test_retainAllLjava_util_Collection, //37.ok
   &CTest::test_setILjava_lang_Object, //38.ok
   &CTest::test_setElementAtLjava_lang_ObjectI, //39.ok
   &CTest::test_setSizeI, //40.ok
   &CTest::test_subListII, //41.ok
   &CTest::test_toArray, //42.ok
   &CTest::test_toArray_Ljava_lang_Object, //43.ok
   &CTest::test_toString, //44.ok
   &CTest::test_trimToSize, //45.ok
   &CTest::test_removeRangeII, //46.
};

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("*ERROR* Require test case number\n");
        return -1;
    }

    int nIndex = atoi(argv[1]) - 1;
    int nLength = sizeof(TestEntry) / sizeof(TestEntry[0]);

    if (nIndex < 0 || nIndex >= nLength) {
        printf("*ERROR* Invalid testing case number\n");
        return -1;
    }

//    TPINIT;
    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

//    TPEND;

    return nQuitCode;
}
