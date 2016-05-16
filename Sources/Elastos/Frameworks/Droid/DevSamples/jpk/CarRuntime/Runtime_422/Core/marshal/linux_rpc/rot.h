
#ifndef __ROT_H__
#define __ROT_H__

#include "prxstub.h"
#include "syslinknode.h"

namespace Elastos {
namespace RPC {

typedef struct ExportObject: SLinkNode
{
    String              m_sNetAddress;
    IInterface          *m_pObject;
    IStub               *m_pIStub;
} ExportObject;

typedef struct ImportObject: SLinkNode
{
    String              m_sNetAddress;
    IProxy              *m_pIProxy;
} ImportObject;

extern ECode InitROT();

extern void UninitROT();

extern ECode RegisterExportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IInterface *pObject,
    /* [in] */ IStub *pIStub);

extern ECode UnregisterExportObject(
    /* [in] */ const String& netAddress);

extern ECode FindExportObject(
    /* [in] */ IInterface *pObject,
    /* [out] */ ExportObject *pExport);

extern ECode FindExportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ExportObject *pExport);

extern ECode RegisterImportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IProxy *pIProxy);

extern ECode UnregisterImportObject(
    /* [in] */ const String& netAddress);

extern ECode FindImportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ImportObject *pImport);

} // namespace RPC
} // namespace Elastos

#endif //__ROT_H__
