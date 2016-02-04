
/*
 * Copyright 2012 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkPaintOptionsAndroid_DEFINED
#define SkPaintOptionsAndroid_DEFINED

#include "SkTypes.h"
#include "SkString.h"

#define  SKLANG_OPT
#ifdef SKLANG_OPT
//For testing this optimization ensure you rebuild and push libandroid_runtime.so
//everytime you build and push libskia.so
#include <pthread.h>
#endif
#undef SKLANG_OPT_DEBUG
#ifdef SKLANG_OPT_DEBUG
#define SkLangOptDebugf(...) SkDebugf(__VA_ARGS__)
#else
#define SkLangOptDebugf(...)
#endif

class SkReadBuffer;
class SkWriteBuffer;

/** \class SkLanguage

    The SkLanguage class represents a human written language, and is used by
    text draw operations to determine which glyph to draw when drawing
    characters with variants (ie Han-derived characters).
*/
class SkLanguage {
public:
    SkLanguage() { }
    SkLanguage(const SkString& tag) : fTag(tag) { }
    SkLanguage(const char* tag) : fTag(tag) { }
    SkLanguage(const char* tag, size_t len) : fTag(tag, len) { }
    SkLanguage(const SkLanguage& b) : fTag(b.fTag) { }

    /** Gets a BCP 47 language identifier for this SkLanguage.
        @return a BCP 47 language identifier representing this language
    */
    const SkString& getTag() const { return fTag; }

    /** Performs BCP 47 fallback to return an SkLanguage one step more general.
        @return an SkLanguage one step more general
    */
    SkLanguage getParent() const;

    bool operator==(const SkLanguage& b) const {
        return fTag == b.fTag;
    }
    bool operator!=(const SkLanguage& b) const {
        return fTag != b.fTag;
    }
    SkLanguage& operator=(const SkLanguage& b) {
        fTag = b.fTag;
        return *this;
    }

private:
    //! BCP 47 language identifier
    SkString fTag;
};

/**************************************************************************************/
#ifdef SKLANG_OPT
class SkLangList {
public:
        SkLangList(const SkLanguage& lang);
        SkLanguage s;
        SkLangList *next;
};

class SkLanguages {
public:
    static SkLanguages* getInstance();
    SkLangList* setLanguage( const SkLanguage& lang );
    SkLangList* setLanguage( const char* langTag );

private:
    SkLanguages();
    ~SkLanguages() {}

    //Disallow copy and assigning by declaring the (copy) constructors
    SkLanguages(const SkLanguages&);
    SkLanguages& operator=(const SkLanguages&);

    static SkLanguages* m_pInstance;
    SkLangList * LocaleArray;
    pthread_mutex_t update_mutex;
};
#endif
/**************************************************************************************/

class SkPaintOptionsAndroid {
public:
    SkPaintOptionsAndroid() {
        SkLangOptDebugf("SKLANG_OPT SkPaintOptionsAndroid constr");
#ifdef SKLANG_OPT
        fpLanguage = NULL;
#endif
        fFontVariant = kDefault_Variant;
        fUseFontFallbacks = false;
    }

    SkPaintOptionsAndroid& operator=(const SkPaintOptionsAndroid& b) {
        SkLangOptDebugf("SKLANG_OPT SkPaintOptionsAndroid overload= this=%p", this);
#ifdef SKLANG_OPT
        fpLanguage = b.fpLanguage;
#else
        fLanguage = b.fLanguage;
#endif
        fFontVariant = b.fFontVariant;
        fUseFontFallbacks = b.fUseFontFallbacks;
        return *this;
    }

    bool operator==(const SkPaintOptionsAndroid& b) const {
        return !(*this != b);
    }

    bool operator!=(const SkPaintOptionsAndroid& b) const {
        SkLangOptDebugf("SKLANG_OPT SkPaintOptionsAndroid overload!= this=%p", this);
        return
#ifdef SKLANG_OPT
               fpLanguage != b.fpLanguage ||
#else
               fLanguage != b.fLanguage ||
#endif
               fFontVariant != b.fFontVariant ||
               fUseFontFallbacks != b.fUseFontFallbacks;
    }

    void flatten(SkWriteBuffer&) const;
    void unflatten(SkReadBuffer&);

    /** Return the paint's language value used for drawing text.
        @return the paint's language value used for drawing text.
    */
#ifdef SKLANG_OPT
     const SkLanguage& getLanguage() const;
#else
     const SkLanguage& getLanguage() const {
         return fLanguage;
     }
#endif

    /** Set the paint's language value used for drawing text.
        @param language set the paint's language value for drawing text.
    */
#ifdef SKLANG_OPT
    void setLanguage(const SkLanguage& language);
    void setLanguage(const char* languageTag);
#else
    void setLanguage(const SkLanguage& language) {
       fLanguage = language;
    }

    void setLanguage(const char* languageTag) {
       fLanguage = SkLanguage(languageTag);
    }
#endif

    enum FontVariant {
       kDefault_Variant = 0x01,
       kCompact_Variant = 0x02,
       kElegant_Variant = 0x04,
       kLast_Variant = kElegant_Variant,
    };

    /** Return the font variant
        @return the font variant used by this paint object
    */
    FontVariant getFontVariant() const { return fFontVariant; }

    /** Set the font variant
      @param fontVariant set the paint's font variant for choosing fonts
    */
    void setFontVariant(FontVariant fontVariant) {
        SkASSERT((unsigned)fontVariant <= kLast_Variant);
        fFontVariant = fontVariant;
    }

    bool isUsingFontFallbacks() const { return fUseFontFallbacks; }

    void setUseFontFallbacks(bool useFontFallbacks) {
        fUseFontFallbacks = useFontFallbacks;
    }

private:
#ifdef SKLANG_OPT
   //Convert all string manipulations to pointer manipulations
   SkLangList* fpLanguage;
#else
   SkLanguage  fLanguage;
#endif
    FontVariant fFontVariant;
    bool fUseFontFallbacks;
};

#endif // #ifndef SkPaintOptionsAndroid_DEFINED
