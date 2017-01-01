
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
//typedef unsigned char byte;
#include <lube.h>

DECL_STATEMENT(NoMean);
DECL_STATEMENT(Text);
DECL_STATEMENT(Output);
DECL_STATEMENT(Template);
DECL_STATEMENT(With);
DECL_STATEMENT(WithAll);
DECL_STATEMENT(If);
DECL_STATEMENT(Member);
DECL_STATEMENT(Func);

const StatementEntry g_statements[] = {
    STATEMENT_(NoMean),
    STATEMENT_(Text),
    STATEMENT_(Output),
    STATEMENT_(Template),
    STATEMENT_(If),
    STATEMENT_(With),
    STATEMENT_(WithAll),
    STATEMENT_(Member),
    STATEMENT_(Func),
};
const int c_cStatements = sizeof(g_statements) / sizeof(StatementEntry);

IMPL_STATEMENT(NoMean)(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    assert(TRUE == FALSE);
    return LUBE_OK;
}

IMPL_STATEMENT(Text)(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    assert(NULL != pDesc->pvData);

    pCtx->PutData(pDesc->pvData, pDesc->uDataSize - 1);
    return LUBE_OK;
}

IMPL_STATEMENT(Output)(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    int nRet;
    FILE *pOrigFile;

    if (pCtx->EnterFile((char *)pDesc->pvData, &pOrigFile) < 0) {
        return LUBE_OK;
    }

    nRet = pCtx->ExecStatements(pDesc->pBlockette);

    pCtx->LeaveFile(pOrigFile);
    return nRet;
}

IMPL_STATEMENT(Member)(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    char szBuf[c_nStrBufSize];

    if (pCtx->GetMemberString(pDesc->object, pDesc->member, szBuf) < 0) {
        return LUBE_FAIL;
    }

    pCtx->PutString(szBuf);
    return LUBE_OK;
}
