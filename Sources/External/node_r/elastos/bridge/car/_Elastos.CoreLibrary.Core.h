#ifndef ___Elastos_CoreLibrary_Core_h__
#define ___Elastos_CoreLibrary_Core_h__

#include <Elastos.CoreLibrary.h>

EXTERN_C ELAPI _Impl_AcquireCallbackHandler(PInterface pServerObj, _ELASTOS REIID iid, PInterface *ppHandler);
EXTERN_C ELAPI _Impl_CheckClsId(PInterface pServerObj, const _ELASTOS ClassID* pClassid, PInterface *ppServerObj);

namespace Elastos {
namespace Core {
CAR_INTERFACE("0C3F3346-90D2-1C44-50CC-8D01EA47950A")
ICharSequence : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICharSequence*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICharSequence*)pObj->Probe(EIID_ICharSequence);
    }

    static CARAPI_(ICharSequence*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICharSequence*)pObj->Probe(EIID_ICharSequence);
    }

    virtual CARAPI GetLength(
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI GetCharAt(
        /* [in] */ _ELASTOS Int32 index,
        /* [out] */ _ELASTOS Char32 * c) = 0;

    virtual CARAPI SubSequence(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end,
        /* [out] */ Elastos::Core::ICharSequence ** csq) = 0;

    virtual CARAPI ToString(
        /* [out] */ _ELASTOS String * str) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("59AF225A-0292-C9B4-F4FC-508036D99044")
IAppendable : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IAppendable*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IAppendable*)pObj->Probe(EIID_IAppendable);
    }

    static CARAPI_(IAppendable*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IAppendable*)pObj->Probe(EIID_IAppendable);
    }

    virtual CARAPI AppendChar(
        /* [in] */ _ELASTOS Char32 c) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::ICharSequence * csq) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::ICharSequence * csq,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("0804101B-F312-31CA-4A96-2FFA33AF1CA3")
ICloneable : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICloneable*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloneable*)pObj->Probe(EIID_ICloneable);
    }

    static CARAPI_(ICloneable*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloneable*)pObj->Probe(EIID_ICloneable);
    }

    virtual CARAPI Clone(
        /* [out] */ IInterface ** object) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("DB241538-2212-3512-4894-7881E24721E3")
IComparable : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IComparable*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IComparable*)pObj->Probe(EIID_IComparable);
    }

    static CARAPI_(IComparable*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IComparable*)pObj->Probe(EIID_IComparable);
    }

    virtual CARAPI CompareTo(
        /* [in] */ IInterface * another,
        /* [out] */ _ELASTOS Int32 * result) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("366B133D-2212-3512-48DC-C8FFA3100000")
IComparator : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IComparator*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IComparator*)pObj->Probe(EIID_IComparator);
    }

    static CARAPI_(IComparator*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IComparator*)pObj->Probe(EIID_IComparator);
    }

    virtual CARAPI Compare(
        /* [in] */ IInterface * lhs,
        /* [in] */ IInterface * rhs,
        /* [out] */ _ELASTOS Int32 * result) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("A9B2FA33-6C12-F736-7F18-57C081810409")
IStringBuilder : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStringBuilder*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStringBuilder*)pObj->Probe(EIID_IStringBuilder);
    }

    static CARAPI_(IStringBuilder*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStringBuilder*)pObj->Probe(EIID_IStringBuilder);
    }

    virtual CARAPI ToASCIILowerCase() = 0;

    virtual CARAPI ToASCIIUpperCase() = 0;

    virtual CARAPI Reset() = 0;

    virtual CARAPI GetByteCount(
        /* [out] */ _ELASTOS Int32 * byteLength) = 0;

    virtual CARAPI GetCapacity(
        /* [out] */ _ELASTOS Int32 * capacity) = 0;

    virtual CARAPI EnsureCapacity(
        /* [in] */ _ELASTOS Int32 min) = 0;

    virtual CARAPI TrimToSize() = 0;

    virtual CARAPI SetCharAt(
        /* [in] */ _ELASTOS Int32 index,
        /* [in] */ _ELASTOS Char32 ch) = 0;

    virtual CARAPI GetCharAt(
        /* [in] */ _ELASTOS Int32 index,
        /* [out] */ _ELASTOS Char32 * c) = 0;

    virtual CARAPI GetChars(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end,
        /* [out] */ _ELASTOS ArrayOf<_ELASTOS Char32> * dst,
        /* [in] */ _ELASTOS Int32 dstStart) = 0;

    virtual CARAPI SetLength(
        /* [in] */ _ELASTOS Int32 length) = 0;

    virtual CARAPI Substring(
        /* [in] */ _ELASTOS Int32 start,
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI Substring(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end,
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI IndexOf(
        /* [in] */ const _ELASTOS String& string,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI IndexOf(
        /* [in] */ const _ELASTOS String& subString,
        /* [in] */ _ELASTOS Int32 start,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI LastIndexOf(
        /* [in] */ const _ELASTOS String& string,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI LastIndexOf(
        /* [in] */ const _ELASTOS String& subString,
        /* [in] */ _ELASTOS Int32 start,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI AppendNULL() = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Boolean b) = 0;

    virtual CARAPI AppendChar(
        /* [in] */ _ELASTOS Char32 c) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Double d) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Float f) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Int32 i) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Int64 l) = 0;

    virtual CARAPI Append(
        /* [in] */ IInterface * obj) = 0;

    virtual CARAPI Append(
        /* [in] */ const _ELASTOS String& str) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::IStringBuffer * sb) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::IStringBuilder * sb) = 0;

    virtual CARAPI Append(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & chars) = 0;

    virtual CARAPI Append(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & str,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 length) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::ICharSequence * csq) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::ICharSequence * csq,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end) = 0;

    virtual CARAPI InsertChar(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Char32 c) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Boolean b) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 i) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int64 l) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Double d) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Float f) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ IInterface * obj) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ const _ELASTOS String& str) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & ch) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & chars,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 length) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ Elastos::Core::ICharSequence * s) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ Elastos::Core::ICharSequence * s,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end) = 0;

    virtual CARAPI Replace(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end,
        /* [in] */ const _ELASTOS String& string) = 0;

    virtual CARAPI Reverse() = 0;

    virtual CARAPI DeleteCharAt(
        /* [in] */ _ELASTOS Int32 location) = 0;

    virtual CARAPI Delete(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("4066FC22-6C12-3F35-8C2B-E0C040828491")
IStringBuffer : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStringBuffer*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStringBuffer*)pObj->Probe(EIID_IStringBuffer);
    }

    static CARAPI_(IStringBuffer*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStringBuffer*)pObj->Probe(EIID_IStringBuffer);
    }

    virtual CARAPI ToASCIILowerCase() = 0;

    virtual CARAPI ToASCIIUpperCase() = 0;

    virtual CARAPI Reset() = 0;

    virtual CARAPI GetByteCount(
        /* [out] */ _ELASTOS Int32 * byteLength) = 0;

    virtual CARAPI GetCapacity(
        /* [out] */ _ELASTOS Int32 * capacity) = 0;

    virtual CARAPI EnsureCapacity(
        /* [in] */ _ELASTOS Int32 min) = 0;

    virtual CARAPI TrimToSize() = 0;

    virtual CARAPI SetCharAt(
        /* [in] */ _ELASTOS Int32 index,
        /* [in] */ _ELASTOS Char32 ch) = 0;

    virtual CARAPI GetCharAt(
        /* [in] */ _ELASTOS Int32 index,
        /* [out] */ _ELASTOS Char32 * c) = 0;

    virtual CARAPI GetChars(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end,
        /* [out] */ _ELASTOS ArrayOf<_ELASTOS Char32> * dst,
        /* [in] */ _ELASTOS Int32 dstStart) = 0;

    virtual CARAPI Substring(
        /* [in] */ _ELASTOS Int32 start,
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI Substring(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end,
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI IndexOf(
        /* [in] */ const _ELASTOS String& string,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI IndexOf(
        /* [in] */ const _ELASTOS String& subString,
        /* [in] */ _ELASTOS Int32 start,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI LastIndexOf(
        /* [in] */ const _ELASTOS String& string,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI LastIndexOf(
        /* [in] */ const _ELASTOS String& subString,
        /* [in] */ _ELASTOS Int32 start,
        /* [out] */ _ELASTOS Int32 * index) = 0;

    virtual CARAPI AppendNULL() = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Boolean b) = 0;

    virtual CARAPI AppendChar(
        /* [in] */ _ELASTOS Char32 c) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Double d) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Float f) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Int32 i) = 0;

    virtual CARAPI Append(
        /* [in] */ _ELASTOS Int64 l) = 0;

    virtual CARAPI Append(
        /* [in] */ IInterface * obj) = 0;

    virtual CARAPI Append(
        /* [in] */ const _ELASTOS String& str) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::IStringBuffer * sb) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::IStringBuilder * sb) = 0;

    virtual CARAPI Append(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & chars) = 0;

    virtual CARAPI Append(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & str,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 length) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::ICharSequence * csq) = 0;

    virtual CARAPI Append(
        /* [in] */ Elastos::Core::ICharSequence * csq,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end) = 0;

    virtual CARAPI InsertChar(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Char32 c) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Boolean b) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 i) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int64 l) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Double d) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Float f) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ IInterface * obj) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ const _ELASTOS String& str) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & ch) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & chars,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 length) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ Elastos::Core::ICharSequence * s) = 0;

    virtual CARAPI Insert(
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ Elastos::Core::ICharSequence * s,
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end) = 0;

    virtual CARAPI Replace(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end,
        /* [in] */ const _ELASTOS String& string) = 0;

    virtual CARAPI Reverse() = 0;

    virtual CARAPI DeleteCharAt(
        /* [in] */ _ELASTOS Int32 location) = 0;

    virtual CARAPI Delete(
        /* [in] */ _ELASTOS Int32 start,
        /* [in] */ _ELASTOS Int32 end) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("88B54626-9612-3FA5-C845-24A799DCF278")
IArrayOf : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IArrayOf*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IArrayOf*)pObj->Probe(EIID_IArrayOf);
    }

    static CARAPI_(IArrayOf*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IArrayOf*)pObj->Probe(EIID_IArrayOf);
    }

    virtual CARAPI GetLength(
        /* [out] */ _ELASTOS Int32 * size) = 0;

    virtual CARAPI Get(
        /* [in] */ _ELASTOS Int32 index,
        /* [out] */ IInterface ** element) = 0;

    virtual CARAPI Set(
        /* [in] */ _ELASTOS Int32 index,
        /* [in] */ IInterface * element) = 0;

    virtual CARAPI GetTypeId(
        /* [out] */ _ELASTOS InterfaceID * id) = 0;

    virtual CARAPI DeepToString(
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI DeepGetHashCode(
        /* [out] */ _ELASTOS Int32 * hash) = 0;

    virtual CARAPI DeepEquals(
        /* [in] */ Elastos::Core::IArrayOf * other,
        /* [out] */ _ELASTOS Boolean * equals) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("502B4432-05D2-301C-4ADC-FFD1E6068AEF")
INumber : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(INumber*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (INumber*)pObj->Probe(EIID_INumber);
    }

    static CARAPI_(INumber*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (INumber*)pObj->Probe(EIID_INumber);
    }

    virtual CARAPI ByteValue(
        /* [out] */ _ELASTOS Byte * result) = 0;

    virtual CARAPI Int16Value(
        /* [out] */ _ELASTOS Int16 * result) = 0;

    virtual CARAPI Int32Value(
        /* [out] */ _ELASTOS Int32 * result) = 0;

    virtual CARAPI Int64Value(
        /* [out] */ _ELASTOS Int64 * result) = 0;

    virtual CARAPI FloatValue(
        /* [out] */ _ELASTOS Float * result) = 0;

    virtual CARAPI DoubleValue(
        /* [out] */ _ELASTOS Double * result) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("83211123-02D2-3219-5EA0-2011E307B9EF")
IBoolean : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IBoolean*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBoolean*)pObj->Probe(EIID_IBoolean);
    }

    static CARAPI_(IBoolean*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBoolean*)pObj->Probe(EIID_IBoolean);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Boolean * value) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("8C450E10-C2D2-E8DF-0FA1-F747BE382A2D")
IByte : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IByte*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IByte*)pObj->Probe(EIID_IByte);
    }

    static CARAPI_(IByte*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IByte*)pObj->Probe(EIID_IByte);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Byte * value) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("6B171015-B312-F592-20FC-107A7FE48BA3")
IChar32 : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IChar32*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IChar32*)pObj->Probe(EIID_IChar32);
    }

    static CARAPI_(IChar32*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IChar32*)pObj->Probe(EIID_IChar32);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Char32 * value) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("3AF41330-4152-2A14-EE07-30F083DC7700")
IInteger16 : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IInteger16*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IInteger16*)pObj->Probe(EIID_IInteger16);
    }

    static CARAPI_(IInteger16*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IInteger16*)pObj->Probe(EIID_IInteger16);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Int16 * value) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("3EF2133C-4152-2A14-EE0F-10F083DC7700")
IInteger32 : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IInteger32*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IInteger32*)pObj->Probe(EIID_IInteger32);
    }

    static CARAPI_(IInteger32*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IInteger32*)pObj->Probe(EIID_IInteger32);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Int32 * value) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("3CEF1327-4152-2A14-EE1B-20F083DC7700")
IInteger64 : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IInteger64*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IInteger64*)pObj->Probe(EIID_IInteger64);
    }

    static CARAPI_(IInteger64*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IInteger64*)pObj->Probe(EIID_IInteger64);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Int64 * value) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("813D1E2B-83D2-3217-48DC-F841EE3BC283")
IFloat : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IFloat*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IFloat*)pObj->Probe(EIID_IFloat);
    }

    static CARAPI_(IFloat*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IFloat*)pObj->Probe(EIID_IFloat);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Float * value) = 0;

    virtual CARAPI IsInfinite(
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI IsNaN(
        /* [out] */ _ELASTOS Boolean * result) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("E0781F25-2ED2-251C-5EA0-F841EE3BC283")
IDouble : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IDouble*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IDouble*)pObj->Probe(EIID_IDouble);
    }

    static CARAPI_(IDouble*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IDouble*)pObj->Probe(EIID_IDouble);
    }

    virtual CARAPI GetValue(
        /* [out] */ _ELASTOS Double * value) = 0;

    virtual CARAPI IsInfinite(
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI IsNaN(
        /* [out] */ _ELASTOS Boolean * result) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("9C710809-7712-6CD7-ACFE-1277D76CACFE")
IString : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IString*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IString*)pObj->Probe(EIID_IString);
    }

    static CARAPI_(IString*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IString*)pObj->Probe(EIID_IString);
    }

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("5EB80D0D-86D2-31C7-59BE-E8BFE1B1B4E1")
IRunnable : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IRunnable*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IRunnable*)pObj->Probe(EIID_IRunnable);
    }

    static CARAPI_(IRunnable*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IRunnable*)pObj->Probe(EIID_IRunnable);
    }

    virtual CARAPI Run() = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("40D83225-EE12-1F1F-A717-2DBA44BC78F1")
