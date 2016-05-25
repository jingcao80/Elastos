
#ifndef __MARSHAL_COMM_H__
#define __MARSHAL_COMM_H__

#include <clsinfo.h>

_ELASTOS_NAMESPACE_USING

#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif

#define MSHDBG_NORMAL           1
#define MSHDBG_CREF             2
#define MSHDBG_WARNING          3
#define MSHDBG_ERROR            4
#define MSHDBG_NONE             100

#define MSHDBG_LEVEL            MSHDBG_ERROR

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
#define MARSHAL_DBGOUT(level, exp) \
    ((level) >= MSHDBG_LEVEL ? (void)(exp) : (void)0)
extern int _DumpGUID(REIID riid);
extern int _DumpCLSID(RClassID rclsid);
extern int _DumpBin(const void *pv, int size);

#define DUMP_GUID(riid)         _DumpGUID(riid)
#define DUMP_CLSID(rclsid)      _DumpCLSID(rclsid)
#else
#define MARSHAL_DBGOUT(level, exp)
#define DUMP_GUID(riid)
#define DUMP_CLSID(rclsid)
#endif // _MARSHAL_DEBUG

#define MARSHAL_MAGIC           0x7a79636f

#define MSH_MAX_METHODS         (224 + 4)

#define MSH_ALIGN_4(n)          RoundUp2((n), 4)

#define MSH_NULL                ((UInt32)0)
#define MSH_NOT_NULL            ((UInt32)-2)

typedef struct MarshalHeader
{
    UInt32              mMagic;           // magic number
    Int16               mInterfaceIndex;  // interface index in object
    Int16               mMethodIndex;     // method index in interface
    UInt32              mInSize;          // size of in data
    UInt32              mOutSize;         // size of out data
}   MarshalHeader;

#endif //__MARSHAL_COMM_H__
