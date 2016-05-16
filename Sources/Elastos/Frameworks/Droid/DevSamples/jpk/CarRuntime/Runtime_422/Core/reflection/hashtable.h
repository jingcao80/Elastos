//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#ifndef __HASHTTABLE_H__
#define __HASHTTABLE_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <clstype.h>

_ELASTOS_NAMESPACE_USING

#define MUL_LOADFACTOR(n) (((n) * 3) >> 2) //n * 0.75

template <class T, CARDataType type = Type_UInt32>
class HashTable
{
public:
    HashTable(int nInitialCapacity = 11,
              float fLoadFactor = 0.75f);

    ~HashTable();

    inline int Size();

    inline bool IsEmpty();

    T* Get(PVoid pKey);

    T* operator[](PVoid pKey);

    bool Put(PVoid pKey, T* pValue);

    bool Put(PVoid pKey, T& value);

    bool Remove(PVoid pKey);

    bool Contains(PVoid pKey);

    void Clear();

private:
    struct HashEntry
    {
        int nHash;
        T value;
        HashEntry *pNext;
        Byte key[1];
    };

    bool Rehash();

    unsigned int Hash(PVoid pKey);

    bool keycmp(HashEntry *e, PVoid pKey);

    void keycpy(HashEntry *e, PVoid pKey);

    int keylen(PVoid pKey);

private:
    struct HashEntry **m_pTable;
    int m_nCapacity;
    int m_nCount;
    int m_nThreshold;
    float m_fLoadFactor;
    int m_nModCount;
};

template <class T, CARDataType type>
HashTable<T, type>::HashTable(int nInitialCapacity, float fLoadFactor) :
    m_pTable(NULL), m_nCapacity(0), m_nCount(0), m_nThreshold(0),
    m_fLoadFactor(0), m_nModCount(0)
{
    if (nInitialCapacity <= 0)
        nInitialCapacity = 10;

#ifndef _arm
    if (fLoadFactor <= 0)
        fLoadFactor = 0.75f;
#endif

    m_nCapacity = nInitialCapacity;
    m_fLoadFactor = fLoadFactor;
#ifndef _arm
    m_nThreshold = (int)(nInitialCapacity * fLoadFactor);
#else
    m_nThreshold = MUL_LOADFACTOR(nInitialCapacity);
#endif
}

template <class T, CARDataType type>
HashTable<T, type>::~HashTable()
{
    if (m_pTable) {
        Clear();
        free(m_pTable);
    }
}

