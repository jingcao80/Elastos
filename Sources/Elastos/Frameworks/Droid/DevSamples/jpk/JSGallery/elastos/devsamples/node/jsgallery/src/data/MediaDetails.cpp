#include "MediaDetails.h"
#include <elastos/Logger.h>
#include <elastos/StringBuilder.h>
#include <elastos/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

Int32 MediaDetails::FlashState::FLASH_FIRED_MASK = 1;
Int32 MediaDetails::FlashState::FLASH_RETURN_MASK = 2 | 4;
Int32 MediaDetails::FlashState::FLASH_MODE_MASK = 8 | 16;
Int32 MediaDetails::FlashState::FLASH_FUNCTION_MASK = 32;
Int32 MediaDetails::FlashState::FLASH_RED_EYE_MASK = 64;

// const Int32 MediaDetails::INDEX_TITLE = 1;
// const Int32 MediaDetails::INDEX_DESCRIPTION = 2;
// const Int32 MediaDetails::INDEX_DATETIME = 3;
// const Int32 MediaDetails::INDEX_LOCATION = 4;
// const Int32 MediaDetails::INDEX_WIDTH = 5;
// const Int32 MediaDetails::INDEX_HEIGHT = 6;
// const Int32 MediaDetails::INDEX_ORIENTATION = 7;
// const Int32 MediaDetails::INDEX_DURATION = 8;
// const Int32 MediaDetails::INDEX_MIMETYPE = 9;
// const Int32 MediaDetails::INDEX_SIZE = 10;

// const Int32 MediaDetails::INDEX_MAKE = 100;
// const Int32 MediaDetails::INDEX_MODEL = 101;
// const Int32 MediaDetails::INDEX_FLASH = 102;
// const Int32 MediaDetails::INDEX_FOCAL_LENGTH = 103;
// const Int32 MediaDetails::INDEX_WHITE_BALANCE = 104;
// const Int32 MediaDetails::INDEX_APERTURE = 105;
// const Int32 MediaDetails::INDEX_SHUTTER_SPEED = 106;
// const Int32 MediaDetails::INDEX_EXPOSURE_TIME = 107;
// const Int32 MediaDetails::INDEX_ISO = 108;

// const Int32 MediaDetails::INDEX_PATH = 200;

const String MediaDetails::TAG("MediaDetails");

MediaDetails::FlashState::FlashState(
    /* [in] */ Int32 state)
    : mState(state)
{}

Boolean MediaDetails::FlashState::IsFlashFired()
{
    return (mState & FLASH_FIRED_MASK) != 0;
}

void MediaDetails::ExtractExifInfo(
    /* [in] */ MediaDetails* details,
    /* [in] */  const String& filePath)
{}

CAR_INTERFACE_IMPL(MediaDetails, IInterface)

void MediaDetails::AddDetail(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    mDetails.Insert(HashMap<Int32, AutoPtr<IInterface> >::ValueType(index, value));
}

AutoPtr<IInterface> MediaDetails::GetDetail(
    /* [in] */ Int32 index)
{
    HashMap<Int32, AutoPtr<IInterface> >::Iterator it = mDetails.Find(index);
    if (it != mDetails.End()) {
        return it->mSecond;
    }
    return NULL;
}

Int32 MediaDetails::Size()
{
    return mDetails.GetSize();
}

void MediaDetails::SetUnit(
    /* [in] */ Int32 index,
    /* [in] */ Int32 unit)
{
    mUnits.Insert(HashMap<Int32, Int32>::ValueType(index, unit));
}

Boolean MediaDetails::HasUnit(
    /* [in] */ Int32 index)
{
    HashMap<Int32, Int32 >::Iterator it = mUnits.Find(index);
    if (it != mUnits.End()) {
        return TRUE;
    }
    return FALSE;
}

Int32 MediaDetails::GetUnit(
    /* [in] */ Int32 index)
{
    HashMap<Int32, Int32 >::Iterator it = mUnits.Find(index);
    if (it != mUnits.End()) {
        return it->mSecond;
    }
    return 0;
}

void MediaDetails::SetExifData(
    /* [in] */ MediaDetails* details,
    /* [in] */ IExifInterface* exif,
    /* [in] */  const String& tag,
    /* [in] */ Int32 key)
{}

} // namespace Gallery
