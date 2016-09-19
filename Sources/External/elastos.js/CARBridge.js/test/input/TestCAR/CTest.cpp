
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <elastos.h>
#include <elastos/core/Object.h>
#include <elastos/coredef.h>

#include "CTest.h"

#include "../../type-traits.h"



CTest::~CTest()
{
    if (fclose(_output) == EOF) {
        perror("CTest");

        abort();
    }
}

CAR_OBJECT_IMPL(CTest)

CAR_INTERFACE_IMPL(CTest, Object, ITest)

ECode CTest::constructor(void)
{
    _output = fopen(OUTPUT_FILE, "w+");
    if (_output == NULL)
        return E_FAIL;

    return NOERROR;
}

template<class T, ECode (CTest::*output)(T, size_t)>
ECode CTest::TestNonarray(T nonarray, char const *name)
{
    ECode ec;

    if (freopen(OUTPUT_FILE, "w+", _output) == NULL)
        return E_FAIL;

    ec = (this->*output)(nonarray, 0);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, " (%s)\n", name) < 0)
        return E_FAIL;

    return NOERROR;
}

inline ECode CTest::Output(ITest *itest, size_t indent = 0)
{
    if (fprintf(_output, "%p", itest) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestSelf(void)
{
    return TestNonarray<ITest *, &Output>(this, "Interface (ITest *)");
}

ECode CTest::TestVoid(void)
{
    if (freopen(OUTPUT_FILE, "w+", _output) == NULL)
        return E_FAIL;

    if (fprintf(_output, "void (void)\n") < 0)
        return E_FAIL;

    return NOERROR;
}

inline ECode CTest::Output(Int16 i16, size_t indent = 0)
{
    if (fprintf(_output, "%d", i16) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestInt16(
        /* [in] */ Int16 i16)
{
    return TestNonarray<Int16, &Output>(i16, "Int16");
}

inline ECode CTest::Output(Int32 i32, size_t indent = 0)
{
    if (fprintf(_output, "%d", i32) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestInt32(
        /* [in] */ Int32 i32)
{
    return TestNonarray<Int32, &Output>(i32, "Int32");
}

inline ECode CTest::Output(Int64 i64, size_t indent = 0)
{
    if (fprintf(_output, "%lld", i64) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestInt64(
        /* [in] */ Int64 i64)
{
    return TestNonarray<Int64, &Output>(i64, "Int64");
}

inline ECode CTest::Output(Byte byte, size_t indent = 0)
{
    if (fprintf(_output, "%u", byte) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestByte(
        /* [in] */ Byte byte)
{
    return TestNonarray<Byte, &Output>(byte, "Byte");
}

inline ECode CTest::Output(Float f, size_t indent = 0)
{
    if (fprintf(_output, "%f", f) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestFloat(
        /* [in] */ Float f)
{
    return TestNonarray<Float, &Output>(f, "Float");
}

inline ECode CTest::Output(Double d, size_t indent = 0)
{
    if (fprintf(_output, "%lf", d) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestDouble(
        /* [in] */ Double d)
{
    return TestNonarray<Double, &Output>(d, "Double");
}

inline ECode CTest::Output(Char32 c32, size_t indent = 0)
{
    if (fprintf(_output, "\'%lc\'", c32) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestChar32(
        /* [in] */ Char32 c32)
{
    return TestNonarray<Char32, &Output>(c32, "Char32");
}

inline ECode CTest::Output(String const &s, size_t indent = 0)
{
    if (fprintf(_output, "\"%s\"", static_cast<char const *>(s)) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestString(
        /* [in] */ String const &s)
{
    return TestNonarray<String const &, &Output>(s, "String");
}

ECode CTest::OutputBoolean(Boolean b, size_t indent = 0)
{
    if (b != FALSE) {
        if (fprintf(_output, "true") < 0)
            return E_FAIL;
    } else {
        if (fprintf(_output, "false") < 0)
            return E_FAIL;
    }

    return NOERROR;
}

ECode CTest::TestBoolean(
        /* [in] */ Boolean b)
{
    return TestNonarray<Boolean, &OutputBoolean>(b, "Boolean");
}

ECode CTest::Output(EMuid const &id, size_t indent = 0)
{
    if (fprintf(_output, "{\n") < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  \"mData1\": %u,\n", indent, ' ', id.mData1) < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  \"mData2\": %u,\n", indent, ' ', id.mData2) < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  \"mData3\": %u,\n", indent, ' ', id.mData3) < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  \"mData4\": [\n", indent, ' ') < 0)
        return E_FAIL;

    for (size_t i = 0; i < 7; ++i)
        if (fprintf(_output, "%*c    %u,\n", indent, ' ', id.mData4[i]) < 0)
            return E_FAIL;

    if (fprintf(_output, "%*c    %u\n", indent, ' ', id.mData4[7]) < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  ]\n", indent, ' ') < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c}", indent, ' ') < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestEMuid(
        /* [in] */ EMuid const &id)
{
    return TestNonarray<EMuid const &, &Output>(id, "EMuid");
}

ECode CTest::Output(EGuid const &id, size_t indent = 0)
{
    ECode ec;

    if (fprintf(_output, "{\n") < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  \"mClsid\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(id, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  \"mUunm\": \"%s\",\n", indent, ' ', id.mUunm) < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c  \"mCarcode\": %u\n", indent, ' ', id.mCarcode) < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c}", indent, ' ') < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestEGuid(
        /* [in] */ EGuid const &id)
{
    return TestNonarray<EGuid const &, &Output>(id, "EGuid");
}

ECode CTest::OutputECode(ECode ecode, size_t indent = 0)
{
    if (fprintf(_output, "%d", ecode) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestECode(
        /* [in] */ ECode ecode)
{
    return TestNonarray<ECode, &OutputECode>(ecode, "ECode");
}

inline ECode CTest::Output(PVoid ptr, size_t indent = 0)
{
    if (fprintf(_output, "%p", ptr) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestLocalPtr(
        /* [in] */ PVoid ptr)
{
    return TestNonarray<PVoid, &Output>(ptr, "LocalPtr (PVoid)");
}

ECode CTest::OutputETest(ETest etest, size_t indent = 0)
{
    switch (etest) {
    case ETest_0:
        if (fprintf(_output, "\"ETest_0\"") < 0)
            return E_FAIL;

        break;

    case ETest_1:
        if (fprintf(_output, "\"ETest_1\"") < 0)
            return E_FAIL;

        break;

    default:
        abort();
    }

    return NOERROR;
}

ECode CTest::TestEnum(
        /* [in] */ ETest etest)
{
    return TestNonarray<ETest, &OutputETest>(etest, "Enum (ETest)");
}

template<class T, ECode (CTest::*output)(T, size_t)>
ECode CTest::Output(size_t size, typename RemoveReference<T>::Type const array[], size_t indent = 0)
{
    ECode ec;

    if (fprintf(_output, "[\n") < 0)
        return E_FAIL;

    for (size_t i = 0; i < size - 1; ++i) {
        if (fprintf(_output, "%*c  ", indent, ' ') < 0)
            return E_FAIL;

        ec = (this->*output)(array[i], indent);
        if (FAILED(ec))
            return ec;

        if (fprintf(_output, ",\n") < 0)
            return E_FAIL;
    }


    if (fprintf(_output, "%*c  ", indent, ' ') < 0)
        return E_FAIL;

    ec = (this->*output)(array[size - 1], indent);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, "\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c]", indent, ' ') < 0)
        return E_FAIL;

    return NOERROR;
}

template<class T, ECode (CTest::*output)(T, size_t)>
ECode CTest::TestArray(
        ArrayOf<typename RemoveConst<typename RemoveReference<T>::Type>::Type> const &array,
        char const *name)
{
    ECode ec;

    if (freopen(OUTPUT_FILE, "w+", _output) == NULL)
        return E_FAIL;

    ec = Output<T, output>(array.GetLength(), array.GetPayload());
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, " (ArrayOf<%s>)\n", name) < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Int16> const &i16s)
{
    return TestArray<Int16, &Output>(i16s, "Int16");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Int32> const &i32s)
{
    return TestArray<Int32, &Output>(i32s, "Int32");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Int64> const &i64s)
{
    return TestArray<Int64, &Output>(i64s, "Int64");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Byte> const &bytes)
{
    return TestArray<Byte, &Output>(bytes, "Byte");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Float> const &fs)
{
    return TestArray<Float, &Output>(fs, "Float");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Double> const &ds)
{
    return TestArray<Double, &Output>(ds, "Double");
}

ECode CTest::TestCARArrayOfEnum(
        /* [in] */ ArrayOf<ETest> const &etests)
{
    return TestArray<ETest, &OutputETest>(etests, "ETest");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<Char32> const &c32s)
{
    return TestArray<Char32, &Output>(c32s, "Char32");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<String> const &ss)
{
    return TestArray<String const &, &Output>(ss, "String");
}

ECode CTest::TestCARArrayOfBoolean(
        /* [in] */ ArrayOf<Boolean> const &bs)
{
    return TestArray<Boolean, &OutputBoolean>(bs, "Boolean");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<EMuid> const &ids)
{
    return TestArray<EMuid const &, &Output>(ids, "EMuid");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<EGuid> const &ids)
{
    return TestArray<EGuid const &, &Output>(ids, "EGuid");
}

ECode CTest::TestCARArrayOfECode(
        /* [in] */ ArrayOf<ECode> const &ecodes)
{
    return TestArray<ECode, &OutputECode>(ecodes, "ECode");
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<ITest *> const &itests)
{
    return TestArray<ITest *, &Output>(itests, "ITest *");
}

ECode CTest::Output(STestHelper const &stest, size_t indent = 0)
{
    ECode ec;

    if (fprintf(_output, "{\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"i16\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.i16, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, "\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c}", indent, ' ') < 0)
        return E_FAIL;

    return NOERROR;
}

template<class T, ECode (CTest::*output)(T, size_t)>
ECode CTest::Output(size_t m, size_t n, typename RemoveReference<T>::Type const *vv, size_t indent = 0)
{
    ECode ec;

    if (fprintf(_output, "[\n") < 0)
        return E_FAIL;

    for (size_t i = 0; i < m - 1; ++i) {
        if (fprintf(_output, "%*c  ", indent, ' ') < 0)
            return E_FAIL;

        ec = Output<T, output>(n, vv + i * n, indent + 2);
        if (FAILED(ec))
            return ec;

        if (fprintf(_output, ",\n") < 0)
            return E_FAIL;
    }

    if (fprintf(_output, "%*c  ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<T, output>(n, vv + (m - 1) * n, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, "\n") < 0)
        return E_FAIL;

    if (fprintf(_output, "%*c]\n", indent, ' ') < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::Output(STest const &stest, size_t indent = 0)
{
    ECode ec;

    if (fprintf(_output, "{\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"i16\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.i16, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"i32\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.i32, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"i64\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.i64, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"byte\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.byte, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"f\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.f, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"d\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.d, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"c32\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.c32, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"b\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.b, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"emuid\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.emuid, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"eguid\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.eguid, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"ecode\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = OutputECode(stest.ecode, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"ptr\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.ptr, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"etest\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.etest, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"stest\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output(stest.stest, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vi16\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Int16, &Output>(2, stest.vi16, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vi32\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Int32, &Output>(2, stest.vi32, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vi64\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Int64, &Output>(2, stest.vi64, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vbyte\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Byte, &Output>(2, stest.vbyte, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vf\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Float, &Output>(2, stest.vf, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vd\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Double, &Output>(2, stest.vd, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vc32\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Char32, &Output>(2, stest.vc32, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vb\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Boolean, &OutputBoolean>(2, stest.vb, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vemuid\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<EMuid const &, &Output>(2, stest.vemuid, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"veguid\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<EGuid const &, &Output>(2, stest.veguid, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vecode\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<ECode, &OutputECode>(2, stest.vecode, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vptr\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<PVoid, &Output>(2, stest.vptr, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vetest\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<ETest, &OutputETest>(2, stest.vetest, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vstest\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<STestHelper const &, &Output>(2, stest.vstest, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvi16\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Int16, &Output>(2, 2, *stest.vvi16, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvi32\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Int32, &Output>(2, 2, *stest.vvi32, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvi64\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Int64, &Output>(2, 2, *stest.vvi64, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvbyte\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Byte, &Output>(2, 2, *stest.vvbyte, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvf\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Float, &Output>(2, 2, *stest.vvf, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvd\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Double, &Output>(2, 2, *stest.vvd, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvc32\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Char32, &Output>(2, 2, *stest.vvc32, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvb\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<Boolean, &OutputBoolean>(2, 2, *stest.vvb, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvemuid\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<EMuid const &, &Output>(2, 2, *stest.vvemuid, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vveguid\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<EGuid const &, &Output>(2, 2, *stest.vveguid, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvecode\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<ECode, &OutputECode>(2, 2, *stest.vvecode, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvptr\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<PVoid, &Output>(2, 2, *stest.vvptr, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvetest\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<ETest, &OutputETest>(2, 2, *stest.vvetest, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c  \"vvstest\": ", indent, ' ') < 0)
        return E_FAIL;

    ec = Output<STestHelper const &, &Output>(2, 2, *stest.vvstest, indent + 2);
    if (FAILED(ec))
        return ec;

    if (fprintf(_output, ",\n") < 0)
        return E_FAIL;


    if (fprintf(_output, "%*c}", indent, ' ') < 0)
        return E_FAIL;

    return NOERROR;
}

ECode CTest::TestCARArray(
        /* [in] */ ArrayOf<STest> const &stests)
{
    return TestArray<STest const &, &Output>(stests, "STest");
}

ECode CTest::TestStruct(
        /* [in] */ STest const &stest)
{
    return TestNonarray<STest const &, &Output>(stest, "Struct (STest)");
}

ECode CTest::TestInterface(
        /* [in] */ ITest *itest)
{
    return TestNonarray<ITest *, &Output>(itest, "Interface (ITest *)");
}

ECode CTest::Lock()
{
    return E_NOT_IMPLEMENTED;
}

ECode CTest::Unlock()
{
    return E_NOT_IMPLEMENTED;
}

ECode CTest::Notify()
{
    return E_NOT_IMPLEMENTED;
}

ECode CTest::NotifyAll()
{
    return E_NOT_IMPLEMENTED;
}

ECode CTest::Wait()
{
    return E_NOT_IMPLEMENTED;
}

ECode CTest::Wait(
        /* [in] */ Int64 millis)
{
    return E_NOT_IMPLEMENTED;
}

ECode CTest::Wait(
        /* [in] */ Int64 millis,
        /* [in] */ Int32 nanos)
{
    return E_NOT_IMPLEMENTED;
}

ECode CTest::GetWeakReference(
        /* [out] */ IWeakReference **weakReference)
{
    return E_NOT_IMPLEMENTED;
}