ISynchronize : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ISynchronize*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ISynchronize*)pObj->Probe(EIID_ISynchronize);
    }

    static CARAPI_(ISynchronize*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ISynchronize*)pObj->Probe(EIID_ISynchronize);
    }

    virtual CARAPI Lock() = 0;

    virtual CARAPI Unlock() = 0;

    virtual CARAPI Notify() = 0;

    virtual CARAPI NotifyAll() = 0;

    virtual CARAPI Wait() = 0;

    virtual CARAPI Wait(
        /* [in] */ _ELASTOS Int64 millis) = 0;

    virtual CARAPI Wait(
        /* [in] */ _ELASTOS Int64 millis,
        /* [in] */ _ELASTOS Int32 nanos) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("1A683151-1B92-24BC-4ED0-E984A35D1450")
IThreadUncaughtExceptionHandler : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IThreadUncaughtExceptionHandler*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThreadUncaughtExceptionHandler*)pObj->Probe(EIID_IThreadUncaughtExceptionHandler);
    }

    static CARAPI_(IThreadUncaughtExceptionHandler*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThreadUncaughtExceptionHandler*)pObj->Probe(EIID_IThreadUncaughtExceptionHandler);
    }

    virtual CARAPI UncaughtException(
        /* [in] */ Elastos::Core::IThread * thread,
        /* [in] */ _ELASTOS ECode ec) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("9768EF3A-F092-791F-8654-E27083840917")
IThread : public IInterface
{
    static const _ELASTOS Int32 MAX_PRIORITY = 10;
    static const _ELASTOS Int32 MIN_PRIORITY = 1;
    static const _ELASTOS Int32 NORM_PRIORITY = 5;
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IThread*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThread*)pObj->Probe(EIID_IThread);
    }

    static CARAPI_(IThread*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThread*)pObj->Probe(EIID_IThread);
    }

    virtual CARAPI CheckAccess() = 0;

    virtual CARAPI CountStackFrames(
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI Destroy() = 0;

    virtual CARAPI GetContextClassLoader(
        /* [out] */ Elastos::Core::IClassLoader ** outload) = 0;

    virtual CARAPI GetId(
        /* [out] */ _ELASTOS Int64 * id) = 0;

    virtual CARAPI GetName(
        /* [out] */ _ELASTOS String * name) = 0;

    virtual CARAPI GetPriority(
        /* [out] */ _ELASTOS Int32 * priority) = 0;

    virtual CARAPI GetState(
        /* [out] */ Elastos::Core::ThreadState * state) = 0;

    virtual CARAPI GetThreadGroup(
        /* [out] */ Elastos::Core::IThreadGroup ** group) = 0;

    virtual CARAPI GetUncaughtExceptionHandler(
        /* [out] */ Elastos::Core::IThreadUncaughtExceptionHandler ** handler) = 0;

    virtual CARAPI Interrupt() = 0;

    virtual CARAPI IsAlive(
        /* [out] */ _ELASTOS Boolean * isAlive) = 0;

    virtual CARAPI IsDaemon(
        /* [out] */ _ELASTOS Boolean * isDaemon) = 0;

    virtual CARAPI IsInterrupted(
        /* [out] */ _ELASTOS Boolean * isInterrupted) = 0;

    virtual CARAPI Join() = 0;

    virtual CARAPI Join(
        /* [in] */ _ELASTOS Int64 millis) = 0;

    virtual CARAPI Join(
        /* [in] */ _ELASTOS Int64 millis,
        /* [in] */ _ELASTOS Int32 nanos) = 0;

    virtual CARAPI Resume() = 0;

    virtual CARAPI SetContextClassLoader(
        /* [in] */ Elastos::Core::IClassLoader * cl) = 0;

    virtual CARAPI SetDaemon(
        /* [in] */ _ELASTOS Boolean isDaemon) = 0;

    virtual CARAPI PushInterruptAction(
        /* [in] */ Elastos::Core::IRunnable * interruptAction) = 0;

    virtual CARAPI PopInterruptAction(
        /* [in] */ Elastos::Core::IRunnable * interruptAction) = 0;

    virtual CARAPI SetName(
        /* [in] */ const _ELASTOS String& threadName) = 0;

    virtual CARAPI SetPriority(
        /* [in] */ _ELASTOS Int32 priority) = 0;

    virtual CARAPI SetUncaughtExceptionHandler(
        /* [in] */ Elastos::Core::IThreadUncaughtExceptionHandler * handler) = 0;

    virtual CARAPI Start() = 0;

    virtual CARAPI Stop() = 0;

    virtual CARAPI Suspend() = 0;

    virtual CARAPI Unpark() = 0;

    virtual CARAPI ParkFor(
        /* [in] */ _ELASTOS Int64 nanos) = 0;

    virtual CARAPI ParkUntil(
        /* [in] */ _ELASTOS Int64 time) = 0;

    virtual CARAPI Detach() = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("FDEFE316-B752-28D6-7942-B58CCFBF62DE")
IThreadGroup : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IThreadGroup*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThreadGroup*)pObj->Probe(EIID_IThreadGroup);
    }

    static CARAPI_(IThreadGroup*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThreadGroup*)pObj->Probe(EIID_IThreadGroup);
    }

    virtual CARAPI ActiveCount(
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI ActiveGroupCount(
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI AllowThreadSuspension(
        /* [in] */ _ELASTOS Boolean b,
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI CheckAccess() = 0;

    virtual CARAPI Destroy() = 0;

    virtual CARAPI Enumerate(
        /* [in] */ _ELASTOS ArrayOf<Elastos::Core::IThread *> * threads,
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI Enumerate(
        /* [in] */ _ELASTOS ArrayOf<Elastos::Core::IThread *> * threads,
        /* [in] */ _ELASTOS Boolean recurse,
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI Enumerate(
        /* [in] */ _ELASTOS ArrayOf<Elastos::Core::IThreadGroup *> * groups,
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI Enumerate(
        /* [in] */ _ELASTOS ArrayOf<Elastos::Core::IThreadGroup *> * groups,
        /* [in] */ _ELASTOS Boolean recurse,
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI GetMaxPriority(
        /* [out] */ _ELASTOS Int32 * maxPriority) = 0;

    virtual CARAPI GetName(
        /* [out] */ _ELASTOS String * name) = 0;

    virtual CARAPI GetParent(
        /* [out] */ Elastos::Core::IThreadGroup ** parent) = 0;

    virtual CARAPI Interrupt() = 0;

    virtual CARAPI IsDaemon(
        /* [out] */ _ELASTOS Boolean * isDaemon) = 0;

    virtual CARAPI IsDestroyed(
        /* [out] */ _ELASTOS Boolean * isDestroyed) = 0;

    virtual CARAPI List() = 0;

    virtual CARAPI IsParentOf(
        /* [in] */ Elastos::Core::IThreadGroup * g,
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI Resume() = 0;

    virtual CARAPI SetDaemon(
        /* [in] */ _ELASTOS Boolean isDaemon) = 0;

    virtual CARAPI SetMaxPriority(
        /* [in] */ _ELASTOS Int32 newMax) = 0;

    virtual CARAPI Stop() = 0;

    virtual CARAPI Suspend() = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("DBE01630-1E92-F78E-7F70-D4A387E3FD1F")
IClassLoader : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IClassLoader*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IClassLoader*)pObj->Probe(EIID_IClassLoader);
    }

    static CARAPI_(IClassLoader*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IClassLoader*)pObj->Probe(EIID_IClassLoader);
    }

    virtual CARAPI LoadClass(
        /* [in] */ const _ELASTOS String& className,
        /* [out] */ IClassInfo ** klass) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("39E0B948-1B92-C2BE-2DFD-78C022E6160E")
IThrowable : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IThrowable*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThrowable*)pObj->Probe(EIID_IThrowable);
    }

    static CARAPI_(IThrowable*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThrowable*)pObj->Probe(EIID_IThrowable);
    }

    virtual CARAPI FillInStackTrace() = 0;

    virtual CARAPI GetMessage(
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI GetLocalizedMessage(
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI GetStackTrace(
        /* [out, callee] */ _ELASTOS ArrayOf<Elastos::Core::IStackTraceElement *> ** outarr) = 0;

    virtual CARAPI SetStackTrace(
        /* [in] */ _ELASTOS ArrayOf<Elastos::Core::IStackTraceElement *> * trace) = 0;

    virtual CARAPI PrintStackTrace() = 0;

    virtual CARAPI PrintStackTrace(
        /* [in] */ Elastos::IO::IPrintStream * err) = 0;

    virtual CARAPI PrintStackTrace(
        /* [in] */ Elastos::IO::IPrintWriter * err) = 0;

    virtual CARAPI PrintStackTrace(
        /* [in] */ Elastos::Core::IAppendable * err,
        /* [in] */ const _ELASTOS String& indent,
        /* [in] */ _ELASTOS ArrayOf<Elastos::Core::IStackTraceElement *> * parentStack) = 0;

    virtual CARAPI InitCause(
        /* [in] */ Elastos::Core::IThrowable * throwable) = 0;

    virtual CARAPI GetCause(
        /* [out] */ Elastos::Core::IThrowable ** outthr) = 0;

    virtual CARAPI AddSuppressed(
        /* [in] */ Elastos::Core::IThrowable * throwable) = 0;

    virtual CARAPI GetSuppressed(
        /* [out, callee] */ _ELASTOS ArrayOf<Elastos::Core::IThrowable *> ** outarr) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("567B1344-8712-ABCA-E2EC-719052E70F00")
IStackTraceElement : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStackTraceElement*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStackTraceElement*)pObj->Probe(EIID_IStackTraceElement);
    }

    static CARAPI_(IStackTraceElement*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStackTraceElement*)pObj->Probe(EIID_IStackTraceElement);
    }

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("3AB44332-1952-3808-48D4-38F1F507D3F3")
IBlockGuardPolicy : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IBlockGuardPolicy*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBlockGuardPolicy*)pObj->Probe(EIID_IBlockGuardPolicy);
    }

    static CARAPI_(IBlockGuardPolicy*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBlockGuardPolicy*)pObj->Probe(EIID_IBlockGuardPolicy);
    }

    virtual CARAPI OnWriteToDisk() = 0;

    virtual CARAPI OnReadFromDisk() = 0;

    virtual CARAPI OnNetwork() = 0;

    virtual CARAPI GetPolicyMask(
        /* [out] */ _ELASTOS Int32 * mask) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("55AE362A-1952-3808-48D4-38F1839CF0AF")
IBlockGuard : public IInterface
{
    static const _ELASTOS Int32 DISALLOW_DISK_WRITE = 0x00000001;
    static const _ELASTOS Int32 DISALLOW_DISK_READ = 0x00000002;
    static const _ELASTOS Int32 DISALLOW_NETWORK = 0x00000004;
    static const _ELASTOS Int32 PASS_RESTRICTIONS_VIA_RPC = 0x00000008;
    static const _ELASTOS Int32 PENALTY_LOG = 0x00000010;
    static const _ELASTOS Int32 PENALTY_DIALOG = 0x00000020;
    static const _ELASTOS Int32 PENALTY_DEATH = 0x00000040;
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IBlockGuard*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBlockGuard*)pObj->Probe(EIID_IBlockGuard);
    }

    static CARAPI_(IBlockGuard*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBlockGuard*)pObj->Probe(EIID_IBlockGuard);
    }

    virtual CARAPI GetThreadPolicy(
        /* [out] */ Elastos::Core::IBlockGuardPolicy ** policy) = 0;

    virtual CARAPI SetThreadPolicy(
        /* [in] */ Elastos::Core::IBlockGuardPolicy * policy) = 0;

    virtual CARAPI GetLaxPolicy(
        /* [out] */ Elastos::Core::IBlockGuardPolicy ** policy) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("ECD81A3A-1FD2-3808-48D4-3811F8FE8FC0")
ICloseGuardReporter : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICloseGuardReporter*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloseGuardReporter*)pObj->Probe(EIID_ICloseGuardReporter);
    }

    static CARAPI_(ICloseGuardReporter*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloseGuardReporter*)pObj->Probe(EIID_ICloseGuardReporter);
    }

    virtual CARAPI Report(
        /* [in] */ const _ELASTOS String& message,
        /* [in] */ Elastos::Core::IThrowable * allocationSite) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("DC551F2B-1FD2-3808-48D4-38F1A3E91FF2")
ICloseGuard : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICloseGuard*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloseGuard*)pObj->Probe(EIID_ICloseGuard);
    }

    static CARAPI_(ICloseGuard*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloseGuard*)pObj->Probe(EIID_ICloseGuard);
    }

    virtual CARAPI Open(
        /* [in] */ const _ELASTOS String& closer) = 0;

    virtual CARAPI Close() = 0;

    virtual CARAPI WarnIfOpen() = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("5DE3351E-F312-36CA-0AE1-647D46E83BA3")
ICloseGuardHelper : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICloseGuardHelper*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloseGuardHelper*)pObj->Probe(EIID_ICloseGuardHelper);
    }

    static CARAPI_(ICloseGuardHelper*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICloseGuardHelper*)pObj->Probe(EIID_ICloseGuardHelper);
    }

    virtual CARAPI Get(
        /* [out] */ Elastos::Core::ICloseGuard ** guard) = 0;

    virtual CARAPI SetEnabled(
        /* [in] */ _ELASTOS Boolean enabled) = 0;

    virtual CARAPI SetReporter(
        /* [in] */ Elastos::Core::ICloseGuardReporter * reporter) = 0;

    virtual CARAPI GetReporter(
        /* [out] */ Elastos::Core::ICloseGuardReporter ** report) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("8B71222E-B192-411F-31D4-38C122068917")
IEnum : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IEnum*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IEnum*)pObj->Probe(EIID_IEnum);
    }

    static CARAPI_(IEnum*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IEnum*)pObj->Probe(EIID_IEnum);
    }

    virtual CARAPI Name(
        /* [out] */ _ELASTOS String * str) = 0;

    virtual CARAPI Ordinal(
        /* [out] */ _ELASTOS Int32 * value) = 0;

    virtual CARAPI GetDeclaringClass(
        /* [out] */ _ELASTOS InterfaceID * riid) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("4012F92C-EED2-9A1F-0F92-3BA8792AB9A9")
