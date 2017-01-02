//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_EMOJI_EMOJIFACTORY_H__
#define __ELASTOS_DROID_EMOJI_EMOJIFACTORY_H__

#include "Elastos.Droid.Emoji.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>
#include <elastos/utility/LinkedHashMap.h>
// #include <Elastos.CoreLibrary.Utility.h>
// #include "elastos/core/Object.h"
//
// #include <elastos/utility/etl/HashMap.h>
// #include <elastos/utility/etl/Map.h>
// #include "_Elastos_Droid_Emoji_CEmojiFactory.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::LinkedHashMap;

namespace Elastos {
namespace Droid {
namespace Emoji {

/**
 * A class for the factories which produce Emoji (pictgram) images.
 * This is intended to be used by IME, Email app, etc.
 * There's no plan to make this public for now.
 * @hide
 */
class EmojiFactory
    : public Object
    , public IEmojiFactory
{
private:
    class CustomLinkedHashMap
        : public LinkedHashMap
    {
    public:
        CustomLinkedHashMap(
            /* [in] */ EmojiFactory* host);

    protected:
        CARAPI_(Boolean) RemoveEldestEntry(
            /* [in] */ IMapEntry* eldest);

    private:
        EmojiFactory* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    EmojiFactory();

    virtual ~EmojiFactory();

    CARAPI constructor(
        /* [in] */ Int64 nativeEmojiFactory,
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns Bitmap object corresponding to the AndroidPua.
     *
     * Note that each Bitmap is cached by this class, which means that, if you modify a
     * Bitmap object (using setPos() method), all same emoji Bitmap will be modified.
     * If it is unacceptable, please copy the object before modifying it.
     *
     * @param pua A unicode codepoint.
     * @return Bitmap object when this factory knows the Bitmap relevant to the codepoint.
     * Otherwise null is returned.
     */
    CARAPI GetBitmapFromAndroidPua(
        /* [in] */ Int32 pua,
        /* [out] */ IBitmap** bitmap);

   /**
    * Returns Bitmap object corresponding to the vendor specified sjis.
    *
    * See comments in getBitmapFromAndroidPua().
    *
    * @param sjis sjis code specific to each career(vendor)
    * @return Bitmap object when this factory knows the Bitmap relevant to the code. Otherwise
    * null is returned.
    */
    CARAPI GetBitmapFromVendorSpecificSjis(
        /* [in] */ Char32 sjis,
        /* [out] */ IBitmap** bitmap);

   /**
    * Returns Bitmap object corresponding to the vendor specific Unicode.
    *
    * See comments in getBitmapFromAndroidPua().
    *
    * @param vsp vendor specific PUA.
    * @return Bitmap object when this factory knows the Bitmap relevant to the code. Otherwise
    * null is returned.
    */
    CARAPI GetBitmapFromVendorSpecificPua(
        /* [in] */ Int32 vsp,
        /* [out] */ IBitmap** bitmap);

   /**
    * Returns Unicode PUA for Android corresponding to the vendor specific sjis.
    *
    * @param sjis vendor specific sjis
    * @return Unicode PUA for Android, or -1 if there's no map for the sjis.
    */
    CARAPI GetAndroidPuaFromVendorSpecificSjis(
        /* [in] */ Char32 sjis,
        /* [out] */ Int32* pua);

   /**
    * Returns vendor specific sjis corresponding to the Unicode AndroidPua.
    *
    * @param pua Unicode PUA for Android,
    * @return vendor specific sjis, or -1 if there's no map for the AndroidPua.
    */
    CARAPI GetVendorSpecificSjisFromAndroidPua(
        /* [in] */ Int32 pua,
        /* [out] */ Int32* sjis);

   /**
    * Returns Unicode PUA for Android corresponding to the vendor specific Unicode.
    *
    * @param vsp vendor specific PUA.
    * @return Unicode PUA for Android, or -1 if there's no map for the
    * Unicode.
    */
    CARAPI GetAndroidPuaFromVendorSpecificPua(
        /* [in] */ Int32 vsp,
        /* [out] */ Int32* pua);

    CARAPI GetAndroidPuaFromVendorSpecificPua(
        /* [in] */ const String& vspString,
        /* [out] */ String* pua);

    /**
     * Returns vendor specific Unicode corresponding to the Unicode AndroidPua.
     *
     * @param pua Unicode PUA for Android,
     * @return vendor specific sjis, or -1 if there's no map for the AndroidPua.
     */
    CARAPI GetVendorSpecificPuaFromAndroidPua(
        /* [in] */ Int32 pua,
        /* [out] */ Int32* vsp);

    CARAPI GetVendorSpecificPuaFromAndroidPua(
        /* [in] */ const String& puaString,
        /* [out] */ String* vspString);

    /**
     * Constructs an instance of EmojiFactory corresponding to the name.
     *
     * @param class_name Name of the factory. This must include complete package name.
     * @return A concrete EmojiFactory instance corresponding to factory_name.
     * If factory_name is invalid, null is returned.
     */
    static CARAPI NewInstance(
        /* [in] */ const String& class_name,
        /* [out] */ IEmojiFactory** EmojiFactory);

    /**
     * Constructs an instance of available EmojiFactory.
     *
     * @return A concrete EmojiFactory instance. If there are several available
     * EmojiFactory class, preferred one is chosen by the system. If there isn't, null
     * is returned.
     */
    static CARAPI NewAvailableInstance(
        /* [out] */ IEmojiFactory** EmojiFactory);

    /**
     * Returns the lowest code point corresponding to an Android
     * emoji character.
     */
    CARAPI GetMinimumAndroidPua(
        /* [out] */ Int32* pua);

    /**
     * Returns the highest code point corresponding to an Android
     * emoji character.
     */
    CARAPI GetMaximumAndroidPua(
        /* [out] */ Int32* RetValue);

private:
    CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 nativeEmojiFactory);

    static CARAPI_(AutoPtr<IBitmap>) NativeGetBitmapFromAndroidPua(
       /* [in] */ Int64 nativeEmojiFactory,
       /* [in] */ Int32 AndroidPua);

    static CARAPI_(Int32) NativeGetAndroidPuaFromVendorSpecificSjis(
       /* [in] */ Int64 nativeEmojiFactory,
       /* [in] */ Char32 sjis);

    static CARAPI_(Int32) NativeGetVendorSpecificSjisFromAndroidPua(
       /* [in] */ Int64 nativeEmojiFactory,
       /* [in] */ Int32 pua);

    static CARAPI_(Int32) NativeGetAndroidPuaFromVendorSpecificPua(
       /* [in] */ Int64 nativeEmojiFactory,
       /* [in] */ Int32 vsp);

    static CARAPI_(Int32) NativeGetVendorSpecificPuaFromAndroidPua(
       /* [in] */ Int64 nativeEmojiFactory,
       /* [in] */ Int32 pua);

    static CARAPI_(Int32) NativeGetMaximumVendorSpecificPua(
       /* [in] */ Int64 nativeEmojiFactory);

    static CARAPI_(Int32) NativeGetMinimumVendorSpecificPua(
       /* [in] */ Int64 nativeEmojiFactory);

    static CARAPI_(Int32) NativeGetMaximumAndroidPua(
       /* [in] */ Int64 nativeEmojiFactory);

    static CARAPI_(Int32) NativeGetMinimumAndroidPua(
        /* [in] */ Int64 nativeEmojiFactory);

//     /*
//      // Returns binary image data corresponding to "pua". The size of binary is
//      // stored to "size". Returns NULL if there's no mapping from the "pua" to a
//      // specific image. Currently, image format is all (animated) gif.
//      //
//      // TODO(dmiyakawa): there should be a way to tell users the format of the
//      // binary.
//      virtual const char *GetImageBinaryFromAndroidPua(int pua, int *size) = 0;
//    */
//    static char *GetImageBinaryFromAndroidPua(Int32 pua, Int32 *size);

private:
    Int32 mCacheSize;
    Int64 mNativeEmojiFactory;
    String mName;
    // Cache.
    AutoPtr<IHashMap> mCache;
};

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_EMOJI_EMOJIFACTORY_H__
