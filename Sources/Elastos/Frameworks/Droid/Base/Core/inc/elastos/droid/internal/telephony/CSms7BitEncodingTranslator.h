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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMS7BITENCODINGTRANSLATOR_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMS7BITENCODINGTRANSLATOR_H__

#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Internal_Telephony_CSms7BitEncodingTranslator.h"
#include "elastos/droid/utility/SparseInt32Array.h"

#include "elastos/core/Singleton.h"

using Elastos::Droid::Utility::ISparseInt32Array;

using Elastos::Core::Singleton;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSms7BitEncodingTranslator)
    , public Singleton
    , public ISms7BitEncodingTranslator
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Translates each message character that is not supported by GSM 7bit
     * alphabet into a supported one
     *
     * @param message
     *            message to be translated
     * @param throwsException
     *            if TRUE and some error occurs during translation, an exception
     *            is thrown; otherwise a NULL String is returned
     * @return translated message or NULL if some error occur
     */
    CARAPI Translate(
        /* [in] */ ICharSequence* message,
        /* [out] */ String* result);


protected:
    CARAPI Sms7BitEncodingTranslator();

private:
    /**
     * Translates a single character into its corresponding acceptable one, if
     * needed, based on GSM 7-bit alphabet
     *
     * @param c
     *            character to be translated
     * @return original character, if it's present on GSM 7-bit alphabet; a
     *         corresponding character, based on the translation table or white
     *         space, if no mapping is found in the translation table for such
     *         character
     */
    static CARAPI_(Char32) TranslateIfNeeded(
        /* [in] */ Char32 c);

    static CARAPI_(Boolean) NoTranslationNeeded(
        /* [in] */ Char32 c);

    static CARAPI_(Boolean) UseCdmaFormatForMoSms();
    /**
     * Load the whole translation table file from the framework resource
     * encoded in XML.
     */
    static CARAPI_(void) Load7BitTranslationTableFromXml();

private:
    static const String TAG;
    static const Boolean DBG;
    static Boolean mIs7BitTranslationTableLoaded;
    static AutoPtr<ISparseInt32Array> mTranslationTable;
    static AutoPtr<ISparseInt32Array> mTranslationTableCommon;
    static AutoPtr<ISparseInt32Array> mTranslationTableGSM;
    static AutoPtr<ISparseInt32Array> mTranslationTableCDMA;

    // Parser variables
    static const String XML_START_TAG;
    static const String XML_TRANSLATION_TYPE_TAG;
    static const String XML_CHARACTOR_TAG;
    static const String XML_FROM_TAG;
    static const String XML_TO_TAG;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMS7BITENCODINGTRANSLATOR_H__
