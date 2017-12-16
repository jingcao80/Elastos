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

#include "elastos/droid/graphics/FontListConverter.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Graphics {

FontListConverter::StaticInitializer::StaticInitializer()
{
    WEIGHTS = ArrayOf<Int32>::Alloc(4);
    (*WEIGHTS)[0] = 400;
    (*WEIGHTS)[1] = 700;
    (*WEIGHTS)[2] = 400;
    (*WEIGHTS)[3] = 700;

    ITALICS = ArrayOf<Boolean>::Alloc(4);
    (*ITALICS)[0] = FALSE;
    (*ITALICS)[1] = FALSE;
    (*ITALICS)[2] = TRUE;
    (*ITALICS)[3] = TRUE;

    DEFAULT_WEIGHT = (*WEIGHTS)[0];

    STYLES = ArrayOf<String>::Alloc(4);
    (*STYLES)[0] = "thin";
    (*STYLES)[1] = "light";
    (*STYLES)[2] = "medium";
    (*STYLES)[3] = "black";
}

AutoPtr< ArrayOf<Int32> > FontListConverter::WEIGHTS;
AutoPtr< ArrayOf<Boolean> > FontListConverter::ITALICS;
Int32 FontListConverter::DEFAULT_WEIGHT;
AutoPtr< ArrayOf<String> > FontListConverter::STYLES;
const FontListConverter::StaticInitializer FontListConverter::sInitializer;

FontListConverter::FontListConverter(
    /* [in] */ IList* families,
    /* [in] */ const String& fontDir)
{
    CArrayList::New((IList**)&mFamilies);
    mFamilies->AddAll(ICollection::Probe(families));
    mFontDir = fontDir;
    FindFamilyVariants();
}

FontListConverter::FontListConverter(
    /* [in] */ LegacyFontListParser::Family* family,
    /* [in] */ const String& fontDir)
{
    CArrayList::New((IList**)&mFamilies);
    mFamilies->Add((IObject*)family);
    mFontDir = fontDir;
    FindFamilyVariants();
}

void FontListConverter::FindFamilyVariants()
{
    Int32 N;
    mFamilies->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mFamilies->Get(i, (IInterface**)&obj);
        LegacyFontListParser::Family* family = (LegacyFontListParser::Family*)IObject::Probe(obj);
        if (IsNormalStyle(family)) {
            AutoPtr<IList> variants = FindVariants(family, mFamilies);
            mFamilyVariants[family] = variants;
        }
    }
}

AutoPtr<IList> FontListConverter::FindVariants(
    /* [in] */ LegacyFontListParser::Family* normalFamily,
    /* [in] */ IList* legacyFamilies)
{
    AutoPtr<IList> variants;
    CArrayList::New((IList**)&variants);

    String normalFamilyName = normalFamily->GetName();

    Int32 N;
    legacyFamilies->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        legacyFamilies->Get(i, (IInterface**)&obj);
        LegacyFontListParser::Family* family = (LegacyFontListParser::Family*)IObject::Probe(obj);
        String name = family->GetName();

        if (name.StartWith(normalFamilyName) && !IsNormalStyle(family)) {
            variants->Add((IObject*)family);
        }
    }
    return variants;
}

AutoPtr<FontListParser::Config> FontListConverter::Convert()
{
    AutoPtr<FontListParser::Config> config = new FontListParser::Config();
    config->mFamilies->AddAll(ICollection::Probe(ConvertFamilies()));
    config->mAliases->AddAll(ICollection::Probe(CreateAliases()));
    return config;
}

Boolean FontListConverter::IsNormalStyle(
    /* [in] */ LegacyFontListParser::Family* family)
{
    String name = family->GetName();
    if (name.IsNull()) return FALSE;

    for (Int32 i = 0; i < STYLES->GetLength(); i++) {
        String style = (*STYLES)[i];
        if (name.EndWith(String("-") + style)) {
            return FALSE;
        }
    }
    return TRUE;
}

AutoPtr<IList> FontListConverter::ConvertFamilies()
{
    AutoPtr<IList> convertedFamilies;
    CArrayList::New((IList**)&convertedFamilies);

    // Only convert normal families. Each normal family will add in its variants
    HashMap< AutoPtr<LegacyFontListParser::Family>, AutoPtr<IList> >::Iterator it;
    for (it = mFamilyVariants.Begin(); it != mFamilyVariants.End(); ++it) {
        AutoPtr<LegacyFontListParser::Family> family = it->mFirst;
        convertedFamilies->Add((IObject*)ConvertFamily(family));
    }

    return convertedFamilies;
}

