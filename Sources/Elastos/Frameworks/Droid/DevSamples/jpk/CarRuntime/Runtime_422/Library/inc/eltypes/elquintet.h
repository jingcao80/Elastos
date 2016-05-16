//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELQUINTET_H__
#define __ELQUINTET_H__

#include <elatypes.h>
#include <elaobj.h>
#include <eladef.h>
#include <elsharedbuf.h>
#include <elquintettype.h>
#include <elrefbase.h>
#include <elautoptr.h>
#include <elstring.h>

#if defined(_DEBUG) || defined(_ELASTOS_DEBUG)
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    _ELASTOS PCarQuintet __cdecl _CarQuintet_Init(_ELASTOS PCarQuintet pCq,
            _ELASTOS PVoid pBuf, _ELASTOS Int32 size, _ELASTOS Int32 used,
            _ELASTOS CarQuintetFlags flags);
    _ELASTOS PCarQuintet __cdecl _CarQuintet_Alloc(_ELASTOS Int32 size);
    void __cdecl _CarQuintet_Free(_ELASTOS PCarQuintet pCq);
    _ELASTOS PCarQuintet __cdecl _CarQuintet_Clone(const _ELASTOS PCarQuintet pCq);
    void __cdecl _CarQuintet_AddRef(const _ELASTOS PCarQuintet pCq);
    _ELASTOS Int32 __cdecl _CarQuintet_Release(_ELASTOS PCarQuintet pCq);

    _ELASTOS Int32 __cdecl _ArrayOf_Copy(_ELASTOS PCarQuintet pcqDst,
            const _ELASTOS CarQuintet* pcqSrc);
    _ELASTOS Int32 __cdecl _ArrayOf_CopyEx(_ELASTOS PCarQuintet pCq,
            const _ELASTOS Byte* p, _ELASTOS Int32 n);
    _ELASTOS PCarQuintet __cdecl _ArrayOf_Alloc(_ELASTOS Int32 size,
            _ELASTOS CarQuintetFlags flags);
    _ELASTOS PCarQuintet __cdecl _ArrayOf_Alloc_Box(_ELASTOS PVoid pBuf,
            _ELASTOS Int32 size, _ELASTOS CarQuintetFlags flags);
    _ELASTOS Int32 __cdecl _ArrayOf_Replace(_ELASTOS PCarQuintet pCq,
            _ELASTOS Int32 offset, const _ELASTOS PByte p, _ELASTOS Int32 n);

    _ELASTOS Int32 __cdecl _BufferOf_Insert(_ELASTOS PCarQuintet pCq,
            _ELASTOS Int32 offset, const _ELASTOS PByte pb, _ELASTOS MemorySize n);
    _ELASTOS Int32 __cdecl _BufferOf_Append(_ELASTOS PCarQuintet pCq,
            const _ELASTOS PByte p, _ELASTOS MemorySize n);
    _ELASTOS Int32 __cdecl _BufferOf_Replace(_ELASTOS PCarQuintet pCq,
            _ELASTOS Int32 offset, const _ELASTOS PByte p, _ELASTOS Int32 n);
    _ELASTOS Int32 __cdecl _BufferOf_Copy(_ELASTOS PCarQuintet pcqDst,
            const _ELASTOS CarQuintet* pcqSrc);
    _ELASTOS Int32 __cdecl _BufferOf_CopyEx(_ELASTOS PCarQuintet pCq,
            const _ELASTOS Byte* p, _ELASTOS Int32 n);
    _ELASTOS PCarQuintet __cdecl _BufferOf_Alloc(_ELASTOS Int32 size,
            _ELASTOS CarQuintetFlags flags);
    _ELASTOS PCarQuintet __cdecl _BufferOf_Alloc_Box(_ELASTOS PVoid pBuf,
            _ELASTOS Int32 capacity, _ELASTOS Int32 used,
            _ELASTOS CarQuintetFlags flags);

    void _MemoryBuf_SetByteValue(_ELASTOS PCarQuintet pCq, _ELASTOS Byte value);
    _ELASTOS Int32 _MemoryBuf_Compare(const _ELASTOS PCarQuintet pCq,
            const _ELASTOS Byte* pBuf, _ELASTOS Int32 count);

    _ELASTOS PCarQuintet __cdecl _CarQuintet_GetNullValue(
            _ELASTOS CarQuintetFlags flag);
#ifdef __cplusplus
}
#endif

//
//  Memory structure of ArrayOf:
//
//           ________
//          |  pBuf  |
//          |        v
//  +------------+   +------------------------+
//  | CarQuintet |   |  C Array Data          |
//  +------------+   +------------------------+
//  ^
//  |____ ArrayOf (m_pCq)
//
//

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

class CString;

