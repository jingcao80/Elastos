//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <eltypes.h>

#ifdef  __cplusplus
extern "C" {
#endif

_ELASTOS_NAMESPACE_USING

static CarQuintet s_nullCq[] = {
    {0, 0, 0, 0, NULL},
    {1, 0, 0, 0, NULL},
    {2, 0, 0, 0, NULL},
    {3, 0, 0, 0, NULL},
    {4, 0, 0, 0, NULL},
    {5, 0, 0, 0, NULL},
    {6, 0, 0, 0, NULL},
    {7, 0, 0, 0, NULL},
    {8, 0, 0, 0, NULL},
    {9, 0, 0, 0, NULL},
    {10, 0, 0, 0, NULL},
    {11, 0, 0, 0, NULL},
    {12, 0, 0, 0, NULL},
    {13, 0, 0, 0, NULL},
    {14, 0, 0, 0, NULL},
    {15, 0, 0, 0, NULL},
    {16, 0, 0, 0, NULL},
    {17, 0, 0, 0, NULL},
    {18, 0, 0, 0, NULL},
};

PCarQuintet __cdecl _CarQuintet_GetNullValue(CarQuintetFlags flag)
{
    flag = flag & CarQuintetFlag_TypeMask;
    if (flag < 1 || flag > 18) {
        flag = 0;
    }

    return &s_nullCq[flag];
}

PCarQuintet __cdecl _CarQuintet_Init(PCarQuintet pCq,
    PVoid pBuf, Int32 size, Int32 used, CarQuintetFlags flags)
{
    ELA_ASSERT_WITH_BLOCK(used <= size) {
        printf("_CarQuintet_Init with invalid arguments. size: %d, used: %d, flags: %08x\n",
                size, used, flags);
    }

    if (used > size) used = size;

    if (pCq) {
        pCq->m_flags = flags;
        pCq->m_reserve = 0;
        if (pBuf) {
            assert(size >=0 && used >= 0);
            pCq->m_size = size;
            pCq->m_used = used;
        }
        else {
            assert(size ==0 && used == 0);
            pCq->m_size = 0;
            pCq->m_used = 0;
        }
        pCq->m_pBuf = pBuf;
    }

    return pCq;
}

PCarQuintet __cdecl _CarQuintet_Alloc(Int32 size)
{
    if (size < 0 || size > (Int32)_MAX_CARQUINTET_SIZE_) {
        return NULL;
    }

    Int32 bufSize = sizeof(CarQuintet) + (size ? size : 1);
    SharedBuffer* buf = SharedBuffer::Alloc(bufSize, FALSE);
    if (buf) {
        return (PCarQuintet)(buf->GetData());
    }
    return NULL;
}

void __cdecl _CarQuintet_Free(PCarQuintet pCq)
{
    if (!pCq) return;

    if (IS_QUINTENT_FLAG(pCq, CarQuintetFlag_HeapAlloced)) {
        SharedBuffer::GetBufferFromData(pCq)->Release();
    }
}

PCarQuintet __cdecl _CarQuintet_Clone(const PCarQuintet pCq)
{
    if (!pCq) return NULL;

    PCarQuintet pNewCq = _CarQuintet_Alloc(pCq->m_size);
    if (pNewCq) {
        CarQuintetFlags flags = pCq->m_flags & CarQuintetFlag_TypeMask;
        flags |= CarQuintetFlag_HeapAlloced;

        _CarQuintet_Init(pNewCq, pNewCq + 1, pCq->m_size, pCq->m_used, flags);
        if (pCq->m_pBuf) {
            memcpy(pNewCq->m_pBuf, pCq->m_pBuf, pCq->m_size);
        }
        else {
            pNewCq->m_pBuf = NULL;
        }
    }

    return pNewCq;
}

void __cdecl _CarQuintet_AddRef(const PCarQuintet pCq)
{
    if (!pCq) return;

    if (IS_QUINTENT_FLAG(pCq, CarQuintetFlag_HeapAlloced)) {
        SharedBuffer::GetBufferFromData(pCq)->Acquire();
        pCq->m_flags |= CarQuintetFlag_AutoRefCounted;
    }
}

Int32  __cdecl _CarQuintet_Release(PCarQuintet pCq)
{
    if (!pCq) return 0;

    if (IS_QUINTENT_FLAG(pCq, CarQuintetFlag_HeapAlloced)) {
        SharedBuffer * buf = SharedBuffer::GetBufferFromData(pCq);
        if (IS_QUINTENT_FLAG(pCq, CarQuintetFlag_AutoRefCounted)) {
            return buf->Release();
        }
        else {
            ELA_ASSERT_WITH_BLOCK(buf->RefCount() == 0) {
                printf(" >> %s %d\n >> Ref count of share buffer %d isn't zero,"
                        " and CarQuintetFlag_AutoRefCounted isn't set too. m_flags:%08x, m_size:%d\n",
                        __FILE__, __LINE__, buf->RefCount(), pCq->m_flags, pCq->m_size);
            }

            SharedBuffer::Dealloc(buf);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------

PCarQuintet __cdecl _ArrayOf_Alloc(Int32 size, CarQuintetFlags flags)
{
    PCarQuintet pCq = _CarQuintet_Alloc(size);
    flags |= CarQuintetFlag_HeapAlloced;
    _CarQuintet_Init(pCq, pCq + 1, size, size, flags);
    if (pCq) memset(pCq->m_pBuf, 0x0, size);

    return pCq;
}

PCarQuintet __cdecl _ArrayOf_Alloc_Box(PVoid pBuf, Int32 size, CarQuintetFlags flags)
{
    if (size < 0) return NULL;

    PCarQuintet pCq = _CarQuintet_Alloc(0);
    flags |= CarQuintetFlag_HeapAlloced;
    return _CarQuintet_Init(pCq, pBuf, size, size, flags);
}

Int32 __cdecl _ArrayOf_Replace(PCarQuintet pCq, Int32 offset, const PByte p, Int32 n)
{
    if (!pCq || !pCq->m_pBuf || offset < 0 || !p || n < 0 || pCq->m_size < offset) {
        return -1;
    }

    Int32 nReplace = MIN(n, pCq->m_size - offset);
    memcpy((PByte)pCq->m_pBuf + offset, p, nReplace);

    return nReplace;
}

Int32 __cdecl _ArrayOf_Copy(PCarQuintet pcqDst, const CarQuintet* pcqSrc)
{
    if (!pcqDst || !pcqDst->m_pBuf || !pcqSrc || !pcqSrc->m_pBuf) {
        return -1;
    }

    Int32 n = MIN(pcqSrc->m_used, pcqDst->m_size);
    memcpy(pcqDst->m_pBuf, pcqSrc->m_pBuf, n);
    return n;
}

Int32 __cdecl _ArrayOf_CopyEx(PCarQuintet pCq, const Byte* p, MemorySize n)
{
    if (!pCq || !pCq->m_pBuf || !p || n < 0) {
        return -1;
    }

    n = MIN(n, pCq->m_size);
    memcpy(pCq->m_pBuf, p, n);
    return n;
}

//----------------------------------------------------------------------------

PCarQuintet __cdecl _BufferOf_Alloc(Int32 size, CarQuintetFlags flags)
{
    if (size < 0) return NULL;

    PCarQuintet pCq = _CarQuintet_Alloc(size);
    flags |= CarQuintetFlag_HeapAlloced;
    _CarQuintet_Init(pCq, pCq + 1, size, 0, flags);
    if (pCq) memset(pCq->m_pBuf, 0x0, size);

    return pCq;
}

PCarQuintet __cdecl _BufferOf_Alloc_Box(PVoid pBuf, Int32 capacity, Int32 used,
    CarQuintetFlags flags)
{
    if (capacity < 0 || used < 0) return NULL;

    PCarQuintet pCq = _CarQuintet_Alloc(0);

    flags |= CarQuintetFlag_HeapAlloced;
    return _CarQuintet_Init(pCq, pBuf, capacity, used, flags);
}

Int32 __cdecl _BufferOf_Insert(PCarQuintet pCq, Int32 offset, const PByte pb,
    MemorySize n)
{
    if (!pCq || !pCq->m_pBuf || offset < 0 || !pb || offset > pCq->m_used
        || offset == pCq->m_size || n < 0) {
        return -1;
    }

    PByte pBuf = (PByte)pCq->m_pBuf;
    Int32 nInsert = MIN(n, pCq->m_size - offset);
    Int32 nMove = MIN(pCq->m_size - offset - nInsert, pCq->m_used - offset);
    if (nMove) {
        Int32 nMove = MIN(pCq->m_size - offset - nInsert, pCq->m_used - offset);
        memmove(pBuf + offset + nInsert, pBuf + offset, nMove);
    }

    memcpy(pBuf + offset, pb, nInsert);

    pCq->m_used = offset + nInsert + nMove;

    return nInsert;
}

Int32 __cdecl _BufferOf_Append(PCarQuintet pCq, const PByte p, MemorySize n)
{
    if (!pCq || !pCq->m_pBuf || p == NULL || n < 0) {
        return -1;
    }

    Int32 append = MIN(n, pCq->m_size - pCq->m_used);

    if (append) {
        memcpy((PByte)pCq->m_pBuf + pCq->m_used, p, append);
        pCq->m_used += append;
    }

    return append;
}

Int32 __cdecl _BufferOf_Replace(PCarQuintet pCq, Int32 offset,
    const PByte p, Int32 n)
{
    /* The offset argument must be greater than or equal to 0 and
     * less than or equal to the length of capacity.
     */
    if (!pCq || !pCq->m_pBuf || offset < 0 || offset > pCq->m_used || !p || n < 0) {
        return -1;
    }

    Int32 nReplace = MIN(n, pCq->m_size - offset);
    pCq->m_used = MAX(offset + nReplace, pCq->m_used);

    memcpy((PByte)pCq->m_pBuf + offset, p, nReplace);

    return nReplace;
}

Int32 __cdecl _BufferOf_Copy(PCarQuintet pcqDst, const CarQuintet* pcqSrc)
{
    if (!pcqDst || !pcqDst->m_pBuf || !pcqSrc || !pcqSrc->m_pBuf) {
        return -1;
    }

    pcqDst->m_used = MIN(pcqSrc->m_used, pcqDst->m_size);
    memcpy(pcqDst->m_pBuf, pcqSrc->m_pBuf, pcqDst->m_used);
    return pcqDst->m_used;
}

Int32 __cdecl _BufferOf_CopyEx(PCarQuintet pCq, const Byte* p, MemorySize n)
{
    if (!pCq || !pCq->m_pBuf || !p || n < 0) {
        return -1;
    }

    n = MIN(n, pCq->m_size);
    memcpy(pCq->m_pBuf, p, n );
    pCq->m_used = n;
    return n;
}

//------------------------------------------------------------------------------

void _MemoryBuf_SetByteValue(PCarQuintet pCq, Byte value)
{
    if (!pCq || !pCq->m_pBuf) return;
    memset(pCq->m_pBuf, value, pCq->m_size);
}

Int32 _MemoryBuf_Compare(const PCarQuintet pCq, const Byte* pBuf, Int32 count)
{
    if (!pCq || !pCq->m_pBuf) return -2;
    if (!pBuf) return 2;

    if (pCq->m_used != count) {
        return pCq->m_used - count;
    }

    return memcmp(pCq->m_pBuf, pBuf, count);
}

#ifdef  __cplusplus
}
#endif
