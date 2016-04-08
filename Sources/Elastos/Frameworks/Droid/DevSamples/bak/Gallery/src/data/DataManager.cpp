#include "DataManager.h"
#include "LocalSource.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const Int32 DataManager::INCLUDE_IMAGE = 1;
const Int32 DataManager::INCLUDE_VIDEO = 2;
const Int32 DataManager::INCLUDE_ALL = INCLUDE_IMAGE | INCLUDE_VIDEO;
const Int32 DataManager::INCLUDE_LOCAL_ONLY = 4;
const Int32 DataManager::INCLUDE_LOCAL_IMAGE_ONLY = INCLUDE_LOCAL_ONLY | INCLUDE_IMAGE;
const Int32 DataManager::INCLUDE_LOCAL_VIDEO_ONLY = INCLUDE_LOCAL_ONLY | INCLUDE_VIDEO;
const Int32 DataManager::INCLUDE_LOCAL_ALL_ONLY = INCLUDE_LOCAL_ONLY | INCLUDE_IMAGE | INCLUDE_VIDEO;
Mutex DataManager::sLOCK;
const String DataManager::TAG("DataManager");
const String DataManager::TOP_SET_PATH = String("/combo/{/local/all}");
const String DataManager::TOP_IMAGE_SET_PATH = String("/combo/{/local/image}");
const String DataManager::TOP_VIDEO_SET_PATH = String("/combo/{/local/video}");
const String DataManager::TOP_LOCAL_SET_PATH = String("/local/all");
const String DataManager::TOP_LOCAL_IMAGE_SET_PATH = String("/local/image");
const String DataManager::TOP_LOCAL_VIDEO_SET_PATH = String("/local/video");

DataManager::DataManager()
    : mActiveCount(0)
{}

DataManager::DataManager(
    /* [in] */ IContext* context)
    : mActiveCount(0)
    , mApplication(context)
{}

CAR_INTERFACE_IMPL(DataManager, IInterface)

void DataManager::InitializeSourceMap()
{
    AutoLock lock(sLOCK);
    if (!mSourceMap.IsEmpty()) return;
    AutoPtr<LocalSource> localSource = new LocalSource(mApplication);
    AddSource(localSource);
    if (mActiveCount > 0) {
        AutoPtr<MediaSource> source;
        HashMap<String, AutoPtr<MediaSource> >::Iterator it = mSourceMap.Begin();
        for (; it != mSourceMap.End(); ++it) {
            source = it->mSecond;
            source->Resume();
        }
    }
}

String DataManager::GetTopSetPath(
    /* [in] */ Int32 typeBits)
{
    Logger::D(TAG, "GetTopSetPath--typeBits:%d", typeBits);
    switch (typeBits) {
        case INCLUDE_IMAGE: return TOP_IMAGE_SET_PATH;
        case INCLUDE_VIDEO: return TOP_VIDEO_SET_PATH;
        case INCLUDE_ALL: return TOP_SET_PATH;
        case INCLUDE_LOCAL_IMAGE_ONLY: return TOP_LOCAL_IMAGE_SET_PATH;
        case INCLUDE_LOCAL_VIDEO_ONLY: return TOP_LOCAL_VIDEO_SET_PATH;
        case INCLUDE_LOCAL_ALL_ONLY: return TOP_LOCAL_SET_PATH;
        default: return String(NULL); //throw new IllegalArgumentException();
    }
}

AutoPtr<MediaObject> DataManager::PeekMediaObject(
    /* [in] */ Path* path)
{
    return path->GetObject();
}

AutoPtr<MediaObject> DataManager::GetMediaObject(
    /* [in] */ Path* path)
{
    return NULL;
}

AutoPtr<MediaObject> DataManager::GetMediaObject(
    /* [in] */ const String& s)
{
    return NULL;
}

AutoPtr<MediaSet> DataManager::GetMediaSet(
    /* [in] */ Path* path)
{
    return NULL;
}

AutoPtr<MediaSet> DataManager::GetMediaSet(
    /* [in] */ const String& s)
{
    return NULL;
}

AutoPtr< ArrayOf<MediaSet*> > DataManager::GetMediaSetsFromString(
    /* [in] */ const String& segment)
{
    return NULL;
}

void DataManager::MapMediaItems(
    /* [in] */ ArrayOf<Path*>* list,
    /* [in] */ MediaSet::ItemConsumer* consumer,
    /* [in] */ Int32 startIndex)
{}

Int32 DataManager::GetSupportedOperations(
    /* [in] */ Path* path)
{
    return 0;
}

void DataManager::Delete(
    /* [in] */ Path* path)
{}

void DataManager::Rotate(
    /* [in] */ Path* path,
    /* [in] */ Int32 degrees)
{}

AutoPtr<IUri> DataManager::GetContentUri(
    /* [in] */ Path* path)
{
    return NULL;
}

Int32 DataManager::GetMediaType(
    /* [in] */ Path* path)
{
    return 0;
}

AutoPtr<Path> DataManager::FindPathByUri(
    /* [in] */ IUri* uri,
    /* [in] */ const String& type)
{
    return NULL;
}

AutoPtr<Path> DataManager::GetDefaultSetOf(
    /* [in] */ Path* item)
{
    return NULL;
}

Int64 DataManager::GetTotalUsedCacheSize()
{
    return 0;
}

Int64 DataManager::GetTotalTargetCacheSize()
{
    return 0;
}

void DataManager::Resume()
{}

void DataManager::Pause()
{}

AutoPtr<IContext> DataManager::GetContext()
{
    return mApplication;
}

void DataManager::SetContext(
    /* [in] */ IContext* context)
{
    mApplication = context;
}

void DataManager::AddSource(
    /* [in] */ MediaSource* source)
{
    if (source == NULL) return;
    String prefix = source->GetPrefix();
    mSourceMap.Insert(HashMap<String, AutoPtr<MediaSource> >::ValueType(prefix, source));
}

} // namespace Gallery
