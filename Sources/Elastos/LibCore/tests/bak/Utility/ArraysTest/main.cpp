#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
//    &CTest::test_asList_Ljava_lang_Object,
    &CTest::test_binarySearch_BB,   //1, (Byte) -1, not surpport
    &CTest::test_binarySearch_CC,   //2, ok
    &CTest::test_binarySearch_DD,   //3, ok
    &CTest::test_binarySearch_FF,   //4, ok
    &CTest::test_binarySearch_II,   //5, ok
    &CTest::test_binarySearch_JJ,   //6, ok
    &CTest::test_binarySearch_Ljava_lang_ObjectLjava_lang_Object,   //7, elem probe IComparable NULL
//    &CTest::test_binarySearch_Ljava_lang_ObjectLjava_lang_ObjectLjava_util_Comparator,
    &CTest::test_binarySearch_SS,   //8, ok
    &CTest::test_fill_BB,     //9, ok
    &CTest::test_fill_BIIB,   //10, ok
    &CTest::test_fill_SS,     //11, ok
    &CTest::test_fill_SIIS,   //12, ok
    &CTest::test_fill_CC,     //13, ok
    &CTest::test_fill_CIIC,   //14, ok
    &CTest::test_fill_II,     //15, ok
    &CTest::test_fill_IIII,   //16, ok
    &CTest::test_fill_JJ,     //17, ok
    &CTest::test_fill_JIIJ,   //18, ok
    &CTest::test_fill_FF,     //19, ok
    &CTest::test_fill_FIIF,   //20, ok
    &CTest::test_fill_DD,     //21, ok
    &CTest::test_fill_DIID,   //22, ok
    &CTest::test_fill_ZZ,     //23, ok
    &CTest::test_fill_ZIIZ,   //24, ok
    &CTest::test_fill_Ljava_lang_ObjectLjava_lang_Object,     //25, ok
    &CTest::test_fill_Ljava_lang_ObjectIILjava_lang_Object,   //26, ok
    &CTest::test_equals_BB,   //27, ok
    &CTest::test_equals_SS,   //28, ok
    &CTest::test_equals_CC,   //29, ok
    &CTest::test_equals_II,   //30, ok
    &CTest::test_equals_JJ,   //31, ok
    &CTest::test_equals_FF,   //32, ok
    &CTest::test_equals_DD,   //33, ok
    &CTest::test_equals_ZZ,   //34, ok
    &CTest::test_equals_Ljava_lang_Object_Ljava_lang_Object,   //35, ok
//    &CTest::test_sort_B,
//    &CTest::test_sort_BII,
//    &CTest::test_sort_C,
//    &CTest::test_sort_CII,
//    &CTest::test_sort_D,
//    &CTest::test_sort_DII,
//    &CTest::test_sort_F,
//    &CTest::test_sort_FII,
//    &CTest::test_sort_I,
//    &CTest::test_sort_III,
//    &CTest::test_sort_J,
//    &CTest::test_sort_JII,
//    &CTest::test_sort_Ljava_lang_Object,
//    &CTest::test_sort_Ljava_lang_ObjectII,
//    &CTest::test_sort_Ljava_lang_ObjectIILjava_util_Comparator,
//    &CTest::test_sort_Ljava_lang_ObjectLjava_util_Comparator,
//    &CTest::test_sort_S,
//    &CTest::test_sort_SII,
//    &CTest::test_java_util_Arrays_sort_byte_array_NPE,
//    &CTest::test_java_util_Arrays_sort_char_array_NPE,
//    &CTest::test_java_util_Arrays_sort_double_array_NPE,
//    &CTest::test_java_util_Arrays_sort_float_array_NPE,
//    &CTest::test_java_util_Arrays_sort_int_array_NPE,
//    &CTest::test_java_util_Arrays_sort_object_array_NPE,
//    &CTest::test_java_util_Arrays_sort_long_array_NPE,
//    &CTest::test_java_util_Arrays_sort_short_array_NPE,
//    &CTest::test_sort,
    &CTest::test_deepEquals_Ljava_lang_ObjectLjava_lang_Object,   //36, ok
    &CTest::test_deepHashCode_Ljava_lang_Object,   //37, ok
    &CTest::test_hashCode_LZ,   //38, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_LI,   //39, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_LC,   //40, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_LB,   //41, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_LJ,   //42, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_LF,   //43, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_LD,   //44, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_LS,   //45, no ok CLinkedList hashcode assert 0 & TODO
    &CTest::test_hashCode_Ljava_lang_Object,   //46, no ok CLinkedList hashcode assert 0 & TODO
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
