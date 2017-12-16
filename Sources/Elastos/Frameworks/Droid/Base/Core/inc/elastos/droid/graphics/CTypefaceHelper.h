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

#ifndef __ELASTOS_DROID_GRAPHICS_CTYPEFACEHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CTYPEFACEHELPER_H__

#include "_Elastos_Droid_Graphics_CTypefaceHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Core::Singleton;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CTypefaceHelper)
    , public Singleton
    , public ITypefaceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetDEFAULT(
        /* [out] */ ITypeface** typeface);

    CARAPI GetDEFAULT_BOLD(
        /* [out] */ ITypeface** typeface);

    CARAPI GetSANS_SERIF(
        /* [out] */ ITypeface** typeface);

    CARAPI GetSERIF(
        /* [out] */ ITypeface** typeface);

    CARAPI GetMONOSPACE(
        /* [out] */ ITypeface** typeface);

    /**
     * Create a typeface object given a family name, and option style information.
     * If null is passed for the name, then the "default" font will be chosen.
     * The resulting typeface object can be queried (getStyle()) to discover what
     * its "real" style characteristics are.
     *
     * @param familyName May be null. The name of the font family.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    CARAPI Create(
        /* [in] */ const String& familyName,
        /* [in] */ Int32 style,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a typeface object that best matches the specified existing
     * typeface and the specified Style. Use this call if you want to pick a new
     * style from the same family of an existing typeface object. If family is
     * null, this selects from the default font's family.
     *
     * @param family May be null. The name of the existing type face.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    CARAPI Create(
        /* [in] */ ITypeface* family,
        /* [in] */ Int32 style,
        /* [out] */ ITypeface** typeface);

    /**
     * Returns one of the default typeface objects, based on the specified style
     *
     * @return the default typeface that corresponds to the style
     */
    CARAPI DefaultFromStyle(
        /* [in] */ Int32 style,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a new typeface from the specified font data.
     * @param mgr The application's asset manager
     * @param path  The file name of the font data in the assets directory
     * @return The new typeface.
     */
    CARAPI CreateFromAsset(
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a new typeface from the specified font file.
     *
     * @param path The path to the font data.
     * @return The new typeface.
     */
    CARAPI CreateFromFile(
        /* [in] */ IFile* path,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a new typeface from the specified font file.
     *
     * @param path The full path to the font data.
     * @return The new typeface.
     */
    CARAPI CreateFromFile(
        /* [in]*/ const String& path,
        /* [out] */ ITypeface** typeface);

    CARAPI CreateFromFamilies(
        /* [in]*/ ArrayOf<IFontFamily*>* families,
        /* [out]*/ ITypeface** typeface);

    CARAPI CreateFromFamiliesWithDefault(
        /* [in] */ ArrayOf<IFontFamily*>* families,
        /* [in] */ Int32 weight,
        /* [in] */ Int32 italic,
        /* [out]*/ ITypeface** typeface);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CTYPEFACEHELPER_H__
