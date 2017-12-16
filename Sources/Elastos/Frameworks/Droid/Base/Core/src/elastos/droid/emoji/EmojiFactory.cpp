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

#include "elastos/droid/emoji/EmojiFactory.h"

namespace Elastos {
namespace Droid {
namespace Emoji {

//=======================================================
// mojiFactory::CustomLinkedHashMap
//=======================================================
EmojiFactory::CustomLinkedHashMap::CustomLinkedHashMap(
    /* [in] */ EmojiFactory* host)
    : mHost(host)
{
    LinkedHashMap::constructor(16, 0.75, TRUE);
}

Boolean EmojiFactory::CustomLinkedHashMap::RemoveEldestEntry(
    /* [in] */ IMapEntry* eldest)
{
    Int32 size;
    GetSize(&size);
    return size > mHost->mCacheSize;
}


//=======================================================
// mojiFactory
//=======================================================
CAR_INTERFACE_IMPL(EmojiFactory, Object, IEmojiFactory);

EmojiFactory::EmojiFactory()
    : mCacheSize(100)
    , mNativeEmojiFactory(0)
{}

EmojiFactory::~EmojiFactory()
{
}

ECode  EmojiFactory::constructor(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ const String& name)
{
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetBitmapFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetBitmapFromVendorSpecificSjis(
    /* [in] */ Char32 sjis,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetBitmapFromVendorSpecificPua(
    /* [in] */ Int32 vsp,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificSjis(
    /* [in] */ Char32 sjis,
    /* [out] */ Int32* pua)
{
    VALIDATE_NOT_NULL(pua);
    *pua = 0;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetVendorSpecificSjisFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ Int32* vsp)
{
    VALIDATE_NOT_NULL(vsp);
    *vsp = 0;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificPua(
    /* [in] */ Int32 vsp,
    /* [out] */ Int32* pua)
{
    VALIDATE_NOT_NULL(pua);
    *pua = 0;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificPua(
    /* [in] */ const String& vspString,
    /* [out] */ String* puaString)
{
    VALIDATE_NOT_NULL(puaString);
    *puaString = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetVendorSpecificPuaFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ Int32* vsp)
{
    VALIDATE_NOT_NULL(vsp);
    *vsp =  0;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetVendorSpecificPuaFromAndroidPua(
    /* [in] */ const String& puaString,
    /* [out] */ String* vspString)
{
    VALIDATE_NOT_NULL(vspString);
    *vspString = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::NewInstance(
    /* [in] */ const String& class_name,
    /* [out] */ IEmojiFactory** emojiFactory)
{
    VALIDATE_NOT_NULL(emojiFactory);
    *emojiFactory = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::NewAvailableInstance(
    /* [out] */ IEmojiFactory** emojiFactory)
{
    VALIDATE_NOT_NULL(emojiFactory);
    *emojiFactory = NULL;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetMinimumAndroidPua(
    /* [out] */ Int32* pua )
{
    VALIDATE_NOT_NULL(pua);
    *pua = 0;
    return E_NOT_SUPPORTED;
}

ECode EmojiFactory::GetMaximumAndroidPua(
    /* [out] */ Int32* pua)
{
    VALIDATE_NOT_NULL(pua);
    *pua = 0;
    return E_NOT_SUPPORTED;
}

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos
