
#ifndef __CMEMORYDUMPER_H__
#define __CMEMORYDUMPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;

namespace Elastos {
namespace MemoryDumper {

class CMemoryDumper
{
public:
    static CARAPI DumpMemory(
        /* [in] */ Int32 fd);

    static CARAPI OutputBackTrace(
        /* [in] */ const String& diffFile,
        /* [in] */ Int32 pid);

private:
    class MapInfoManager : public ElLightRefBase {
    public:
        struct MapInfo {
            struct MapInfo* next;
            unsigned start;
            unsigned end;
            char name[];
        };

        MapInfoManager(
            /* [in] */ Int32 pid);

        ~MapInfoManager();

        MapInfo* miList;

        CARAPI InitMapInfo(
            /* [in] */ Int32 pid);

        void DeinitMapInfo();

        const char *Map2Name(
            /* [in] */ unsigned pc,
            /* [in] */ const char* def);

        const MapInfo *Pc2Mapinfo(
            /* [in] */ unsigned pc,
            /* [out] */ unsigned *rel_pc);

    private:
        MapInfo* ParseMapsLine(
            /* [in] */ char* line);
    };

    static CARAPI ProcessAllocEntry(
        /* [in] */ char* oneLine,
        /* [in] */ Int32 pid);

    static CARAPI Print(
        /* [in] */ List<String>* list,
        /* [in] */ Int32 pid);
};

}
}

#endif // __CMEMORYDUMPER_H__

