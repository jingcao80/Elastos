
#ifndef __ELASTOS_DROID_SERVER_CSAMPLINGPROFILERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CSAMPLINGPROFILERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CSamplingProfilerService.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/FileObserver.h"

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::IDropBoxManager;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CSamplingProfilerService)
{
private:
    class SamplingProfilerSettingsObserver : public ContentObserver
    {
    public:
        SamplingProfilerSettingsObserver(
            /* [in] */ IContentResolver *contentResolver);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<IContentResolver> mContentResolver;
    };

    class MyFileObserver : public FileObserver
    {
    public:
        MyFileObserver(
            /* [in] */ const String& path,
            /* [in] */ Int32 mask,
            /* [in] */ CSamplingProfilerService* owner,
            /* [in] */ IDropBoxManager *dropbox);

        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);

    private:
        CSamplingProfilerService* mOwner;
        IDropBoxManager* mDropBox;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext *context);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor *fd,
        /* [in] */ IPrintWriter *pw,
        /* [in] */ ArrayOf<String> *args);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI StartWorking(
        /* [in] */ IContext *context);

    CARAPI HandleSnapshotFile(
        /* [in] */ IFile *file,
        /* [in] */ IDropBoxManager *dropbox);

    CARAPI RegisterSettingObserver(
        /* [in] */ IContext *context);

public:
    const static String SNAPSHOT_DIR;

private:
    const static String TAG;
    const static Boolean LOCAL_LOGV;

    AutoPtr<IContext> mContext;
    AutoPtr<IFileObserver> mSnapshotObserver;

};

}// Server
}// Droid
}// Elastos

#endif // __ELASTOS_DROID_SERVER_CSAMPLINGPROFILERSERVICE_H__
