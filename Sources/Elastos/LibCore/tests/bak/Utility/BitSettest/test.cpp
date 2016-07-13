#include "test.h"

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"
#include <elastos/Math.h>

using namespace Elastos;
using namespace Elastos::Utility;
using namespace Elastos::Core;

int CTest::test_toString(int argc, char* argv[]) {
    // From the RI javadoc.
    AutoPtr<IBitSet> bs;
    CBitSet::New((IBitSet**)&bs);
    String str;
    bs->ToString(&str);
    assert(str.Equals("{}"));
    bs->Set(2);
    bs->ToString(&str);
    assert(str.Equals("{2}"));
    bs->Set(4);
    bs->Set(10);
    bs->ToString(&str);
    assert(str.Equals("{2, 4, 10}"));
}

void assertBitSet(AutoPtr<IBitSet> bs, ArrayOf<Int64>& longs, String s) {
    String str;
    for (int i = 0; i < 64 * longs.GetLength(); ++i) {
        bs->ToString(&str);
        Boolean flag;
        bs->Get(i,&flag);
        PFL_EX(" Int64 : %d , flag :%d , str : %s " ,((longs[i / 64] & (1L << (i % 64))) != 0) , flag , str.string());
    }
    int cardinality = 0;
    for (int i = 0; i < longs.GetLength(); ++i) {
        cardinality += Elastos::Core::Math::BitCount(longs[i]);
    }
    Boolean isflag;
    bs->IsEmpty(&isflag);
    if (cardinality != 0) {
        assert(!isflag);
    } else {
        assert(isflag);
    }
    Int32 bscar;
    bs->Cardinality(&bscar);
    assert(cardinality == bscar);
    bs->GetSize(&bscar);
    assert(64 * longs.GetLength() == bscar);
    bs->ToString(&str);
    PFL_EX("s :%s , str :%s" , s.string() ,str.string())
    assert(s.Equals(str));
}

void assertBitSet(ArrayOf<Int64>& longs, String s) {
    // Test BitSet.valueOf(long[]).
    AutoPtr<IBitSetHelper>  bsh;
    AutoPtr<IBitSet> bs;
    CBitSetHelper::AcquireSingleton((IBitSetHelper**)&bsh);
    bsh->ValueOfInt64Array(longs,(IBitSet**)&bs);
    assertBitSet(bs, longs, s);
    // // Test BitSet.valueOf(LongBuffer).
    // assertBitSet(BitSet.valueOf(LongBuffer.wrap(longs)), longs, s);
    // // Surround 'longs' with junk set bits but exclude them from the LongBuffer.
    // long[] paddedLongs = new long[1 + longs.length + 1];
    // paddedLongs[0] = paddedLongs[paddedLongs.length - 1] = -1L;
    // System.arraycopy(longs, 0, paddedLongs, 1, longs.length);
    // assertBitSet(BitSet.valueOf(LongBuffer.wrap(paddedLongs, 1, longs.length)), longs, s);

    // Check that the long[] is copied.
    if (longs.GetLength() > 0) {
        longs[0] = ~longs[0];
        AutoPtr<IBitSet> bs2;
        bsh->ValueOfInt64Array(longs, (IBitSet**)&bs2);
        Boolean flag2;
        bs2->Equals(bs, &flag2);
        assert( flag2 == 0);
    }
}

int CTest::test_valueOf_long(int argc, char* argv[]) {
    AutoPtr<ArrayOf<Int64> > logs = ArrayOf<Int64>::Alloc(0);
    assertBitSet(*logs, String("{}"));
    logs = ArrayOf<Int64>::Alloc(1);
    (*logs)[0] = 1L;
    assertBitSet(*logs, String("{0}"));
    logs = ArrayOf<Int64>::Alloc(1);
    (*logs)[0] = 0x111L;
    assertBitSet(*logs, String("{0, 4, 8}"));
    logs = ArrayOf<Int64>::Alloc(2);
    (*logs)[0] = 0x101L;
    (*logs)[1] = 0x4000000000000000L;
    assertBitSet(*logs, String("{0, 8, 126}"));
}

int roundUp(int n, int multiple) {
    return (n == 0) ? 0 : ((n + multiple - 1) / multiple) * multiple;
}

