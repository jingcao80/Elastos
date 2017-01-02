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

#ifndef __ELASTOS_DROID_TEXT_CAUTOTEXT_H__
#define __ELASTOS_DROID_TEXT_CAUTOTEXT_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Text_CAutoText.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IView;
using Elastos::Utility::ILocale;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CAutoText)
    , public Object
    , public IAutoText
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAutoText();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IResources* resources);

    static CARAPI_(String) Get(
        /* [in] */ ICharSequence* src,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ IView* view);

    static CARAPI_(Int32) GetSize(
        /* [in] */ IView* view);

private:
    /**
     * Returns the instance of AutoText. If the locale has changed, it will create a new
     * instance of AutoText for the locale.
     * @param view to get the resources from
     * @return the single instance of AutoText
     */
    static CARAPI_(AutoPtr<IAutoText>) GetInstance(
        /* [in] */ IView* view);

    static CARAPI_(AutoPtr<IAutoText>) GetStaticInstance();
private:
    /**
     * Returns the size of the dictionary.
     */
    CARAPI_(Int32) GetSize();

    CARAPI_(String) Lookup(
        /* [in] */ ICharSequence* src,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) Init(
        /* [in] */ IResources* r);

    CARAPI_(void) Add(
        /* [in] */ const String& src,
        /* [in] */ Char32 off);

    CARAPI_(Char32) NewTrieNode();

private:
    static const Int32 TRIE_C;// = 0;
    static const Int32 TRIE_OFF;// = 1;
    static const Int32 TRIE_CHILD;// = 2;
    static const Int32 TRIE_NEXT;// = 3;

    static const Int32 TRIE_SIZEOF;// = 4;
    static const Char32 TRIE_NULL;// = (char) -1;
    static const Int32 TRIE_ROOT;// = 0;

    static const Int32 INCREMENT;// = 1024;

    static const Int32 DEFAULT;// = 14337; // Size of the Trie 13 Aug 2007

    static const Int32 RIGHT;// = 9300; // Size of 'right' 13 Aug 2007

    static Object sLock;

    static AutoPtr<IAutoText> sInstance;// = new AutoText(Resources.getSystem());
    static Boolean sInited;

    // TODO:
    //
    // Note the assumption that the destination strings total less than
    // 64K characters and that the trie for the source side totals less
    // than 64K chars/offsets/child pointers/next pointers.
    //
    // This seems very safe for English (currently 7K of destination,
    // 14K of trie) but may need to be revisited.
private:
    AutoPtr< ArrayOf<Char32> > mTrie;
    Char32 mTrieUsed;
    String mText;
    AutoPtr<ILocale> mLocale;
    Int32 mSize;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CAUTOTEXT_H__
