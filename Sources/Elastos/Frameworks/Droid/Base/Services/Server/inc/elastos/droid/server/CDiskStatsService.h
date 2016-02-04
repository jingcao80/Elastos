#ifndef __ELASTOS_DROID_SERVER_CDISKSTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_CDISKSTATSSERVICE_H__

#include "_Elastos_Droid_Server_CDiskStatsService.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IContext;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * This service exists only as a "dumpsys" target which reports
 * statistics about the status of the disk.
 */
CarClass(CDiskStatsService)
    , public Object
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) ReportFreeSpace(
        /* [in] */ IFile* path,
        /* [in] */ const String& name,
        /* [in] */ IPrintWriter* pw);

private:
    const static String TAG;
    AutoPtr<IContext> mContext;

};

}//Server
}//Droid
}//Elastos

#endif //__ELASTOS_DROID_SERVER_CDISKSTATSSERVICE_H__
