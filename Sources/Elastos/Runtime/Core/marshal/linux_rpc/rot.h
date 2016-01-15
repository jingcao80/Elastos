
#ifndef __ROT_H__
#define __ROT_H__

#include <prxstub.h>
#include <elcontainer.h>

namespace Elastos {
namespace RPC {

typedef struct ExportObject: SLinkNode
{
    String              mNetAddress;
    IInterface*         mObject;
    IStub*              mIStub;
} ExportObject;

typedef struct ImportObject: SLinkNode
{
    String              mNetAddress;
    IProxy*             mIProxy;
} ImportObject;

extern ECode InitROT();

extern void UninitROT();

extern ECode RegisterExportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IInterface* object,
    /* [in] */ IStub* stub);

extern ECode UnregisterExportObject(
    /* [in] */ const String& netAddress);

extern ECode FindExportObject(
    /* [in] */ IInterface* object,
    /* [out] */ ExportObject* expObj);

extern ECode FindExportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ExportObject* expObj);

extern ECode RegisterImportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IProxy* proxy);

extern ECode UnregisterImportObject(
    /* [in] */ const String& netAddress);

extern ECode FindImportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ImportObject* impObj);

} // namespace RPC
} // namespace Elastos

#endif //__ROT_H__
