#ifndef __ARRAYS_TEST_H
#define __ARRAYS_TEST_H

#include "Elastos.CoreLibrary.h"
using namespace Elastos;
using namespace Elastos::Core;
using Elastos::Utility::Arrays;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

    class ReversedIntegerComparator
    {
    public:
        Int32 Compare(IInterface* o1, IInterface* o2);
        Boolean Equals(IInterface* o1, IInterface* o2);
    };

public:
    int test_asList_Ljava_lang_Object(int argc, char* argv[]);

    int test_binarySearch_BB(int argc, char* argv[]);

    int test_binarySearch_CC(int argc, char* argv[]);

    int test_binarySearch_DD(int argc, char* argv[]);

    int test_binarySearch_FF(int argc, char* argv[]);

    int test_binarySearch_II(int argc, char* argv[]);

    int test_binarySearch_JJ(int argc, char* argv[]);

    int test_binarySearch_Ljava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);

    int test_binarySearch_Ljava_lang_ObjectLjava_lang_ObjectLjava_util_Comparator(int argc, char* argv[]);

    int test_binarySearch_SS(int argc, char* argv[]);

    int test_fill_BB(int argc, char* argv[]);

    int test_fill_BIIB(int argc, char* argv[]);

    int test_fill_SS(int argc, char* argv[]);

    int test_fill_SIIS(int argc, char* argv[]);

    int test_fill_CC(int argc, char* argv[]);

    int test_fill_CIIC(int argc, char* argv[]);

    int test_fill_II(int argc, char* argv[]);

    int test_fill_IIII(int argc, char* argv[]);

    int test_fill_JJ(int argc, char* argv[]);

    int test_fill_JIIJ(int argc, char* argv[]);

    int test_fill_FF(int argc, char* argv[]);

    int test_fill_FIIF(int argc, char* argv[]);

    int test_fill_DD(int argc, char* argv[]);

    int test_fill_DIID(int argc, char* argv[]);

    int test_fill_ZZ(int argc, char* argv[]);

    int test_fill_ZIIZ(int argc, char* argv[]);

    int test_fill_Ljava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);

    int test_fill_Ljava_lang_ObjectIILjava_lang_Object(int argc, char* argv[]);

    int test_equals_BB(int argc, char* argv[]);

    int test_equals_SS(int argc, char* argv[]);

    int test_equals_CC(int argc, char* argv[]);

    int test_equals_II(int argc, char* argv[]);

    int test_equals_JJ(int argc, char* argv[]);

    int test_equals_FF(int argc, char* argv[]);

    int test_equals_DD(int argc, char* argv[]);

    int test_equals_ZZ(int argc, char* argv[]);

    int test_equals_Ljava_lang_Object_Ljava_lang_Object(int argc, char* argv[]);

    int test_sort_B(int argc, char* argv[]);

    int test_sort_BII(int argc, char* argv[]);

    int test_sort_C(int argc, char* argv[]);

    int test_sort_CII(int argc, char* argv[]);

    int test_sort_D(int argc, char* argv[]);

    int test_sort_DII(int argc, char* argv[]);

    int test_sort_F(int argc, char* argv[]);

    int test_sort_FII(int argc, char* argv[]);

    int test_sort_I(int argc, char* argv[]);

    int test_sort_III(int argc, char* argv[]);

    int test_sort_J(int argc, char* argv[]);

    int test_sort_JII(int argc, char* argv[]);

    int test_sort_Ljava_lang_Object(int argc, char* argv[]);

    int test_sort_Ljava_lang_ObjectII(int argc, char* argv[]);

    int test_sort_Ljava_lang_ObjectIILjava_util_Comparator(int argc, char* argv[]);

    int test_sort_Ljava_lang_ObjectLjava_util_Comparator(int argc, char* argv[]);

    int test_sort_S(int argc, char* argv[]);

    int test_sort_SII(int argc, char* argv[]);

    int test_java_util_Arrays_sort_byte_array_NPE(int argc, char* argv[]);

    int test_java_util_Arrays_sort_char_array_NPE(int argc, char* argv[]);

    int test_java_util_Arrays_sort_double_array_NPE(int argc, char* argv[]);

    int test_java_util_Arrays_sort_float_array_NPE(int argc, char* argv[]);

    int test_java_util_Arrays_sort_int_array_NPE(int argc, char* argv[]);

    int test_java_util_Arrays_sort_object_array_NPE(int argc, char* argv[]);

    int test_java_util_Arrays_sort_long_array_NPE(int argc, char* argv[]);

    int test_java_util_Arrays_sort_short_array_NPE(int argc, char* argv[]);

    int test_sort(int argc, char* argv[]);

    int test_deepEquals_Ljava_lang_ObjectLjava_lang_Object(int argc, char* argv[]);

    int test_deepHashCode_Ljava_lang_Object(int argc, char* argv[]);

    int test_hashCode_LZ(int argc, char* argv[]);

    int test_hashCode_LI(int argc, char* argv[]);

    int test_hashCode_LC(int argc, char* argv[]);

    int test_hashCode_LB(int argc, char* argv[]);

    int test_hashCode_LJ(int argc, char* argv[]);

    int test_hashCode_LF(int argc, char* argv[]);

    int test_hashCode_LD(int argc, char* argv[]);

    int test_hashCode_LS(int argc, char* argv[]);

    int test_hashCode_Ljava_lang_Object(int argc, char* argv[]);

private:
    void sort(IInterface* array);

    void checkSorted(IInterface* array);

    void checkSorted(ArrayOf<Int32>* a);

    void checkSorted(ArrayOf<Int64>* a);

    void checkSorted(ArrayOf<Int16>* a);

    void checkSorted(ArrayOf<Byte>* a);

    void checkSorted(ArrayOf<Char32>* a);

    void checkSorted(ArrayOf<Float>* a);

    void checkSorted(ArrayOf<Double>* a);

    void orderFail(Int32 index, String value1, String value2);

    Int32 checkSum(IInterface* array);

    Int32 checkSum(ArrayOf<Int32>* a);

    Int32 checkSum(ArrayOf<Int64>* a);

    Int32 checkSum(ArrayOf<Int16>* a);

    Int32 checkSum(ArrayOf<Byte>* a);

    Int32 checkSum(ArrayOf<Char32>* a);

    Int32 checkSum(ArrayOf<Float>* a);

    Int32 checkSum(ArrayOf<Double>* a);

protected:
    void setUp();

    void tearDown();

public:
    static Int32 arraySize;

    AutoPtr<ArrayOf<IInterface*> > objArray;

    AutoPtr<ArrayOf<Boolean> > booleanArray;

    AutoPtr<ArrayOf<Byte> > byteArray;

    AutoPtr<ArrayOf<Char32> > charArray;

    AutoPtr<ArrayOf<Double> > doubleArray;

    AutoPtr<ArrayOf<Float> > floatArray;

    AutoPtr<ArrayOf<Int32> > intArray;

    AutoPtr<ArrayOf<Int64> > longArray;

    AutoPtr<ArrayOf<IInterface*> > objectArray;

    AutoPtr<ArrayOf<Int16> > shortArray;

//private:
//    static AutoPtr<ArrayOf<Int32> > LENGTHS;
};

#endif //__ARRAYS_TEST_H
