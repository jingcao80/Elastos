
#include "test.h"
#include <elastos/Math.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::IList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::CLinkedList;
using Elastos::Core::Math;

Int32 CTest::ReversedIntegerComparator::Compare(IInterface* o1, IInterface* o2)
{
//    return -(((Integer) o1).compareTo((Integer) o2));
    return 0;
}

Boolean CTest::ReversedIntegerComparator::Equals(IInterface* o1, IInterface* o2)
{
//    return ((Integer) o1).compareTo((Integer) o2) == 0;
    return 0;
}

Int32 CTest::arraySize = 100;

CTest::CTest()
{
    setUp();
}

CTest::~CTest()
{
    tearDown();
}

//NOT_IMPLEMENT
int CTest::test_asList_Ljava_lang_Object(int argc, char* argv[])
{
    // // Test for method java.util.List
    // // java.util.Arrays.asList(java.lang.Object [])
    // List convertedList = Arrays.asList(objectArray);
    // for (Int32 counter = 0; counter < arraySize; counter++) {
    //     assertTrue(
    //             "Array and List converted from array do not contain identical elements",
    //             convertedList.get(counter) == (*objectArray)[counter]);
    // }
    // convertedList.set(50, new Integer(1000));
    // assertTrue("set/get did not work on coverted list", convertedList.get(
    //         50).equals(new Integer(1000)));
    // convertedList.set(50, new Integer(50));
    // new Support_UnmodifiableCollectionTest("", convertedList).runTest();

    // Object[] myArray = (Object[]) (objectArray.clone());
    // myArray[30] = NULL;
    // myArray[60] = NULL;
    // convertedList = Arrays.asList(myArray);
    // for (Int32 counter = 0; counter < arraySize; counter++) {
    //     assertTrue(
    //             "Array and List converted from array do not contain identical elements",
    //             convertedList.get(counter) == myArray[counter]);
    // }

    // try {
    //     Arrays.asList((Object[])NULL);
    //     fail("asList with null arg didn't throw NPE");
    // } catch (NullPointerException e) {
    //     // Expected
    // }
    return 0;
}

// test 1
int CTest::test_binarySearch_BB(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(byte [], byte)
    for (Byte counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(byteArray, counter, &index);
        assert(index == counter);
    }
    ec = Arrays::BinarySearch(byteArray, (Byte) -1, &index);
    printf("ec = %d index = %d\n", ec, index);
//    assert(index == -1);
    ec = Arrays::BinarySearch(byteArray, (Byte) arraySize, &index);
    assert(index == -(arraySize + 1));

    for (Byte counter = 0; counter < arraySize; counter++) {
        byteArray->Set(counter, (*byteArray)[counter]-50);
    }
    for (Byte counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(byteArray, (Byte) (counter - 50), &index);
//        assert(index == counter);
    }
    return ec;
}

// test 2
int CTest::test_binarySearch_CC(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(char [], char)
    for (Char32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(charArray, counter + 1, &index);
        assert(index == counter);
    }
    ec = Arrays::BinarySearch(charArray, 0x0000, &index);
    assert(index == -1);
    ec = Arrays::BinarySearch(charArray, arraySize + 1, &index);
    assert(index == -(arraySize + 1));
    return ec;
}

// test 3
/**
 * java.util.Arrays#binarySearch(double[], double)
 */
int CTest::test_binarySearch_DD(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(double [], double)
    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(doubleArray, counter, &index);
        assert(index == counter);
    }
    ec = Arrays::BinarySearch(doubleArray, -1, &index);
    assert(index == -1);
    ec = Arrays::BinarySearch(doubleArray, arraySize, &index);
    assert(index == -(arraySize + 1));

    for (Int32 counter = 0; counter < arraySize; counter++) {
        doubleArray->Set(counter, (*doubleArray)[counter] - 50);
    }
    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(doubleArray, (Double) (counter - 50), &index);
        assert(index == (Double) counter);
    }

    AutoPtr<ArrayOf<Double> > specials = ArrayOf<Double>::Alloc(11);
    specials->Set(0, Elastos::Core::Math::DOUBLE_NEGATIVE_INFINITY);
    specials->Set(1, -Elastos::Core::Math::DOUBLE_MAX_VALUE);
    specials->Set(2, -2.0);
    specials->Set(3, -Elastos::Core::Math::DOUBLE_MIN_VALUE);
    specials->Set(4, -0.0);
    specials->Set(5, 0.0);
    specials->Set(6, Elastos::Core::Math::DOUBLE_MIN_VALUE);
    specials->Set(7, 2.0);
    specials->Set(8, Elastos::Core::Math::DOUBLE_MAX_VALUE);
    specials->Set(9, Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY);
    specials->Set(10, Elastos::Core::Math::DOUBLE_NAN);
    for (Int32 i = 0; i < specials->GetLength(); i++) {
        ec = Arrays::BinarySearch(specials, (*specials)[i], &index);
        assert(index == i);
    }
    ec = Arrays::BinarySearch(specials, -1, &index);
    assert(index == -4);
    ec = Arrays::BinarySearch(specials, 1, &index);
    assert(index == -8);
    return ec;
}

// test 4
int CTest::test_binarySearch_FF(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(float [], float)
    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(floatArray, counter, &index);
        assert(index == counter);
    }
    ec = Arrays::BinarySearch(floatArray, -1, &index);
    assert(index == -1);
    ec = Arrays::BinarySearch(floatArray, arraySize, &index);
    assert(index == -(arraySize + 1));
    for (Int32 counter = 0; counter < arraySize; counter++) {
        floatArray->Set(counter, (*floatArray)[counter] - 50);
    }
    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(floatArray, counter - 50, &index);
        assert(index == counter);
    }

    AutoPtr<ArrayOf<Float> > specials = ArrayOf<Float>::Alloc(11);
    specials->Set(0, Elastos::Core::Math::FLOAT_NEGATIVE_INFINITY);
    specials->Set(1, -Elastos::Core::Math::FLOAT_MAX_VALUE);
    specials->Set(2, -2.0);
    specials->Set(3, -Elastos::Core::Math::FLOAT_MIN_VALUE);
    specials->Set(4, -0.0);
    specials->Set(5, 0.0);
    specials->Set(6, Elastos::Core::Math::FLOAT_MIN_VALUE);
    specials->Set(7, 2.0);
    specials->Set(8, Elastos::Core::Math::FLOAT_MAX_VALUE);
    specials->Set(9, Elastos::Core::Math::FLOAT_POSITIVE_INFINITY);
    specials->Set(10, Elastos::Core::Math::FLOAT_NAN);
    for (Int32 i = 0; i < specials->GetLength(); i++) {
        Int32 result;
        ec = Arrays::BinarySearch(specials, (*specials)[i], &result);
        assert(result == i);
    }
    ec = Arrays::BinarySearch(specials, -1.0, &index);
    assert(index == -4);
    ec = Arrays::BinarySearch(specials, 1.0, &index);
    assert(index == -8);
    return ec;
}

// test 5
int CTest::test_binarySearch_II(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(int [], int)
    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(intArray, counter, &index);
        assert(index == counter);
    }
    ec = Arrays::BinarySearch(intArray, -1, &index);
    assert(index == -1);
    ec = Arrays::BinarySearch(intArray, arraySize, &index);
    assert(index == -(arraySize + 1));

    for (Int32 counter = 0; counter < arraySize; counter++) {
        intArray->Set(counter, (*intArray)[counter] - 50);
    }
    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(intArray, counter - 50, &index);
        assert(index == counter);
    }
    return ec;
}

// test 6
int CTest::test_binarySearch_JJ(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(long [], long)
    for (Int64 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(longArray, counter, &index);
        assert(index == counter);
    }
    ec = Arrays::BinarySearch(longArray, -1, &index);
    assert(index == -1);
    ec = Arrays::BinarySearch(longArray, arraySize, &index);
    assert(index == -(arraySize + 1));
    for (Int64 counter = 0; counter < arraySize; counter++) {
        longArray->Set((Int32)counter, (*longArray)[(Int32) counter] - 50);
    }
    for (Int64 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(longArray, counter - (Int64) 50, &index);
        assert(index == counter);
    }
    return ec;
}

// test 7
int CTest::test_binarySearch_Ljava_lang_ObjectLjava_lang_Object(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(java.lang.Object
    // [], java.lang.Object)
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(0);
    AutoPtr<IInterface> obj;
    ec = Arrays::BinarySearch(arr, obj, &index);
    assert(-1 == index);

    AutoPtr<IInteger32> num1;
    CInteger32::New(-1, (IInteger32**)&num1);
    ec = Arrays::BinarySearch(arr, num1, &index);
    assert(-1 == index);

    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(objectArray, (*objArray)[counter], &index);
        assert(index == counter);
    }
    ec = Arrays::BinarySearch(objectArray, num1, &index);
    assert(-1 == index);
    AutoPtr<IInteger32> num2;
    CInteger32::New(arraySize, (IInteger32**)&num2);
    ec = Arrays::BinarySearch(objectArray, num2, &index);
    assert(index == -(arraySize + 1));

    AutoPtr<ArrayOf<IInterface*> > oArray = ArrayOf<IInterface*>::Alloc(5);
    AutoPtr<ICharSequence> str1;
    CString::New(String("1"), (ICharSequence**)&str1);
    AutoPtr<ICharSequence> str2;
    CString::New(String("2"), (ICharSequence**)&str2);
    AutoPtr<ICharSequence> str3;
    CString::New(String("3"), (ICharSequence**)&str3);
    AutoPtr<ICharSequence> str4;
    CString::New(String("4"), (ICharSequence**)&str4);
    AutoPtr<ICharSequence> str5;
    CString::New(String(""), (ICharSequence**)&str5);
    oArray->Set(0, str1);
    oArray->Set(1, str2);
    oArray->Set(2, str3);
    oArray->Set(3, str4);
    oArray->Set(4, str5);

    AutoPtr<IInteger32> num3;
    CInteger32::New(10, (IInteger32**)&num3);
    ec = Arrays::BinarySearch(oArray, num3, &index);
    assert(ec != NOERROR);
    return ec;
}

