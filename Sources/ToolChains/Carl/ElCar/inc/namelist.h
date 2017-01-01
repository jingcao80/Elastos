
#ifndef __NAMELIST_H__
#define __NAMELIST_H__

class NameList
{
public:
    void Initialize(char *);
    void Reset();
    BOOL MoveNext();
    char *Current();

private:
    char    *m_pList;
    char    *m_pTail;
    char    *m_pCurr;
    char    *m_pNext;
};

inline void NameList::Initialize(char *pszList)
{
    while (';' == *pszList) pszList++;
    m_pList = pszList;
    m_pTail = pszList + strlen(pszList);
    m_pCurr = m_pNext = m_pList;
}

inline void NameList::Reset()
{
    char *p = m_pList;

    while (p != m_pTail) {
        if (0 == *p) *p = ';';
        p++;
    }
    m_pCurr = m_pNext = m_pList;
}

inline BOOL NameList::MoveNext()
{
    if (0 == *m_pNext) return FALSE;
    m_pCurr = m_pNext;

    while (0 != *m_pNext) {
        if (';' == *m_pNext) {
            if (m_pNext != m_pCurr) {
                *m_pNext++ = 0;
                return TRUE;
            }
            m_pCurr = m_pNext + 1;
        }
        m_pNext++;
    }
    return m_pNext != m_pCurr;
}

inline char *NameList::Current()
{
    return m_pCurr;
}

#endif // __NAMELIST_H__
