//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