AutoPtr<FontListParser::Family> FontListConverter::ConvertFamily(
    /* [in] */ LegacyFontListParser::Family* legacyFamily)
{
    AutoPtr<IList> nameset = legacyFamily->mNameset;
    AutoPtr<IList> fileset = legacyFamily->mFileset;

    // Arbitrarily choose the first entry in the nameset to be the name
    String name(NULL);
    Boolean isEmpty;
    if (nameset->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<IInterface> obj;
        nameset->Get(0, (IInterface**)&obj);
        name = CoreUtils::Unbox(ICharSequence::Probe(obj));
    }

    AutoPtr<IList> fonts = ConvertFonts(fileset);

    // Add fonts from other variants
    AutoPtr<IList> families;
    HashMap< AutoPtr<LegacyFontListParser::Family>, AutoPtr<IList> >::Iterator it = mFamilyVariants.Find(legacyFamily);
    if (it != mFamilyVariants.End()) families = it->mSecond;
    if (families != NULL) {
        Int32 N;
        families->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            families->Get(i, (IInterface**)&obj);
            LegacyFontListParser::Family* variantFamily = (LegacyFontListParser::Family*)IObject::Probe(obj);
            fonts->AddAll(ICollection::Probe(ConvertFonts(variantFamily->mFileset)));
        }
    }

    return new FontListParser::Family(name, fonts, String(NULL), String(NULL));
}

AutoPtr<IList> FontListConverter::ConvertFonts(
    /* [in] */ IList* fileset)
{
    AutoPtr<IList> fonts;
    CArrayList::New((IList**)&fonts);

    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    Char32 separatorChar;
    helper->GetSeparatorChar(&separatorChar);
    String prefix(mFontDir);
    prefix.Append(separatorChar);

    Int32 N;
    fileset->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        fileset->Get(i, (IInterface**)&obj);
        String file = CoreUtils::Unbox(ICharSequence::Probe(obj));
        String fullpath = prefix + file;
        // fileset should only be 4 entries, but if
        // its more we can just assign a default.
        Int32 weight = i < WEIGHTS->GetLength() ? (*WEIGHTS)[i] : DEFAULT_WEIGHT;
        Boolean isItalic = i < ITALICS->GetLength() ? (*ITALICS)[i] : FALSE;

        AutoPtr<FontListParser::Font> font = new FontListParser::Font(fullpath, 0, NULL, weight, isItalic);
        fonts->Add((IObject*)font);
    }

    return fonts;
}

AutoPtr<IList> FontListConverter::CreateAliases()
{
    AutoPtr<IList> aliases;
    CArrayList::New((IList**)&aliases);

    HashMap< AutoPtr<LegacyFontListParser::Family>, AutoPtr<IList> >::Iterator it;
    for (it = mFamilyVariants.Begin(); it != mFamilyVariants.End(); ++it) {
        AutoPtr<LegacyFontListParser::Family> family = it->mFirst;
        // Get any aliases that might be from a normal family's nameset.
        // eg sans-serif, arial, helvetica, tahoma etc.
        if (IsNormalStyle(family)) {
            aliases->AddAll(ICollection::Probe(AdaptNamesetAliases(family->mNameset)));
        }
    }

    aliases->AddAll(ICollection::Probe(GetAliasesForRelatedFamilies()));

    return aliases;
}

AutoPtr<IList> FontListConverter::GetAliasesForRelatedFamilies()
{
    AutoPtr<IList> aliases;
    CArrayList::New((IList**)&aliases);

    HashMap< AutoPtr<LegacyFontListParser::Family>, AutoPtr<IList> >::Iterator it;
    for (it = mFamilyVariants.Begin(); it != mFamilyVariants.End(); ++it) {
        AutoPtr<IInterface> obj;
        it->mFirst->mNameset->Get(0, (IInterface**)&obj);
        String toName = CoreUtils::Unbox(ICharSequence::Probe(obj));
        AutoPtr<IList> relatedFamilies = it->mSecond;
        Int32 N;
        relatedFamilies->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            relatedFamilies->Get(i, (IInterface**)&obj);
            LegacyFontListParser::Family* relatedFamily = (LegacyFontListParser::Family*)IObject::Probe(obj);
            aliases->AddAll(ICollection::Probe(AdaptNamesetAliases(relatedFamily->mNameset, toName)));
        }
    }
    return aliases;
}

AutoPtr<IList> FontListConverter::AdaptNamesetAliases(
    /* [in] */ IList* nameset,
    /* [in] */ const String& toName)
{
    AutoPtr<IList> aliases;
    CArrayList::New((IList**)&aliases);
    Int32 N;
    nameset->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        nameset->Get(i, (IInterface**)&obj);
        String name = CoreUtils::Unbox(ICharSequence::Probe(obj));
        AutoPtr<FontListParser::Alias> alias = new FontListParser::Alias();
        alias->mName = name;
        alias->mToName = toName;
        aliases->Add((IObject*)alias);
    }
    return aliases;
}

AutoPtr<IList> FontListConverter::AdaptNamesetAliases(
    /* [in] */ IList* nameset)
{
    AutoPtr<IList> aliases;
    CArrayList::New((IList**)&aliases);
    Int32 N;
    nameset->GetSize(&N);
    if (N < 2) return aliases; // An alias requires a name and toName

    AutoPtr<IInterface> obj;
    nameset->Get(0, (IInterface**)&obj);
    String toName = CoreUtils::Unbox(ICharSequence::Probe(obj));
    for(Int32 i = 1; i < N; i++) {
        AutoPtr<FontListParser::Alias> alias = new FontListParser::Alias();
        obj = NULL;
        nameset->Get(i, (IInterface**)&obj);
        alias->mName = CoreUtils::Unbox(ICharSequence::Probe(obj));
        alias->mToName = toName;
        aliases->Add((IObject*)alias);
    }

    return aliases;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