//IComparator NOT_IMPLEMENT
int CTest::test_binarySearch_Ljava_lang_ObjectLjava_lang_ObjectLjava_util_Comparator(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(java.lang.Object
    // [], java.lang.Object, java.util.Comparator)
    AutoPtr<IComparator> comp/* = new ReversedIntegerComparator()*/;
    for (Int32 counter = 0; counter < arraySize; counter++)
        objectArray->Set(counter, (*objArray)[arraySize - counter - 1]);
    AutoPtr<IInteger32> num1;
    CInteger32::New(-1, (IInteger32**)&num1);
    ec = Arrays::BinarySearch(objectArray, num1, comp, &index);
    assert(-(arraySize + 1) == index);
    AutoPtr<IInteger32> num2;
    CInteger32::New(arraySize, (IInteger32**)&num2);
    ec = Arrays::BinarySearch(objectArray, num2, comp, &index);
    assert(-1 == index);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(objectArray, (*objArray)[counter], comp, &index);
        assert(arraySize - counter - 1 == index);
    }
    return ec;
}

// test 8
int CTest::test_binarySearch_SS(int argc, char* argv[])
{
    Int32 index = -1;
    ECode ec = 0;
    // Test for method int java.util.Arrays.binarySearch(short [], short)
    for (Int16 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(shortArray, counter, &index);
        assert(index == counter);
        ec = Arrays::BinarySearch(intArray, (Int16) -1, &index);
        assert(index == -1);
        ec = Arrays::BinarySearch(intArray, (Int16) arraySize, &index);
        assert(index == -(arraySize + 1));
    }
    for (Int16 counter = 0; counter < arraySize; counter++) {
        shortArray->Set(counter, (*shortArray)[counter] - 50);
    }
    for (Int16 counter = 0; counter < arraySize; counter++) {
        ec = Arrays::BinarySearch(shortArray, (Int16) (counter - 50), &index);
        assert(index == counter);
    }
    return ec;
}

// test 9
int CTest::test_fill_BB(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(byte [], byte)
    AutoPtr<ArrayOf<Byte> > d = ArrayOf<Byte>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::BYTE_MAX_VALUE);
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert((*d)[i] == Elastos::Core::Math::BYTE_MAX_VALUE);
    }
    return ec;
}

// test 10
int CTest::test_fill_BIIB(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(byte [], int, int, byte)
    Byte val = Elastos::Core::Math::BYTE_MAX_VALUE;
    AutoPtr<ArrayOf<Byte> > d = ArrayOf<Byte>::Alloc(1000);
    ec = Arrays::Fill(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++) {
        assert((*d)[i] == val);
    }

    AutoPtr<ArrayOf<Byte> > b1 = ArrayOf<Byte>::Alloc(2);
    ec = Arrays::Fill(b1, 2, 1, (Byte) 27);
    assert( ec == E_ILLEGAL_ARGUMENT_EXCEPTION );

    AutoPtr<ArrayOf<Byte> > b2 = ArrayOf<Byte>::Alloc(2);
    ec = Arrays::Fill(b2, -1, 1, (Byte) 27);
    assert( ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION );

    AutoPtr<ArrayOf<Byte> > b3 = ArrayOf<Byte>::Alloc(2);
    ec = Arrays::Fill(b3, 1, 4, (Byte) 27);
    assert( ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION );
    return ec;
}

// test 11
int CTest::test_fill_SS(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(short [], short)
    AutoPtr<ArrayOf<Int16> > d = ArrayOf<Int16>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::INT16_MAX_VALUE);
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert((*d)[i] == Elastos::Core::Math::INT16_MAX_VALUE);
    }
    return ec;
}

// test 12
int CTest::test_fill_SIIS(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(short [], int, int, short)
    Int16 val = Elastos::Core::Math::INT16_MAX_VALUE;
    AutoPtr<ArrayOf<Int16> > d = ArrayOf<Int16>::Alloc(1000);
    ec = Arrays::Fill(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++) {
        assert((*d)[i] == val);
    }

    ec = Arrays::Fill(d, 10, 0, val);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::Fill(d, -10, 0, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::Fill(d, 10, d->GetLength()+1, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 13
int CTest::test_fill_CC(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(char [], char)

    AutoPtr<ArrayOf<Char32> > d = ArrayOf<Char32>::Alloc(1000);
    ec = Arrays::Fill(d, 'V');
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert('V' == (*d)[i]);
    }
    return ec;
}

// test 14
int CTest::test_fill_CIIC(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(char [], int, int, char)
    Char32 val = 'T';
    AutoPtr<ArrayOf<Char32> > d = ArrayOf<Char32>::Alloc(1000);
    ec = Arrays::Fill(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++)
        assert((*d)[i] == val);

    ec = Arrays::Fill(d, 10, 0, val);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::Fill(d, -10, 0, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::Fill(d, 10, d->GetLength()+1, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 15
int CTest::test_fill_II(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(int [], int)
    AutoPtr<ArrayOf<Int32> > d = ArrayOf<Int32>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::INT32_MAX_VALUE);
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert((*d)[i] == Elastos::Core::Math::INT32_MAX_VALUE);
    }
    return ec;
}

// test 16
int CTest::test_fill_IIII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(int [], int, int, int)
    Int32 val = Elastos::Core::Math::INT32_MAX_VALUE;
    AutoPtr<ArrayOf<Int32> > d = ArrayOf<Int32>::Alloc(1000);
    ec = Arrays::Fill(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++) {
        assert((*d)[i] == val);
    }

    ec = Arrays::Fill(d, 10, 0, val);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::Fill(d, -10, 0, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::Fill(d, 10, d->GetLength()+1, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 17
int CTest::test_fill_JJ(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(long [], long)
    AutoPtr<ArrayOf<Int64> > d = ArrayOf<Int64>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::INT64_MAX_VALUE);
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert((*d)[i] == Elastos::Core::Math::INT64_MAX_VALUE);
    }
    return ec;
}

// test 18
int CTest::test_fill_JIIJ(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(long [], int, int, long)
    AutoPtr<ArrayOf<Int64> > d = ArrayOf<Int64>::Alloc(1000);
    ec = Arrays::Fill(d, 400, d->GetLength(), Elastos::Core::Math::INT64_MAX_VALUE);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == Elastos::Core::Math::INT64_MAX_VALUE));
    }
    for (Int32 i = 400; i < d->GetLength(); i++) {
        assert((*d)[i] == Elastos::Core::Math::INT64_MAX_VALUE);
    }

    ec = Arrays::Fill(d, 10, 0, Elastos::Core::Math::INT64_MIN_VALUE);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::Fill(d, -10, 0, Elastos::Core::Math::INT64_MAX_VALUE);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::Fill(d, 10, d->GetLength()+1, Elastos::Core::Math::INT64_MAX_VALUE);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 19
int CTest::test_fill_FF(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(float [], float)
    AutoPtr<ArrayOf<Float> > d = ArrayOf<Float>::Alloc(1000);
    ec = Arrays::FillFloat(d, Elastos::Core::Math::FLOAT_MAX_VALUE);
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert((*d)[i] == Elastos::Core::Math::FLOAT_MAX_VALUE);
    }
    return ec;
}

