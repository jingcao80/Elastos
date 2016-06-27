#include "LocalMediaItem.h"
#include <elastos/Logger.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Core::CDouble;
using Elastos::Core::IDouble;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger64;
using Elastos::Core::CStringWrapper;
using Elastos::Text::IDateFormat;
using Elastos::Text::CDateFormatHelper;
using Elastos::Text::IDateFormatHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

// d8a4a15b-7400-4b1b-b522-c88f60a9579a
extern "C" const InterfaceID EIID_LocalMediaItem =
    { 0xd8a4a15b, 0x7400, 0x4b1b, { 0xb5, 0x22, 0xc8, 0x8f, 0x60, 0xa9, 0x57, 0x9a } };

const String LocalMediaItem::TAG("LocalMediaItem");

LocalMediaItem::LocalMediaItem(
    /* [in] */ Path* path,
    /* [in] */ Int64 version)
    : MediaItem(path, version)
    , id(0)
    , bucketId(0)
    , width(0)
    , height(0)
    , fileSize(0)
    , dateTakenInMs(0)
    , dateAddedInSec(0)
    , dateModifiedInSec(0)
    , latitude(INVALID_LATLNG)
    , longitude(INVALID_LATLNG)
{}

Int64 LocalMediaItem::GetDateInMs()
{
    return dateTakenInMs;
}

String LocalMediaItem::GetName()
{
    return caption;
}

void LocalMediaItem::GetLatLong(
    /* [in] */ ArrayOf<Double>* latLong)
{
    (*latLong)[0] = latitude;
    (*latLong)[1] = longitude;
}

Int32 LocalMediaItem::GetBucketId()
{
    return bucketId;
}

AutoPtr<MediaDetails> LocalMediaItem::GetDetails()
{
    AutoPtr<MediaDetails> details = MediaObject::GetDetails();

    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(filePath, (ICharSequence**)&cs);
    details->AddDetail(MediaDetails::INDEX_PATH, cs);

    cs = NULL;
    CStringWrapper::New(caption, (ICharSequence**)&cs);
    details->AddDetail(MediaDetails::INDEX_TITLE, cs);

    AutoPtr<IDateFormatHelper> formatHelper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&formatHelper);
    AutoPtr<IDateFormat> formater;
    formatHelper->GetDateTimeInstance((IDateFormat**)&formater);
    AutoPtr<IDate> date;
    CDate::New(dateModifiedInSec * 1000, (IDate**)&date);
    String dataStr;
    formater->FormatDate(date, &dataStr);
    cs = NULL;
    CStringWrapper::New(dataStr, (ICharSequence**)&cs);
    details->AddDetail(MediaDetails::INDEX_DATETIME, cs);

    AutoPtr<IInteger32> w;
    AutoPtr<IInteger32> h;
    CInteger32::New(width, (IInteger32**)&w);
    CInteger32::New(height, (IInteger32**)&h);
    details->AddDetail(MediaDetails::INDEX_WIDTH, w);
    details->AddDetail(MediaDetails::INDEX_HEIGHT, h);

    // if (GalleryUtils.isValidLocation(latitude, longitude)) {
        AutoPtr<IDouble> lat;
        AutoPtr<IDouble> lon;
        CDouble::New(latitude, (IDouble**)&lat);
        CDouble::New(longitude, (IDouble**)&lon);
        AutoPtr<IArrayOf> array;
        CArrayOf::New(EIID_IDouble, 2, (IArrayOf**)&array);
        array->Put(0, lat);
        array->Put(1, lon);
        details->AddDetail(MediaDetails::INDEX_LOCATION, array);
    // }
    if (fileSize > 0) {
        AutoPtr<IInteger64> fs;
        CInteger64::New(fileSize, (IInteger64**)&fs);
        details->AddDetail(MediaDetails::INDEX_SIZE, fs);
    }

    return details;
}

Int64 LocalMediaItem::GetSize()
{
    return fileSize;
}

String LocalMediaItem::GetMimeType()
{
    return mimeType;
}

void LocalMediaItem::UpdateContent(
    /* [in] */ ICursor* cursor)
{
    // if (UpdateFromCursor(cursor)) {
    //     mDataVersion = NextVersionNumber();
    // }
}

} // namespace Gallery
