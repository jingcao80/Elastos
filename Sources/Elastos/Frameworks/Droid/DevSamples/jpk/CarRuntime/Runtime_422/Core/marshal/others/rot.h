
#ifndef __ROT_H__
#define __ROT_H__

#include "prxstub.h"
#include "syslinknode.h"

_ELASTOS_NAMESPACE_USING

typedef struct ExportObject: SLinkNode 
{
    ExportObject() : m_connName(NULL) {}

    ~ExportObject()
    {
        if (!m_connName) free(m_connName);
    }
    char            *m_connName;
    IInterface      *m_pObject;
    IStub           *m_pIStub;
} ExportObject;

typedef struct ImportObject: SLinkNode 
{
    ImportObject() : m_stubConnName(NULL) {}

    ~ImportObject()
    {
        if (!m_stubConnName) free(m_stubConnName);
    }
    char            *m_stubConnName;
    IProxy          *m_pIProxy;
} ImportObject;

extern ECode InitROT();

extern void UninitROT();

extern ECode RegisterExportObject(
        /* [in] */ const char* connName,
        /* [in] */ IInterface *pObject,
        /* [in] */ IStub *pIStub);

extern ECode UnregisterExportObject(
        /* [in] */ const char* connName);

extern ECode FindExportObject(
        /* [in] */ IInterface *pObject,
        /* [out] */ ExportObject *pExport);

extern ECode FindExportObject(
        /* [in] */ const char* connName,
        /* [out] */ ExportObject *pExport);

extern ECode RegisterImportObject(
        /* [in] */ const char* stubConnName,
        /* [in] */ IProxy *pIProxy);

extern ECode UnregisterImportObject(
        /* [in] */ const char* stubConnName);

extern ECode FindImportObject(
        /* [in] */ const char* stubConnName,
        /* [out] */ ImportObject *pImport);

#endif //__ROT_H__