// test 20
int CTest::test_fill_FIIF(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(float [], int, int, float)
    Float val = Elastos::Core::Math::FLOAT_MAX_VALUE;
    AutoPtr<ArrayOf<Float> > d = ArrayOf<Float>::Alloc(1000);
    ec = Arrays::FillFloatEx(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++) {
        assert((*d)[i] == val);
    }

    ec = Arrays::FillFloatEx(d, 10, 0, val);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::FillFloatEx(d, -10, 0, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::FillFloatEx(d, 10, d->GetLength()+1, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 21
int CTest::test_fill_DD(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(double [], double)
    AutoPtr<ArrayOf<Double> > d = ArrayOf<Double>::Alloc(1000);
    ec = Arrays::FillDouble(d, Elastos::Core::Math::DOUBLE_MAX_VALUE);
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert((*d)[i] == Elastos::Core::Math::DOUBLE_MAX_VALUE);
    }
    return ec;
}

// test 22
int CTest::test_fill_DIID(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(double [], int, int,
    // double)
    Double val = Elastos::Core::Math::DOUBLE_MAX_VALUE;
    AutoPtr<ArrayOf<Double> > d = ArrayOf<Double>::Alloc(1000);
    ec = Arrays::FillDoubleEx(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++) {
        assert((*d)[i] == val);
    }

    ec = Arrays::FillDoubleEx(d, 10, 0, val);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::FillDoubleEx(d, -10, 0, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::FillDoubleEx(d, 10, d->GetLength()+1, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 23
int CTest::test_fill_ZZ(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(boolean [], boolean)
    AutoPtr<ArrayOf<Boolean> > d = ArrayOf<Boolean>::Alloc(1000);
    ec = Arrays::FillBoolean(d, TRUE);
    for (Int32 i = 0; i < d->GetLength(); i++) {
        assert((*d)[i] == TRUE);
    }
    return ec;
}

// test 24
int CTest::test_fill_ZIIZ(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(boolean [], int, int,
    // boolean)
    Boolean val = TRUE;
    AutoPtr<ArrayOf<Boolean> > d = ArrayOf<Boolean>::Alloc(1000);
    ec = Arrays::FillBooleanEx(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++) {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++) {
        assert((*d)[i] == val);
    }

    ec = Arrays::FillBooleanEx(d, 10, 0, val);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::FillBooleanEx(d, -10, 0, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::FillBooleanEx(d, 10, d->GetLength()+1, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 25
int CTest::test_fill_Ljava_lang_ObjectLjava_lang_Object(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(java.lang.Object [],
    // java.lang.Object)
    AutoPtr<IInterface> val;
    AutoPtr<ArrayOf<IInterface*> > d = ArrayOf<IInterface*>::Alloc(1000);
    ec = Arrays::FillObjectEx(d, 0, d->GetLength(), val);
    for (Int32 i = 0; i < d->GetLength(); i++)
        assert((*d)[i] == val);
    return ec;
}

// test 26
int CTest::test_fill_Ljava_lang_ObjectIILjava_lang_Object(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.fill(java.lang.Object [], int,
    // int, java.lang.Object)
    AutoPtr<IInterface> val;
    CInteger32::New(99, (IInteger32**)&val);
    AutoPtr<ArrayOf<IInterface*> > d = ArrayOf<IInterface*>::Alloc(1000);
    ec = Arrays::FillObjectEx(d, 400, d->GetLength(), val);
    for (Int32 i = 0; i < 400; i++)
    {
        assert(!((*d)[i] == val));
    }
    for (Int32 i = 400; i < d->GetLength(); i++)
        assert((*d)[i] == val);

    ec = Arrays::FillObjectEx(d, 400, d->GetLength(), NULL);
    for (Int32 i = 400; i < d->GetLength(); i++)
        assert((*d)[i] == NULL);

    ec = Arrays::FillObjectEx(d, 10, 0, val);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::FillObjectEx(d, -10, 0, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::FillObjectEx(d, 10, d->GetLength()+1, val);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

// test 27
int CTest::test_equals_BB(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(byte [], byte [])
    AutoPtr<ArrayOf<Byte> > d = ArrayOf<Byte>::Alloc(1000);
    AutoPtr<ArrayOf<Byte> > x = ArrayOf<Byte>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::BYTE_MAX_VALUE);
    ec = Arrays::Fill(x, Elastos::Core::Math::BYTE_MIN_VALUE);
    Boolean result;
    ec = Arrays::EqualsByte(d, x, &result);
    assert(!result);
    ec = Arrays::Fill(x, Elastos::Core::Math::BYTE_MAX_VALUE);
    ec = Arrays::EqualsByte(d, x, &result);
    assert(result);
    return ec;
}

// test 28
int CTest::test_equals_SS(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(short [], short [])
    AutoPtr<ArrayOf<Int16> > d = ArrayOf<Int16>::Alloc(1000);
    AutoPtr<ArrayOf<Int16> > x = ArrayOf<Int16>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::INT16_MAX_VALUE);
    ec = Arrays::Fill(x, Elastos::Core::Math::INT16_MIN_VALUE);
    Boolean result;
    ec = Arrays::EqualsInt16(d, x, &result);
    assert(!result);
    ec = Arrays::Fill(x, Elastos::Core::Math::INT16_MAX_VALUE);
    ec = Arrays::EqualsInt16(d, x, &result);
    assert(result);
    return ec;
}

// test 29
int CTest::test_equals_CC(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(char [], char [])
    AutoPtr<ArrayOf<Char32> > d = ArrayOf<Char32>::Alloc(1000);
    AutoPtr<ArrayOf<Char32> > x = ArrayOf<Char32>::Alloc(1000);
    Char32 c = 'T';
    ec = Arrays::Fill(d, c);
    ec = Arrays::Fill(x, 'L');
    Boolean result = FALSE;
    ec = Arrays::EqualsChar32(d, x, &result);
    assert(!result);
    ec = Arrays::Fill(x, c);
    ec = Arrays::EqualsChar32(d, x, &result);
    assert(result);
    return ec;
}

// test 30
int CTest::test_equals_II(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(int [], int [])
    AutoPtr<ArrayOf<Int32> > d = ArrayOf<Int32>::Alloc(1000);
    AutoPtr<ArrayOf<Int32> > x = ArrayOf<Int32>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::INT32_MAX_VALUE);
    ec = Arrays::Fill(x, Elastos::Core::Math::INT32_MIN_VALUE);
    Boolean result;
    ec = Arrays::EqualsInt32(d, x, &result);
    assert(!result);
    ec = Arrays::Fill(x, Elastos::Core::Math::INT32_MAX_VALUE);
    ec = Arrays::EqualsInt32(d, x, &result);
    assert(result);
    AutoPtr<ArrayOf<Int32> > it1 = ArrayOf<Int32>::Alloc(2);
    ec = Arrays::EqualsInt32(it1, NULL, &result);
    assert(!result);
    AutoPtr<ArrayOf<Int32> > it2 = ArrayOf<Int32>::Alloc(2);
    ec = Arrays::EqualsInt32(NULL, it2, &result);
    assert(!result);
    return ec;
}

// test 31
int CTest::test_equals_JJ(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(long [], long [])
    AutoPtr<ArrayOf<Int64> > d = ArrayOf<Int64>::Alloc(1000);
    AutoPtr<ArrayOf<Int64> > x = ArrayOf<Int64>::Alloc(1000);
    ec = Arrays::Fill(d, Elastos::Core::Math::INT64_MAX_VALUE);
    ec = Arrays::Fill(x, Elastos::Core::Math::INT64_MIN_VALUE);
    Boolean result;
    ec = Arrays::EqualsInt64(d, x, &result);
    assert(!result);
    ec = Arrays::Fill(x, Elastos::Core::Math::INT64_MAX_VALUE);
    ec = Arrays::EqualsInt64(d, x, &result);
    assert(result);
    AutoPtr<ArrayOf<Int64> > l1 = ArrayOf<Int64>::Alloc(1);
    l1->Set(0, 0x100000000L);
    AutoPtr<ArrayOf<Int64> > l2 = ArrayOf<Int64>::Alloc(1);
    l2->Set(0, 0x200000000L);
    ec = Arrays::EqualsInt64(l1, l2, &result);
    assert(!result);
    return ec;
}

// test 32
int CTest::test_equals_FF(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(float [], float [])
    AutoPtr<ArrayOf<Float> > d = ArrayOf<Float>::Alloc(1000);
    AutoPtr<ArrayOf<Float> > x = ArrayOf<Float>::Alloc(1000);
    ec = Arrays::FillFloat(d, Elastos::Core::Math::FLOAT_MAX_VALUE);
    ec = Arrays::FillFloat(x, Elastos::Core::Math::FLOAT_MIN_VALUE);
    Boolean result;
    ec = Arrays::EqualsFloat(d, x, &result);
    assert(!result);
    ec = Arrays::FillFloat(x, Elastos::Core::Math::FLOAT_MAX_VALUE);
    ec = Arrays::EqualsFloat(d, x, &result);
    assert(result);
    AutoPtr<ArrayOf<Float> > f1 = ArrayOf<Float>::Alloc(1);
    f1->Set(0, Elastos::Core::Math::FLOAT_NAN);
    AutoPtr<ArrayOf<Float> > f2 = ArrayOf<Float>::Alloc(1);
    f2->Set(0, Elastos::Core::Math::FLOAT_NAN);
    ec = Arrays::EqualsFloat(f1, f2, &result);
    assert(result);
    AutoPtr<ArrayOf<Float> > f3 = ArrayOf<Float>::Alloc(1);
    f3->Set(0, 0.0);
    AutoPtr<ArrayOf<Float> > f4 = ArrayOf<Float>::Alloc(1);
    f4->Set(0, -0.0);
    ec = Arrays::EqualsFloat(f3, f4, &result);
    assert(!result);
    return ec;
}

// test 33
int CTest::test_equals_DD(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(double [], double [])
    AutoPtr<ArrayOf<Double> > d = ArrayOf<Double>::Alloc(1000);
    AutoPtr<ArrayOf<Double> > x = ArrayOf<Double>::Alloc(1000);
    ec = Arrays::FillDouble(d, Elastos::Core::Math::DOUBLE_MAX_VALUE);
    ec = Arrays::FillDouble(x, Elastos::Core::Math::DOUBLE_MIN_VALUE);
    Boolean result;
    ec = Arrays::EqualsDouble(d, x, &result);
    assert(!result);
    ec = Arrays::FillDouble(x, Elastos::Core::Math::DOUBLE_MAX_VALUE);
    ec = Arrays::EqualsDouble(d, x, &result);
    assert(result);
    AutoPtr<ArrayOf<Double> > dl1 = ArrayOf<Double>::Alloc(1);
    dl1->Set(0, 1.0);
    AutoPtr<ArrayOf<Double> > dl2 = ArrayOf<Double>::Alloc(1);
    dl2->Set(0, 2.0);
    ec = Arrays::EqualsDouble(dl1, dl2, &result);
    assert(!result);
    dl1->Set(0, Elastos::Core::Math::DOUBLE_NAN);
    dl2->Set(0, Elastos::Core::Math::DOUBLE_NAN);
    ec = Arrays::EqualsDouble(dl1, dl2, &result);
    assert(result);
    dl1->Set(0, 0.0);
    dl2->Set(0, -0.0);
    ec = Arrays::EqualsDouble(dl1, dl2, &result);
    assert(!result);
    return ec;
}

// test 34
int CTest::test_equals_ZZ(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method boolean java.util.Arrays.equals(boolean [], boolean
    // [])
    AutoPtr<ArrayOf<Boolean> > d = ArrayOf<Boolean>::Alloc(1000);
    AutoPtr<ArrayOf<Boolean> > x = ArrayOf<Boolean>::Alloc(1000);
    ec = Arrays::FillBoolean(d, TRUE);
    ec = Arrays::FillBoolean(x, FALSE);
    Boolean result;
    ec = Arrays::EqualsBoolean(d, x, &result);
    assert(!result);
    ec = Arrays::FillBoolean(x, TRUE);
    ec = Arrays::EqualsBoolean(d, x, &result);
    assert(result);
    return ec;
}

// test 35
int CTest::test_equals_Ljava_lang_Object_Ljava_lang_Object(int argc, char* argv[])
{
    ECode ec = 0;
    Boolean result;
    // Test for method boolean java.util.Arrays.equals(java.lang.Object [],
    // java.lang.Object [])
    AutoPtr<ArrayOf<IInterface*> > d = ArrayOf<IInterface*>::Alloc(1000);
    AutoPtr<ArrayOf<IInterface*> > x = ArrayOf<IInterface*>::Alloc(1000);
    AutoPtr<IInterface> o;
    CInteger32::New(0, (IInteger32**)&o);
    ec = Arrays::FillObject(d, o);
    AutoPtr<IInterface> elem;
    CInteger32::New(0, (IInteger32**)&elem);
    ec = Arrays::FillObject(x, elem);
    ec = Arrays::Equals(d, x, &result);
    assert(!result);
    ec = Arrays::FillObject(x, o);
    d->Set(50, NULL);
    x->Set(50, NULL);
    ec = Arrays::Equals(d, x, &result);
    assert(result == TRUE);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_B(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(byte [])
    AutoPtr<ArrayOf<Byte> > reversedArray = ArrayOf<Byte>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (Byte) (arraySize - counter - 1));
    }
    ec = Arrays::SortByte(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (Byte) counter);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_BII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(byte [], int, int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<Byte> > reversedArray = ArrayOf<Byte>::Alloc(arraySize);
    AutoPtr<ArrayOf<Byte> > originalReversedArray = ArrayOf<Byte>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (Byte) (arraySize - counter - 1));
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    Arrays::SortByteEx(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert((*reversedArray)[counter] <= (*reversedArray)[counter + 1]);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortByteEx(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortByteEx(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortByteEx(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_C(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(char [])
    AutoPtr<ArrayOf<Char32> > reversedArray = ArrayOf<Char32>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        reversedArray->Set(counter, (Char32) (arraySize - counter - 1));
    Arrays::SortChar32(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (Char32) counter);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_CII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(char [], int, int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<Char32> > reversedArray = ArrayOf<Char32>::Alloc(arraySize);
    AutoPtr<ArrayOf<Char32> > originalReversedArray = ArrayOf<Char32>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (Char32) (arraySize - counter - 1));
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    Arrays::SortChar32Ex(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert((*reversedArray)[counter] <= (*reversedArray)[counter + 1]);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortChar32Ex(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortChar32Ex(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortChar32Ex(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_D(int argc, char* argv[])
{
    ECode ec = 0;
    Boolean result;
    // Test for method void java.util.Arrays.sort(double [])
    AutoPtr<ArrayOf<Double> > reversedArray = ArrayOf<Double>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        reversedArray->Set(counter, (Double) (arraySize - counter - 1));
    ec = Arrays::SortDouble(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (Double) counter);

    AutoPtr<ArrayOf<Double> > specials1 = ArrayOf<Double>::Alloc(7);
    specials1->Set(0, Elastos::Core::Math::DOUBLE_NAN);
    specials1->Set(1, Elastos::Core::Math::DOUBLE_MAX_VALUE);
    specials1->Set(2, Elastos::Core::Math::DOUBLE_MIN_VALUE);
    specials1->Set(3, 0);
    specials1->Set(4, -0);
    specials1->Set(5, Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY);
    specials1->Set(6, Elastos::Core::Math::DOUBLE_NEGATIVE_INFINITY);
    AutoPtr<ArrayOf<Double> > specials2 = ArrayOf<Double>::Alloc(7);
    specials2->Set(0, 0);
    specials2->Set(1, Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY);
    specials2->Set(2, -0);
    specials2->Set(3, Elastos::Core::Math::DOUBLE_NEGATIVE_INFINITY);
    specials2->Set(4, Elastos::Core::Math::DOUBLE_MIN_VALUE);
    specials2->Set(5, Elastos::Core::Math::DOUBLE_NAN);
    specials2->Set(6, Elastos::Core::Math::DOUBLE_MAX_VALUE);
    AutoPtr<ArrayOf<Double> > answer = ArrayOf<Double>::Alloc(7);
    answer->Set(0, Elastos::Core::Math::DOUBLE_NEGATIVE_INFINITY);
    answer->Set(1, 0);
    answer->Set(2, -0);
    answer->Set(3, Elastos::Core::Math::DOUBLE_MIN_VALUE);
    answer->Set(4, Elastos::Core::Math::DOUBLE_MAX_VALUE);
    answer->Set(5, Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY);
    answer->Set(6, Elastos::Core::Math::DOUBLE_NAN);

    ec = Arrays::SortDouble(specials1);
    AutoPtr<ArrayOf<IInterface*> > print1 = ArrayOf<IInterface*>::Alloc(specials1->GetLength());
    for (Int32 i = 0; i < specials1->GetLength(); i++) {
        AutoPtr<IDouble> elem;
        CDouble::New((*specials1)[i], (IDouble**)&elem);
        print1->Set(i, elem);
    }
    ec = Arrays::EqualsDouble(specials1, answer, &result);
    assert(result == TRUE);

    ec = Arrays::SortDouble(specials2);
    AutoPtr<ArrayOf<IInterface*> > print2 = ArrayOf<IInterface*>::Alloc(specials2->GetLength());
    for (Int32 i = 0; i < specials2->GetLength(); i++) {
        AutoPtr<IDouble> elem;
        CDouble::New((*specials2)[i], (IDouble**)&elem);
        print2->Set(i, elem);
    }
    ec = Arrays::EqualsDouble(specials2, answer, &result);
    assert(result == TRUE);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_DII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(double [], int, int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<Double> > reversedArray = ArrayOf<Double>::Alloc(arraySize);
    AutoPtr<ArrayOf<Double> > originalReversedArray = ArrayOf<Double>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (Double) (arraySize - counter - 1));
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    ec = Arrays::SortDoubleEx(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert((*reversedArray)[counter] <= (*reversedArray)[counter + 1]);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortDoubleEx(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortDoubleEx(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortDoubleEx(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_F(int argc, char* argv[])
{
    ECode ec = 0;
    Boolean result;
    // Test for method void java.util.Arrays.sort(float [])
    AutoPtr<ArrayOf<Float> > reversedArray = ArrayOf<Float>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        reversedArray->Set(counter, (Float) (arraySize - counter - 1));
    ec = Arrays::SortFloat(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (Float) counter);

    AutoPtr<ArrayOf<Float> > specials1 = ArrayOf<Float>::Alloc(7);
    specials1->Set(0, Elastos::Core::Math::FLOAT_NAN);
    specials1->Set(1, Elastos::Core::Math::FLOAT_MAX_VALUE);
    specials1->Set(2, Elastos::Core::Math::FLOAT_MIN_VALUE);
    specials1->Set(3, 0);
    specials1->Set(4, -0);
    specials1->Set(5, Elastos::Core::Math::FLOAT_POSITIVE_INFINITY);
    specials1->Set(6, Elastos::Core::Math::FLOAT_NEGATIVE_INFINITY);
    AutoPtr<ArrayOf<Float> > specials2 = ArrayOf<Float>::Alloc(7);
    specials2->Set(0, 0);
    specials2->Set(1, Elastos::Core::Math::FLOAT_POSITIVE_INFINITY);
    specials2->Set(2, -0);
    specials2->Set(3, Elastos::Core::Math::FLOAT_NEGATIVE_INFINITY);
    specials2->Set(4, Elastos::Core::Math::FLOAT_MIN_VALUE);
    specials2->Set(5, Elastos::Core::Math::FLOAT_NAN);
    specials2->Set(6, Elastos::Core::Math::FLOAT_MAX_VALUE);
    AutoPtr<ArrayOf<Float> > answer = ArrayOf<Float>::Alloc(7);
    answer->Set(0, Elastos::Core::Math::FLOAT_NEGATIVE_INFINITY);
    answer->Set(1, 0);
    answer->Set(2, -0);
    answer->Set(3, Elastos::Core::Math::FLOAT_MIN_VALUE);
    answer->Set(4, Elastos::Core::Math::FLOAT_MAX_VALUE);
    answer->Set(5, Elastos::Core::Math::FLOAT_POSITIVE_INFINITY);
    answer->Set(6, Elastos::Core::Math::FLOAT_NAN);

    ec = Arrays::SortFloat(specials1);
    AutoPtr<ArrayOf<IInterface*> > print1 = ArrayOf<IInterface*>::Alloc(specials1->GetLength());
    for (Int32 i = 0; i < specials1->GetLength(); i++) {
        AutoPtr<IFloat> elem;
        CFloat::New((*specials1)[i], (IFloat**)&elem);
        print1->Set(i, elem);
    }
    ec = Arrays::EqualsFloat(specials1, answer, &result);
    assert(result == TRUE);

    ec = Arrays::SortFloat(specials2);
    AutoPtr<ArrayOf<IInterface*> > print2 = ArrayOf<IInterface*>::Alloc(specials2->GetLength());
    for (Int32 i = 0; i < specials2->GetLength(); i++) {
        AutoPtr<IFloat> elem;
        CFloat::New((*specials2)[i], (IFloat**)&elem);
        print2->Set(i, elem);
    }
    ec = Arrays::EqualsFloat(specials2, answer, &result);
    assert(result == TRUE);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_FII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(float [], int, int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<Float> > reversedArray = ArrayOf<Float>::Alloc(arraySize);
    AutoPtr<ArrayOf<Float> > originalReversedArray = ArrayOf<Float>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (Float) (arraySize - counter - 1));
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    ec = Arrays::SortFloatEx(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert((*reversedArray)[counter] <= (*reversedArray)[counter + 1]);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortFloatEx(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortFloatEx(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortFloatEx(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_I(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(int [])
    AutoPtr<ArrayOf<Int32> > reversedArray = ArrayOf<Int32>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        reversedArray->Set(counter, arraySize - counter - 1);
    ec = Arrays::SortInt32(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == counter);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_III(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(int [], int, int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<Int32> > reversedArray = ArrayOf<Int32>::Alloc(arraySize);
    AutoPtr<ArrayOf<Int32> > originalReversedArray = ArrayOf<Int32>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, arraySize - counter - 1);
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    ec = Arrays::SortInt32Ex(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert((*reversedArray)[counter] <= (*reversedArray)[counter + 1]);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortInt32Ex(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortInt32Ex(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortInt32Ex(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_J(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(long [])
    AutoPtr<ArrayOf<Int64> > reversedArray = ArrayOf<Int64>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        reversedArray->Set(counter, (Int64) (arraySize - counter - 1));
    ec = Arrays::SortInt64(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (Int64) counter);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_JII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(long [], int, int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<Int64> > reversedArray = ArrayOf<Int64>::Alloc(arraySize);
    AutoPtr<ArrayOf<Int64> > originalReversedArray = ArrayOf<Int64>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (Int64) (arraySize - counter - 1));
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    ec = Arrays::SortInt64Ex(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert((*reversedArray)[counter] <= (*reversedArray)[counter + 1]);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortInt64Ex(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortInt64Ex(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortInt64Ex(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_Ljava_lang_Object(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(java.lang.Object [])
    AutoPtr<ArrayOf<IInterface*> > reversedArray = ArrayOf<IInterface*>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        reversedArray->Set(counter, (*objectArray)[arraySize - counter - 1]);
    ec = Arrays::SortObject(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*objectArray)[counter]);

    AutoPtr<ICharSequence> str;
    CString::New(String("String"), (ICharSequence**)&str);
    ec = Arrays::FillObjectEx(reversedArray, 0, reversedArray->GetLength()/2, str);
    AutoPtr<IInteger32> it;
    CInteger32::New(1, (IInteger32**)&it);
    ec = Arrays::FillObjectEx(reversedArray, reversedArray->GetLength()/2, reversedArray->GetLength(), it);

    ec = Arrays::SortObject(reversedArray);
    assert(ec != NOERROR);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_Ljava_lang_ObjectII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(java.lang.Object [], int,
    // int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<IInterface*> > reversedArray = ArrayOf<IInterface*>::Alloc(arraySize);
    AutoPtr<ArrayOf<IInterface*> > originalReversedArray = ArrayOf<IInterface*>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (*objectArray)[arraySize - counter - 1]);
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    ec = Arrays::SortObjectEx(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        // assertTrue("Array not sorted within bounds",
        //         ((Comparable) reversedArray[counter])
        //                 .compareTo(reversedArray[counter + 1]) <= 0);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortObjectEx(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortObjectEx(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortObjectEx(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    AutoPtr<ICharSequence> str;
    CString::New(String("String"), (ICharSequence**)&str);
    ec = Arrays::FillObjectEx(reversedArray, 0, reversedArray->GetLength()/2, str);
    AutoPtr<IInteger32> it;
    CInteger32::New(1, (IInteger32**)&it);
    ec = Arrays::FillObjectEx(reversedArray, reversedArray->GetLength()/2, reversedArray->GetLength(), it);

    ec = Arrays::SortObjectEx(reversedArray, reversedArray->GetLength()/4, 3*reversedArray->GetLength()/4);
    assert(ec != NOERROR);

    ec = Arrays::SortObjectEx(reversedArray, 0, reversedArray->GetLength()/4);
    ec = Arrays::SortObjectEx(reversedArray, 3*reversedArray->GetLength()/4, reversedArray->GetLength());
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_Ljava_lang_ObjectIILjava_util_Comparator(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(java.lang.Object [], int,
    // int, java.util.Comparator)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    ReversedIntegerComparator* comp/* = new ReversedIntegerComparator()*/;
    AutoPtr<ArrayOf<IInterface*> > originalArray = ArrayOf<IInterface*>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        originalArray->Set(counter, (*objectArray)[counter]);
    //ec = Arrays::SortEx(objectArray, startIndex, endIndex, comp);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*objectArray)[counter] == (*originalArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert(comp->Compare((*objectArray)[counter], (*objectArray)[counter + 1]) <= 0);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*objectArray)[counter] == (*originalArray)[counter]);
    AutoPtr<ICharSequence> str;
    CString::New(String("String"), (ICharSequence**)&str);
    ec = Arrays::FillObjectEx(originalArray, 0, originalArray->GetLength()/2, str);
    AutoPtr<IInteger32> it;
    CInteger32::New(1, (IInteger32**)&it);
    ec = Arrays::FillObjectEx(originalArray, originalArray->GetLength()/2, originalArray->GetLength(), it);

    //ec = Arrays::Sort(originalArray, startIndex, endIndex, comp);
    //assert(ec != NOERROR);

    // ec = Arrays::SortEx(originalArray, endIndex, originalArray->GetLength(), comp);

    // ec = Arrays::SortEx(originalArray, endIndex, originalArray->GetLength() + 1, comp);
    // assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    // ec = Arrays::SortEx(originalArray, -1, startIndex, comp);
    // assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    // ec = Arrays::SortEx(originalArray, originalArray->GetLength(), endIndex, comp);
    // assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_Ljava_lang_ObjectLjava_util_Comparator(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(java.lang.Object [],
    // java.util.Comparator)
    ReversedIntegerComparator* comp/* = new ReversedIntegerComparator()*/;
    //ec = Arrays::Sort(objectArray, comp);
    for (Int32 counter = 0; counter < arraySize - 1; counter++)
        assert(comp->Compare((*objectArray)[counter],
                                (*objectArray)[counter + 1]) <= 0);

    AutoPtr<ICharSequence> str;
    CString::New(String("String"), (ICharSequence**)&str);
    ec = Arrays::FillObjectEx(objectArray, 0, objectArray->GetLength()/2, str);
    AutoPtr<IInteger32> it;
    CInteger32::New(1, (IInteger32**)&it);
    ec = Arrays::FillObjectEx(objectArray, objectArray->GetLength()/2, objectArray->GetLength(), it);

    //ec = Arrays::Sort(objectArray, comp);
    //assert(ec != NOERROR);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_S(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(short [])
    AutoPtr<ArrayOf<Int16> > reversedArray = ArrayOf<Int16>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++)
        reversedArray->Set(counter, (Int16) (arraySize - counter - 1));
    ec = Arrays::SortInt16(reversedArray);
    for (Int32 counter = 0; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (Int16) counter);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_sort_SII(int argc, char* argv[])
{
    ECode ec = 0;
    // Test for method void java.util.Arrays.sort(short [], int, int)
    Int32 startIndex = arraySize / 4;
    Int32 endIndex = 3 * arraySize / 4;
    AutoPtr<ArrayOf<Int16> > reversedArray = ArrayOf<Int16>::Alloc(arraySize);
    AutoPtr<ArrayOf<Int16> > originalReversedArray = ArrayOf<Int16>::Alloc(arraySize);
    for (Int32 counter = 0; counter < arraySize; counter++) {
        reversedArray->Set(counter, (Int16) (arraySize - counter - 1));
        originalReversedArray->Set(counter, (*reversedArray)[counter]);
    }
    ec = Arrays::SortInt16Ex(reversedArray, startIndex, endIndex);
    for (Int32 counter = 0; counter < startIndex; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);
    for (Int32 counter = startIndex; counter < endIndex - 1; counter++)
        assert((*reversedArray)[counter] <= (*reversedArray)[counter + 1]);
    for (Int32 counter = endIndex; counter < arraySize; counter++)
        assert((*reversedArray)[counter] == (*originalReversedArray)[counter]);

    //exception testing
    ec = Arrays::SortInt16Ex(reversedArray, startIndex + 1, startIndex);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = Arrays::SortInt16Ex(reversedArray, -1, startIndex);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);

    ec = Arrays::SortInt16Ex(reversedArray, startIndex, reversedArray->GetLength() + 1);
    assert(ec == E_INDEX_OUT_OF_BOUNDS_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_byte_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<Byte> > byte_array_null = NULL;
    ec = Arrays::SortByte(byte_array_null);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortByteEx(byte_array_null, (Int32) -1, (Int32) 1);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_char_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<Char32> > char_array_null = NULL;
    ec = Arrays::SortChar32(char_array_null);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortChar32Ex(char_array_null, -1, 1);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_double_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<Double> > double_array_null = NULL;
    ec = Arrays::SortDouble(double_array_null);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortDoubleEx(double_array_null, -1, 1);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_float_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<Float> > float_array_null = NULL;
    ec = Arrays::SortFloat(float_array_null);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortFloatEx(float_array_null, -1, 1);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_int_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<Int32> > int_array_null = NULL;
    ec = Arrays::SortInt32(int_array_null);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortInt32Ex(int_array_null, -1, 1);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_object_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<IInterface*> > object_array_null = NULL;
    ec = Arrays::SortObject(object_array_null);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortObjectEx(object_array_null, (Int32) -1, (Int32) 1);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortEx(object_array_null, (Int32) -1, (Int32) 1, NULL);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_long_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<Int64> > long_array_null = NULL;
    ec = Arrays::SortInt64(long_array_null);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    // Regression for HARMONY-378
    ec = Arrays::SortInt64Ex(long_array_null, -1, 1);
    assert(ec == E_NULL_POINTER_EXCEPTION);
    return ec;
}

//NOT_IMPLEMENTED
int CTest::test_java_util_Arrays_sort_short_array_NPE(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<ArrayOf<Int16> > short_array_null = NULL;
    ec = Arrays::SortInt16(short_array_null);
    assert(ec != NOERROR);

    // Regression for HARMONY-378
    ec = Arrays::SortInt16Ex(short_array_null, (Int32) -1, (Int32) 1);
    assert(ec != NOERROR);
    return ec;
}

//AutoPtr<ArrayOf<Int32> > CTest::LENGTHS = { 0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 100, 1000, 10000 };

//NOT_IMPLEMENTED
int CTest::test_sort(int argc, char* argv[])
{
    // AutoPtr<IArrays> ars;
    // CArrays::AcquireSingleton((IArrays**)&ars);
    // ECode ec = 0;
    // for (Int32 len = 0; len < LENGTHS; len++) {
    //     PrimitiveTypeArrayBuilder.reset();
    //     AutoPtr<ArrayOf<Int32> > golden = ArrayOf::Alloc(len);
    //     for (Int32 m = 1; m < 2 * len; m *= 2) {
    //         for (PrimitiveTypeArrayBuilder builder : PrimitiveTypeArrayBuilder.values()) {
    //             builder.build(golden, m);
    //             Int32[] test = golden.clone();

    //             for (PrimitiveTypeConverter converter : PrimitiveTypeConverter.values()) {
    //                 Object convertedGolden = converter.convert(golden);
    //                 Object convertedTest = converter.convert(test);
    //                 sort(convertedTest);
    //                 checkSorted(convertedTest);
    //                 assert(checkSum(convertedGolden) == checkSum(convertedTest));
    //             }
    //         }
    //     }
    // }
    return 0;
}

//NOT_IMPLEMENTED
void CTest::sort(IInterface* array)
{
    // AutoPtr<IArrays> ars;
    // CArrays::AcquireSingleton((IArrays**)&ars);
    // ECode ec = 0;
    // if (array instanceof Int32[]) {
    //     Arrays.sort((Int32[]) array);
    // }
    // else if (array instanceof Int64[]) {
    //     Arrays.sort((Int64[]) array);
    // }
    // else if (array instanceof short[]) {
    //     Arrays.sort((short[]) array);
    // }
    // else if (array instanceof Byte[]) {
    //     Arrays.sort((Byte[]) array);
    // }
    // else if (array instanceof char[]) {
    //     Arrays.sort((char[]) array);
    // }
    // else if (array instanceof Float[]) {
    //     Arrays.sort((Float[]) array);
    // }
    // else if (array instanceof Double[]) {
    //     Arrays.sort((Double[]) array);
    // }
    // else {
    //     fail("Unknow type of array: " + array.getClass());
    // }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(IInterface* array)
{
    // if (array instanceof Int32[]) {
    //     checkSorted((Int32[]) array);
    // }
    // else if (array instanceof Int64[]) {
    //     checkSorted((Int64[]) array);
    // }
    // else if (array instanceof short[]) {
    //     checkSorted((short[]) array);
    // }
    // else if (array instanceof Byte[]) {
    //     checkSorted((Byte[]) array);
    // }
    // else if (array instanceof char[]) {
    //     checkSorted((char[]) array);
    // }
    // else if (array instanceof Float[]) {
    //     checkSorted((Float[]) array);
    // }
    // else if (array instanceof Double[]) {
    //     checkSorted((Double[]) array);
    // }
    // else {
    //     fail("Unknow type of array: " + array.getClass());
    // }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(ArrayOf<Int32>* a)
{
    for (Int32 i = 0; i < a->GetLength() - 1; i++) {
        if ((*a)[i] > (*a)[i + 1]) {
            //orderFail(i, "" + (*a)[i], "" + (*a)[i + 1]);
            assert(FALSE);
        }
    }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(ArrayOf<Int64>* a)
{
    for (Int32 i = 0; i < a->GetLength() - 1; i++) {
        if ((*a)[i] > (*a)[i + 1]) {
            //orderFail(i, "" + (*a)[i], "" + (*a)[i + 1]);
            assert(FALSE);
        }
    }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(ArrayOf<Int16>* a)
{
    for (Int32 i = 0; i < a->GetLength() - 1; i++) {
        if ((*a)[i] > (*a)[i + 1]) {
            //orderFail(i, "" + (*a)[i], "" + (*a)[i + 1]);
            assert(FALSE);
        }
    }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(ArrayOf<Byte>* a)
{
    for (Int32 i = 0; i < a->GetLength() - 1; i++) {
        if ((*a)[i] > (*a)[i + 1]) {
            //orderFail(i, "" + (*a)[i], "" + (*a)[i + 1]);
            assert(FALSE);
        }
    }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(ArrayOf<Char32>* a)
{
    for (Int32 i = 0; i < a->GetLength() - 1; i++) {
        if ((*a)[i] > (*a)[i + 1]) {
            //orderFail(i, "" + (*a)[i], "" + (*a)[i + 1]);
            assert(FALSE);
        }
    }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(ArrayOf<Float>* a)
{
    for (Int32 i = 0; i < a->GetLength() - 1; i++) {
        if ((*a)[i] > (*a)[i + 1]) {
            assert(FALSE);
        }
    }
}

//NOT_IMPLEMENTED
void CTest::checkSorted(ArrayOf<Double>* a)
{
    for (Int32 i = 0; i < a->GetLength() - 1; i++) {
        if ((*a)[i] > (*a)[i + 1]) {
            //orderFail(i, "" + (*a)[i], "" + (*a)[i + 1]);
            assert(FALSE);
        }
    }
}

//NOT_IMPLEMENTED
void CTest::orderFail(Int32 index, String value1, String value2)
{
    //fail("Array is not sorted at " + index + "-th position: " + value1 + " and " + value2);
}

Int32 CTest::checkSum(IInterface* array)
{
    // if (array instanceof Int32[]) {
    //     return checkSum((Int32[]) array);
    // }
    // else if (array instanceof Int64[]) {
    //     return checkSum((Int64[]) array);
    // }
    // else if (array instanceof short[]) {
    //     return checkSum((Int16[]) array);
    // }
    // else if (array instanceof Byte[]) {
    //     return checkSum((Byte[]) array);
    // }
    // else if (array instanceof char[]) {
    //     return checkSum((Char32[]) array);
    // }
    // else if (array instanceof Float[]) {
    //     return checkSum((Float[]) array);
    // }
    // else if (array instanceof Double[]) {
    //     return checkSum((Double[]) array);
    // }
    // else {
    //     fail("Unknow type of array: " + array.getClass());
    // }
    // throw new AssertionError(); // Needed to shut up compiler
    return 0;
}

Int32 CTest::checkSum(ArrayOf<Int32>* a)
{
    Int32 checkSum = 0;

    for (Int32 i = 0;i < a->GetLength();i++) {
        Int32 e = (*a)[i];
        checkSum ^= e; // xor
    }
    return checkSum;
}

Int32 CTest::checkSum(ArrayOf<Int64>* a)
{
    Int64 checkSum = 0;

    for (Int32 i = 0;i < a->GetLength();i++) {
        Int64 e = (*a)[i];
        checkSum ^= e; // xor
    }
    return (Int32) checkSum;
}

Int32 CTest::checkSum(ArrayOf<Int16>* a)
{
    Int16 checkSum = 0;

    for (Int32 i = 0;i < a->GetLength();i++) {
        Int16 e = (*a)[i];
        checkSum ^= e; // xor
    }
    return (Int32) checkSum;
}

Int32 CTest::checkSum(ArrayOf<Byte>* a)
{
    Byte checkSum = 0;

    for (Int32 i = 0;i < a->GetLength();i++) {
        Byte e = (*a)[i];
        checkSum ^= e; // xor
    }
    return (Int32) checkSum;
}

Int32 CTest::checkSum(ArrayOf<Char32>* a)
{
    Float checkSum = 0;

    for (Int32 i = 0;i < a->GetLength();i++) {
        Char32 e = (*a)[i];
        checkSum = (Int32) checkSum ^ e; // xor
    }
    return (Int32) checkSum;
}

Int32 CTest::checkSum(ArrayOf<Float>* a)
{
    Int32 checkSum = 0;

    for (Int32 i = 0;i < a->GetLength();i++) {
        Float e = (*a)[i];
        checkSum ^= (Int32) e; // xor
    }
    return checkSum;
}

Int32 CTest::checkSum(ArrayOf<Double>* a)
{
    Int32 checkSum = 0;

    for (Int32 i = 0;i < a->GetLength();i++) {
        Double e = (*a)[i];
        checkSum ^= (Int32) e; // xor
    }
    return checkSum;
}

// private enum PrimitiveTypeArrayBuilder {

//     RANDOM {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = ourRandom.nextInt();
//             }
//         }
//     },

//     ASCENDING {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = m + i;
//             }
//         }
//     },

//     DESCENDING {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = a->GetLength() - m - i;
//             }
//         }
//     },

//     ALL_EQUAL {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = m;
//             }
//         }
//     },

//     SAW {
//         void build(Int32[] a, Int32 m) {
//             Int32 incCount = 1;
//             Int32 decCount = a->GetLength();
//             Int32 i = 0;
//             Int32 period = m;
//             m--;

//             while (true) {
//                 for (Int32 k = 1; k <= period; k++) {
//                     if (i >= a->GetLength()) {
//                         return;
//                     }
//                     a[i++] = incCount++;
//                 }
//                 period += m;

//                 for (Int32 k = 1; k <= period; k++) {
//                     if (i >= a->GetLength()) {
//                         return;
//                     }
//                     a[i++] = decCount--;
//                 }
//                 period += m;
//             }
//         }
//     },

//     REPEATED {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = i % m;
//             }
//         }
//     },

//     DUPLICATED {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = ourRandom.nextInt(m);
//             }
//         }
//     },

//     ORGAN_PIPES {
//         void build(Int32[] a, Int32 m) {
//             Int32 middle = a->GetLength() / (m + 1);

//             for (Int32 i = 0; i < middle; i++) {
//                 a[i] = i;
//             }
//             for (Int32 i = middle; i < a->GetLength() ; i++) {
//                 a[i] = a->GetLength() - i - 1;
//             }
//         }
//     },

//     STAGGER {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = (i * m + i) % a->GetLength();
//             }
//         }
//     },

//     PLATEAU {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] =  Math.min(i, m);
//             }
//         }
//     },

//     SHUFFLE {
//         void build(Int32[] a, Int32 m) {
//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 a[i] = ourRandom.nextBoolean() ? (ourFirst += 2) : (ourSecond += 2);
//             }
//         }
//     };

//     abstract void build(Int32[] a, Int32 m);

//     static void reset() {
//         ourRandom = new Random(666);
//         ourFirst = 0;
//         ourSecond = 0;
//     }

//     @Override
//     public String toString() {
//         String name = name();

//         for (Int32 i = name->GetLength()(); i < 12; i++) {
//             name += " " ;
//         }
//         return name;
//     }

//     private static Int32 ourFirst;
//     private static Int32 ourSecond;
//     private static Random ourRandom = new Random(666);
// }

// private enum PrimitiveTypeConverter {

//     INT {
//         Object convert(Int32[] a) {
//             return a;
//         }
//     },

//     LONG {
//         Object convert(Int32[] a) {
//             Int64[] b = new Int64[a->GetLength()];

//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 b[i] = (Int32) a[i];
//             }
//             return b;
//         }
//     },

//     BYTE {
//         Object convert(Int32[] a) {
//             Byte[] b = new Byte[a->GetLength()];

//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 b[i] = (Byte) a[i];
//             }
//             return b;
//         }
//     },

//     SHORT {
//         Object convert(Int32[] a) {
//             short[] b = new short[a->GetLength()];

//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 b[i] = (short) a[i];
//             }
//             return b;
//         }
//     },

//     CHAR {
//         Object convert(Int32[] a) {
//             char[] b = new char[a->GetLength()];

//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 b[i] = (char) a[i];
//             }
//             return b;
//         }
//     },

//     FLOAT {
//         Object convert(Int32[] a) {
//             Float[] b = new Float[a->GetLength()];

//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 b[i] = (Float) a[i];
//             }
//             return b;
//         }
//     },

//     DOUBLE {
//         Object convert(Int32[] a) {
//             Double[] b = new Double[a->GetLength()];

//             for (Int32 i = 0; i < a->GetLength(); i++) {
//                 b[i] = (Double) a[i];
//             }
//             return b;
//         }
//     };

//     abstract Object convert(Int32[] a);

//     public String toString() {
//         String name = name();

//         for (Int32 i = name->GetLength()(); i < 9; i++) {
//             name += " " ;
//         }
//         return name;
//     }
// }

// test 36
int CTest::test_deepEquals_Ljava_lang_ObjectLjava_lang_Object(int argc, char* argv[])
{
    ECode ec = 0;
    Boolean result;
    AutoPtr<IArrayOf> a1;
    CArrayOf::New(EIID_IInteger32, 3, (IArrayOf**)&a1);
    AutoPtr<IInteger32> num0;
    CInteger32::New(1, (IInteger32**)&num0);
    AutoPtr<IInteger32> num1;
    CInteger32::New(2, (IInteger32**)&num1);
    AutoPtr<IInteger32> num2;
    CInteger32::New(3, (IInteger32**)&num2);
    a1->Put(0, num0);
    a1->Put(1, num1);
    a1->Put(2, num2);

    AutoPtr<IArrayOf> a2;
    CArrayOf::New(EIID_IInteger16, 2, (IArrayOf**)&a2);
    AutoPtr<IInteger16> num3;
    CInteger16::New(0, (IInteger16**)&num3);
    AutoPtr<IInteger16> num4;
    CInteger16::New(1, (IInteger16**)&num4);
    a2->Put(0, num3);
    a2->Put(1, num4);

    AutoPtr<IInteger32> num5;
    CInteger32::New(1, (IInteger32**)&num5);
    AutoPtr<IArrayOf> a3;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&a3);
    a3->Put(0, num5);
    a3->Put(1, a2);

    AutoPtr<IArrayOf> a4;
    CArrayOf::New(EIID_IInteger32, 3, (IArrayOf**)&a4);
    AutoPtr<IInteger32> num6;
    CInteger32::New(6, (IInteger32**)&num6);
    AutoPtr<IInteger32> num7;
    CInteger32::New(5, (IInteger32**)&num7);
    AutoPtr<IInteger32> num8;
    CInteger32::New(4, (IInteger32**)&num8);
    a4->Put(0, num6);
    a4->Put(1, num7);
    a4->Put(2, num8);

    AutoPtr<IArrayOf> b1;
    CArrayOf::New(EIID_IInteger32, 3, (IArrayOf**)&b1);
    AutoPtr<IInteger32> num9;
    CInteger32::New(1, (IInteger32**)&num9);
    AutoPtr<IInteger32> num10;
    CInteger32::New(2, (IInteger32**)&num10);
    AutoPtr<IInteger32> num11;
    CInteger32::New(3, (IInteger32**)&num11);
    b1->Put(0, num9);
    b1->Put(1, num10);
    b1->Put(2, num11);

    AutoPtr<IArrayOf> b2;
    CArrayOf::New(EIID_IInteger16, 2, (IArrayOf**)&b2);
    AutoPtr<IInteger16> num12;
    CInteger16::New(0, (IInteger16**)&num12);
    AutoPtr<IInteger16> num13;
    CInteger16::New(1, (IInteger16**)&num13);
    b2->Put(0, num12);
    b2->Put(1, num13);

    AutoPtr<IArrayOf> b3;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&b3);
    b3->Put(0, num5);
    b3->Put(1, b2);

    AutoPtr<ArrayOf<IInterface*> > a = ArrayOf<IInterface*>::Alloc(3);
    a->Set(0, a1);
    a->Set(1, a2);
    a->Set(2, a3);
    AutoPtr<ArrayOf<IInterface*> > b = ArrayOf<IInterface*>::Alloc(3);
    b->Set(0, b1);
    b->Set(1, b2);
    b->Set(2, b3);

    ec = Arrays::Equals(a, b, &result);
    assert(result == FALSE);
    ec = Arrays::DeepEquals(a, b, &result);
    assert(result == TRUE);

    a->Set(2, a4);
    ec = Arrays::DeepEquals(a, b, &result);
    assert(result == FALSE);
    return ec;
}

// test 37
int CTest::test_deepHashCode_Ljava_lang_Object(int argc, char* argv[])
{
    ECode ec = 0;
    AutoPtr<IArrayOf> a1;
    CArrayOf::New(EIID_IInteger32, 3, (IArrayOf**)&a1);
    AutoPtr<IInteger32> num0;
    CInteger32::New(1, (IInteger32**)&num0);
    AutoPtr<IInteger32> num1;
    CInteger32::New(2, (IInteger32**)&num1);
    AutoPtr<IInteger32> num2;
    CInteger32::New(3, (IInteger32**)&num2);
    a1->Put(0, num0);
    a1->Put(1, num1);
    a1->Put(2, num2);
    AutoPtr<IArrayOf> a2;
    CArrayOf::New(EIID_IInteger16, 2, (IArrayOf**)&a2);
    AutoPtr<IInteger16> num3;
    CInteger16::New(0, (IInteger16**)&num3);
    AutoPtr<IInteger16> num4;
    CInteger16::New(1, (IInteger16**)&num4);
    a2->Put(0, num3);
    a2->Put(1, num4);
    AutoPtr<IInteger32> num5;
    CInteger32::New(1, (IInteger32**)&num5);
    AutoPtr<IArrayOf> a3;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&a3);
    a3->Put(0, num5);
    a3->Put(1, a2);

    AutoPtr<IArrayOf> b1;
    CArrayOf::New(EIID_IInteger32, 3, (IArrayOf**)&b1);
    AutoPtr<IInteger32> num9;
    CInteger32::New(1, (IInteger32**)&num9);
    AutoPtr<IInteger32> num10;
    CInteger32::New(2, (IInteger32**)&num10);
    AutoPtr<IInteger32> num11;
    CInteger32::New(3, (IInteger32**)&num11);
    b1->Put(0, num9);
    b1->Put(1, num10);
    b1->Put(2, num11);

    AutoPtr<IArrayOf> b2;
    CArrayOf::New(EIID_IInteger16, 2, (IArrayOf**)&b2);
    AutoPtr<IInteger16> num12;
    CInteger16::New(0, (IInteger16**)&num12);
    AutoPtr<IInteger16> num13;
    CInteger16::New(1, (IInteger16**)&num13);
    b2->Put(0, num12);
    b2->Put(1, num13);

    AutoPtr<IArrayOf> b3;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&b3);
    b3->Put(0, num5);
    b3->Put(1, b2);
    AutoPtr<ArrayOf<IInterface*> > a = ArrayOf<IInterface*>::Alloc(3);
    a->Set(0, a1);
    a->Set(1, a2);
    a->Set(2, a3);
    AutoPtr<ArrayOf<IInterface*> > b = ArrayOf<IInterface*>::Alloc(3);
    b->Set(0, b1);
    b->Set(1, b2);
    b->Set(2, b3);
    Int32 deep_hash_a;
    Arrays::DeepHashCode(a, &deep_hash_a);
    Int32 deep_hash_b;
    Arrays::DeepHashCode(b, &deep_hash_b);

    assert(deep_hash_a == deep_hash_b);
    return ec;
 }

// test 38
int CTest::test_hashCode_LZ(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Boolean> > boolArr = ArrayOf<Boolean>::Alloc(5);
    boolArr->Set(0, TRUE);
    boolArr->Set(1, FALSE);
    boolArr->Set(2, FALSE);
    boolArr->Set(3, TRUE);
    boolArr->Set(4, FALSE);
    AutoPtr<IList> listOfBoolean;
    CLinkedList::New((ILinkedList**)&listOfBoolean);
    for (Int32 i = 0; i < boolArr->GetLength(); i++) {
        AutoPtr<IBoolean> num;
        CBoolean::New((*boolArr)[i], (IBoolean**)&num);
        Boolean result  = FALSE;
        ec = listOfBoolean->Add(num, &result);
        assert(result);
    }
    ec = listOfBoolean->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(boolArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);
    return ec;
}

// test 39
int CTest::test_hashCode_LI(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Int32> > intArr = ArrayOf<Int32>::Alloc(5);
    intArr->Set(0, 10);
    intArr->Set(1, 5);
    intArr->Set(2, 134);
    intArr->Set(3, 7);
    intArr->Set(4, 19);
    AutoPtr<IList> listOfInteger;
    CLinkedList::New((ILinkedList**)&listOfInteger);
    for (Int32 i = 0; i < intArr->GetLength(); i++) {
        AutoPtr<IInteger32> num;
        CInteger32::New((*intArr)[i], (IInteger32**)&num);
        Boolean result  = FALSE;
        ec = listOfInteger->Add(num, &result);
        assert(result);
    }
    ec = listOfInteger->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(intArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);

    AutoPtr<ArrayOf<Int32> > intArr2 = ArrayOf<Int32>::Alloc(5);
    intArr2->Set(0, 10);
    intArr2->Set(1, 5);
    intArr2->Set(2, 134);
    intArr2->Set(3, 7);
    intArr2->Set(4, 19);
    Int32 codeArr2, codeArr;
    ec = Arrays::GetHashCode(intArr2, &codeArr2);
    ec = Arrays::GetHashCode(intArr, &codeArr);
    assert(codeArr2 == codeArr);
    return ec;
}

// test 40
int CTest::test_hashCode_LC(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Char32> > charArr = ArrayOf<Char32>::Alloc(5);
    charArr->Set(0, 'a');
    charArr->Set(1, 'g');
    charArr->Set(2, 'x');
    charArr->Set(3, 'c');
    charArr->Set(4, 'm');
    AutoPtr<IList> listOfCharacter;
    CLinkedList::New((ILinkedList**)&listOfCharacter);
    for (Int32 i = 0; i < charArr->GetLength(); i++) {
        AutoPtr<IChar32> num;
        CChar32::New((*charArr)[i], (IChar32**)&num);
        Boolean result  = FALSE;
        ec = listOfCharacter->Add(num, &result);
        assert(result);
    }
    ec = listOfCharacter->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(charArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);
    return ec;
}

// test 41
int CTest::test_hashCode_LB(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Byte> > byteArr = ArrayOf<Byte>::Alloc(5);
    byteArr->Set(0, 5);
    byteArr->Set(1, 9);
    byteArr->Set(2, 7);
    byteArr->Set(3, 6);
    byteArr->Set(4, 17);
    AutoPtr<IList> listOfByte;
    CLinkedList::New((ILinkedList**)&listOfByte);
    for (Int32 i = 0; i < byteArr->GetLength(); i++) {
        AutoPtr<IByte> num;
        CByte::New((*byteArr)[i], (IByte**)&num);
        Boolean result  = FALSE;
        ec = listOfByte->Add(num, &result);
        assert(result);
    }
    ec = listOfByte->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(byteArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);
    return ec;
}

// test 42
int CTest::test_hashCode_LJ(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Int64> > longArr = ArrayOf<Int64>::Alloc(5);
    longArr->Set(0, 67890234512l);
    longArr->Set(1, 97587236923425l);
    longArr->Set(2, 257421912912l);
    longArr->Set(3, 6754268100l);
    longArr->Set(4, 5);
    AutoPtr<IList> listOfLong;
    CLinkedList::New((ILinkedList**)&listOfLong);
    for (Int32 i = 0; i < longArr->GetLength(); i++) {
        AutoPtr<IInteger64> num;
        CInteger64::New((*longArr)[i], (IInteger64**)&num);
        Boolean result  = FALSE;
        ec = listOfLong->Add(num, &result);
        assert(result);
    }
    ec = listOfLong->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(longArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);
    return ec;
}

// test 43
int CTest::test_hashCode_LF(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Float> > floatArr = ArrayOf<Float>::Alloc(5);
    floatArr->Set(0, 0.13497f);
    floatArr->Set(1, 0.268934f);
    floatArr->Set(2, 12e-5f);
    floatArr->Set(3, -3e+2f);
    floatArr->Set(4, 10e-4f);
    AutoPtr<IList> listOfFloat;
    CLinkedList::New((ILinkedList**)&listOfFloat);
    for (Int32 i = 0; i < floatArr->GetLength(); i++) {
        AutoPtr<IFloat> num;
        CFloat::New((*floatArr)[i], (IFloat**)&num);
        Boolean result  = FALSE;
        ec = listOfFloat->Add(num, &result);
        assert(result);
    }
    ec = listOfFloat->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(floatArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);

    AutoPtr<ArrayOf<Float> > floatArr2 = ArrayOf<Float>::Alloc(5);
    floatArr->Set(0, 0.13497f);
    floatArr->Set(1, 0.268934f);
    floatArr->Set(2, 12e-5f);
    floatArr->Set(3, -3e+2f);
    floatArr->Set(4, 10e-4f);
    Int32 codeArr2, codeArr;
    ec = Arrays::GetHashCode(floatArr2, &codeArr2);
    ec = Arrays::GetHashCode(floatArr, &codeArr);
    assert(codeArr2 == codeArr);
    return ec;
}

// test 44
int CTest::test_hashCode_LD(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Double> > doubleArr = ArrayOf<Double>::Alloc(5);
    doubleArr->Set(0, 0.134945657);
    doubleArr->Set(1, 0.0038754);
    doubleArr->Set(2, 11e-150);
    doubleArr->Set(3, -30e-300);
    doubleArr->Set(4, 10e-4);
    AutoPtr<IList> listOfDouble;
    CLinkedList::New((ILinkedList**)&listOfDouble);
    for (Int32 i = 0; i < doubleArr->GetLength(); i++) {
        AutoPtr<IDouble> num;
        CDouble::New((*doubleArr)[i], (IDouble**)&num);
        Boolean result  = FALSE;
        ec = listOfDouble->Add(num, &result);
        assert(result);
    }
    ec = listOfDouble->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(doubleArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);
    return ec;
}

// test 45
int CTest::test_hashCode_LS(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<Int16> > shortArr = ArrayOf<Int16>::Alloc(5);
    shortArr->Set(0, 35);
    shortArr->Set(1, 13);
    shortArr->Set(2, 45);
    shortArr->Set(3, 2);
    shortArr->Set(4, 91);
    AutoPtr<IList> listOfShort;
    CLinkedList::New((ILinkedList**)&listOfShort);
    for (Int32 i = 0; i < shortArr->GetLength(); i++) {
        AutoPtr<IInteger16> num;
        CInteger16::New((*shortArr)[i], (IInteger16**)&num);
        Boolean result  = FALSE;
        ec = listOfShort->Add(num, &result);
        assert(result);
    }
    ec = listOfShort->GetHashCode(&listHashCode);
    ec = Arrays::GetHashCode(shortArr, &arrayHashCode);
    assert(listHashCode == arrayHashCode);
    return ec;
}

// test 46
int CTest::test_hashCode_Ljava_lang_Object(int argc, char* argv[])
{
    ECode ec = 0;
    Int32 listHashCode;
    Int32 arrayHashCode;

    AutoPtr<ArrayOf<IInterface*> > objectArr = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<IInteger32> mem1;
    CInteger32::New(1, (IInteger32**)&mem1);
    AutoPtr<IFloat> mem2;
    CFloat::New(10e-12f, (IFloat**)&mem2);
    objectArr->Set(0, mem1);
    objectArr->Set(1, mem2);
    objectArr->Set(2, NULL);
    AutoPtr<IList> listOfObject;
    CLinkedList::New((ILinkedList**)&listOfObject);
    Boolean result;
    ec = listOfObject->Add(mem1, &result);
    assert(result);
    ec = listOfObject->Add(mem2, &result);
    assert(result);
    ec = listOfObject->Add(NULL, &result);
    assert(result);
    ec = listOfObject->GetHashCode(&listHashCode);
    arrayHashCode = Arrays::Get>HashCode(objectArr);
    assert(listHashCode == arrayHashCode);
    return ec;
}

void CTest::setUp()
{
    objArray = ArrayOf<IInterface*>::Alloc(arraySize);
    for (Int32 i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<IInteger32> num;
        CInteger32::New(i, (IInteger32**)&num);
        objArray->Set(i, num);
    }

    booleanArray = ArrayOf<Boolean>::Alloc(arraySize);
    byteArray = ArrayOf<Byte>::Alloc(arraySize);
    charArray = ArrayOf<Char32>::Alloc(arraySize);
    doubleArray = ArrayOf<Double>::Alloc(arraySize);
    floatArray = ArrayOf<Float>::Alloc(arraySize);
    intArray = ArrayOf<Int32>::Alloc(arraySize);
    longArray = ArrayOf<Int64>::Alloc(arraySize);
    objectArray = ArrayOf<IInterface*>::Alloc(arraySize);
    shortArray = ArrayOf<Int16>::Alloc(arraySize);

    for (Int32 counter = 0; counter < arraySize; counter++) {
        byteArray->Set(counter, (Byte) counter);
        charArray->Set(counter, (Char32) (counter + 1));
        doubleArray->Set(counter, counter);
        floatArray->Set(counter, counter);
        intArray->Set(counter, counter);
        longArray->Set(counter, counter);
        objectArray->Set(counter, (*objArray)[counter]);
        shortArray->Set(counter, (Int16) counter);
    }
    for (Int32 counter = 0; counter < arraySize; counter += 2) {
        booleanArray->Set(counter, FALSE);
        booleanArray->Set(counter + 1, TRUE);
    }
}

void CTest::tearDown()
{
    objArray = NULL;
    booleanArray = NULL;
    byteArray = NULL;
    charArray = NULL;
    doubleArray = NULL;
    floatArray = NULL;
    intArray = NULL;
    longArray = NULL;
    objectArray = NULL;
    shortArray = NULL;
}