// NOTE1: MS CL compiler can't support function's template specialization well,
//   so only template class works.
// NOTE2: We shall emit a COMPILE-TIME error if user let ArrayOf
//   to contain a
//   non-automation type. There's an undefined variable in Type2Flag::Flag()'s
//   default implementation, which will emit the error. Also, we must deal
//   with illegal types at runtime.
//
template <class T> struct Type2Flag
{
    static Int32 Flag() {
        if (SUPERSUBCLASS_EX(IInterface*, T)) {
            return CarQuintetFlag_Type_IObject;
        }
        else if (SUPERSUBCLASS_EX(ElRefBase*, T)) {
            return CarQuintetFlag_Type_RefObject;
        }
        else if (SUPERSUBCLASS_EX(ElLightRefBase*, T)) {
            return CarQuintetFlag_Type_LightRefObject;
        }
        else {
            return CarQuintetFlag_Type_Struct;
        }
    }
};

#define DECL_TYPE2FLAG_TMPL(type, flag) \
    template <> struct Type2Flag<type> { static Int32 Flag() \
    { return (flag); } }

DECL_TYPE2FLAG_TMPL(Int8,               CarQuintetFlag_Type_Int8);
DECL_TYPE2FLAG_TMPL(Int16,              CarQuintetFlag_Type_Int16);
DECL_TYPE2FLAG_TMPL(Int32,              CarQuintetFlag_Type_Int32);
DECL_TYPE2FLAG_TMPL(Int64,              CarQuintetFlag_Type_Int64);

DECL_TYPE2FLAG_TMPL(Byte,               CarQuintetFlag_Type_Byte);
DECL_TYPE2FLAG_TMPL(UInt16,             CarQuintetFlag_Type_UInt16);
DECL_TYPE2FLAG_TMPL(UInt32,             CarQuintetFlag_Type_UInt32);
DECL_TYPE2FLAG_TMPL(UInt64,             CarQuintetFlag_Type_UInt64);

DECL_TYPE2FLAG_TMPL(Float,              CarQuintetFlag_Type_Float);
DECL_TYPE2FLAG_TMPL(Double,             CarQuintetFlag_Type_Double);

DECL_TYPE2FLAG_TMPL(Char8,              CarQuintetFlag_Type_Char8);
//DECL_TYPE2FLAG_TMPL(Char16,             CarQuintetFlag_Type_Char16);
DECL_TYPE2FLAG_TMPL(CString,            CarQuintetFlag_Type_CString);
DECL_TYPE2FLAG_TMPL(String,             CarQuintetFlag_Type_String);

DECL_TYPE2FLAG_TMPL(EMuid,              CarQuintetFlag_Type_EMuid);
DECL_TYPE2FLAG_TMPL(EGuid,              CarQuintetFlag_Type_EGuid);
DECL_TYPE2FLAG_TMPL(IInterface *,       CarQuintetFlag_Type_IObject);
DECL_TYPE2FLAG_TMPL(ElRefBase *,        CarQuintetFlag_Type_RefObject);
DECL_TYPE2FLAG_TMPL(ElLightRefBase *,   CarQuintetFlag_Type_LightRefObject);

#define _MAX_CARQUINTET_SIZE_  (0x7FFFFFFF - sizeof(CarQuintet) - sizeof(SharedBuffer))
#define IS_QUINTENT_FLAG(pcq, flag)     ((pcq) && ((pcq)->m_flags & (flag)))

template <class T>
class ArrayOf;

//template <class T>
//class BufferOf;

//---------------QuintetObjectReleaseOp----------------------------------------

template <class T>
struct QuintetObjectReleaseOp
{
    void operator()(void const * buf);
};

template <>
struct QuintetObjectReleaseOp<String>
{
    void operator()(void const * buf);
};

template <class T>
struct QuintetObjectReleaseOp<AutoPtr<T> >
{
    void operator()(void const * buf);
};

//---------------QuintetObjectCopyOp-------------------------------------------

struct QuintetObjectCopyOp
{
    template <class T>
    Int32 operator()(
        ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count);

    template <class T>
    Int32 operator()(
        ArrayOf<AutoPtr<T> >* dst, Int32 offset,
        AutoPtr<T> const* src, Int32 srcOffset, Int32 count);

    Int32 operator()(
        ArrayOf<String>* dst, Int32 offset,
        String const* src, Int32 srcOffset, Int32 count);
};

//---------------ArrayOf-------------------------------------------------------

/** @addtogroup CARTypesRef
  *   @{
  */
template <class T>
class ArrayOf : public CarQuintet
{
public:
    T* GetPayload() const {
        return (T*)m_pBuf;
    };

    operator PCarQuintet() {
        return this;
    }

    Int32 GetLength() const {
        return m_pBuf ? m_size / sizeof(T) : 0;
    }

    Int32 Copy(T const* pBuf, Int32 n) {
        QuintetObjectCopyOp copyOp;
        return copyOp(this, 0, pBuf, 0, n);
    }

    Int32 Copy(Int32 offset, T const* pBuf, Int32 n) {
        QuintetObjectCopyOp copyOp;
        return copyOp(this, offset, pBuf, 0, n);
    }

    Int32 Copy(const ArrayOf<T> *pSrc) {
        if (this == pSrc) {
            return GetLength();
        }

        QuintetObjectCopyOp copyOp;
        return copyOp(this, 0, (T const*)pSrc->GetPayload(), 0, pSrc->GetLength());
    }

