//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#ifndef __HOOK_H__
#define __HOOK_H__

#include <stdio.h>
#include <assert.h>

//#define _DBGHOOK

#define IsInValidHook(x)  (((x) < 0) ||((x) >= MAX_ITEMS))

typedef void (*HOOKPROC)(int id, int nCode, void *pParam);

typedef struct {
//    ListNode m_Link;
    void * m_Proc;
}HOOKNODE;

class Hook
{
public:
    Hook(int MaxItem=32);
    ~Hook(void);

    int     Set(int id, const void *pHookProc);
    int     Unhook(int id);
    void    CallNext(int id, int nCode, void *pParam);
#if defined(_DBGHOOK)
    void    Dump(void);
#endif

private:
    const int MAX_ITEMS;// The actual items
    HOOKNODE *Chain;    // The actual hook array
};

// Hook Constructor function
Hook::Hook(int MaxItem) : MAX_ITEMS( MaxItem )
{
    Chain = new HOOKNODE[MAX_ITEMS];
    assert(Chain);
#if defined(_DBGHOOK)
    printf("Hook %d\n", MAX_ITEMS);
#endif
    for (int i = 0; i < MAX_ITEMS; i++) {
        Chain[i].m_Proc = NULL;
    }
}

// Hook Destructor function
Hook::~Hook(void)
{
    delete[] Chain;
}

#if defined(_DBGHOOK)
// Dump() function
void Hook::Dump(void)
{
    printf("\tTotal items: %d\n", MAX_ITEMS);
    for (int i = 0; i < MAX_ITEMS; i++)
        if (Chain[i].m_Proc)
            printf("\t[%d] (%x)\n", i, Chain[i]);
    return ;
}
#endif

int Hook::Set(int id, const void *pHookProc)
{
    if (IsInValidHook(id)) return -1;

    switch (id) {
        case 0:
            break;
        default:
            HOOKNODE *h = &Chain[id];
            //if (h->m_Proc) return 1;//BUGBUG:
            h->m_Proc = (void *)pHookProc;
            break;
    }
    return 0;
}

int Hook::Unhook(int id)
{
    return -1;//Not implement
}

void Hook::CallNext(int id, int nCode, void *pParam)
{
    if (IsInValidHook(id)) return;

    HOOKNODE *h = &Chain[id];
    if (h->m_Proc) {
        ((HOOKPROC)(h->m_Proc))(id, nCode, pParam);//Call
    }
    return;
}

#endif /*__HOOK_H__*/