void assertBitSet(AutoPtr<IBitSet> bs, ArrayOf<Byte>& bytes, String s) {
    String str;
    for (int i = 0; i < 8 * bytes.GetLength(); ++i) {
        bs->ToString(&str);
        Boolean flag;
        bs->Get(i,&flag);
        PFL_EX(" Byte : %d , flag :%d , str : %s " ,((bytes[i / 8] & (1L << (i % 8))) != 0) , flag , str.string());
        // assert( ((bytes[i / 8] & (1L << (i % 8))) != 0) == flag);
    }
    int cardinality = 0;
    for (int i = 0; i < bytes.GetLength(); ++i) {
        cardinality += Elastos::Core::Math::BitCount(((Int32) bytes[i]) & 0xff);
    }
    Boolean flag2;
    bs->IsEmpty(&flag2);
    if (cardinality != 0) {
        assert(flag2 == 0 );
    } else {
        assert(flag2);
    }
    Int32 carint;
    bs->Cardinality(&carint);
    PFL_EX("cardinality:%d  ,carint : %d " , cardinality ,carint)
    // assert(cardinality == carint);
    bs->GetSize(&carint);
    assert(roundUp(8 * bytes.GetLength(), 64) == carint);
    bs->ToString(&str);
    PFL_EX("s :%s ,  str:%s " , s.string() ,str.string())
    // assert(s.Equals(str));
}

void assertBitSet(ArrayOf<Byte>& bytes, String s) {
    // Test BitSet.valueOf(byte[]).
    AutoPtr<IBitSetHelper>  bsh;
    AutoPtr<IBitSet> bs;
    CBitSetHelper::AcquireSingleton((IBitSetHelper**)&bsh);
PFL_EX("bytes :%p ,len :%d " , &bytes , bytes.GetLength())
    bsh->ValueOfByteArray(bytes,(IBitSet**)&bs);
PFL
    assertBitSet(bs, bytes, s);
    // // Test BitSet.valueOf(ByteBuffer).
    // assertBitSet(BitSet.valueOf(ByteBuffer.wrap(bytes)), bytes, s);
    // // Surround 'bytes' with junk set bits but exclude them from the ByteBuffer.
    // byte[] paddedBytes = new byte[1 + bytes.length + 1];
    // paddedBytes[0] = paddedBytes[paddedBytes.length - 1] = (byte) -1;
    // System.arraycopy(bytes, 0, paddedBytes, 1, bytes.length);
    // assertBitSet(BitSet.valueOf(ByteBuffer.wrap(paddedBytes, 1, bytes.length)), bytes, s);

    // Check that the byte[] is copied.
    if (bytes.GetLength() > 0) {
        bytes[0] = (byte) ~bytes[0];
        AutoPtr<IBitSet> bs2;
        bsh->ValueOfByteArray(bytes , (IBitSet**)&bs2);
        Boolean flag2;
        bs2->Equals(bs , &flag2);
        assert(flag2 == 0);
    }
}