    Int32 Copy(const ArrayOf<T> *pSrc, Int32 count) {
        QuintetObjectCopyOp copyOp;
        return copyOp(this, 0, (T const*)pSrc->GetPayload(), 0, count);
    }

    Int32 Copy(const ArrayOf<T> *pSrc, Int32 srcOffset, Int32 count) {
        QuintetObjectCopyOp copyOp;
        return copyOp(this, 0, (T const*)pSrc->GetPayload(), srcOffset, count);
    }

    Int32 Copy(Int32 offset, const ArrayOf<T> *pSrc) {
        QuintetObjectCopyOp copyOp;
        return copyOp(this, offset, (T const*)pSrc->GetPayload(), 0, pSrc->GetLength());
    }

    Int32 Copy(Int32 offset, const ArrayOf<T> *pSrc, Int32 count) {
        QuintetObjectCopyOp copyOp;
        return copyOp(this, offset, (T const*)pSrc->GetPayload(), 0, count);
    }

    Int32 Copy(Int32 offset, const ArrayOf<T> *pSrc, Int32 srcOffset, Int32 count) {
        QuintetObjectCopyOp copyOp;
        return copyOp(this, offset, (T const*)pSrc->GetPayload(), srcOffset, count);
    }

    ArrayOf<T> *Clone() const {
        PCarQuintet pNewCq = _CarQuintet_Alloc(m_size);
        if (pNewCq) {
            CarQuintetFlags flags = m_flags & CarQuintetFlag_TypeMask;
            flags |= CarQuintetFlag_HeapAlloced;
            _CarQuintet_Init(pNewCq, pNewCq + 1, m_size, m_used, flags);
            if (this->m_pBuf) {
                if (pNewCq) memset(pNewCq->m_pBuf, 0x0, m_size);
                QuintetObjectCopyOp copyOp;
                copyOp((ArrayOf<T> *)pNewCq, 0, (T const*)GetPayload(), 0, this->GetLength());
            }
            else {
                pNewCq->m_pBuf = NULL;
            }
        }

        return (ArrayOf<T> *)pNewCq;
    }

    void AddRef() const {
        _CarQuintet_AddRef((const PCarQuintet)this);
    }

    Int32 GetRefCount() {
        PCarQuintet pCq = (const PCarQuintet)this;
        if (IS_QUINTENT_FLAG(pCq, CarQuintetFlag_HeapAlloced)) {
            SharedBuffer * buf = SharedBuffer::GetBufferFromData(pCq);
            return buf->RefCount();
        }
        return 0;
    }

    Int32 Release() const {
        PCarQuintet pCq = (const PCarQuintet)this;
        if (IS_QUINTENT_FLAG(pCq, CarQuintetFlag_HeapAlloced)) {
            QuintetObjectReleaseOp<T> releaseOp;
            SharedBuffer * buf = SharedBuffer::GetBufferFromData(pCq);
            if (IS_QUINTENT_FLAG(pCq, CarQuintetFlag_AutoRefCounted)) {
                return buf->Release(releaseOp, 0);
            }
            else {
                ELA_ASSERT_WITH_BLOCK(buf->RefCount() == 0) {
                    printf(" >> %s %d\n >> Ref count of share buffer %d isn't zero,"
                            " and CarQuintetFlag_AutoRefCounted isn't set too. m_flags:%08x, m_size:%d\n",
                            __FILE__, __LINE__, buf->RefCount(), m_flags, m_size);
                }

                SharedBuffer::Dealloc(buf, releaseOp);
            }
        }
        return 0;
    }

    void Set(Int32 index, T const other) {
        Copy(index, &other, 1);
    }

    T& operator[](Int32 index) {
        assert(m_pBuf && index >= 0 && index < GetLength());
        return ((T*)(m_pBuf))[index];
    }

    const T& operator[](Int32 index) const {   // nothing
        assert(m_pBuf && index >= 0 && index < GetLength());
        return ((T*)(m_pBuf))[index];
    }

//    operator const BufferOf<T>&() const {
//        return *(const BufferOf<T>*)this;
//    }

    static ArrayOf<T> *Alloc(Int32 capacity) {
        return (ArrayOf<T> *)_ArrayOf_Alloc(capacity * sizeof(T),
        Type2Flag<T>::Flag());
    }

    static ArrayOf<T> *Alloc(T *pBuf, Int32 capacity) {
        return (ArrayOf<T> *)_ArrayOf_Alloc_Box(
            pBuf, capacity * sizeof(T), Type2Flag<T>::Flag());
    }

    static void Free(ArrayOf<T> *pArray) {
        if (NULL != pArray) {
            pArray->Release();
        }
    }

    Boolean Contains(T const& value) {
        return IndexOf(value) != -1;
    }

    Int32 IndexOf(T const& value) {
        for (Int32 i = 0; i < GetLength(); ++i) {
            if (((T*)(m_pBuf))[i] == value) {
                return i;
            }
        }
        return -1;
    }

