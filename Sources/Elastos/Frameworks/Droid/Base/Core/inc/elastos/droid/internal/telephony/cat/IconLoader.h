#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_ICONLOADER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_ICONLOADER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/os/Handler.h"

// import android.graphics.Color;
// import android.os.AsyncResult;
// import android.os.HandlerThread;
// import java.util.HashMap;

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for loading icons from the SIM card. Has two states: single, for loading
  * one icon. Multi, for loading icons list.
  *
  */
class IconLoader
    : public Handler
{
public:
    static CARAPI_(AutoPtr<IconLoader>) GetInstance(
        /* [in] */ IHandler* caller,
        /* [in] */ IIccFileHandler* fh);

    virtual CARAPI LoadIcons(
        /* [in] */ ArrayOf<Int32>* recordNumbers,
        /* [in] */ IMessage* msg);

    virtual CARAPI LoadIcon(
        /* [in] */ Int32 recordNumber,
        /* [in] */ IMessage* msg);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
      * Convert a TS 131.102 image instance of code scheme '11' into Bitmap
      * @param data The raw data
      * @param length The length of image body
      * @return The bitmap
      */
    static CARAPI_(AutoPtr<IBitmap>) ParseToBnW(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 length);

    /**
      * a TS 131.102 image instance of code scheme '11' into color Bitmap
      *
      * @param data The raw data
      * @param length the length of image body
      * @param transparency with or without transparency
      * @param clut coulor lookup table
      * @return The color bitmap
      */
    static CARAPI_(AutoPtr<IBitmap>) ParseToRGB(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 length,
        /* [in] */ Boolean transparency,
        /* [in] */ ArrayOf<Byte>* clut);

    virtual CARAPI Dispose();

private:
    IconLoader(
        /* [in] */ ILooper* looper,
        /* [in] */ IIccFileHandler* fh);

    CARAPI_(void) StartLoadingIcon(
        /* [in] */ Int32 recordNumber);

    /**
      * Handles Image descriptor parsing and required processing. This is the
      * first step required to handle retrieving icons from the SIM.
      *
      * @param rawData byte [] containing Image Instance descriptor as defined in
      * TS 51.011.
      */
    CARAPI_(Boolean) HandleImageDescriptor(
        /* [in] */ ArrayOf<Byte>* rawData);

    // Start reading color lookup table from SIM card.
    CARAPI_(void) ReadClut();

    // Start reading Image Descriptor from SIM card.
    CARAPI_(void) ReadId();

    // Start reading icon bytes array from SIM card.
    CARAPI_(void) ReadIconData();

    // When all is done pass icon back to caller.
    CARAPI_(void) PostIcon();

    /**
      * Decode one bit to a black and white color:
      * 0 is black
      * 1 is white
      * @param bit to decode
      * @return RGB color
      */
    static CARAPI_(Int32) BitToBnW(
        /* [in] */ Int32 bit);

    /**
      * Calculate bit mask for a given number of bits. The mask should enable to
      * make a bitwise and to the given number of bits.
      * @param numOfBits number of bits to calculate mask for.
      * @return bit mask
      */
    static CARAPI_(Int32) GetMask(
        /* [in] */ Int32 numOfBits);

private:
    // members
    Int32 mState;
    AutoPtr<IImageDescriptor> mId;
    AutoPtr<IBitmap> mCurrentIcon;
    Int32 mRecordNumber;
    AutoPtr<IIccFileHandler> mSimFH;
    AutoPtr<IMessage> mEndMsg;
    AutoPtr<ArrayOf<Byte> > mIconData;
    // multi icons state members
    AutoPtr<ArrayOf<Int32> > mRecordNumbers;
    Int32 mCurrentRecordIndex;
    AutoPtr<ArrayOf<IBitmap*> > mIcons;
    AutoPtr<IHashMap/*<Integer, IBitmap>*/> mIconsCache;
    static AutoPtr<IconLoader> sLoader;
    static AutoPtr<IHandlerThread> sThread;
    // Loader state values.
    static const Int32 STATE_SINGLE_ICON = 1;
    static const Int32 STATE_MULTI_ICONS = 2;
    // Finished loading single record from a linear-fixed EF-IMG.
    static const Int32 EVENT_READ_EF_IMG_RECOED_DONE = 1;
    // Finished loading single icon from a Transparent DF-Graphics.
    static const Int32 EVENT_READ_ICON_DONE = 2;
    // Finished loading single colour icon lookup table.
    static const Int32 EVENT_READ_CLUT_DONE = 3;
    // Color lookup table offset inside the EF.
    static const Int32 CLUT_LOCATION_OFFSET = 4;
    // CLUT entry size, {Red, Green, Black}
    static const Int32 CLUT_ENTRY_SIZE = 3;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_ICONLOADER_H__