ISystem : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ISystem*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ISystem*)pObj->Probe(EIID_ISystem);
    }

    static CARAPI_(ISystem*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ISystem*)pObj->Probe(EIID_ISystem);
    }

    virtual CARAPI SetIn(
        /* [in] */ Elastos::IO::IInputStream * newIn) = 0;

    virtual CARAPI GetIn(
        /* [out] */ Elastos::IO::IInputStream ** inputStream) = 0;

    virtual CARAPI SetOut(
        /* [in] */ Elastos::IO::IPrintStream * newIn) = 0;

    virtual CARAPI GetOut(
        /* [out] */ Elastos::IO::IPrintStream ** outputStream) = 0;

    virtual CARAPI SetErr(
        /* [in] */ Elastos::IO::IPrintStream * newIn) = 0;

    virtual CARAPI GetErr(
        /* [out] */ Elastos::IO::IPrintStream ** errStream) = 0;

    virtual CARAPI GetCurrentTimeMillis(
        /* [out] */ _ELASTOS Int64 * value) = 0;

    virtual CARAPI GetNanoTime(
        /* [out] */ _ELASTOS Int64 * value) = 0;

    virtual CARAPI Exit(
        /* [in] */ _ELASTOS Int32 code) = 0;

    virtual CARAPI GC() = 0;

    virtual CARAPI IdentityHashCode(
        /* [in] */ IInterface * anObject,
        /* [out] */ _ELASTOS Int32 * hash) = 0;

    virtual CARAPI GetLineSeparator(
        /* [out] */ _ELASTOS String * value) = 0;

    virtual CARAPI GetEnv(
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ _ELASTOS String * value) = 0;

    virtual CARAPI GetEnv(
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ const _ELASTOS String& defaultValue,
        /* [out] */ _ELASTOS String * value) = 0;

    virtual CARAPI GetEnvs(
        /* [out] */ Elastos::Utility::IMap ** map) = 0;

    virtual CARAPI InheritedChannel(
        /* [out] */ Elastos::IO::Channels::IChannel ** channel) = 0;

    virtual CARAPI GetProperties(
        /* [out] */ Elastos::Utility::IProperties ** prop) = 0;

    virtual CARAPI GetProperty(
        /* [in] */ const _ELASTOS String& propertyName,
        /* [out] */ _ELASTOS String * value) = 0;

    virtual CARAPI GetProperty(
        /* [in] */ const _ELASTOS String& prop,
        /* [in] */ const _ELASTOS String& defaultValue,
        /* [out] */ _ELASTOS String * value) = 0;

    virtual CARAPI SetProperty(
        /* [in] */ const _ELASTOS String& prop,
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ _ELASTOS String * oldValue) = 0;

    virtual CARAPI ClearProperty(
        /* [in] */ const _ELASTOS String& prop,
        /* [out] */ _ELASTOS String * value) = 0;

    virtual CARAPI Load(
        /* [in] */ const _ELASTOS String& pathName) = 0;

    virtual CARAPI LoadLibrary(
        /* [in] */ const _ELASTOS String& libName) = 0;

    virtual CARAPI MapLibraryName(
        /* [in] */ const _ELASTOS String& userLibName,
        /* [out] */ _ELASTOS String * filename) = 0;

    virtual CARAPI RunFinalization() = 0;

    virtual CARAPI RunFinalizersOnExit(
        /* [in] */ _ELASTOS Boolean flag) = 0;

    virtual CARAPI SetProperties(
        /* [in] */ Elastos::Utility::IProperties * properties) = 0;

    virtual CARAPI GetInstructionSet(
        /* [in] */ const _ELASTOS String& abi,
        /* [out] */ _ELASTOS String * instructionSet) = 0;

    virtual CARAPI Is64BitInstructionSet(
        /* [in] */ const _ELASTOS String& instructionSet,
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI Is64BitAbi(
        /* [in] */ const _ELASTOS String& abi,
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI Is64Bit(
        /* [out] */ _ELASTOS Boolean * result) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("93222521-7092-3F8F-C8F5-FBAEB92F0000")
IThreadLocal : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IThreadLocal*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThreadLocal*)pObj->Probe(EIID_IThreadLocal);
    }

    static CARAPI_(IThreadLocal*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IThreadLocal*)pObj->Probe(EIID_IThreadLocal);
    }

    virtual CARAPI Get(
        /* [out] */ IInterface ** obj) = 0;

    virtual CARAPI InitialValue(
        /* [out] */ IInterface ** obj) = 0;

    virtual CARAPI Set(
        /* [in] */ IInterface * obj) = 0;

    virtual CARAPI Remove() = 0;

};
}
}
namespace Elastos {
namespace IO {
CAR_INTERFACE("72390E07-8712-2CD6-F9B2-3D59BEE82F71")
ISerializable : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ISerializable*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ISerializable*)pObj->Probe(EIID_ISerializable);
    }

    static CARAPI_(ISerializable*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ISerializable*)pObj->Probe(EIID_ISerializable);
    }

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("383D6121-8312-7AAC-38DE-D763F48725A3")
ICPathClassLoaderClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICPathClassLoaderClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICPathClassLoaderClassObject*)pObj->Probe(EIID_ICPathClassLoaderClassObject);
    }

    static CARAPI_(ICPathClassLoaderClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICPathClassLoaderClassObject*)pObj->Probe(EIID_ICPathClassLoaderClassObject);
    }

    virtual CARAPI CreateObjectWithClassPathParent(
        /* [in] */ const _ELASTOS String& classPath,
        /* [in] */ Elastos::Core::IClassLoader * parent,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("0E864C24-0312-7AD8-8CFE-B06414048B1B")
ICStringClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStringClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStringClassObject*)pObj->Probe(EIID_ICStringClassObject);
    }

    static CARAPI_(ICStringClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStringClassObject*)pObj->Probe(EIID_ICStringClassObject);
    }

    virtual CARAPI CreateObjectWithStr(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("ACD94F3A-8312-3205-64BC-404122861EA3")
ICBooleanClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICBooleanClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBooleanClassObject*)pObj->Probe(EIID_ICBooleanClassObject);
    }

    static CARAPI_(ICBooleanClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBooleanClassObject*)pObj->Probe(EIID_ICBooleanClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("12A14C21-8312-3736-50E8-31FAC3925110")
ICByteClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICByteClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICByteClassObject*)pObj->Probe(EIID_ICByteClassObject);
    }

    static CARAPI_(ICByteClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICByteClassObject*)pObj->Probe(EIID_ICByteClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("E47F4E22-0312-2B06-48D4-1820408FD11F")
ICChar32ClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICChar32ClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICChar32ClassObject*)pObj->Probe(EIID_ICChar32ClassObject);
    }

    static CARAPI_(ICChar32ClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICChar32ClassObject*)pObj->Probe(EIID_ICChar32ClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("7F91512E-8312-2882-54DC-0F60408FD11F")
ICInteger16ClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICInteger16ClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICInteger16ClassObject*)pObj->Probe(EIID_ICInteger16ClassObject);
    }

    static CARAPI_(ICInteger16ClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICInteger16ClassObject*)pObj->Probe(EIID_ICInteger16ClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("7D95512D-8312-2882-54DC-1F20408FD11F")
ICInteger32ClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICInteger32ClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICInteger32ClassObject*)pObj->Probe(EIID_ICInteger32ClassObject);
    }

    static CARAPI_(ICInteger32ClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICInteger32ClassObject*)pObj->Probe(EIID_ICInteger32ClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("7A935139-8312-2882-54DC-3740408FD11F")
ICInteger64ClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICInteger64ClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICInteger64ClassObject*)pObj->Probe(EIID_ICInteger64ClassObject);
    }

    static CARAPI_(ICInteger64ClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICInteger64ClassObject*)pObj->Probe(EIID_ICInteger64ClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("1C354D34-8312-2F07-6490-B8A1C7E80F4B")
ICFloatClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICFloatClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICFloatClassObject*)pObj->Probe(EIID_ICFloatClassObject);
    }

    static CARAPI_(ICFloatClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICFloatClassObject*)pObj->Probe(EIID_ICFloatClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("BA314E2A-8312-385D-4ABC-40A1C7E80F4B")
ICDoubleClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICDoubleClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICDoubleClassObject*)pObj->Probe(EIID_ICDoubleClassObject);
    }

    static CARAPI_(ICDoubleClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICDoubleClassObject*)pObj->Probe(EIID_ICDoubleClassObject);
    }

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("34375238-0312-4B2C-F518-FD61C9280816")
ICArrayOfClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICArrayOfClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICArrayOfClassObject*)pObj->Probe(EIID_ICArrayOfClassObject);
    }

    static CARAPI_(ICArrayOfClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICArrayOfClassObject*)pObj->Probe(EIID_ICArrayOfClassObject);
    }

    virtual CARAPI CreateObjectWithRiidSize(
        /* [in] */ const _ELASTOS InterfaceID & riid,
        /* [in] */ _ELASTOS Int32 size,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("8DA5AB5B-0312-7C37-855B-6260D2868D2F")
ICThrowableClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICThrowableClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICThrowableClassObject*)pObj->Probe(EIID_ICThrowableClassObject);
    }

    static CARAPI_(ICThrowableClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICThrowableClassObject*)pObj->Probe(EIID_ICThrowableClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithDetailMessage(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithDetailMessageCause(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithCause(
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("05005835-8312-1032-7090-A871428FD11F")
ICBlockGuardClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICBlockGuardClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBlockGuardClassObject*)pObj->Probe(EIID_ICBlockGuardClassObject);
    }

    static CARAPI_(ICBlockGuardClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBlockGuardClassObject*)pObj->Probe(EIID_ICBlockGuardClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("F5045831-8312-103F-7090-A871428FD11F")
ICCloseGuardClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICCloseGuardClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICCloseGuardClassObject*)pObj->Probe(EIID_ICCloseGuardClassObject);
    }

    static CARAPI_(ICCloseGuardClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICCloseGuardClassObject*)pObj->Probe(EIID_ICCloseGuardClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("ACEA5E1C-C312-B2BC-8D42-38599F11FACE")
ICCloseGuardHelperClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICCloseGuardHelperClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICCloseGuardHelperClassObject*)pObj->Probe(EIID_ICCloseGuardHelperClassObject);
    }

    static CARAPI_(ICCloseGuardHelperClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICCloseGuardHelperClassObject*)pObj->Probe(EIID_ICCloseGuardHelperClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("A5E75434-0312-7AA3-8CFE-B06414048B1B")
ICObjectClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICObjectClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICObjectClassObject*)pObj->Probe(EIID_ICObjectClassObject);
    }

    static CARAPI_(ICObjectClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICObjectClassObject*)pObj->Probe(EIID_ICObjectClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("86D8542D-8312-7ADD-8CFE-B06414048B1B")
ICSystemClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICSystemClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICSystemClassObject*)pObj->Probe(EIID_ICSystemClassObject);
    }

    static CARAPI_(ICSystemClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICSystemClassObject*)pObj->Probe(EIID_ICSystemClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("97F8E63F-0312-7AE1-8CFE-B06414048B1B")
ICThreadClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICThreadClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICThreadClassObject*)pObj->Probe(EIID_ICThreadClassObject);
    }

    static CARAPI_(ICThreadClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICThreadClassObject*)pObj->Probe(EIID_ICThreadClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithRunnable(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithRunnableThreadName(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithThreadName(
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithGroupRunnable(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithGroupRunnableThreadName(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithGroupThreadName(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithGroupRunnableThreadNameStackSize(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [in] */ _ELASTOS Int64 stackSize,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithGroupNamePriorityDaemon(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ _ELASTOS Int32 priority,
        /* [in] */ _ELASTOS Boolean daemon,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Core {
CAR_INTERFACE("C7A98734-0312-10E1-6AC8-C031438FD11F")
ICThreadGroupClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICThreadGroupClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICThreadGroupClassObject*)pObj->Probe(EIID_ICThreadGroupClassObject);
    }

    static CARAPI_(ICThreadGroupClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICThreadGroupClassObject*)pObj->Probe(EIID_ICThreadGroupClassObject);
    }

    virtual CARAPI CreateObjectWithName(
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithParentName(
        /* [in] */ Elastos::Core::IThreadGroup * parent,
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("00844150-8592-2B47-8868-DE71E347950C")
IMathContext : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IMathContext*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IMathContext*)pObj->Probe(EIID_IMathContext);
    }

    static CARAPI_(IMathContext*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IMathContext*)pObj->Probe(EIID_IMathContext);
    }

    virtual CARAPI GetPrecision(
        /* [out] */ _ELASTOS Int32 * precision) = 0;

    virtual CARAPI GetRoundingMode(
        /* [out] */ Elastos::Math::RoundingMode * roundingMode) = 0;

    virtual CARAPI Equals(
        /* [in] */ IInterface * obj,
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI GetHashCode(
        /* [out] */ _ELASTOS Int32 * pCode) = 0;

    virtual CARAPI ToString(
        /* [out] */ _ELASTOS String * string) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("0839441F-4592-2BDE-23C7-37BADF11FACE")
IMathContextHelper : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IMathContextHelper*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IMathContextHelper*)pObj->Probe(EIID_IMathContextHelper);
    }

    static CARAPI_(IMathContextHelper*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IMathContextHelper*)pObj->Probe(EIID_IMathContextHelper);
    }

    virtual CARAPI GetDECIMAL32(
        /* [out] */ Elastos::Math::IMathContext ** context) = 0;

    virtual CARAPI GetDECIMAL64(
        /* [out] */ Elastos::Math::IMathContext ** context) = 0;

    virtual CARAPI GetDECIMAL128(
        /* [out] */ Elastos::Math::IMathContext ** context) = 0;

    virtual CARAPI GetUNLIMITED(
        /* [out] */ Elastos::Math::IMathContext ** context) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("4E5AFC59-02D2-2A16-8EA8-CAF9E2071020")
IBigDecimal : public IInterface
{
    static const _ELASTOS Int32 ROUND_UP = 0;
    static const _ELASTOS Int32 ROUND_DOWN = 1;
    static const _ELASTOS Int32 ROUND_CEILING = 2;
    static const _ELASTOS Int32 ROUND_FLOOR = 3;
    static const _ELASTOS Int32 ROUND_HALF_UP = 4;
    static const _ELASTOS Int32 ROUND_HALF_DOWN = 5;
    static const _ELASTOS Int32 ROUND_HALF_EVEN = 6;
    static const _ELASTOS Int32 ROUND_UNNECESSARY = 7;
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IBigDecimal*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigDecimal*)pObj->Probe(EIID_IBigDecimal);
    }

    static CARAPI_(IBigDecimal*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigDecimal*)pObj->Probe(EIID_IBigDecimal);
    }

    virtual CARAPI Add(
        /* [in] */ Elastos::Math::IBigDecimal * augend,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Add(
        /* [in] */ Elastos::Math::IBigDecimal * augend,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Subtract(
        /* [in] */ Elastos::Math::IBigDecimal * subtrahend,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Subtract(
        /* [in] */ Elastos::Math::IBigDecimal * subtrahend,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Multiply(
        /* [in] */ Elastos::Math::IBigDecimal * multiplicand,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Multiply(
        /* [in] */ Elastos::Math::IBigDecimal * multiplicand,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Divide(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Divide(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Divide(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Divide(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI DivideToIntegralValue(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI DivideToIntegralValue(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Remainder(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Remainder(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI DivideAndRemainder(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [out, callee] */ _ELASTOS ArrayOf<Elastos::Math::IBigDecimal *> ** result) = 0;

    virtual CARAPI DivideAndRemainder(
        /* [in] */ Elastos::Math::IBigDecimal * divisor,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out, callee] */ _ELASTOS ArrayOf<Elastos::Math::IBigDecimal *> ** result) = 0;

    virtual CARAPI Pow(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Pow(
        /* [in] */ _ELASTOS Int32 n,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Abs(
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Abs(
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Negate(
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Negate(
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Plus(
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Plus(
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI GetSignum(
        /* [out] */ _ELASTOS Int32 * sign) = 0;

    virtual CARAPI GetScale(
        /* [out] */ _ELASTOS Int32 * scale) = 0;

    virtual CARAPI GetPrecision(
        /* [out] */ _ELASTOS Int32 * precision) = 0;

    virtual CARAPI GetUnscaledValue(
        /* [out] */ Elastos::Math::IBigInteger ** value) = 0;

    virtual CARAPI Round(
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI SetScale(
        /* [in] */ _ELASTOS Int32 newScale,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI SetScale(
        /* [in] */ _ELASTOS Int32 newScale,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI MovePointLeft(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI MovePointRight(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI ScaleByPowerOfTen(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI StripTrailingZeros(
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Min(
        /* [in] */ Elastos::Math::IBigDecimal * val,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI Max(
        /* [in] */ Elastos::Math::IBigDecimal * val,
        /* [out] */ Elastos::Math::IBigDecimal ** result) = 0;

    virtual CARAPI ToEngineeringString(
        /* [out] */ _ELASTOS String * representation) = 0;

    virtual CARAPI ToPlainString(
        /* [out] */ _ELASTOS String * representation) = 0;

    virtual CARAPI ToBigInteger(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI ToBigIntegerExact(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Int64ValueExact(
        /* [out] */ _ELASTOS Int64 * value) = 0;

    virtual CARAPI Int32ValueExact(
        /* [out] */ _ELASTOS Int32 * value) = 0;

    virtual CARAPI Int16ValueExact(
        /* [out] */ _ELASTOS Int16 * value) = 0;

    virtual CARAPI ByteValueExact(
        /* [out] */ _ELASTOS Byte * value) = 0;

    virtual CARAPI Ulp(
        /* [out] */ Elastos::Math::IBigDecimal ** newObj) = 0;

    virtual CARAPI Equals(
        /* [in] */ IInterface * obj,
        /* [out] */ _ELASTOS Boolean * result) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("AF233A0F-C2D2-0DAA-6AB2-30F946E83BA3")
IBigDecimalHelper : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IBigDecimalHelper*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigDecimalHelper*)pObj->Probe(EIID_IBigDecimalHelper);
    }

    static CARAPI_(IBigDecimalHelper*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigDecimalHelper*)pObj->Probe(EIID_IBigDecimalHelper);
    }

    virtual CARAPI ValueOf(
        /* [in] */ _ELASTOS Int64 unscaledVal,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Math::IBigDecimal ** newObj) = 0;

    virtual CARAPI ValueOf(
        /* [in] */ _ELASTOS Int64 unscaledVal,
        /* [out] */ Elastos::Math::IBigDecimal ** newObj) = 0;

    virtual CARAPI ValueOf(
        /* [in] */ _ELASTOS Double val,
        /* [out] */ Elastos::Math::IBigDecimal ** newObj) = 0;

    virtual CARAPI GetZERO(
        /* [out] */ Elastos::Math::IBigDecimal ** zero) = 0;

    virtual CARAPI GetONE(
        /* [out] */ Elastos::Math::IBigDecimal ** one) = 0;

    virtual CARAPI GetTEN(
        /* [out] */ Elastos::Math::IBigDecimal ** ten) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("5AFFFA04-C2D2-52AA-7CA3-2A5AFF9DBCF0")
IBigInteger : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IBigInteger*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigInteger*)pObj->Probe(EIID_IBigInteger);
    }

    static CARAPI_(IBigInteger*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigInteger*)pObj->Probe(EIID_IBigInteger);
    }

    virtual CARAPI ToByteArray(
        /* [out, callee] */ _ELASTOS ArrayOf<_ELASTOS Byte> ** bytes) = 0;

    virtual CARAPI Abs(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Negate(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Add(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Subtract(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI GetSignum(
        /* [out] */ _ELASTOS Int32 * sign) = 0;

    virtual CARAPI ShiftRight(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI ShiftLeft(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI ShiftLeftOneBit(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI BitLength(
        /* [out] */ _ELASTOS Int32 * length) = 0;

    virtual CARAPI TestBit(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI SetBit(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI ClearBit(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI FlipBit(
        /* [in] */ _ELASTOS Int32 n,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI GetLowestSetBit(
        /* [out] */ _ELASTOS Int32 * position) = 0;

    virtual CARAPI BitCount(
        /* [out] */ _ELASTOS Int32 * number) = 0;

    virtual CARAPI Not(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI And(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Or(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Xor(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI AndNot(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Min(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Max(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Gcd(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Multiply(
        /* [in] */ Elastos::Math::IBigInteger * value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Pow(
        /* [in] */ _ELASTOS Int32 exp,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI DivideAndRemainder(
        /* [in] */ Elastos::Math::IBigInteger * divisor,
        /* [out, callee] */ _ELASTOS ArrayOf<Elastos::Math::IBigInteger *> ** bigIntegerArray) = 0;

    virtual CARAPI Divide(
        /* [in] */ Elastos::Math::IBigInteger * divisor,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Remainder(
        /* [in] */ Elastos::Math::IBigInteger * divisor,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI ModInverse(
        /* [in] */ Elastos::Math::IBigInteger * divisor,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI ModPow(
        /* [in] */ Elastos::Math::IBigInteger * exponent,
        /* [in] */ Elastos::Math::IBigInteger * m,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI Mod(
        /* [in] */ Elastos::Math::IBigInteger * m,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI IsProbablePrime(
        /* [in] */ _ELASTOS Int32 certainty,
        /* [out] */ _ELASTOS Boolean * result) = 0;

    virtual CARAPI NextProbablePrime(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI ToString(
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ _ELASTOS String * string) = 0;

    virtual CARAPI Equals(
        /* [in] */ IInterface * obj,
        /* [out] */ _ELASTOS Boolean * result) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("3318370E-C2D2-52AA-7CA3-2A5A47E83BA3")
IBigIntegerHelper : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IBigIntegerHelper*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigIntegerHelper*)pObj->Probe(EIID_IBigIntegerHelper);
    }

    static CARAPI_(IBigIntegerHelper*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IBigIntegerHelper*)pObj->Probe(EIID_IBigIntegerHelper);
    }

    virtual CARAPI ValueOf(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI GetMINUSOne(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI GetZERO(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI GetONE(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

    virtual CARAPI GetTEN(
        /* [out] */ Elastos::Math::IBigInteger ** result) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("C63CFB2D-8312-2C05-5488-620183C40B3D")
ICBigDecimalClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICBigDecimalClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigDecimalClassObject*)pObj->Probe(EIID_ICBigDecimalClassObject);
    }

    static CARAPI_(ICBigDecimalClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigDecimalClassObject*)pObj->Probe(EIID_ICBigDecimalClassObject);
    }

    virtual CARAPI CreateObjectWithI32Value(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithI64Value(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithDValue(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithI64SmallValueScale(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithI32SmallValueScale(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithSVal(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithBuf(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithBufOffsetLength(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithVal(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithValScale(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithValScaleMc(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithBufOffsetLengthMc(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithBufMc(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithStrValMc(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithDValMc(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithBigIntegerValMc(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithInt32ValMc(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithInt64ValMc(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("AF105E08-B312-6AB0-839A-2C4CBE11FACE")
ICBigDecimalHelperClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICBigDecimalHelperClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigDecimalHelperClassObject*)pObj->Probe(EIID_ICBigDecimalHelperClassObject);
    }

    static CARAPI_(ICBigDecimalHelperClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigDecimalHelperClassObject*)pObj->Probe(EIID_ICBigDecimalHelperClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("DB9BE636-8312-2C05-5414-44A1E2BE1EA3")
ICBigIntegerClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICBigIntegerClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigIntegerClassObject*)pObj->Probe(EIID_ICBigIntegerClassObject);
    }

    static CARAPI_(ICBigIntegerClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigIntegerClassObject*)pObj->Probe(EIID_ICBigIntegerClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithNumBitsRandom(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithBitLengthCertaintyRandom(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithSignValue(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithSignNumberLengthDigits(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithValue(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithValueRadix(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithSignumMagnitude(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithArrValue(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("99075E0E-B312-AAB0-14DF-A88AD611FACE")
ICBigIntegerHelperClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICBigIntegerHelperClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigIntegerHelperClassObject*)pObj->Probe(EIID_ICBigIntegerHelperClassObject);
    }

    static CARAPI_(ICBigIntegerHelperClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICBigIntegerHelperClassObject*)pObj->Probe(EIID_ICBigIntegerHelperClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("77E07C0D-6312-F791-CAC8-F18DEE37F392")
ICMathContextClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICMathContextClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICMathContextClassObject*)pObj->Probe(EIID_ICMathContextClassObject);
    }

    static CARAPI_(ICMathContextClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICMathContextClassObject*)pObj->Probe(EIID_ICMathContextClassObject);
    }

    virtual CARAPI CreateObjectWithPrecision(
        /* [in] */ _ELASTOS Int32 precision,
        /* [out] */ IInterface ** newObj) = 0;

    virtual CARAPI CreateObjectWithPrecisionRoundingMode(
        /* [in] */ _ELASTOS Int32 precision,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Math {
CAR_INTERFACE("56BA5F0E-6312-F791-CAC8-F18DEE7784BE")
ICMathContextHelperClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICMathContextHelperClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICMathContextHelperClassObject*)pObj->Probe(EIID_ICMathContextHelperClassObject);
    }

    static CARAPI_(ICMathContextHelperClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICMathContextHelperClassObject*)pObj->Probe(EIID_ICMathContextHelperClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("3CAF8430-EC52-2C22-62A4-90F1839C361B")
IStructAddrinfo : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructAddrinfo*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructAddrinfo*)pObj->Probe(EIID_IStructAddrinfo);
    }

    static CARAPI_(IStructAddrinfo*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructAddrinfo*)pObj->Probe(EIID_IStructAddrinfo);
    }

    virtual CARAPI GetFlags(
        /* [out] */ _ELASTOS Int32 * flags) = 0;

    virtual CARAPI SetFlags(
        /* [in] */ _ELASTOS Int32 flags) = 0;

    virtual CARAPI GetFamily(
        /* [out] */ _ELASTOS Int32 * family) = 0;

    virtual CARAPI SetFamily(
        /* [in] */ _ELASTOS Int32 family) = 0;

    virtual CARAPI GetSocktype(
        /* [out] */ _ELASTOS Int32 * socktype) = 0;

    virtual CARAPI SetSocktype(
        /* [in] */ _ELASTOS Int32 socktype) = 0;

    virtual CARAPI GetProtocol(
        /* [out] */ _ELASTOS Int32 * protocol) = 0;

    virtual CARAPI SetProtocol(
        /* [in] */ _ELASTOS Int32 protocol) = 0;

    virtual CARAPI GetAddrlen(
        /* [out] */ _ELASTOS Int32 * addrlen) = 0;

    virtual CARAPI GetAddr(
        /* [out] */ Elastos::Net::IInetAddress ** addr) = 0;

    virtual CARAPI SetAddr(
        /* [in] */ Elastos::Net::IInetAddress * addr) = 0;

    virtual CARAPI GetNext(
        /* [out] */ Elastos::Droid::System::IStructAddrinfo ** next) = 0;

    virtual CARAPI SetNext(
        /* [in] */ Elastos::Droid::System::IStructAddrinfo * next) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("19D1553D-EC52-2F07-6498-70F183DC72CD")
IStructFlock : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructFlock*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructFlock*)pObj->Probe(EIID_IStructFlock);
    }

    static CARAPI_(IStructFlock*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructFlock*)pObj->Probe(EIID_IStructFlock);
    }

    virtual CARAPI GetType(
        /* [out] */ _ELASTOS Int16 * type) = 0;

    virtual CARAPI SetType(
        /* [in] */ _ELASTOS Int16 type) = 0;

    virtual CARAPI GetWhence(
        /* [out] */ _ELASTOS Int16 * whence) = 0;

    virtual CARAPI SetWhence(
        /* [in] */ _ELASTOS Int16 whence) = 0;

    virtual CARAPI GetStart(
        /* [out] */ _ELASTOS Int64 * start) = 0;

    virtual CARAPI SetStart(
        /* [in] */ _ELASTOS Int64 start) = 0;

    virtual CARAPI GetLen(
        /* [out] */ _ELASTOS Int64 * len) = 0;

    virtual CARAPI SetLen(
        /* [in] */ _ELASTOS Int64 len) = 0;

    virtual CARAPI GetPid(
        /* [out] */ _ELASTOS Int32 * pid) = 0;

    virtual CARAPI SetPid(
        /* [in] */ _ELASTOS Int32 pid) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("EE423056-1B12-AF1C-20F0-D59D86C68F2A")
IStructGroupReq : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructGroupReq*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructGroupReq*)pObj->Probe(EIID_IStructGroupReq);
    }

    static CARAPI_(IStructGroupReq*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructGroupReq*)pObj->Probe(EIID_IStructGroupReq);
    }

    virtual CARAPI GetGrInterface(
        /* [out] */ _ELASTOS Int32 * grInterface) = 0;

    virtual CARAPI GetGrGroup(
        /* [out] */ Elastos::Net::IInetAddress ** grGroup) = 0;

    virtual CARAPI ToString(
        /* [out] */ _ELASTOS String * str) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("59F83C33-6C52-3508-64E0-98617A030A1A")
IStructGroupSourceReq : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructGroupSourceReq*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructGroupSourceReq*)pObj->Probe(EIID_IStructGroupSourceReq);
    }

    static CARAPI_(IStructGroupSourceReq*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructGroupSourceReq*)pObj->Probe(EIID_IStructGroupSourceReq);
    }

    virtual CARAPI GetGsrInterface(
        /* [out] */ _ELASTOS Int32 * itf) = 0;

    virtual CARAPI GetGsrGroup(
        /* [out] */ Elastos::Net::IInetAddress ** group) = 0;

    virtual CARAPI GetGsrSource(
        /* [out] */ Elastos::Net::IInetAddress ** source) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("70122325-EC52-F50A-EFFF-20C754468A14")
IStructLinger : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructLinger*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructLinger*)pObj->Probe(EIID_IStructLinger);
    }

    static CARAPI_(IStructLinger*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructLinger*)pObj->Probe(EIID_IStructLinger);
    }

    virtual CARAPI GetOnoff(
        /* [out] */ _ELASTOS Int32 * onoff) = 0;

    virtual CARAPI GetLinger(
        /* [out] */ _ELASTOS Int32 * linger) = 0;

    virtual CARAPI IsOn(
        /* [out] */ _ELASTOS Boolean * isOn) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("DF312F21-EC52-240C-6CD8-D071E207B9A0")
IStructPasswd : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructPasswd*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructPasswd*)pObj->Probe(EIID_IStructPasswd);
    }

    static CARAPI_(IStructPasswd*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructPasswd*)pObj->Probe(EIID_IStructPasswd);
    }

    virtual CARAPI GetName(
        /* [out] */ _ELASTOS String * name) = 0;

    virtual CARAPI GetUid(
        /* [out] */ _ELASTOS Int32 * uid) = 0;

    virtual CARAPI GetGid(
        /* [out] */ _ELASTOS Int32 * gid) = 0;

    virtual CARAPI GetDir(
        /* [out] */ _ELASTOS String * dir) = 0;

    virtual CARAPI GetShell(
        /* [out] */ _ELASTOS String * shell) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("9E745429-6C52-29B0-4EFC-20F7E084E607")
IStructPollfd : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructPollfd*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructPollfd*)pObj->Probe(EIID_IStructPollfd);
    }

    static CARAPI_(IStructPollfd*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructPollfd*)pObj->Probe(EIID_IStructPollfd);
    }

    virtual CARAPI GetFd(
        /* [out] */ Elastos::IO::IFileDescriptor ** fd) = 0;

    virtual CARAPI SetFd(
        /* [in] */ Elastos::IO::IFileDescriptor * fd) = 0;

    virtual CARAPI GetEvents(
        /* [out] */ _ELASTOS Int16 * events) = 0;

    virtual CARAPI SetEvents(
        /* [in] */ _ELASTOS Int16 events) = 0;

    virtual CARAPI GetRevents(
        /* [out] */ _ELASTOS Int16 * revents) = 0;

    virtual CARAPI SetRevents(
        /* [in] */ _ELASTOS Int16 revents) = 0;

    virtual CARAPI GetUserData(
        /* [out] */ IInterface ** userData) = 0;

    virtual CARAPI SetUserData(
        /* [in] */ IInterface * userData) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("05606C25-6C52-3FD6-C835-409183DC4119")
IStructStat : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructStat*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructStat*)pObj->Probe(EIID_IStructStat);
    }

    static CARAPI_(IStructStat*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructStat*)pObj->Probe(EIID_IStructStat);
    }

    virtual CARAPI GetDev(
        /* [out] */ _ELASTOS Int64 * dev) = 0;

    virtual CARAPI GetIno(
        /* [out] */ _ELASTOS Int64 * ino) = 0;

    virtual CARAPI GetMode(
        /* [out] */ _ELASTOS Int32 * mode) = 0;

    virtual CARAPI GetNlink(
        /* [out] */ _ELASTOS Int64 * nlink) = 0;

    virtual CARAPI GetUid(
        /* [out] */ _ELASTOS Int32 * uid) = 0;

    virtual CARAPI GetGid(
        /* [out] */ _ELASTOS Int32 * gid) = 0;

    virtual CARAPI GetRdev(
        /* [out] */ _ELASTOS Int64 * rdev) = 0;

    virtual CARAPI GetSize(
        /* [out] */ _ELASTOS Int64 * size) = 0;

    virtual CARAPI GetAtime(
        /* [out] */ _ELASTOS Int64 * atime) = 0;

    virtual CARAPI GetMtime(
        /* [out] */ _ELASTOS Int64 * mtime) = 0;

    virtual CARAPI GetCtime(
        /* [out] */ _ELASTOS Int64 * ctime) = 0;

    virtual CARAPI GetBlksize(
        /* [out] */ _ELASTOS Int64 * blksize) = 0;

    virtual CARAPI GetBlocks(
        /* [out] */ _ELASTOS Int64 * blocks) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("25936B3B-6C52-1FD6-52D8-F8416E810D16")
IStructStatVfs : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructStatVfs*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructStatVfs*)pObj->Probe(EIID_IStructStatVfs);
    }

    static CARAPI_(IStructStatVfs*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructStatVfs*)pObj->Probe(EIID_IStructStatVfs);
    }

    virtual CARAPI GetBsize(
        /* [out] */ _ELASTOS Int64 * bsize) = 0;

    virtual CARAPI GetFrsize(
        /* [out] */ _ELASTOS Int64 * frsize) = 0;

    virtual CARAPI GetBlocks(
        /* [out] */ _ELASTOS Int64 * blocks) = 0;

    virtual CARAPI GetBfree(
        /* [out] */ _ELASTOS Int64 * bfree) = 0;

    virtual CARAPI GetBavail(
        /* [out] */ _ELASTOS Int64 * bavail) = 0;

    virtual CARAPI GetFiles(
        /* [out] */ _ELASTOS Int64 * files) = 0;

    virtual CARAPI GetFfree(
        /* [out] */ _ELASTOS Int64 * ffree) = 0;

    virtual CARAPI GetFavail(
        /* [out] */ _ELASTOS Int64 * favail) = 0;

    virtual CARAPI GetFsid(
        /* [out] */ _ELASTOS Int64 * fsid) = 0;

    virtual CARAPI GetFlag(
        /* [out] */ _ELASTOS Int64 * flag) = 0;

    virtual CARAPI GetNamemax(
        /* [out] */ _ELASTOS Int64 * namemax) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("C6442424-EC52-39E2-48BC-F8418E030A13")
IStructTimeval : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructTimeval*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructTimeval*)pObj->Probe(EIID_IStructTimeval);
    }

    static CARAPI_(IStructTimeval*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructTimeval*)pObj->Probe(EIID_IStructTimeval);
    }

    virtual CARAPI GetSec(
        /* [out] */ _ELASTOS Int64 * sec) = 0;

    virtual CARAPI GetUsec(
        /* [out] */ _ELASTOS Int64 * usec) = 0;

    virtual CARAPI ToMillis(
        /* [out] */ _ELASTOS Int64 * millis) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("9E281F2B-EC52-39E2-48BC-8880E2C58CFB")
IStructTimevalHelper : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructTimevalHelper*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructTimevalHelper*)pObj->Probe(EIID_IStructTimevalHelper);
    }

    static CARAPI_(IStructTimevalHelper*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructTimevalHelper*)pObj->Probe(EIID_IStructTimevalHelper);
    }

    virtual CARAPI FromMillis(
        /* [in] */ _ELASTOS Int64 millis,
        /* [out] */ Elastos::Droid::System::IStructTimeval ** tv) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("D7791F37-6C52-260F-6AE8-FF412E038B13")
IStructUcred : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructUcred*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructUcred*)pObj->Probe(EIID_IStructUcred);
    }

    static CARAPI_(IStructUcred*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructUcred*)pObj->Probe(EIID_IStructUcred);
    }

    virtual CARAPI GetPid(
        /* [out] */ _ELASTOS Int32 * pid) = 0;

    virtual CARAPI GetUid(
        /* [out] */ _ELASTOS Int32 * uid) = 0;

    virtual CARAPI GetGid(
        /* [out] */ _ELASTOS Int32 * gid) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("0900422E-6C52-370F-6CC4-200103C50F72")
IStructUtsname : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IStructUtsname*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructUtsname*)pObj->Probe(EIID_IStructUtsname);
    }

    static CARAPI_(IStructUtsname*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IStructUtsname*)pObj->Probe(EIID_IStructUtsname);
    }

    virtual CARAPI GetSysname(
        /* [out] */ _ELASTOS String * sysname) = 0;

    virtual CARAPI GetNodename(
        /* [out] */ _ELASTOS String * nodename) = 0;

    virtual CARAPI GetRelease(
        /* [out] */ _ELASTOS String * release) = 0;

    virtual CARAPI GetVersion(
        /* [out] */ _ELASTOS String * ver) = 0;

    virtual CARAPI GetMachine(
        /* [out] */ _ELASTOS String * machine) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("70092F0A-C8D2-F2AB-8D46-B2ECDA7F56A3")
IZygoteHooks : public IInterface
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(IZygoteHooks*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (IZygoteHooks*)pObj->Probe(EIID_IZygoteHooks);
    }

    static CARAPI_(IZygoteHooks*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (IZygoteHooks*)pObj->Probe(EIID_IZygoteHooks);
    }

    virtual CARAPI PreFork() = 0;

    virtual CARAPI PostForkChild(
        /* [in] */ _ELASTOS Int32 debugFlags,
        /* [in] */ const _ELASTOS String& instructionSet) = 0;

    virtual CARAPI PostForkCommon() = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("CF655C22-8312-45D8-58C4-4821438FD11F")
ICStructAddrinfoClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructAddrinfoClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructAddrinfoClassObject*)pObj->Probe(EIID_ICStructAddrinfoClassObject);
    }

    static CARAPI_(ICStructAddrinfoClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructAddrinfoClassObject*)pObj->Probe(EIID_ICStructAddrinfoClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("ACC56557-0312-3836-5E41-E0AB3B0D0D06")
ICStructGroupReqClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructGroupReqClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructGroupReqClassObject*)pObj->Probe(EIID_ICStructGroupReqClassObject);
    }

    static CARAPI_(ICStructGroupReqClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructGroupReqClassObject*)pObj->Probe(EIID_ICStructGroupReqClassObject);
    }

    virtual CARAPI CreateObjectWithGr_interfaceGr_group(
        /* [in] */ _ELASTOS Int32 gr_interface,
        /* [in] */ Elastos::Net::IInetAddress * gr_group,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("66025E3A-8312-15D8-EADF-D763F48725A3")
ICStructLingerClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructLingerClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructLingerClassObject*)pObj->Probe(EIID_ICStructLingerClassObject);
    }

    static CARAPI_(ICStructLingerClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructLingerClassObject*)pObj->Probe(EIID_ICStructLingerClassObject);
    }

    virtual CARAPI CreateObjectWithL_onoffL_linger(
        /* [in] */ _ELASTOS Int32 l_onoff,
        /* [in] */ _ELASTOS Int32 l_linger,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("0D06702A-8312-19D8-48D8-B0A1E3841EA3")
ICStructPasswdClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructPasswdClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructPasswdClassObject*)pObj->Probe(EIID_ICStructPasswdClassObject);
    }

    static CARAPI_(ICStructPasswdClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructPasswdClassObject*)pObj->Probe(EIID_ICStructPasswdClassObject);
    }

    virtual CARAPI CreateObjectWithPw_namePw_uidPw_gidPw_dirPw_shell(
        /* [in] */ const _ELASTOS String& pw_name,
        /* [in] */ _ELASTOS Int32 pw_uid,
        /* [in] */ _ELASTOS Int32 pw_gid,
        /* [in] */ const _ELASTOS String& pw_dir,
        /* [in] */ const _ELASTOS String& pw_shell,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("43B85A34-8312-60D8-539C-D063F48725A3")
ICStructPollfdClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructPollfdClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructPollfdClassObject*)pObj->Probe(EIID_ICStructPollfdClassObject);
    }

    static CARAPI_(ICStructPollfdClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructPollfdClassObject*)pObj->Probe(EIID_ICStructPollfdClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("8F45863D-8312-ACD8-F518-FD61C9280816")
ICStructStatClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructStatClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructStatClassObject*)pObj->Probe(EIID_ICStructStatClassObject);
    }

    static CARAPI_(ICStructStatClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructStatClassObject*)pObj->Probe(EIID_ICStructStatClassObject);
    }

    virtual CARAPI CreateObjectWithDevInoModeNlinkUidGidRdevSizeAtimeMtimeCtimeBlksizeBlocks(
        /* [in] */ _ELASTOS Int64 dev,
        /* [in] */ _ELASTOS Int64 ino,
        /* [in] */ _ELASTOS Int32 mode,
        /* [in] */ _ELASTOS Int64 nlink,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [in] */ _ELASTOS Int64 rdev,
        /* [in] */ _ELASTOS Int64 size,
        /* [in] */ _ELASTOS Int64 atime,
        /* [in] */ _ELASTOS Int64 mtime,
        /* [in] */ _ELASTOS Int64 ctime,
        /* [in] */ _ELASTOS Int64 blksize,
        /* [in] */ _ELASTOS Int64 blocks,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("85E0A12E-8312-ACD8-3FA4-B0A1C7E80F4B")
ICStructStatVfsClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructStatVfsClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructStatVfsClassObject*)pObj->Probe(EIID_ICStructStatVfsClassObject);
    }

    static CARAPI_(ICStructStatVfsClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructStatVfsClassObject*)pObj->Probe(EIID_ICStructStatVfsClassObject);
    }

    virtual CARAPI CreateObjectWithF_bsizeF_frsizeF_blocksF_bfreeF_bavailF_filesF_ffreeF_favailF_fsidF_flagF_namemax(
        /* [in] */ _ELASTOS Int64 f_bsize,
        /* [in] */ _ELASTOS Int64 f_frsize,
        /* [in] */ _ELASTOS Int64 f_blocks,
        /* [in] */ _ELASTOS Int64 f_bfree,
        /* [in] */ _ELASTOS Int64 f_bavail,
        /* [in] */ _ELASTOS Int64 f_files,
        /* [in] */ _ELASTOS Int64 f_ffree,
        /* [in] */ _ELASTOS Int64 f_favail,
        /* [in] */ _ELASTOS Int64 f_fsid,
        /* [in] */ _ELASTOS Int64 f_flag,
        /* [in] */ _ELASTOS Int64 f_namemax,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("69055930-8312-0FD8-5EC8-30E1428FD11F")
ICStructFlockClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructFlockClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructFlockClassObject*)pObj->Probe(EIID_ICStructFlockClassObject);
    }

    static CARAPI_(ICStructFlockClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructFlockClassObject*)pObj->Probe(EIID_ICStructFlockClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("F6475731-8312-C5D8-7390-78A1C7E80F4B")
ICStructTimevalClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructTimevalClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructTimevalClassObject*)pObj->Probe(EIID_ICStructTimevalClassObject);
    }

    static CARAPI_(ICStructTimevalClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructTimevalClassObject*)pObj->Probe(EIID_ICStructTimevalClassObject);
    }

    virtual CARAPI CreateObjectWithSecUsec(
        /* [in] */ _ELASTOS Int64 sec,
        /* [in] */ _ELASTOS Int64 usec,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("EE39613A-8312-C5D8-7390-781101C58B19")
ICStructTimevalHelperClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructTimevalHelperClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructTimevalHelperClassObject*)pObj->Probe(EIID_ICStructTimevalHelperClassObject);
    }

    static CARAPI_(ICStructTimevalHelperClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructTimevalHelperClassObject*)pObj->Probe(EIID_ICStructTimevalHelperClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("21545725-8312-1ED8-4CD4-D0AFC7E80F4B")
ICStructUcredClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructUcredClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructUcredClassObject*)pObj->Probe(EIID_ICStructUcredClassObject);
    }

    static CARAPI_(ICStructUcredClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructUcredClassObject*)pObj->Probe(EIID_ICStructUcredClassObject);
    }

    virtual CARAPI CreateObjectWithPidUidGid(
        /* [in] */ _ELASTOS Int32 pid,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("CA5F7029-8312-1ED8-6ED8-884102060A3D")
ICStructUtsnameClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructUtsnameClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructUtsnameClassObject*)pObj->Probe(EIID_ICStructUtsnameClassObject);
    }

    static CARAPI_(ICStructUtsnameClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructUtsnameClassObject*)pObj->Probe(EIID_ICStructUtsnameClassObject);
    }

    virtual CARAPI CreateObjectWithSysnameNodenameReleaseVerMachine(
        /* [in] */ const _ELASTOS String& sysname,
        /* [in] */ const _ELASTOS String& nodename,
        /* [in] */ const _ELASTOS String& release,
        /* [in] */ const _ELASTOS String& ver,
        /* [in] */ const _ELASTOS String& machine,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("E08D6529-8312-10D8-6AC8-C031C3F40614")
ICStructGroupSourceReqClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICStructGroupSourceReqClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructGroupSourceReqClassObject*)pObj->Probe(EIID_ICStructGroupSourceReqClassObject);
    }

    static CARAPI_(ICStructGroupSourceReqClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICStructGroupSourceReqClassObject*)pObj->Probe(EIID_ICStructGroupSourceReqClassObject);
    }

    virtual CARAPI CreateObjectWithItfGroupSource(
        /* [in] */ _ELASTOS Int32 itf,
        /* [in] */ Elastos::Net::IInetAddress * group,
        /* [in] */ Elastos::Net::IInetAddress * source,
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
CAR_INTERFACE("6DEE5915-3312-AAF2-7CA3-912CBB36F392")
ICZygoteHooksClassObject : public IClassObject
{
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ _ELASTOS REIID riid) = 0;

    static CARAPI_(ICZygoteHooksClassObject*) Probe(PInterface pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICZygoteHooksClassObject*)pObj->Probe(EIID_ICZygoteHooksClassObject);
    }

    static CARAPI_(ICZygoteHooksClassObject*) Probe(IObject* pObj)
    {
        if (pObj == NULL) return NULL;
        return (ICZygoteHooksClassObject*)pObj->Probe(EIID_ICZygoteHooksClassObject);
    }

    virtual CARAPI CreateObjectWithDefaultCtor(
        /* [out] */ IInterface ** newObj) = 0;

};
}
}
}
namespace Elastos {
namespace Core {
class CPathClassLoader
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& classPath,
        /* [in] */ Elastos::Core::IClassLoader * parent,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICPathClassLoaderClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CPathClassLoader, RGM_SAME_DOMAIN,
                EIID_ICPathClassLoaderClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithClassPathParent(classPath, parent, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& classPath,
        /* [in] */ Elastos::Core::IClassLoader * parent,
        /* [out] */ Elastos::Core::IClassLoader** __IClassLoader)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(classPath, parent, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IClassLoader = (Elastos::Core::IClassLoader*)__pNewObj->Probe(Elastos::Core::EIID_IClassLoader);
        if (*__IClassLoader) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& classPath,
        /* [in] */ Elastos::Core::IClassLoader * parent,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(classPath, parent, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& classPath,
        /* [in] */ Elastos::Core::IClassLoader * parent,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(classPath, parent, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& classPath,
        /* [in] */ Elastos::Core::IClassLoader * parent,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(classPath, parent, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CString
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStringClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CString, RGM_SAME_DOMAIN,
                EIID_ICStringClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithStr(str, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ Elastos::Core::ICharSequence** __ICharSequence)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(str, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ICharSequence = (Elastos::Core::ICharSequence*)__pNewObj->Probe(Elastos::Core::EIID_ICharSequence);
        if (*__ICharSequence) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ Elastos::Core::IString** __IString)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(str, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IString = (Elastos::Core::IString*)__pNewObj->Probe(Elastos::Core::EIID_IString);
        if (*__IString) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(str, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(str, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(str, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& str,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(str, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CBoolean
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBooleanClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBoolean, RGM_SAME_DOMAIN,
                EIID_ICBooleanClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ Elastos::Core::IBoolean** __IBoolean)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBoolean = (Elastos::Core::IBoolean*)__pNewObj->Probe(Elastos::Core::EIID_IBoolean);
        if (*__IBoolean) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Boolean value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CByte
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICByteClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CByte, RGM_SAME_DOMAIN,
                EIID_ICByteClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ Elastos::Core::IByte** __IByte)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IByte = (Elastos::Core::IByte*)__pNewObj->Probe(Elastos::Core::EIID_IByte);
        if (*__IByte) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Byte value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CChar32
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICChar32ClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CChar32, RGM_SAME_DOMAIN,
                EIID_ICChar32ClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ Elastos::Core::IChar32** __IChar32)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IChar32 = (Elastos::Core::IChar32*)__pNewObj->Probe(Elastos::Core::EIID_IChar32);
        if (*__IChar32) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Char32 value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CInteger16
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICInteger16ClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CInteger16, RGM_SAME_DOMAIN,
                EIID_ICInteger16ClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ Elastos::Core::IInteger16** __IInteger16)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IInteger16 = (Elastos::Core::IInteger16*)__pNewObj->Probe(Elastos::Core::EIID_IInteger16);
        if (*__IInteger16) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int16 value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CInteger32
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICInteger32ClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CInteger32, RGM_SAME_DOMAIN,
                EIID_ICInteger32ClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ Elastos::Core::IInteger32** __IInteger32)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IInteger32 = (Elastos::Core::IInteger32*)__pNewObj->Probe(Elastos::Core::EIID_IInteger32);
        if (*__IInteger32) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CInteger64
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICInteger64ClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CInteger64, RGM_SAME_DOMAIN,
                EIID_ICInteger64ClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Core::IInteger64** __IInteger64)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IInteger64 = (Elastos::Core::IInteger64*)__pNewObj->Probe(Elastos::Core::EIID_IInteger64);
        if (*__IInteger64) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CFloat
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICFloatClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CFloat, RGM_SAME_DOMAIN,
                EIID_ICFloatClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ Elastos::Core::IFloat** __IFloat)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IFloat = (Elastos::Core::IFloat*)__pNewObj->Probe(Elastos::Core::EIID_IFloat);
        if (*__IFloat) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Float value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CDouble
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICDoubleClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CDouble, RGM_SAME_DOMAIN,
                EIID_ICDoubleClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ Elastos::Core::IDouble** __IDouble)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IDouble = (Elastos::Core::IDouble*)__pNewObj->Probe(Elastos::Core::EIID_IDouble);
        if (*__IDouble) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ Elastos::IO::ISerializable** __ISerializable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISerializable = (Elastos::IO::ISerializable*)__pNewObj->Probe(Elastos::IO::EIID_ISerializable);
        if (*__ISerializable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CArrayOf
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS InterfaceID & riid,
        /* [in] */ _ELASTOS Int32 size,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICArrayOfClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CArrayOf, RGM_SAME_DOMAIN,
                EIID_ICArrayOfClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithRiidSize(riid, size, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS InterfaceID & riid,
        /* [in] */ _ELASTOS Int32 size,
        /* [out] */ Elastos::Core::IArrayOf** __IArrayOf)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(riid, size, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IArrayOf = (Elastos::Core::IArrayOf*)__pNewObj->Probe(Elastos::Core::EIID_IArrayOf);
        if (*__IArrayOf) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS InterfaceID & riid,
        /* [in] */ _ELASTOS Int32 size,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(riid, size, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS InterfaceID & riid,
        /* [in] */ _ELASTOS Int32 size,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(riid, size, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS InterfaceID & riid,
        /* [in] */ _ELASTOS Int32 size,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(riid, size, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CThrowable
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThrowableClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThrowable, RGM_SAME_DOMAIN,
                EIID_ICThrowableClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithDetailMessage(detailMessage, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThrowableClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThrowable, RGM_SAME_DOMAIN,
                EIID_ICThrowableClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithDetailMessageCause(detailMessage, cause, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThrowableClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThrowable, RGM_SAME_DOMAIN,
                EIID_ICThrowableClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithCause(cause, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::IThrowable** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThrowable, RGM_SAME_DOMAIN, Elastos::Core::EIID_IThrowable, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThrowable, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThrowable, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThrowable, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [out] */ Elastos::Core::IThrowable** __IThrowable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThrowable = (Elastos::Core::IThrowable*)__pNewObj->Probe(Elastos::Core::EIID_IThrowable);
        if (*__IThrowable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ Elastos::Core::IThrowable** __IThrowable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThrowable = (Elastos::Core::IThrowable*)__pNewObj->Probe(Elastos::Core::EIID_IThrowable);
        if (*__IThrowable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& detailMessage,
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(detailMessage, cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ Elastos::Core::IThrowable** __IThrowable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThrowable = (Elastos::Core::IThrowable*)__pNewObj->Probe(Elastos::Core::EIID_IThrowable);
        if (*__IThrowable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThrowable * cause,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(cause, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CBlockGuard
{
public:
    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::IBlockGuard** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBlockGuard, RGM_SAME_DOMAIN, Elastos::Core::EIID_IBlockGuard, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBlockGuard, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBlockGuard, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBlockGuard, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Core {
class CCloseGuard
{
public:
    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ICloseGuard** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuard, RGM_SAME_DOMAIN, Elastos::Core::EIID_ICloseGuard, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuard, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuard, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuard, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Core {
class CCloseGuardHelper
{
public:
    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ICloseGuardHelper** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuardHelper, RGM_SAME_DOMAIN, Elastos::Core::EIID_ICloseGuardHelper, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuardHelper, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuardHelper, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CCloseGuardHelper, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Core {
class CObject
{
public:
    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CObject, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CObject, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CObject, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Core {
class CSystem
{
public:
    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISystem** __object)
    {
        return _CObject_CreateInstance(ECLSID_CSystem, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISystem, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CSystem, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CSystem, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CSystem, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Core {
class CThread
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithRunnable(runnable, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithRunnableThreadName(runnable, threadName, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithThreadName(threadName, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithGroupRunnable(group, runnable, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithGroupRunnableThreadName(group, runnable, threadName, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithGroupThreadName(group, threadName, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [in] */ _ELASTOS Int64 stackSize,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithGroupRunnableThreadNameStackSize(group, runnable, threadName, stackSize, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ _ELASTOS Int32 priority,
        /* [in] */ _ELASTOS Boolean daemon,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThread, RGM_SAME_DOMAIN,
                EIID_ICThreadClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithGroupNamePriorityDaemon(group, name, priority, daemon, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::IThread** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThread, RGM_SAME_DOMAIN, Elastos::Core::EIID_IThread, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::IRunnable** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThread, RGM_SAME_DOMAIN, Elastos::Core::EIID_IRunnable, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThread, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThread, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThread, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& threadName,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, threadName, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [in] */ _ELASTOS Int64 stackSize,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, stackSize, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [in] */ _ELASTOS Int64 stackSize,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, stackSize, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [in] */ _ELASTOS Int64 stackSize,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, stackSize, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [in] */ _ELASTOS Int64 stackSize,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, stackSize, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ Elastos::Core::IRunnable * runnable,
        /* [in] */ const _ELASTOS String& threadName,
        /* [in] */ _ELASTOS Int64 stackSize,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, runnable, threadName, stackSize, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ _ELASTOS Int32 priority,
        /* [in] */ _ELASTOS Boolean daemon,
        /* [out] */ Elastos::Core::IThread** __IThread)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, name, priority, daemon, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThread = (Elastos::Core::IThread*)__pNewObj->Probe(Elastos::Core::EIID_IThread);
        if (*__IThread) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ _ELASTOS Int32 priority,
        /* [in] */ _ELASTOS Boolean daemon,
        /* [out] */ Elastos::Core::IRunnable** __IRunnable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, name, priority, daemon, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IRunnable = (Elastos::Core::IRunnable*)__pNewObj->Probe(Elastos::Core::EIID_IRunnable);
        if (*__IRunnable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ _ELASTOS Int32 priority,
        /* [in] */ _ELASTOS Boolean daemon,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, name, priority, daemon, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ _ELASTOS Int32 priority,
        /* [in] */ _ELASTOS Boolean daemon,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, name, priority, daemon, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * group,
        /* [in] */ const _ELASTOS String& name,
        /* [in] */ _ELASTOS Int32 priority,
        /* [in] */ _ELASTOS Boolean daemon,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(group, name, priority, daemon, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Core {
class CThreadGroup
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadGroupClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThreadGroup, RGM_SAME_DOMAIN,
                EIID_ICThreadGroupClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithName(name, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Core::IThreadGroup * parent,
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICThreadGroupClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CThreadGroup, RGM_SAME_DOMAIN,
                EIID_ICThreadGroupClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithParentName(parent, name, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::IThreadGroup** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThreadGroup, RGM_SAME_DOMAIN, Elastos::Core::EIID_IThreadGroup, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThreadGroup, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThreadGroup, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CThreadGroup, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ Elastos::Core::IThreadGroup** __IThreadGroup)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThreadGroup = (Elastos::Core::IThreadGroup*)__pNewObj->Probe(Elastos::Core::EIID_IThreadGroup);
        if (*__IThreadGroup) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * parent,
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ Elastos::Core::IThreadGroup** __IThreadGroup)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(parent, name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IThreadGroup = (Elastos::Core::IThreadGroup*)__pNewObj->Probe(Elastos::Core::EIID_IThreadGroup);
        if (*__IThreadGroup) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * parent,
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(parent, name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * parent,
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(parent, name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Core::IThreadGroup * parent,
        /* [in] */ const _ELASTOS String& name,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(parent, name, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Math {
class CBigDecimal
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithI32Value(i32Value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithI64Value(i64Value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithDValue(dValue, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithI64SmallValueScale(i64SmallValue, scale, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithI32SmallValueScale(i32SmallValue, scale, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithSVal(sVal, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithBuf(buf, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithBufOffsetLength(buf, offset, length, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithVal(val, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValScale(val, scale, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValScaleMc(val, scale, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithBufOffsetLengthMc(buf, offset, length, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithBufMc(buf, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithStrValMc(strVal, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithDValMc(dVal, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithBigIntegerValMc(bigIntegerVal, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithInt32ValMc(int32Val, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigDecimalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigDecimal, RGM_SAME_DOMAIN,
                EIID_ICBigDecimalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithInt64ValMc(int64Val, mc, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32Value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64Value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64Value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dValue,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 i64SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i64SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 i32SmallValue,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(i32SmallValue, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sVal, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sVal, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sVal, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sVal, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sVal, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sVal,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sVal, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * val,
        /* [in] */ _ELASTOS Int32 scale,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(val, scale, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ _ELASTOS Int32 offset,
        /* [in] */ _ELASTOS Int32 length,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, offset, length, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Char32> & buf,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(buf, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(strVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(strVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(strVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(strVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(strVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& strVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(strVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Double dVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bigIntegerVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bigIntegerVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bigIntegerVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bigIntegerVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bigIntegerVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ Elastos::Math::IBigInteger * bigIntegerVal,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bigIntegerVal, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int32Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int32Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int32Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int32Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int32Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 int32Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int32Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int64Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int64Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Math::IBigDecimal** __IBigDecimal)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int64Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigDecimal = (Elastos::Math::IBigDecimal*)__pNewObj->Probe(Elastos::Math::EIID_IBigDecimal);
        if (*__IBigDecimal) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int64Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int64Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 int64Val,
        /* [in] */ Elastos::Math::IMathContext * mc,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(int64Val, mc, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Math {
class CBigDecimalHelper
{
public:
    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Math::IBigDecimalHelper** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigDecimalHelper, RGM_SAME_DOMAIN, Elastos::Math::EIID_IBigDecimalHelper, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigDecimalHelper, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigDecimalHelper, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigDecimalHelper, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Math {
class CBigInteger
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithNumBitsRandom(numBits, random, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithBitLengthCertaintyRandom(bitLength, certainty, random, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithSignValue(sign, value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithSignNumberLengthDigits(sign, numberLength, digits, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValue(value, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithValueRadix(value, radix, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithSignumMagnitude(signum, magnitude, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICBigIntegerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CBigInteger, RGM_SAME_DOMAIN,
                EIID_ICBigIntegerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithArrValue(arrValue, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::INumber** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigInteger, RGM_SAME_DOMAIN, Elastos::Core::EIID_INumber, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::IComparable** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigInteger, RGM_SAME_DOMAIN, Elastos::Core::EIID_IComparable, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Math::IBigInteger** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigInteger, RGM_SAME_DOMAIN, Elastos::Math::EIID_IBigInteger, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigInteger, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigInteger, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigInteger, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(numBits, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(numBits, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(numBits, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(numBits, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(numBits, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 numBits,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(numBits, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bitLength, certainty, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bitLength, certainty, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bitLength, certainty, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bitLength, certainty, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bitLength, certainty, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 bitLength,
        /* [in] */ _ELASTOS Int32 certainty,
        /* [in] */ Elastos::Utility::IRandom * random,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(bitLength, certainty, random, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int64 value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, numberLength, digits, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, numberLength, digits, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, numberLength, digits, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, numberLength, digits, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, numberLength, digits, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 sign,
        /* [in] */ _ELASTOS Int32 numberLength,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Int32> & digits,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sign, numberLength, digits, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, radix, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, radix, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, radix, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, radix, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, radix, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& value,
        /* [in] */ _ELASTOS Int32 radix,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(value, radix, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(signum, magnitude, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(signum, magnitude, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(signum, magnitude, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(signum, magnitude, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(signum, magnitude, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 signum,
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & magnitude,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(signum, magnitude, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ Elastos::Core::INumber** __INumber)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(arrValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__INumber = (Elastos::Core::INumber*)__pNewObj->Probe(Elastos::Core::EIID_INumber);
        if (*__INumber) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ Elastos::Core::IComparable** __IComparable)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(arrValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IComparable = (Elastos::Core::IComparable*)__pNewObj->Probe(Elastos::Core::EIID_IComparable);
        if (*__IComparable) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ Elastos::Math::IBigInteger** __IBigInteger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(arrValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IBigInteger = (Elastos::Math::IBigInteger*)__pNewObj->Probe(Elastos::Math::EIID_IBigInteger);
        if (*__IBigInteger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(arrValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(arrValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS ArrayOf<_ELASTOS Byte> & arrValue,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(arrValue, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Math {
class CBigIntegerHelper
{
public:
    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Math::IBigIntegerHelper** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigIntegerHelper, RGM_SAME_DOMAIN, Elastos::Math::EIID_IBigIntegerHelper, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigIntegerHelper, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigIntegerHelper, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CBigIntegerHelper, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Math {
class CMathContext
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICMathContextClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CMathContext, RGM_SAME_DOMAIN,
                EIID_ICMathContextClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithPrecision(precision, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICMathContextClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CMathContext, RGM_SAME_DOMAIN,
                EIID_ICMathContextClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithPrecisionRoundingMode(precision, roundingMode, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [out] */ Elastos::Math::IMathContext** __IMathContext)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IMathContext = (Elastos::Math::IMathContext*)__pNewObj->Probe(Elastos::Math::EIID_IMathContext);
        if (*__IMathContext) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ Elastos::Math::IMathContext** __IMathContext)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, roundingMode, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IMathContext = (Elastos::Math::IMathContext*)__pNewObj->Probe(Elastos::Math::EIID_IMathContext);
        if (*__IMathContext) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, roundingMode, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, roundingMode, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 precision,
        /* [in] */ Elastos::Math::RoundingMode roundingMode,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(precision, roundingMode, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
namespace Elastos {
namespace Math {
class CMathContextHelper
{
public:
    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Math::IMathContextHelper** __object)
    {
        return _CObject_CreateInstance(ECLSID_CMathContextHelper, RGM_SAME_DOMAIN, Elastos::Math::EIID_IMathContextHelper, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CMathContextHelper, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CMathContextHelper, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CMathContextHelper, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructAddrinfo
{
public:
    static _ELASTOS ECode New(
        /* [out] */ Elastos::Droid::System::IStructAddrinfo** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructAddrinfo, RGM_SAME_DOMAIN, Elastos::Droid::System::EIID_IStructAddrinfo, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructAddrinfo, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructAddrinfo, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructAddrinfo, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructGroupReq
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 gr_interface,
        /* [in] */ Elastos::Net::IInetAddress * gr_group,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructGroupReqClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructGroupReq, RGM_SAME_DOMAIN,
                EIID_ICStructGroupReqClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithGr_interfaceGr_group(gr_interface, gr_group, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 gr_interface,
        /* [in] */ Elastos::Net::IInetAddress * gr_group,
        /* [out] */ Elastos::Droid::System::IStructGroupReq** __IStructGroupReq)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(gr_interface, gr_group, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructGroupReq = (Elastos::Droid::System::IStructGroupReq*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructGroupReq);
        if (*__IStructGroupReq) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 gr_interface,
        /* [in] */ Elastos::Net::IInetAddress * gr_group,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(gr_interface, gr_group, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 gr_interface,
        /* [in] */ Elastos::Net::IInetAddress * gr_group,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(gr_interface, gr_group, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 gr_interface,
        /* [in] */ Elastos::Net::IInetAddress * gr_group,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(gr_interface, gr_group, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructLinger
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 l_onoff,
        /* [in] */ _ELASTOS Int32 l_linger,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructLingerClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructLinger, RGM_SAME_DOMAIN,
                EIID_ICStructLingerClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithL_onoffL_linger(l_onoff, l_linger, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 l_onoff,
        /* [in] */ _ELASTOS Int32 l_linger,
        /* [out] */ Elastos::Droid::System::IStructLinger** __IStructLinger)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(l_onoff, l_linger, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructLinger = (Elastos::Droid::System::IStructLinger*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructLinger);
        if (*__IStructLinger) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 l_onoff,
        /* [in] */ _ELASTOS Int32 l_linger,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(l_onoff, l_linger, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 l_onoff,
        /* [in] */ _ELASTOS Int32 l_linger,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(l_onoff, l_linger, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 l_onoff,
        /* [in] */ _ELASTOS Int32 l_linger,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(l_onoff, l_linger, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructPasswd
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& pw_name,
        /* [in] */ _ELASTOS Int32 pw_uid,
        /* [in] */ _ELASTOS Int32 pw_gid,
        /* [in] */ const _ELASTOS String& pw_dir,
        /* [in] */ const _ELASTOS String& pw_shell,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructPasswdClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructPasswd, RGM_SAME_DOMAIN,
                EIID_ICStructPasswdClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithPw_namePw_uidPw_gidPw_dirPw_shell(pw_name, pw_uid, pw_gid, pw_dir, pw_shell, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& pw_name,
        /* [in] */ _ELASTOS Int32 pw_uid,
        /* [in] */ _ELASTOS Int32 pw_gid,
        /* [in] */ const _ELASTOS String& pw_dir,
        /* [in] */ const _ELASTOS String& pw_shell,
        /* [out] */ Elastos::Droid::System::IStructPasswd** __IStructPasswd)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pw_name, pw_uid, pw_gid, pw_dir, pw_shell, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructPasswd = (Elastos::Droid::System::IStructPasswd*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructPasswd);
        if (*__IStructPasswd) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& pw_name,
        /* [in] */ _ELASTOS Int32 pw_uid,
        /* [in] */ _ELASTOS Int32 pw_gid,
        /* [in] */ const _ELASTOS String& pw_dir,
        /* [in] */ const _ELASTOS String& pw_shell,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pw_name, pw_uid, pw_gid, pw_dir, pw_shell, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& pw_name,
        /* [in] */ _ELASTOS Int32 pw_uid,
        /* [in] */ _ELASTOS Int32 pw_gid,
        /* [in] */ const _ELASTOS String& pw_dir,
        /* [in] */ const _ELASTOS String& pw_shell,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pw_name, pw_uid, pw_gid, pw_dir, pw_shell, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& pw_name,
        /* [in] */ _ELASTOS Int32 pw_uid,
        /* [in] */ _ELASTOS Int32 pw_gid,
        /* [in] */ const _ELASTOS String& pw_dir,
        /* [in] */ const _ELASTOS String& pw_shell,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pw_name, pw_uid, pw_gid, pw_dir, pw_shell, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructPollfd
{
public:
    static _ELASTOS ECode New(
        /* [out] */ Elastos::Droid::System::IStructPollfd** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructPollfd, RGM_SAME_DOMAIN, Elastos::Droid::System::EIID_IStructPollfd, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructPollfd, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructPollfd, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructPollfd, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructStat
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int64 dev,
        /* [in] */ _ELASTOS Int64 ino,
        /* [in] */ _ELASTOS Int32 mode,
        /* [in] */ _ELASTOS Int64 nlink,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [in] */ _ELASTOS Int64 rdev,
        /* [in] */ _ELASTOS Int64 size,
        /* [in] */ _ELASTOS Int64 atime,
        /* [in] */ _ELASTOS Int64 mtime,
        /* [in] */ _ELASTOS Int64 ctime,
        /* [in] */ _ELASTOS Int64 blksize,
        /* [in] */ _ELASTOS Int64 blocks,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructStatClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructStat, RGM_SAME_DOMAIN,
                EIID_ICStructStatClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithDevInoModeNlinkUidGidRdevSizeAtimeMtimeCtimeBlksizeBlocks(dev, ino, mode, nlink, uid, gid, rdev, size, atime, mtime, ctime, blksize, blocks, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 dev,
        /* [in] */ _ELASTOS Int64 ino,
        /* [in] */ _ELASTOS Int32 mode,
        /* [in] */ _ELASTOS Int64 nlink,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [in] */ _ELASTOS Int64 rdev,
        /* [in] */ _ELASTOS Int64 size,
        /* [in] */ _ELASTOS Int64 atime,
        /* [in] */ _ELASTOS Int64 mtime,
        /* [in] */ _ELASTOS Int64 ctime,
        /* [in] */ _ELASTOS Int64 blksize,
        /* [in] */ _ELASTOS Int64 blocks,
        /* [out] */ Elastos::Droid::System::IStructStat** __IStructStat)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dev, ino, mode, nlink, uid, gid, rdev, size, atime, mtime, ctime, blksize, blocks, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructStat = (Elastos::Droid::System::IStructStat*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructStat);
        if (*__IStructStat) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 dev,
        /* [in] */ _ELASTOS Int64 ino,
        /* [in] */ _ELASTOS Int32 mode,
        /* [in] */ _ELASTOS Int64 nlink,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [in] */ _ELASTOS Int64 rdev,
        /* [in] */ _ELASTOS Int64 size,
        /* [in] */ _ELASTOS Int64 atime,
        /* [in] */ _ELASTOS Int64 mtime,
        /* [in] */ _ELASTOS Int64 ctime,
        /* [in] */ _ELASTOS Int64 blksize,
        /* [in] */ _ELASTOS Int64 blocks,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dev, ino, mode, nlink, uid, gid, rdev, size, atime, mtime, ctime, blksize, blocks, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 dev,
        /* [in] */ _ELASTOS Int64 ino,
        /* [in] */ _ELASTOS Int32 mode,
        /* [in] */ _ELASTOS Int64 nlink,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [in] */ _ELASTOS Int64 rdev,
        /* [in] */ _ELASTOS Int64 size,
        /* [in] */ _ELASTOS Int64 atime,
        /* [in] */ _ELASTOS Int64 mtime,
        /* [in] */ _ELASTOS Int64 ctime,
        /* [in] */ _ELASTOS Int64 blksize,
        /* [in] */ _ELASTOS Int64 blocks,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dev, ino, mode, nlink, uid, gid, rdev, size, atime, mtime, ctime, blksize, blocks, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 dev,
        /* [in] */ _ELASTOS Int64 ino,
        /* [in] */ _ELASTOS Int32 mode,
        /* [in] */ _ELASTOS Int64 nlink,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [in] */ _ELASTOS Int64 rdev,
        /* [in] */ _ELASTOS Int64 size,
        /* [in] */ _ELASTOS Int64 atime,
        /* [in] */ _ELASTOS Int64 mtime,
        /* [in] */ _ELASTOS Int64 ctime,
        /* [in] */ _ELASTOS Int64 blksize,
        /* [in] */ _ELASTOS Int64 blocks,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(dev, ino, mode, nlink, uid, gid, rdev, size, atime, mtime, ctime, blksize, blocks, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructStatVfs
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int64 f_bsize,
        /* [in] */ _ELASTOS Int64 f_frsize,
        /* [in] */ _ELASTOS Int64 f_blocks,
        /* [in] */ _ELASTOS Int64 f_bfree,
        /* [in] */ _ELASTOS Int64 f_bavail,
        /* [in] */ _ELASTOS Int64 f_files,
        /* [in] */ _ELASTOS Int64 f_ffree,
        /* [in] */ _ELASTOS Int64 f_favail,
        /* [in] */ _ELASTOS Int64 f_fsid,
        /* [in] */ _ELASTOS Int64 f_flag,
        /* [in] */ _ELASTOS Int64 f_namemax,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructStatVfsClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructStatVfs, RGM_SAME_DOMAIN,
                EIID_ICStructStatVfsClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithF_bsizeF_frsizeF_blocksF_bfreeF_bavailF_filesF_ffreeF_favailF_fsidF_flagF_namemax(f_bsize, f_frsize, f_blocks, f_bfree, f_bavail, f_files, f_ffree, f_favail, f_fsid, f_flag, f_namemax, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 f_bsize,
        /* [in] */ _ELASTOS Int64 f_frsize,
        /* [in] */ _ELASTOS Int64 f_blocks,
        /* [in] */ _ELASTOS Int64 f_bfree,
        /* [in] */ _ELASTOS Int64 f_bavail,
        /* [in] */ _ELASTOS Int64 f_files,
        /* [in] */ _ELASTOS Int64 f_ffree,
        /* [in] */ _ELASTOS Int64 f_favail,
        /* [in] */ _ELASTOS Int64 f_fsid,
        /* [in] */ _ELASTOS Int64 f_flag,
        /* [in] */ _ELASTOS Int64 f_namemax,
        /* [out] */ Elastos::Droid::System::IStructStatVfs** __IStructStatVfs)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(f_bsize, f_frsize, f_blocks, f_bfree, f_bavail, f_files, f_ffree, f_favail, f_fsid, f_flag, f_namemax, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructStatVfs = (Elastos::Droid::System::IStructStatVfs*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructStatVfs);
        if (*__IStructStatVfs) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 f_bsize,
        /* [in] */ _ELASTOS Int64 f_frsize,
        /* [in] */ _ELASTOS Int64 f_blocks,
        /* [in] */ _ELASTOS Int64 f_bfree,
        /* [in] */ _ELASTOS Int64 f_bavail,
        /* [in] */ _ELASTOS Int64 f_files,
        /* [in] */ _ELASTOS Int64 f_ffree,
        /* [in] */ _ELASTOS Int64 f_favail,
        /* [in] */ _ELASTOS Int64 f_fsid,
        /* [in] */ _ELASTOS Int64 f_flag,
        /* [in] */ _ELASTOS Int64 f_namemax,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(f_bsize, f_frsize, f_blocks, f_bfree, f_bavail, f_files, f_ffree, f_favail, f_fsid, f_flag, f_namemax, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 f_bsize,
        /* [in] */ _ELASTOS Int64 f_frsize,
        /* [in] */ _ELASTOS Int64 f_blocks,
        /* [in] */ _ELASTOS Int64 f_bfree,
        /* [in] */ _ELASTOS Int64 f_bavail,
        /* [in] */ _ELASTOS Int64 f_files,
        /* [in] */ _ELASTOS Int64 f_ffree,
        /* [in] */ _ELASTOS Int64 f_favail,
        /* [in] */ _ELASTOS Int64 f_fsid,
        /* [in] */ _ELASTOS Int64 f_flag,
        /* [in] */ _ELASTOS Int64 f_namemax,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(f_bsize, f_frsize, f_blocks, f_bfree, f_bavail, f_files, f_ffree, f_favail, f_fsid, f_flag, f_namemax, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 f_bsize,
        /* [in] */ _ELASTOS Int64 f_frsize,
        /* [in] */ _ELASTOS Int64 f_blocks,
        /* [in] */ _ELASTOS Int64 f_bfree,
        /* [in] */ _ELASTOS Int64 f_bavail,
        /* [in] */ _ELASTOS Int64 f_files,
        /* [in] */ _ELASTOS Int64 f_ffree,
        /* [in] */ _ELASTOS Int64 f_favail,
        /* [in] */ _ELASTOS Int64 f_fsid,
        /* [in] */ _ELASTOS Int64 f_flag,
        /* [in] */ _ELASTOS Int64 f_namemax,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(f_bsize, f_frsize, f_blocks, f_bfree, f_bavail, f_files, f_ffree, f_favail, f_fsid, f_flag, f_namemax, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructFlock
{
public:
    static _ELASTOS ECode New(
        /* [out] */ Elastos::Droid::System::IStructFlock** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructFlock, RGM_SAME_DOMAIN, Elastos::Droid::System::EIID_IStructFlock, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructFlock, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructFlock, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructFlock, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructTimeval
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int64 sec,
        /* [in] */ _ELASTOS Int64 usec,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructTimevalClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructTimeval, RGM_SAME_DOMAIN,
                EIID_ICStructTimevalClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithSecUsec(sec, usec, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 sec,
        /* [in] */ _ELASTOS Int64 usec,
        /* [out] */ Elastos::Droid::System::IStructTimeval** __IStructTimeval)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sec, usec, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructTimeval = (Elastos::Droid::System::IStructTimeval*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructTimeval);
        if (*__IStructTimeval) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 sec,
        /* [in] */ _ELASTOS Int64 usec,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sec, usec, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 sec,
        /* [in] */ _ELASTOS Int64 usec,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sec, usec, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int64 sec,
        /* [in] */ _ELASTOS Int64 usec,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sec, usec, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructTimevalHelper
{
public:
    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Droid::System::IStructTimevalHelper** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructTimevalHelper, RGM_SAME_DOMAIN, Elastos::Droid::System::EIID_IStructTimevalHelper, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructTimevalHelper, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructTimevalHelper, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode AcquireSingleton(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CStructTimevalHelper, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructUcred
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 pid,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructUcredClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructUcred, RGM_SAME_DOMAIN,
                EIID_ICStructUcredClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithPidUidGid(pid, uid, gid, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 pid,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [out] */ Elastos::Droid::System::IStructUcred** __IStructUcred)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pid, uid, gid, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructUcred = (Elastos::Droid::System::IStructUcred*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructUcred);
        if (*__IStructUcred) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 pid,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pid, uid, gid, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 pid,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pid, uid, gid, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 pid,
        /* [in] */ _ELASTOS Int32 uid,
        /* [in] */ _ELASTOS Int32 gid,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(pid, uid, gid, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructUtsname
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ const _ELASTOS String& sysname,
        /* [in] */ const _ELASTOS String& nodename,
        /* [in] */ const _ELASTOS String& release,
        /* [in] */ const _ELASTOS String& ver,
        /* [in] */ const _ELASTOS String& machine,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructUtsnameClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructUtsname, RGM_SAME_DOMAIN,
                EIID_ICStructUtsnameClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithSysnameNodenameReleaseVerMachine(sysname, nodename, release, ver, machine, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sysname,
        /* [in] */ const _ELASTOS String& nodename,
        /* [in] */ const _ELASTOS String& release,
        /* [in] */ const _ELASTOS String& ver,
        /* [in] */ const _ELASTOS String& machine,
        /* [out] */ Elastos::Droid::System::IStructUtsname** __IStructUtsname)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sysname, nodename, release, ver, machine, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructUtsname = (Elastos::Droid::System::IStructUtsname*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructUtsname);
        if (*__IStructUtsname) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sysname,
        /* [in] */ const _ELASTOS String& nodename,
        /* [in] */ const _ELASTOS String& release,
        /* [in] */ const _ELASTOS String& ver,
        /* [in] */ const _ELASTOS String& machine,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sysname, nodename, release, ver, machine, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sysname,
        /* [in] */ const _ELASTOS String& nodename,
        /* [in] */ const _ELASTOS String& release,
        /* [in] */ const _ELASTOS String& ver,
        /* [in] */ const _ELASTOS String& machine,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sysname, nodename, release, ver, machine, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ const _ELASTOS String& sysname,
        /* [in] */ const _ELASTOS String& nodename,
        /* [in] */ const _ELASTOS String& release,
        /* [in] */ const _ELASTOS String& ver,
        /* [in] */ const _ELASTOS String& machine,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(sysname, nodename, release, ver, machine, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CStructGroupSourceReq
{
public:
    static _ELASTOS ECode _New(
        /* [in] */ _ELASTOS Int32 itf,
        /* [in] */ Elastos::Net::IInetAddress * group,
        /* [in] */ Elastos::Net::IInetAddress * source,
        /* [out] */ IInterface** __object)
    {
        _ELASTOS ECode ec;
        ICStructGroupSourceReqClassObject* pClassObject;
        PInterface pObject = NULL;

        ec = _CObject_AcquireClassFactory(ECLSID_CStructGroupSourceReq, RGM_SAME_DOMAIN,
                EIID_ICStructGroupSourceReqClassObject, (IInterface**)&pClassObject);
        if (FAILED(ec)) return ec;

        ec = pClassObject->CreateObjectWithItfGroupSource(itf, group, source, &pObject);

        *__object = pObject;

        pClassObject->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 itf,
        /* [in] */ Elastos::Net::IInetAddress * group,
        /* [in] */ Elastos::Net::IInetAddress * source,
        /* [out] */ Elastos::Droid::System::IStructGroupSourceReq** __IStructGroupSourceReq)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(itf, group, source, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IStructGroupSourceReq = (Elastos::Droid::System::IStructGroupSourceReq*)__pNewObj->Probe(Elastos::Droid::System::EIID_IStructGroupSourceReq);
        if (*__IStructGroupSourceReq) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 itf,
        /* [in] */ Elastos::Net::IInetAddress * group,
        /* [in] */ Elastos::Net::IInetAddress * source,
        /* [out] */ IObject** __IObject)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(itf, group, source, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IObject = (IObject*)__pNewObj->Probe(EIID_IObject);
        if (*__IObject) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 itf,
        /* [in] */ Elastos::Net::IInetAddress * group,
        /* [in] */ Elastos::Net::IInetAddress * source,
        /* [out] */ Elastos::Core::ISynchronize** __ISynchronize)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(itf, group, source, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__ISynchronize = (Elastos::Core::ISynchronize*)__pNewObj->Probe(Elastos::Core::EIID_ISynchronize);
        if (*__ISynchronize) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

    static _ELASTOS ECode New(
        /* [in] */ _ELASTOS Int32 itf,
        /* [in] */ Elastos::Net::IInetAddress * group,
        /* [in] */ Elastos::Net::IInetAddress * source,
        /* [out] */ IWeakReferenceSource** __IWeakReferenceSource)
    {
        IInterface* __pNewObj = NULL;

        _ELASTOS ECode ec = _New(itf, group, source, &__pNewObj);
        if (FAILED(ec)) return ec;

        *__IWeakReferenceSource = (IWeakReferenceSource*)__pNewObj->Probe(EIID_IWeakReferenceSource);
        if (*__IWeakReferenceSource) __pNewObj->AddRef();
        else ec = E_NO_INTERFACE;
        __pNewObj->Release();

        return ec;
    }

};
}
}
}
namespace Elastos {
namespace Droid {
namespace System {
class CZygoteHooks
{
public:
    static _ELASTOS ECode New(
        /* [out] */ Elastos::Droid::System::IZygoteHooks** __object)
    {
        return _CObject_CreateInstance(ECLSID_CZygoteHooks, RGM_SAME_DOMAIN, Elastos::Droid::System::EIID_IZygoteHooks, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IObject** __object)
    {
        return _CObject_CreateInstance(ECLSID_CZygoteHooks, RGM_SAME_DOMAIN, EIID_IObject, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ Elastos::Core::ISynchronize** __object)
    {
        return _CObject_CreateInstance(ECLSID_CZygoteHooks, RGM_SAME_DOMAIN, Elastos::Core::EIID_ISynchronize, (PInterface*)__object);
    }

    static _ELASTOS ECode New(
        /* [out] */ IWeakReferenceSource** __object)
    {
        return _CObject_CreateInstance(ECLSID_CZygoteHooks, RGM_SAME_DOMAIN, EIID_IWeakReferenceSource, (PInterface*)__object);
    }

};
}
}
}

#endif // __Elastos_CoreLibrary_Core_h__