    // Notes: Make sure the array is ordered before searching.
    Int32 BinarySearch(T const& value) {
        return BinarySearch(this, value);
    }

    // Notes: Make sure the array is ordered before searching.
    static Int32 BinarySearch(const ArrayOf<T> *pArray, T const& value)
    {
        assert(pArray && pArray->m_pBuf);

        Int32 low = 0;
        Int32 high = pArray->GetLength() - 1;
        Int32 mid;

        while (low <= high) {
            mid = (low + high) >> 1;

            if (value == (*pArray)[mid]) {
                return mid;
            }

            if (value < (*pArray)[mid]) {
                high = mid - 1;
            }
            else {
                low = mid + 1;
            }
        }

        return ~low;
    }

    Boolean Equals(const ArrayOf<T> *other)
    {
        return Equals(this, other);
    }

    static Boolean Equals(const ArrayOf<T> *a, const ArrayOf<T> *b)
    {
        if (a == NULL && b == NULL) {
            return TRUE;
        }

        Int32 length = a->GetLength();
        if (length != b->GetLength()) {
            return FALSE;
        }

        for (Int32 i = 0; i < length; ++i) {
            if ((*a)[i] != (*b)[i]) {
                return FALSE;
            }
        }

        return TRUE;
    }

    ArrayOf(T *pBuf, Int32 capacity) {
        _CarQuintet_Init(this, pBuf, capacity * sizeof(T),
                        capacity * sizeof(T), Type2Flag<T>::Flag());
    }

private:
    // prohibit 'new' operator
    void * operator new(size_t cb);
    ArrayOf() {}
    ArrayOf& operator = (const ArrayOf& buf) {}
    ArrayOf(const ArrayOf& buf) {}
};
/** @} */

/** @addtogroup BaseTypesRef
  *   @{
  */
template <class T, Int32 SIZE>
class ArrayOf_ : public ArrayOf<T>
{
public:
    ArrayOf_() : ArrayOf<T>(m_buf, SIZE)
    {
        if (Type2Flag<T>::Flag() != CarQuintetFlag_Type_Struct) {
            _MemoryBuf_SetByteValue(this, 0x0);
        }
    }

protected:
    T   m_buf[SIZE ? SIZE : 1];

private:
    // prohibit "=" operator
    ArrayOf_& operator = (const ArrayOf<T>& buf) {}
    ArrayOf_& operator = (const ArrayOf_& buf) {}
    ArrayOf_(const ArrayOf<T>& buf) {}
    ArrayOf_(const ArrayOf_& buf) {}
};
/** @} */

typedef AutoPtr<ArrayOf<Byte> >     ByteArray;
typedef AutoPtr<ArrayOf<Char32> >   Char32Array;
typedef AutoPtr<ArrayOf<String> >   StringArray;
typedef AutoPtr<ArrayOf<Int32> >    Int32Array;
typedef AutoPtr<ArrayOf<Int64> >    Int64Array;
typedef AutoPtr<ArrayOf<Float> >    FloatArray;
typedef AutoPtr<ArrayOf<Double> >   DoubleArray;

template <typename T>
class ArrayOf2 {
public:
    typedef AutoPtr<ArrayOf<T> >    ElementType;
    typedef ArrayOf<ElementType>    Type;
};

//---------------QuintetObjectReleaseOp----------------------------------------
template<class OrigT, class TargetT>
void ReleaseFunc(void const * buf)
{
    ArrayOf<OrigT>* pcq = (ArrayOf<OrigT>*)buf;
    Int32 length = pcq->GetLength();
    OrigT* p = (OrigT*)(pcq->m_pBuf);

    for(Int32 i = 0; i < length; ++i) {
        TargetT* prb = (TargetT*)(*p);
        if (prb) {
            prb->Release();
            *p = NULL;
        }
        ++p;
    }
}

template<class T, Boolean isElRefBaseObj, Boolean isElLightRefBaseObj, Boolean isInterfaceObj>
struct ReleaseOpImpl
{
    void operator()(void const * buf)
    {
    }
};

template<class T>
struct ReleaseOpImpl<T, TRUE, TRUE, TRUE>
{
    void operator()(void const * buf)
    {
        ReleaseFunc<T, ElRefBase>(buf);
    }
};

template<class T>
struct ReleaseOpImpl<T, TRUE, TRUE, FALSE>
{
    void operator()(void const * buf)
    {
        ReleaseFunc<T, ElRefBase>(buf);
    }
};

template<class T>
struct ReleaseOpImpl<T, TRUE, FALSE, TRUE>
{
    void operator()(void const * buf)
    {
        ReleaseFunc<T, ElRefBase>(buf);
    }
};

template<class T>
struct ReleaseOpImpl<T, TRUE, FALSE, FALSE>
{
    void operator()(void const * buf)
    {
        ReleaseFunc<T, ElRefBase>(buf);
    }
};

template<class T>
struct ReleaseOpImpl<T, FALSE, TRUE, TRUE>
{
    void operator()(void const * buf)
    {
        ReleaseFunc<T, ElLightRefBase>(buf);
    }
};

