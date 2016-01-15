//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __STATEMENT_H__
#define __STATEMENT_H__

typedef int (*StatementFn_t)(PLUBECTX, PSTATEDESC);

#define IMPL_STATEMENT(func) int Statement_##func
#define DECL_STATEMENT(func) extern int Statement_##func(PLUBECTX, PSTATEDESC)
#define CALL_STATEMENT(func) Statement_##func

typedef struct StatementEntry {
    StatementType   type;
    StatementFn_t   fnStatement;
}   StatementEntry;

extern const StatementEntry g_statements[];
EXTERN_C const int c_cStatements;

#define STATEMENT_(func) { State_##func, Statement_##func }

inline StatementFn_t GetStatementFunc(int nIndex)
{
    assert(0 < nIndex && nIndex < c_cStatements);
    return g_statements[nIndex].fnStatement;
}

inline int GetStatementIndex(StatementType type)
{
    for (int n = 0; n < c_cStatements; n++) {
        if (g_statements[n].type == type) return n;
    }
    return 0;
}

#endif // __STATEMENT_H__
