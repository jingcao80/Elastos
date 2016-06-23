#ifndef __ELASTOS_DROID_FRAMEWORKDEF_H__
#define __ELASTOS_DROID_FRAMEWORKDEF_H__

#ifndef __ELASTOS_CORE_DEF_H__
#include <elastos/coredef.h>
#endif

#include "Elastos.Droid.Core.h"

#ifndef TO_ATTRS_ARRAYOF(attrs)
#define TO_ATTRS_ARRAYOF (ArrayOf<Int32>::Alloc(const_cast<Int32 *>(attrs), ArraySize(attrs)))
#endif

// #define MAX_PATH    256

// #define XML_NAMESPACE   "http://schemas.elastos.com/capsule/res/elastos"

#ifndef DUMP_CLSID
#define DUMP_CLSID(intf) \
    do { \
        ClassID clsid; \
        IObject* obj = IObject::Probe(intf); \
        assert(obj != NULL); \
        obj->GetClassID(&clsid); \
        printf("======== DUMP_CLSID ========\n"); \
        printf("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                clsid.clsid.Data1, clsid.clsid.Data2, clsid.clsid.Data3, \
                clsid.clsid.Data4[0], clsid.clsid.Data4[1], \
                clsid.clsid.Data4[2], clsid.clsid.Data4[3], \
                clsid.clsid.Data4[4], clsid.clsid.Data4[5], \
                clsid.clsid.Data4[6], clsid.clsid.Data4[7]); \
        printf("============================\n"); \
    } while(0);
#endif

#ifndef DUMP_ITFID
#define DUMP_ITFID(intf) \
    do { \
        InterfaceID iid; \
        intf->GetInterfaceID(intf, &iid); \
        printf("======== DUMP_ITFID ========\n"); \
        printf("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                iid.Data1, iid.Data2, iid.Data3, \
                iid.Data4[0], iid.Data4[1], \
                iid.Data4[2], iid.Data4[3], \
                iid.Data4[4], iid.Data4[5], \
                iid.Data4[6], iid.Data4[7]); \
        printf("============================\n"); \
    } while(0);
#endif


#endif //__ELASTOS_DROID_FRAMEWORKDEF_H__