template<class T>
struct ReleaseOpImpl<T, FALSE, TRUE, FALSE>
{
    void operator()(void const * buf)
    {
        ReleaseFunc<T, ElLightRefBase>(buf);
    }
};

template<class T>
struct ReleaseOpImpl<T, FALSE, FALSE, TRUE>
{
    void operator()(void const * buf)
    {
        ReleaseFunc<T, IInterface>(buf);
    }
};

template <class T>
inline void QuintetObjectReleaseOp<T>::operator()(void const* buf)
{
    if (NULL != buf) {
        ReleaseOpImpl<T,
            SUPERSUBCLASS_EX(ElRefBase*, T),
            SUPERSUBCLASS_EX(ElLightRefBase*, T),
            SUPERSUBCLASS_EX(IInterface*, T)> impl;
        impl(buf);
    }
}

template <class T>
inline void QuintetObjectReleaseOp<AutoPtr<T> >::operator()(void const* buf)
{
    if (NULL != buf) {
        ArrayOf<AutoPtr<T> >* pcq = (ArrayOf<AutoPtr<T> >*)buf;
        Int32 length = pcq->GetLength();

        for(Int32 i = 0; i < length; ++i) {
            if (NULL != (*pcq)[i]) {
                (*pcq)[i] = NULL;
            }
        }
    }
}

inline void QuintetObjectReleaseOp<String>::operator()(void const* buf)
{
    if (NULL != buf) {
        ArrayOf<String>* pcq = (ArrayOf<String>*)buf;
        Int32 length = pcq->GetLength();

        for(Int32 i = 0; i < length; ++i) {
            if (NULL != (*pcq)[i]) {
                (*pcq)[i] = NULL;
            }
        }
    }
}

//---------------QuintetObjectCopyOp--------------------------------------------
// Notes: ArrayOf::Copy equals System.Copy in java, not memcpy in c/c++.
//
template<class OrigT, class TargetT>
Int32 CopyFunc(ArrayOf<OrigT>* dstArray, Int32 dstOffset, OrigT const* src, Int32 srcOffset, Int32 count)
{
    OrigT* dst = (OrigT*)(dstArray->m_pBuf);
    Int32 copyCount = MIN(count, dstArray->GetLength() - dstOffset);

    // self-copy to the same position.
    Boolean isSelfCopy = (dst == src);
    if ((isSelfCopy && dstOffset == srcOffset) || copyCount == 0) {
        return copyCount;
    }

    TargetT* prb = NULL;
    dst += dstOffset;
    src += srcOffset;

    Boolean isOverlap = (isSelfCopy && (dstOffset > srcOffset) && (dstOffset < srcOffset + copyCount));
    if (isOverlap) {
        for (Int32 i = copyCount - 1; i >= 0; --i) {
            prb = (TargetT*)(*(src + i));
            if (prb) {
                prb->AddRef();
            }

            prb = (TargetT*)(*(dst + i));
            if (prb) {
                prb->Release();
            }

            *(dst + i) = *(src + i);
        }
    }
    else {
        for (Int32 i = 0; i < copyCount; ++i) {
            prb = (TargetT*)(*src);
            if (prb) {
                prb->AddRef();
            }

            prb = (TargetT*)(*dst);
            if (prb) {
                prb->Release();
            }

            *dst++ = *src++;
        }
    }

    return copyCount;
}

template<class T>
static Int32 PlainCopy(T* dst, Int32 dstOffset, T const * src, Int32 srcOffset, Int32 copyCount)
{
    assert(dst != NULL && dstOffset >= 0 && src != NULL && srcOffset >= 0 && copyCount >= 0);

    // self-copy to the same position .
    Boolean isSelfCopy = (dst == src);
    if ((isSelfCopy && dstOffset == srcOffset) || copyCount == 0) {
        return copyCount;
    }

    dst += dstOffset;
    src += srcOffset;

    if (copyCount == 1) {
        *dst = *src;
        return copyCount;
    }

    Boolean isOverlap = (isSelfCopy && (dstOffset > srcOffset) && (dstOffset < srcOffset + copyCount));
    if (isOverlap) {
        for (Int32 i = copyCount - 1; i >= 0; --i) {
            *(dst + i) = *(src + i);
        }
    }
    else {
        for (Int32 i = 0; i < copyCount; ++i) {
            *dst++ = *src++;
        }
    }

    return copyCount;
}

template<class T, Boolean isElRefBaseObj, Boolean isElLightRefBaseObj, Boolean isInterfaceObj>
struct CopyOpImpl
{
    Int32 operator()(ArrayOf<T>* dst, Int32 dstOffset, T const* src, Int32 srcOffset, Int32 count)
    {
        Int32 copyCount = MIN(count, dst->GetLength() - dstOffset);
        return PlainCopy((T*)(dst->m_pBuf), dstOffset, src, srcOffset, copyCount);
    }
};

