
#ifndef __DATAMANAGER_H__
#define __DATAMANAGER_H__

#include "app/Activity.h"
#include <elastos/HashMap.h>
#include <elastos/Mutex.h>
#include "MediaSource.h"
#include "MediaObject.h"
#include "MediaSet.h"
#include "Path.h"

using Elastos::Core::Threading::Mutex;
using Elastos::Utility::HashMap;

namespace Gallery {

class DataManager
    : public ElRefBase
    , public IInterface
{
public:
    DataManager();

    DataManager(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL()

    CARAPI_(void) InitializeSourceMap();

    CARAPI_(String) GetTopSetPath(
        /* [in] */ Int32 typeBits);

    CARAPI_(AutoPtr<MediaObject>) PeekMediaObject(
        /* [in] */ Path* path);

    CARAPI_(AutoPtr<MediaObject>) GetMediaObject(
        /* [in] */ Path* path);

    CARAPI_(AutoPtr<MediaObject>) GetMediaObject(
        /* [in] */ const String& s);

    CARAPI_(AutoPtr<MediaSet>) GetMediaSet(
        /* [in] */ Path* path);

    CARAPI_(AutoPtr<MediaSet>) GetMediaSet(
        /* [in] */ const String& s);

    CARAPI_(AutoPtr< ArrayOf<MediaSet*> >) GetMediaSetsFromString(
        /* [in] */ const String& segment);

    CARAPI_(void) MapMediaItems(
        /* [in] */ ArrayOf<Path*>* list,
        /* [in] */ MediaSet::ItemConsumer* consumer,
        /* [in] */ Int32 startIndex);

    CARAPI_(Int32) GetSupportedOperations(
        /* [in] */ Path* path);

    CARAPI_(void) Delete(
        /* [in] */ Path* path);

    CARAPI_(void) Rotate(
        /* [in] */ Path* path,
        /* [in] */ Int32 degrees);

    CARAPI_(AutoPtr<IUri>) GetContentUri(
        /* [in] */ Path* path);

    CARAPI_(Int32) GetMediaType(
        /* [in] */ Path* path);

    CARAPI_(AutoPtr<Path>) FindPathByUri(
        /* [in] */ IUri* uri,
        /* [in] */ const String& type);

    CARAPI_(AutoPtr<Path>) GetDefaultSetOf(
        /* [in] */ Path* item);

    CARAPI_(Int64) GetTotalUsedCacheSize();

    CARAPI_(Int64) GetTotalTargetCacheSize();

    // void registerChangeNotifier(Uri uri, ChangeNotifier notifier);

    CARAPI_(void) Resume();

    CARAPI_(void) Pause();

    CARAPI_(AutoPtr<IContext>) GetContext();

    CARAPI_(void) SetContext(
        /* [in] */ IContext* context);

private:
    CARAPI_(void) AddSource(
        /* [in] */ MediaSource* source);

public:
    static const Int32 INCLUDE_IMAGE;
    static const Int32 INCLUDE_VIDEO;
    static const Int32 INCLUDE_ALL;
    static const Int32 INCLUDE_LOCAL_ONLY;
    static const Int32 INCLUDE_LOCAL_IMAGE_ONLY;
    static const Int32 INCLUDE_LOCAL_VIDEO_ONLY;
    static const Int32 INCLUDE_LOCAL_ALL_ONLY;

    static Mutex sLOCK;

private:
    static const String TAG;
    static const String TOP_SET_PATH;
    static const String TOP_IMAGE_SET_PATH;
    static const String TOP_VIDEO_SET_PATH;
    static const String TOP_LOCAL_SET_PATH;
    static const String TOP_LOCAL_IMAGE_SET_PATH;
    static const String TOP_LOCAL_VIDEO_SET_PATH;

private:
    Int32 mActiveCount;
    // final Handler mDefaultMainHandler;
    AutoPtr<IContext> mApplication;
    HashMap<String, AutoPtr<MediaSource> > mSourceMap;
};

} // namespace Gallery

#endif // __DATAMANAGER_H__