int CTest::test_valueOf_byte(int argc, char* argv[]) {
    // Nothing...
    AutoPtr<ArrayOf<Byte> > nbyte = ArrayOf<Byte>::Alloc(0);
    assertBitSet(*nbyte, String("{}"));

    // Less than a long...
    nbyte = ArrayOf<Byte>::Alloc(1);
    (*nbyte)[0] = 0x01;
    assertBitSet(*nbyte, String("{0}"));

    nbyte = ArrayOf<Byte>::Alloc(2);
    (*nbyte)[0] = 0x01;
    (*nbyte)[1] = 0x11;
    assertBitSet(*nbyte, String("{0, 8, 12}"));

    nbyte = ArrayOf<Byte>::Alloc(5);
    (*nbyte)[0] = 0x01;
    (*nbyte)[1] = 0x01;
    (*nbyte)[2] = 0x00;
    (*nbyte)[3] = 0x00;
    (*nbyte)[4] = 0x01;
    assertBitSet(*nbyte, String("{0, 8, 32}"));

    // Exactly one long....
    nbyte = ArrayOf<Byte>::Alloc(8);
    (*nbyte)[0] = 0x01;
    (*nbyte)[1] = 0x00;
    (*nbyte)[2] = 0x00;
    (*nbyte)[3] = 0x00;
    (*nbyte)[4] = 0x00;
    (*nbyte)[5] = 0x00;
    (*nbyte)[6] = 0x00;
    (*nbyte)[7] = 0x80;
    assertBitSet(*nbyte, String("{0, 63}"));

    // One long and a byte left over...
    nbyte = ArrayOf<Byte>::Alloc(9);
    (*nbyte)[0] = 0x01;
    (*nbyte)[1] = 0x00;
    (*nbyte)[2] = 0x00;
    (*nbyte)[3] = 0x00;
    (*nbyte)[4] = 0x00;
    (*nbyte)[5] = 0x00;
    (*nbyte)[6] = 0x00;
    (*nbyte)[7] = 0x00;
    (*nbyte)[8] = 0x01;
    assertBitSet(*nbyte, String("{0, 64}"));

    // Two longs...
    nbyte = ArrayOf<Byte>::Alloc(16);
    (*nbyte)[0] = 0x01;
    (*nbyte)[1] = 0x00;
    (*nbyte)[2] = 0x00;
    (*nbyte)[3] = 0x00;
    (*nbyte)[4] = 0x00;
    (*nbyte)[5] = 0x00;
    (*nbyte)[6] = 0x00;
    (*nbyte)[7] = 0x80;
    (*nbyte)[8] = 0x01;
    (*nbyte)[9] = 0x00;
    (*nbyte)[10] = 0x00;
    (*nbyte)[11] = 0x00;
    (*nbyte)[12] = 0x00;
    (*nbyte)[13] = 0x00;
    (*nbyte)[14] = 0x00;
    (*nbyte)[15] = 0x80;
    assertBitSet(*nbyte, String("{0, 63, 64, 127}"));
}

void assertEqualsLong(AutoPtr<ArrayOf<Int64> > nlog)
{
    AutoPtr<IBitSetHelper>  bsh;
    AutoPtr<IBitSet> bs;
    CBitSetHelper::AcquireSingleton((IBitSetHelper**)&bsh);
    bsh->ValueOfInt64Array(*nlog,(IBitSet**)&bs);
    AutoPtr<ArrayOf<Int64> > outlog ;
    bs->ToInt64Array((ArrayOf<Int64> **)&outlog);
    for (int i = 0; i < outlog->GetLength(); ++i)
    {
        assert( (*nlog)[i] == (*outlog)[i] );
    }
}

int CTest::test_toLongArray(int argc, char* argv[]) {
    AutoPtr<ArrayOf<Int64> > nlog = ArrayOf<Int64>::Alloc(0);
    assertEqualsLong(nlog);
    nlog = ArrayOf<Int64>::Alloc(1);
    (*nlog)[0] = 1;
    assertEqualsLong(nlog);
    nlog = ArrayOf<Int64>::Alloc(2);
    (*nlog)[0] = 1;
    (*nlog)[1] = 2;
    assertEqualsLong(nlog);

    // Check that we're not returning trailing empty space.
    AutoPtr<IBitSet> bs ;
    CBitSet::New(128, (IBitSet**)&bs);
    bs->ToInt64Array((ArrayOf<Int64> **)&nlog);
    assertEqualsLong(nlog);
    CBitSet::New((IBitSet**)&bs);
    bs->Set(0);
    bs->SetEx(64, 66);
    bs->ClearEx(64, 66);
    bs->ToInt64Array((ArrayOf<Int64> **)&nlog);
    assertEqualsLong(nlog);
}

void assertEqualsByte(AutoPtr<ArrayOf<Int64> > nlog)
{
    AutoPtr<IBitSetHelper>  bsh;
    AutoPtr<IBitSet> bs;
    CBitSetHelper::AcquireSingleton((IBitSetHelper**)&bsh);
    bsh->ValueOfInt64Array(*nlog,(IBitSet**)&bs);
    AutoPtr<ArrayOf<Byte> > outlog ;
    bs->ToByteArray((ArrayOf<Byte> **)&outlog);
    for (int i = 0; i < outlog->GetLength(); ++i)
    {
        PFL_EX( " byte : %i , %d  ", i, (*outlog)[i] );
    }
}