template<class T>
struct CopyOpImpl<T, TRUE, TRUE, TRUE>
{
    Int32 operator()(ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
    {
        return CopyFunc<T, ElRefBase>(dst, offset, src, srcOffset, count);
    }
};

template<class T>
struct CopyOpImpl<T, TRUE, TRUE, FALSE>
{
    Int32 operator()(ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
    {
        return CopyFunc<T, ElRefBase>(dst, offset, src, srcOffset, count);
    }
};

template<class T>
struct CopyOpImpl<T, TRUE, FALSE, TRUE>
{
    Int32 operator()(ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
    {
        return CopyFunc<T, ElRefBase>(dst, offset, src, srcOffset, count);
    }
};

template<class T>
struct CopyOpImpl<T, TRUE, FALSE, FALSE>
{
    Int32 operator()(ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
    {
        return CopyFunc<T, ElRefBase>(dst, offset, src, srcOffset, count);
    }
};

template<class T>
struct CopyOpImpl<T, FALSE, TRUE, TRUE>
{
    Int32 operator()(ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
    {
        return CopyFunc<T, ElLightRefBase>(dst, offset, src, srcOffset, count);
    }
};

template<class T>
struct CopyOpImpl<T, FALSE, TRUE, FALSE>
{
    Int32 operator()(ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
    {
        return CopyFunc<T, ElLightRefBase>(dst, offset, src, srcOffset, count);
    }
};

template<class T>
struct CopyOpImpl<T, FALSE, FALSE, TRUE>
{
    Int32 operator()(ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
    {
        return CopyFunc<T, IInterface>(dst, offset, src, srcOffset, count);
    }
};

template <class T>
inline Int32 QuintetObjectCopyOp::operator()(
    ArrayOf<T>* dst, Int32 offset, T const* src, Int32 srcOffset, Int32 count)
{
    Int32 realOffset = offset * sizeof(T);
    if (!dst || !dst->m_pBuf || !(src + srcOffset) || count <= 0
            || offset < 0  || dst->m_size < realOffset) {
        return -1;
    }

    CopyOpImpl<T,
        SUPERSUBCLASS_EX(ElRefBase*, T),
        SUPERSUBCLASS_EX(ElLightRefBase*, T),
        SUPERSUBCLASS_EX(IInterface*, T)> impl;
    return impl(dst, offset, src, srcOffset, count);
}

template <class T>
inline Int32 QuintetObjectCopyOp::operator()(
    ArrayOf<AutoPtr<T> >* dst, Int32 dstOffset, AutoPtr<T> const* src, Int32 srcOffset, Int32 count)
{
    if (!dst || !dst->m_pBuf || !(src + srcOffset) || count <= 0
            || dstOffset < 0 || dst->GetLength() < dstOffset) {
        return -1;
    }

    Int32 copyCount = MIN(count, dst->GetLength() - dstOffset);
    return PlainCopy((AutoPtr<T>*)(dst->m_pBuf), dstOffset, src, srcOffset, copyCount);
}

inline Int32 QuintetObjectCopyOp::operator()(
    ArrayOf<String>* dst, Int32 dstOffset, String const* src, Int32 srcOffset, Int32 count)
{
    if (!dst || !dst->m_pBuf || !(src + srcOffset) || count <= 0
            || dstOffset < 0 || dst->GetLength() < dstOffset) {
        return -1;
    }

    Int32 copyCount = MIN(count, dst->GetLength() - dstOffset);
    return PlainCopy((String*)(dst->m_pBuf), dstOffset, src, srcOffset, copyCount);
}

/** @addtogroup CARTypesRef
  *   @{
  */
//---------------BufferOf-----------------------------------------------------

template <class T>
class BufferOf : public CarQuintet
{
public:
    T* GetPayload() const {
        return (T*)m_pBuf;
    };

    operator PCarQuintet() {
        return this;
    }

    Int32 GetCapacity() const {
        return m_size / sizeof(T);
    }

    Int32 GetAvailableSpace() const {
        return (m_size - m_used) / sizeof(T);
    }

    Boolean IsNull() const {
        return m_pBuf == NULL;
    }

    Boolean IsEmpty() const {
        return m_used == 0;
    }

    Boolean IsNullOrEmpty() const {
        return (m_pBuf == NULL || m_used == 0);
    }

    Int32 SetUsed(Int32 used) {
        if (used < 0) {
            return -1;
        }
        used = MIN(used, GetCapacity());
        m_used = used * sizeof(T);
        return used;
    }

    Int32 GetUsed() const {
        return m_used / sizeof(T);
    }

    BufferOf& Insert(Int32 offset, const T* p, Int32 n) {
        _BufferOf_Insert(this, offset * sizeof(T), (PByte)p, n * sizeof(T));
        return *this;
    }

    BufferOf& Replace(Int32 offset, const T* pBuf, Int32 n) {
        _BufferOf_Replace(this, offset * sizeof(T), (PByte)pBuf,
        n * sizeof(T));
        return *this;
    }

    BufferOf& Append(T value) {
        _BufferOf_Append(this, (PByte)&value, sizeof(T));
        return *this;
    }

    BufferOf& Append(const T* pBuf, Int32 n) {
        _BufferOf_Append(this, (PByte)pBuf, n * sizeof(T));
        return *this;
    }

