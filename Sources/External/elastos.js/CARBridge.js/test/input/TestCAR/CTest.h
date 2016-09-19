#ifndef __CTEST_H__
# define __CTEST_H__

# include <stddef.h>
# include <stdio.h>

# include <elastos.h>
# include <elastos/core/Object.h>
# include <elastos/coredef.h>

# include "_CTest.h"

# include "../../type-traits.h"



CarClass(CTest), public Object, public ITest {
public:
    ~CTest();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(void);

    CARAPI TestSelf(void);

    CARAPI TestVoid(void);

    CARAPI TestInt16(
            /* [in] */ Int16 i16);

    CARAPI TestInt32(
            /* [in] */ Int32 i32);

    CARAPI TestInt64(
            /* [in] */ Int64 i64);

    CARAPI TestByte(
            /* [in] */ Byte byte);

    CARAPI TestFloat(
            /* [in] */ Float f);

    CARAPI TestDouble(
            /* [in] */ Double d);

    CARAPI TestChar32(
            /* [in] */ Char32 c32);

    CARAPI TestString(
            /* [in] */ String const &s);

    CARAPI TestBoolean(
            /* [in] */ Boolean b);

    CARAPI TestEMuid(
            /* [in] */ EMuid const &id);

    CARAPI TestEGuid(
            /* [in] */ EGuid const &id);

    CARAPI TestECode(
            /* [in] */ ECode ecode);

    CARAPI TestLocalPtr(
            /* [in] */ PVoid ptr);

    CARAPI TestEnum(
            /* [in] */ ETest etest);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<Int16> const &i16s);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<Int32> const &i32s);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<Int64> const &i64s);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<Byte> const &bytes);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<Float> const &fs);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<Double> const &ds);

    CARAPI TestCARArrayOfEnum(
            /* [in] */ ArrayOf<ETest> const &etests);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<Char32> const &c32s);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<String> const &ss);

    CARAPI TestCARArrayOfBoolean(
            /* [in] */ ArrayOf<Boolean> const &bs);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<EMuid> const &ids);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<EGuid> const &ids);

    CARAPI TestCARArrayOfECode(
            /* [in] */ ArrayOf<ECode> const &ecodes);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<ITest *> const &itests);

    CARAPI TestCARArray(
            /* [in] */ ArrayOf<STest> const &stests);

    CARAPI TestStruct(
            /* [in] */ STest const &stest);

    CARAPI TestInterface(
            /* [in] */ ITest *itest);

    CARAPI Lock();

    CARAPI Unlock();

    CARAPI Notify();

    CARAPI NotifyAll();

    CARAPI Wait();

    CARAPI Wait(
            /* [in] */ Int64 millis);

    CARAPI Wait(
            /* [in] */ Int64 millis,
            /* [in] */ Int32 nanos);

    CARAPI GetWeakReference(
            /* [out] */ IWeakReference **weakReference);

private:
    FILE *_output;

    template<class T, ECode (CTest::*)(T, size_t)>
    ECode TestNonarray(T nonarray, char const *name);

    ECode Output(ITest *itest, size_t indent);

    ECode Output(Int16 i16, size_t indent);

    ECode Output(Int32 i32, size_t indent);

    ECode Output(Int64 i64, size_t indent);

    ECode Output(Byte byte, size_t indent);

    ECode Output(Float f, size_t indent);

    ECode Output(Double d, size_t indent);

    ECode Output(Char32 c32, size_t indent);

    ECode Output(String const &s, size_t indent);

    ECode OutputBoolean(Boolean b, size_t indent);

    ECode Output(EMuid const &id, size_t indent);

    ECode Output(EGuid const &id, size_t indent);

    ECode OutputECode(ECode ecode, size_t indent);

    ECode Output(PVoid ptr, size_t indent);

    ECode OutputETest(ETest etest, size_t indent);

    template<class T, ECode (CTest::*)(T, size_t)>
    ECode Output(size_t size, typename RemoveReference<T>::Type const array[], size_t indent);

    template<class T, ECode (CTest::*)(T, size_t)>
    ECode TestArray(
            ArrayOf<typename RemoveConst<typename RemoveReference<T>::Type>::Type> const &array,
            char const *name);

    ECode Output(STestHelper const &stest, size_t indent);

    ECode Output(STest const &stest, size_t indent);

    template<class T, ECode (CTest::*)(T, size_t)>
    ECode Output(size_t m, size_t n, typename RemoveReference<T>::Type const *vv, size_t indent);
};

#endif // __CTEST_H__
