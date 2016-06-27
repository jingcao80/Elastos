
#ifndef __MEDIADETAILS_H__
#define __MEDIADETAILS_H__

#include "app/Activity.h"
#include <elastos/HashMap.h>

using Elastos::Droid::Media::IExifInterface;
using Elastos::Utility::HashMap;

namespace Gallery {

class MediaDetails
    : public ElRefBase
    , public IInterface
{
    class FlashState
        : public ElRefBase
    {
    public:
        FlashState(
            /* [in] */ Int32 state);

        CARAPI_(Boolean) IsFlashFired();

    private:
        static Int32 FLASH_FIRED_MASK;
        static Int32 FLASH_RETURN_MASK;
        static Int32 FLASH_MODE_MASK;
        static Int32 FLASH_FUNCTION_MASK;
        static Int32 FLASH_RED_EYE_MASK;

    private:
        Int32 mState;
    };

public:
    static CARAPI_(void) ExtractExifInfo(
        /* [in] */ MediaDetails* details,
        /* [in] */ const String& filePath);

public:
    CAR_INTERFACE_DECL()

    CARAPI_(void) AddDetail(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value);

    CARAPI_(AutoPtr<IInterface>) GetDetail(
        /* [in] */ Int32 index);

    CARAPI_(Int32) Size();

    CARAPI_(void) SetUnit(
        /* [in] */ Int32 index,
        /* [in] */ Int32 unit);

    CARAPI_(Boolean) HasUnit(
        /* [in] */ Int32 index);

    CARAPI_(Int32) GetUnit(
        /* [in] */ Int32 index);

private:
    CARAPI_(void) SetExifData(
        /* [in] */ MediaDetails* details,
        /* [in] */ IExifInterface* exif,
        /* [in] */ const String& tag,
        /* [in] */ Int32 key);

public:
    static const Int32 INDEX_TITLE = 1;
    static const Int32 INDEX_DESCRIPTION = 2;
    static const Int32 INDEX_DATETIME = 3;
    static const Int32 INDEX_LOCATION = 4;
    static const Int32 INDEX_WIDTH = 5;
    static const Int32 INDEX_HEIGHT = 6;
    static const Int32 INDEX_ORIENTATION = 7;
    static const Int32 INDEX_DURATION = 8;
    static const Int32 INDEX_MIMETYPE = 9;
    static const Int32 INDEX_SIZE = 10;

    // for EXIF
    static const Int32 INDEX_MAKE = 100;
    static const Int32 INDEX_MODEL = 101;
    static const Int32 INDEX_FLASH = 102;
    static const Int32 INDEX_FOCAL_LENGTH = 103;
    static const Int32 INDEX_WHITE_BALANCE = 104;
    static const Int32 INDEX_APERTURE = 105;
    static const Int32 INDEX_SHUTTER_SPEED = 106;
    static const Int32 INDEX_EXPOSURE_TIME = 107;
    static const Int32 INDEX_ISO = 108;

    // Put this last because it may be long.
    static const Int32 INDEX_PATH = 200;

    static const String TAG;

private:
    // TreeMap<Integer, Object> mDetails;
    HashMap<Int32, AutoPtr<IInterface> > mDetails;
    HashMap<Int32, Int32> mUnits;

};

} // namespace Gallery

#endif // __MEDIADETAILS_H__