    BufferOf& Append(const BufferOf<T> *pSrc) {
        _BufferOf_Append(this, (PByte)pSrc->m_pBuf,
        pSrc->GetUsed() * sizeof(T));
        return *this;
    }

    BufferOf& Copy(const BufferOf<T> *pSrc) {
        _BufferOf_Copy(this, pSrc);
        return *this;
    }

    BufferOf& Copy(const T* pBuf, Int32 n) {
        _BufferOf_CopyEx(this, (Byte *)pBuf, n * sizeof(T));
        return *this;
    }

    BufferOf<T> *Clone() const {
        return (BufferOf<T> *)_CarQuintet_Clone((const PCarQuintet)this);
    }

    void AddRef() const {
        _CarQuintet_AddRef((const PCarQuintet)this);
    }

    Int32 Release() const {
        return _CarQuintet_Release((const PCarQuintet)this);
    }

    T& operator[](Int32 index) {
        assert(m_pBuf && index >= 0 && index < GetUsed());
        return ((T*)(m_pBuf))[index];
    }

    const T& operator[](Int32 index) const {
        assert(m_pBuf && index >= 0 && index < GetUsed());
        return ((T*)(m_pBuf))[index];
    }

    operator const ArrayOf<T>&() const {
        return *(const ArrayOf<T>*)this;
    }

    static BufferOf<T> *Alloc(Int32 capacity) {
        return (BufferOf<T> *)_BufferOf_Alloc(capacity * sizeof(T),
        Type2Flag<T>::Flag());
    }

    static BufferOf<T> *Alloc(T *pBuf, Int32 capacity, Int32 used) {
        assert(used <= capacity);
        return (BufferOf<T> *)_BufferOf_Alloc_Box(pBuf, capacity * sizeof(T),
            used * sizeof(T), Type2Flag<T>::Flag());
    }

    static BufferOf<T> *Alloc(T *pBuf, Int32 capacity) {
        return (BufferOf<T> *)_BufferOf_Alloc_Box(
            pBuf, capacity * sizeof(T),
            capacity * sizeof(T), Type2Flag<T>::Flag());
    }

    static void Free(BufferOf<T> *pArray) {
        if (pArray) {
            pArray->Release();
        }
    }

    BufferOf(T *pBuf, Int32 capacity, Int32 used) {
        assert(used <= capacity);
        _CarQuintet_Init(this, pBuf, capacity * sizeof(T),
                        used * sizeof(T), Type2Flag<T>::Flag());
    }

    BufferOf(T *pBuf, Int32 capacity) {
        _CarQuintet_Init(this, pBuf, capacity * sizeof(T),
                        capacity * sizeof(T), Type2Flag<T>::Flag());
    }

protected:
    // prohibit 'new' operator
    void * operator new(size_t cb);
    BufferOf() {}
    BufferOf& operator = (const BufferOf& buf) {}
    BufferOf(const BufferOf& buf) {}
};
/** @} */

/** @addtogroup BaseTypesRef
  *   @{
  */
template <class T, Int32 SIZE>
class BufferOf_ : public BufferOf<T>
{
public:
    BufferOf_() : BufferOf<T>(m_buf, SIZE, 0)
    {
#ifndef _RELEASE
        if (Type2Flag<T>::Flag() != CarQuintetFlag_Type_Struct) {
            _MemoryBuf_SetByteValue(this, 0x0);
        }
#endif
    }

protected:
    T   m_buf[SIZE ? SIZE : 1];

private:
    // prohibit "=" operator
    BufferOf_& operator = (const BufferOf<T>& buf) {}
    BufferOf_& operator = (const BufferOf_& buf) {}
    BufferOf_(const BufferOf<T>& buf) {}
    BufferOf_(const BufferOf_& buf) {}
};
/** @} */

/** @addtogroup CARTypesRef
  *   @{
  */
//---------------MemoryBuf---------------------------------------------------

class MemoryBuf : public BufferOf<Byte>
{
public:
    MemoryBuf& SetByteValue(Byte value)
    {
        _MemoryBuf_SetByteValue(this, value);
        return *this;
    }

    Int32 Compare(const Byte* pBuf, Int32 n) const
    {
        return _MemoryBuf_Compare((const PCarQuintet)this, pBuf, n);
    }

    Int32 Compare(const PCarQuintet pCq) const
    {
        assert(pCq && pCq->m_pBuf);
        return _MemoryBuf_Compare((const PCarQuintet)this,
                        (const Byte*)pCq->m_pBuf, pCq->m_used);
    }

    MemoryBuf& Insert(Int32 offset, const Byte* p, Int32 n)
    {
        BufferOf<Byte>::Insert(offset, p, n);
        return *this;
    }

    MemoryBuf& Replace(Int32 offset, const Byte* pBuf, Int32 n)
    {
        BufferOf<Byte>::Replace(offset, pBuf, n);
        return *this;
    }

    MemoryBuf& Append(Byte value)
    {
        BufferOf<Byte>::Append(value);
        return *this;
    }

