#ifndef __ELASTOS_CORE_COREUTILS_H__
#define __ELASTOS_CORE_COREUTILS_H__

#include <elastos/core/Object.h>
#ifdef ELASTOS_CORELIBRARY
#include "CArrayOf.h"
#endif

using Elastos::Core::IByte;
using Elastos::Core::IChar32;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::ICharSequence;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;

namespace Elastos {
namespace Core {

class ECO_PUBLIC CoreUtils
{
public:
    static CARAPI_(AutoPtr<IByte>) ConvertByte(
        /* [in] */ Byte value);

    static CARAPI_(AutoPtr<IChar32>) ConvertChar32(
        /* [in] */ Char32 value);

    static CARAPI_(AutoPtr<IBoolean>) Convert(
        /* [in] */ Boolean value);

    static CARAPI_(AutoPtr<IInteger16>) Convert(
        /* [in] */ Int16 value);

    static CARAPI_(AutoPtr<IInteger32>) Convert(
        /* [in] */ Int32 value);

    static CARAPI_(Int32) Unbox(
        /* [in] */ IInteger32* value);

    static CARAPI_(AutoPtr<IInteger64>) Convert(
        /* [in] */ Int64 value);

    static CARAPI_(AutoPtr<IFloat>) Convert(
        /* [in] */ Float value);

    static CARAPI_(AutoPtr<IDouble>) Convert(
        /* [in] */ Double value);

    static CARAPI_(AutoPtr<ICharSequence>) Convert(
        /* [in] */ const String & value);

    static CARAPI_(String) Unbox(
        /* [in] */ ICharSequence* value);

    static CARAPI_(String) Unbox(
        /* [in] */ IString* value);

    static CARAPI_(AutoPtr<ICharSequence>) Convert(
        /* [in] */ const char* str);

    static CARAPI_(AutoPtr<IArrayOf>) ConvertByteArray(
        /* [in] */ ArrayOf<Byte>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) ConvertChar32Array(
        /* [in] */ ArrayOf<Char32>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<Boolean>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<Int16>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<Int32>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<Int64>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<Float>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<Double>* arr);

    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<String>* arr);

    template<typename T>
    static CARAPI_(AutoPtr<IArrayOf>) Convert(
        /* [in] */ ArrayOf<T*>* arr)
    {
        if (arr) {
            Int32 length = arr->GetLength();
            AutoPtr<IArrayOf> arrObj;
            CArrayOf::New(EIID_IInterface, length, (IArrayOf**)&arrObj);
            for (Int32 i = 0; i < length; ++i) {
                arrObj->Set(i, TO_IINTERFACE((*arr)[i]));
            }

            return arrObj;
        }
        return NULL;
    }

private:
    template<typename T1, typename T2>
    ECO_LOCAL static CARAPI_(AutoPtr<IArrayOf>) ConvertImpl(
        /* [in] */ ArrayOf<T1>* arr,
        /* [in] */ const InterfaceID& itfc)
    {
        if (arr) {
            Int32 length = arr->GetLength();
            AutoPtr<IArrayOf> arrObj;
            CArrayOf::New(itfc, length, (IArrayOf**)&arrObj);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<T2> item = Convert((*arr)[i]);
                arrObj->Set(i, item.Get());
            }

            return arrObj;
        }
        return NULL;
    }

private:
    ECO_LOCAL CoreUtils();
    ECO_LOCAL ~CoreUtils();
};

} // namespace Core
} // namespace Elastos

using Elastos::Core::CoreUtils;

#endif // __ELASTOS_CORE_COREUTILS_H__