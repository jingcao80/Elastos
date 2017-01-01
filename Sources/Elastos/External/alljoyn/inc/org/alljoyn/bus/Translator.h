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

#ifndef __ORG_ALLJOYN_BUS_TRANSLATOR_H__
#define __ORG_ALLJOYN_BUS_TRANSLATOR_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

class Translator
    : public Object
    , public ITranslator
{
public:
    CAR_INTERFACE_DECL()

    Translator();

    virtual ~Translator();

    CARAPI constructor();

    /**
     * Get the number of target languages this Translator supports
     *
     * @return numver of target languages
     */
    virtual CARAPI NumTargetLanguages(
        /* [out] */ Int32* num) = 0;

    /**
     * Retrieve one of the list of target languages this Translator supports
     *
     * @param index the index of the requested target language within the list
     * @return The requested target language or null if index is out of bounds
     */
    virtual CARAPI GetTargetLanguage(
        /* [in] */ Int32 index,
        /* [out] */ String* str) = 0;

    /**
     * Translate a string into another language
     *
     * @param fromLanguage the language to translate from
     * @param toLanguage the language to translate to
     * @param fromText the text to be translated
     * @return the translated text or null if it can not be translated
     */
    virtual CARAPI Translate(
        /* [in] */ const String& fromLanguage,
        /* [in] */ const String& toLanguage,
        /* [in] */ const String& fromText,
        /* [out] */ String* str) = 0;

private:
    /**
     * Create any native resources held by objects of this class.  Specifically,
     * we allocate a C++ counterpart of this Translator object.
     */
    CARAPI_(void) Create();

    /**
     * Release any native resources held by objects of this class.
     * Specifically, we may delete a C++ counterpart of this Translator object.
     */
    CARAPI_(void) Destroy();

private:
    friend class InterfaceDescription;
    friend class CBusAttachment;

    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_TRANSLATOR_H__