    MemoryBuf& Append(const Byte* pBuf, Int32 n)
    {
        BufferOf<Byte>::Append(pBuf, n);
        return *this;
    }

    MemoryBuf& Append(const PCarQuintet pSrc)
    {
        BufferOf<Byte>::Append((BufferOf<Byte> *)pSrc);
        return *this;
    }

    MemoryBuf& Copy(const PCarQuintet pSrc)
    {
        BufferOf<Byte>::Copy((BufferOf<Byte> *)pSrc);
        return *this;
    }

    MemoryBuf& Copy(const Byte* pBuf, Int32 n)
    {
        BufferOf<Byte>::Copy(pBuf, n);
        return *this;
    }

    MemoryBuf *Clone() const
    {
        return (MemoryBuf *)BufferOf<Byte>::Clone();
    }

    void AddRef() const {
        _CarQuintet_AddRef((const PCarQuintet)this);
    }

    Int32 Release() const {
        return _CarQuintet_Release((const PCarQuintet)this);
    }

    static MemoryBuf *Alloc(Int32 capacity)
    {
        return (MemoryBuf *)BufferOf<Byte>::Alloc(capacity);
    }

    static MemoryBuf *Alloc(Byte *pBuf, Int32 capacity, Int32 used)
    {
        return (MemoryBuf *)BufferOf<Byte>::Alloc(pBuf, capacity, used);
    }

    static MemoryBuf *Alloc(Byte *pBuf, Int32 capacity)
    {
        return (MemoryBuf *)BufferOf<Byte>::Alloc(pBuf, capacity);
    }

    MemoryBuf(Byte *pBuf, Int32 capacity, Int32 used) :
        BufferOf<Byte>(pBuf, capacity, used)
    {
    }

    MemoryBuf(Byte *pBuf, Int32 capacity) :
        BufferOf<Byte>(pBuf, capacity)
    {
    }
};
/** @} */

/** @addtogroup BaseTypesRef
  *   @{
  */
template <Int32 SIZE>
class MemoryBuf_ : public MemoryBuf
{
public:
    MemoryBuf_() : MemoryBuf(m_buf, SIZE, 0)
    {
#ifndef _RELEASE
        _MemoryBuf_SetByteValue(this, 0xCC);
#endif
    }

protected:
    Byte   m_buf[SIZE ? SIZE : 1];
};
/** @} */

typedef MemoryBuf*              PMemoryBuf;

#define NULL_ARRAYOF(type) \
        *(const ArrayOf<type> *)_CarQuintet_GetNullValue( \
            Type2Flag<type>::Flag())

#define NULL_BUFFEROF(type) \
        *(const BufferOf<type> *)_CarQuintet_GetNullValue( \
            Type2Flag<type>::Flag())


#define NULL_MEMORYBUF    \
        *(const MemoryBuf *)_CarQuintet_GetNullValue( \
            CarQuintetFlag_Type_Byte)


_ELASTOS_NAMESPACE_END

#else // !__cplusplus

typedef CarQuintet  ArrayOf;
typedef CarQuintet  BufferOf;
typedef BufferOf    MemoryBuf;

#define NULL_ARRAYOF            0
#define NULL_BUFFEROF           0
#define NULL_MEMORYBUF          0

//ArrayOf
#define ARRAYOF_BOX(pCq, pBuf, size, flags) \
            do { \
                assert((size) >= 0); \
                (pCq)->m_flags = flags; \
                (pCq)->m_reserve = 0; \
                (pCq)->m_size = size; \
                (pCq)->m_used = size; \
                (pCq)->m_pBuf = pBuf; \
            } while (0)

#define ARRAYOF_UNBOX(Cq, ppBuf, pSize, elemsize) \
            do { \
                (*ppBuf) = (Cq).m_pBuf; \
                *(Int32*)pSize= (Cq).m_size / elemsize; \
            } while (0)

//BufferOf
#define BUFFEROF_BOX(pCq, pBuf, size, used, flags) \
            do { \
                assert((size) >= 0 && (used) >= 0); \
                (pCq)->m_flags = flags; \
                (pCq)->m_reserve = 0; \
                (pCq)->m_size = size; \
                (pCq)->m_used = used; \
                (pCq)->m_pBuf = pBuf; \
            } while (0)

#define BUFFEROF_UNBOX(Cq, ppBuf, pSize, pUsed, elemsize) \
            do { \
                (*ppBuf) = (Cq).m_pBuf; \
                *(Int32*)pSize= (Cq).m_size / elemsize; \
                *(Int32*)pUsed = (Cq).m_used / elemsize; \
            } while (0)

//MemoryBuf
#define MEMORYBUF_BOX(pCq, pBuf, size, used) \
            BUFFEROF_BOX(pCq, pBuf, size, used, CarQuintetFlag_Type_Byte)
#define MEMORYBUF_UNBOX(Cq, ppBuf, pSize, pUsed) \
            BUFFEROF_UNBOX(Cq, ppBuf, pSize, pUsed, 1)

#endif // __cplusplus

#endif // __ELQUINTET_H__