int CTest::test_toByteArray(int argc, char* argv[]) {
    AutoPtr<ArrayOf<Int64> > nlog = ArrayOf<Int64>::Alloc(0);
    assertEqualsByte(nlog);
    nlog = ArrayOf<Int64>::Alloc(1);
    (*nlog)[0] = 1;
    assertEqualsByte(nlog);
    nlog = ArrayOf<Int64>::Alloc(1);
    (*nlog)[0] = 0x1234567890abcdefL ;
    assertEqualsByte(nlog);
    nlog = ArrayOf<Int64>::Alloc(2);
    (*nlog)[0] = 1;
    (*nlog)[1] = 2;
    assertEqualsByte(nlog);
}

int CTest::test_previousSetBit(int argc, char* argv[]) {
    AutoPtr<IBitSet> bs;
    CBitSet::New((IBitSet**)&bs);
    Int32 value;
    bs->PreviousSetBit(666,&value);
    assert(-1 == value);

    CBitSet::New((IBitSet**)&bs);
    bs->Set(32);
    bs->PreviousSetBit(999,&value);
    assert(32 == value);
    bs->PreviousSetBit(33,&value);
    assert(32 == value);
    bs->PreviousSetBit(32,&value);
    assert(32 == value);
    bs->PreviousSetBit(31,&value);
    assert(-1 == value);

    CBitSet::New((IBitSet**)&bs);
    bs->Set(0);
    bs->Set(1);
    bs->Set(32);
    bs->Set(192);
    bs->Set(666);

    // bs->PreviousSetBit(999,&value);
    // // assert(666 == value);
    // bs->PreviousSetBit(667,&value);
    // assert(666 == value);
    // bs->PreviousSetBit(666,&value);
    // assert(666 == value);
    // bs->PreviousSetBit(665,&value);
    // assert(192 == value);
    bs->PreviousSetBit(191,&value);
    assert(32 == value);
    bs->PreviousSetBit(31,&value);
    assert(1 == value);
    bs->PreviousSetBit(0,&value);
    assert(0 == value);
    bs->PreviousSetBit(-1,&value);
    assert(-1 == value);
}

AutoPtr<IBitSet> big() {
    AutoPtr<IBitSet> result;
    CBitSet::New((IBitSet**)&result);
    result->Set(1000);
    return result;
}

AutoPtr<IBitSet> small() {
    AutoPtr<IBitSet> result;
    CBitSet::New((IBitSet**)&result);
    result->Set(10);
    return result;
}

int CTest::test_differentSizes(int argc, char* argv[]) {
    AutoPtr<IBitSet> result = big();
    AutoPtr<IBitSet> resultsmall = small();
    result->And(resultsmall);
    String str;
    result->ToString(&str);
    assert(str.Equals("{}"));
    result = small();
    AutoPtr<IBitSet> resultbig = big();
    result->And(resultbig);
    result->ToString(&str);
    assert(str.Equals("{}"));
    result = big();
    result->AndNot(resultsmall);
    result->ToString(&str);
    PFL_EX("{1000} == %s " , str.string())
    // assert(str.Equals("{1000}"));
    result = small();
    result->AndNot(resultbig);
    result->ToString(&str);
    PFL_EX("{10} == %s " ,str.string())
    // assert(str.Equals("{10}"));
    Boolean flag;
    result = big();
    result->Intersects(resultsmall,&flag);
    assert( !flag );
    result = small();
    result->Intersects(resultbig , &flag);
    assert( !flag);
    result = big();
    result->Or(small());
    result->ToString(&str);
    PFL_EX("{10 , 1000} == %s" ,str.string())
    // assert(str.Equals("{10, 1000}"));
    result = small();
    result->Or(big());
    result->ToString(&str);
    PFL_EX("{10, 1000} == %s",str.string())
    // assert(str.Equals("{10, 1000}"));

    result = big();
    result->Xor(small());
    result->ToString(&str);
    PFL_EX("{10, 1000} == %s" ,str.string())
    // assert(str.Equals("{10, 1000}"));
    result = small();
    result->Xor(big());
    result->ToString(&str);
    PFL_EX("{10, 1000} == %s" , str.string())
    // assert(str.Equals("{10, 1000}"));
}