template <class T, CARDataType type>
bool HashTable<T, type>::keycmp(HashEntry *e, PVoid pKey)
{
    int nRet = 0;
    switch (type) {
        case Type_UInt32:
            nRet = memcmp(e->key, pKey, sizeof(int));
            break;
        case Type_UInt64:
            nRet = memcmp(e->key, pKey, sizeof(UInt64));
            break;
        case Type_String:
            nRet = strcmp((char *)e->key, (char*)pKey);
            break;
        case Type_EMuid:
            nRet = memcmp(e->key, pKey, sizeof(EMuid));
            break;
        default:
            nRet = 1;
            break;
    }

    if (!nRet) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

template <class T, CARDataType type>
void HashTable<T, type>::keycpy(HashEntry *e, PVoid pKey)
{
    switch (type) {
        case Type_UInt32:
            *(UInt32 *)e->key = *(UInt32 *)pKey;
            break;
        case Type_UInt64:
            *(UInt64 *)e->key = *(UInt64 *)pKey;
            break;
        case Type_String:
            strcpy((char *)e->key, (char*)pKey);
            break;
        case Type_EMuid:
            memcpy(e->key, pKey, sizeof(EMuid));
            break;
        default:
            break;
    }
    return;
}

template <class T, CARDataType type>
int HashTable<T, type>::keylen(PVoid pKey)
{
    int len = 0;
    switch (type) {
        case Type_UInt32:
            len = sizeof(int);
            break;
        case Type_UInt64:
            len = sizeof(UInt64);
            break;
        case Type_String:
            len  = strlen((char*)pKey) + 1;
            break;
        case Type_EMuid:
            len = sizeof(EMuid);
            break;
        default:
            break;
    }
    return len;
}

template <class T, CARDataType type>
int HashTable<T, type>::Size()
{
    return m_nCount;
}

template <class T, CARDataType type>
bool HashTable<T, type>::IsEmpty()
{
    return m_nCount == 0;
}

template <class T, CARDataType type>
T* HashTable<T, type>::Get(PVoid pKey)
{
    assert(pKey  && "NULL or empty key name!");

    if (!pKey || !m_pTable) {
        return NULL;
    }

    int nHash = Hash(pKey);
    int nIndex = (nHash & 0x7FFFFFFF) % m_nCapacity;
    for (struct HashEntry *e = m_pTable[nIndex]; e != NULL ; e = e->pNext) {
        if ((e->nHash == nHash) && keycmp(e, pKey)) {
            return &e->value;
        }
    }

    return NULL;
}

template <class T, CARDataType type>
T* HashTable<T, type>::operator[](PVoid pKey)
{
    return Get(pKey);
}

template <class T, CARDataType type>
bool HashTable<T, type>::Rehash()
{
    int nOldCapacity = m_nCapacity;
    struct HashEntry **pOldTable = m_pTable;

    int nNewCapacity = nOldCapacity * 2 + 1;
    struct HashEntry **pNewTable = (struct HashEntry **)malloc(
            sizeof(struct HashEntry **) * nNewCapacity);
    if (!pNewTable)
        return false;

    memset(pNewTable, 0, sizeof(struct HashEntry **) * nNewCapacity);

    m_nModCount++;
#ifndef _arm
    m_nThreshold = (int)(nNewCapacity * m_fLoadFactor);
#else
    m_nThreshold = MUL_LOADFACTOR(nNewCapacity);
#endif
    m_nCapacity = nNewCapacity;
    m_pTable = pNewTable;

    for (int i = nOldCapacity ; i--> 0 ;) {
        for (struct HashEntry *p = pOldTable[i]; p != NULL ;) {
            struct HashEntry *e = p;
            p = p->pNext;

            int nIndex = (e->nHash & 0x7FFFFFFF) % nNewCapacity;
            e->pNext = pNewTable[nIndex];
            pNewTable[nIndex] = e;
        }
    }

    free(pOldTable);

    return true;
}

template <class T, CARDataType type>
bool HashTable<T, type>::Put(PVoid pKey, T* pValue)
{
    assert(pKey && "NULL or empty key name!");
    assert(pValue && "Can not put NULL value to Hashtable!");

    if (!pKey || !pValue)
        return false;

    if (!m_pTable) {
        m_pTable = (struct HashEntry **)malloc(sizeof(struct HashEntry **)
                    * m_nCapacity);

        if (!m_pTable)
            return false;

        memset(m_pTable, 0, sizeof(struct HashEntry **) * m_nCapacity);
    }

    // Makes sure the key is not already in the hashtable.
    int nHash = Hash(pKey);
    int nIndex = (nHash & 0x7FFFFFFF) % m_nCapacity;
    struct HashEntry *e;
    for (e = m_pTable[nIndex] ; e != NULL ; e = e->pNext) {
        if ((e->nHash == nHash) && keycmp(e, pKey)) {
            memcpy(&(e->value), pValue, sizeof(T));
            return true;
        }
    }

    m_nModCount++;
    if (m_nCount >= m_nThreshold) {
        // Rehash the table if the threshold is exceeded
        if (!Rehash())
            return false;

        nIndex = (nHash & 0x7FFFFFFF) % m_nCapacity;
    }

    // Creates the new entry.
    int size = keylen(pKey);

    e = (struct HashEntry *)malloc(
            sizeof(struct HashEntry) + size);
    if (!e)
        return false;

    e->nHash = nHash;
    e->pNext = m_pTable[nIndex];
    memcpy(&(e->value), pValue, sizeof(T));
    keycpy(e, pKey);

    m_pTable[nIndex] = e;
    m_nCount++;

    return true;
}

template <class T, CARDataType type>
bool HashTable<T, type>::Put(PVoid pKey, T& value)
{
    return Put(pKey, &value);
}

template <class T, CARDataType type>
bool HashTable<T, type>::Remove(PVoid pKey)
{
    assert(pKey && "NULL or empty key name!");

    if (!pKey || !m_pTable)
        return false;

    int nHash = Hash(pKey);
    int nIndex = (nHash & 0x7FFFFFFF) % m_nCapacity;
    for (struct HashEntry *e = m_pTable[nIndex], *prev = NULL ; e != NULL ;
        prev = e, e = e->pNext) {
        if ((e->nHash == nHash) && keycmp(e, pKey)) {
            m_nModCount++;
            if (prev != NULL) {
                prev->pNext = e->pNext;
            }
            else {
                m_pTable[nIndex] = e->pNext;
            }

            m_nCount--;

            free(e);
            return true;
        }
    }

    return false;
}

template <class T, CARDataType type>
bool HashTable<T, type>::Contains(PVoid pKey)
{
    assert(pKey  && "NULL or empty key name!");

    if (!pKey || !m_pTable)
        return false;

    int nHash = Hash(pKey);
    int nIndex = (nHash & 0x7FFFFFFF) % m_nCapacity;
    for (struct HashEntry *e = m_pTable[nIndex] ; e != NULL ; e = e->pNext) {
        if ((e->nHash == nHash) && keycmp(e, pKey))
            return true;
    }

    return false;
}

template <class T, CARDataType type>
void HashTable<T, type>::Clear()
{
    if (!m_pTable)
        return;

    m_nModCount++;

    for (int nIndex = m_nCapacity;--nIndex >= 0;) {
        for (struct HashEntry *e = m_pTable[nIndex]; e != NULL;) {
            struct HashEntry *p = e;
            e = e->pNext;
            free(p);
        }

        m_pTable[nIndex] = NULL;
    }

    m_nCount = 0;
}

template <class T, CARDataType type>
unsigned int HashTable<T, type>::Hash(PVoid pKey)
{
    unsigned int value = 0L;
    char ch = '\0', *str = (char *)pKey;
    unsigned long *pValue = (unsigned long *)pKey;
    int len = sizeof(EMuid) / sizeof(unsigned long);
    int i = 0;
    switch (type) {
        case Type_UInt32:
            value = value ^ ((value << 5) + (value >> 3) + *pValue);
            break;
        case Type_UInt64:
            value = value ^ ((value << 5) + (value >> 3) + pValue[0]);
            value = value ^ ((value << 5) + (value >> 3) + pValue[1]);
            break;
        case Type_String:
            if (str != NULL) {
                value += 30 * (*str);
                while ((ch = *str++) != 0) {
                    value = value ^ ((value << 5) + (value >> 3)
                            + (unsigned long)ch);
                }
            }
            break;
        case Type_EMuid:
            for (i = 0; i < len; i++) {
                value = value ^ ((value << 5) + (value >> 3) + pValue[i]);
            }
            break;
        default:
            break;
    }

    return value;
}

#endif // __HASHTTABLE_H__
