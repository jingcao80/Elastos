
#include "XmlKeyboardLoader.h"
#include "SoftKeyboard.h"
#include "SkbTemplate.h"
#include "SkbPool.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

String XmlKeyboardLoader::XMLTAG_SKB_TEMPLATE("skb_template");
String XmlKeyboardLoader::XMLTAG_KEYTYPE("key_type");
String XmlKeyboardLoader::XMLTAG_KEYICON("key_icon");
String XmlKeyboardLoader::XMLATTR_KEY_XMARGIN("key_xmargin");
String XmlKeyboardLoader::XMLATTR_KEY_YMARGIN("key_ymargin");
String XmlKeyboardLoader::XMLATTR_SKB_BG("skb_bg");
String XmlKeyboardLoader::XMLATTR_BALLOON_BG("balloon_bg");
String XmlKeyboardLoader::XMLATTR_POPUP_BG("popup_bg");
String XmlKeyboardLoader::XMLATTR_COLOR("color");
String XmlKeyboardLoader::XMLATTR_COLOR_HIGHLIGHT("color_highlight");
String XmlKeyboardLoader::XMLATTR_COLOR_BALLOON("color_balloon");
String XmlKeyboardLoader::XMLATTR_ID("id");
String XmlKeyboardLoader::XMLATTR_KEYTYPE_BG("bg");
String XmlKeyboardLoader::XMLATTR_KEYTYPE_HLBG("hlbg");
String XmlKeyboardLoader::XMLATTR_START_POS_X("start_pos_x");
String XmlKeyboardLoader::XMLATTR_START_POS_Y("start_pos_y");
String XmlKeyboardLoader::XMLATTR_ROW_ID("row_id");
String XmlKeyboardLoader::XMLTAG_KEYBOARD("keyboard");
String XmlKeyboardLoader::XMLTAG_ROW("row");
String XmlKeyboardLoader::XMLTAG_KEYS("keys");
String XmlKeyboardLoader::XMLTAG_KEY("key");
String XmlKeyboardLoader::XMLTAG_TOGGLE_STATE("toggle_state");
String XmlKeyboardLoader::XMLATTR_TOGGLE_STATE_ID("state_id");
String XmlKeyboardLoader::XMLATTR_SKB_TEMPLATE("skb_template");
String XmlKeyboardLoader::XMLATTR_SKB_CACHE_FLAG("skb_cache_flag");
String XmlKeyboardLoader::XMLATTR_SKB_STICKY_FLAG("skb_sticky_flag");
String XmlKeyboardLoader::XMLATTR_QWERTY("qwerty");
String XmlKeyboardLoader::XMLATTR_QWERTY_UPPERCASE("qwerty_uppercase");
String XmlKeyboardLoader::XMLATTR_KEY_TYPE("key_type");
String XmlKeyboardLoader::XMLATTR_KEY_WIDTH("width");
String XmlKeyboardLoader::XMLATTR_KEY_HEIGHT("height");
String XmlKeyboardLoader::XMLATTR_KEY_REPEAT("repeat");
String XmlKeyboardLoader::XMLATTR_KEY_BALLOON("balloon");
String XmlKeyboardLoader::XMLATTR_KEY_SPLITTER("splitter");
String XmlKeyboardLoader::XMLATTR_KEY_LABELS("labels");
String XmlKeyboardLoader::XMLATTR_KEY_CODES("codes");
String XmlKeyboardLoader::XMLATTR_KEY_LABEL("label");
String XmlKeyboardLoader::XMLATTR_KEY_CODE("code");
String XmlKeyboardLoader::XMLATTR_KEY_ICON("icon");
String XmlKeyboardLoader::XMLATTR_KEY_ICON_POPUP("icon_popup");
String XmlKeyboardLoader::XMLATTR_KEY_POPUP_SKBID("popup_skb");
Boolean XmlKeyboardLoader::DEFAULT_SKB_CACHE_FLAG = TRUE;
Boolean XmlKeyboardLoader::DEFAULT_SKB_STICKY_FLAG = TRUE;
const Int32 XmlKeyboardLoader::KEYTYPE_ID_LAST = -1;


XmlKeyboardLoader::KeyCommonAttributes::KeyCommonAttributes(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ XmlKeyboardLoader* host)
    : mXrp(xrp)
    , mKeyType(0)
    , mKeyWidth(0.f)
    , mKeyHeight(0.f)
    , mRepeat(FALSE)
    , mBalloon(TRUE)
    , mHost(host)
{
}

Boolean XmlKeyboardLoader::KeyCommonAttributes::GetAttributes(
    /* [in] */ KeyCommonAttributes* defAttr)
{
    assert(defAttr != NULL);
    mKeyType = mHost->GetInteger(mXrp, XMLATTR_KEY_TYPE, defAttr->mKeyType);
    mKeyWidth = mHost->GetFloat(mXrp, XMLATTR_KEY_WIDTH, defAttr->mKeyWidth);
    mKeyHeight = mHost->GetFloat(mXrp, XMLATTR_KEY_HEIGHT, defAttr->mKeyHeight);
    mRepeat = mHost->GetBoolean(mXrp, XMLATTR_KEY_REPEAT, defAttr->mRepeat);
    mBalloon = mHost->GetBoolean(mXrp, XMLATTR_KEY_BALLOON, defAttr->mBalloon);
    if (mKeyType < 0 || mKeyWidth <= 0 || mKeyHeight <= 0) {
        return FALSE;
    }
    return TRUE;
}

XmlKeyboardLoader::XmlKeyboardLoader(
    /* [in] */ IContext* context)
    : mContext(context)
    , mXmlEventType(0)
    , mKeyXPos(0.f)
    , mKeyYPos(0.f)
    , mSkbWidth(0)
    , mSkbHeight(0)
    , mKeyXMargin(0.f)
    , mKeyYMargin(0.f)
    , mNextEventFetched(FALSE)
{
    mContext->GetResources((IResources**)&mResources);
}

AutoPtr<SkbTemplate> XmlKeyboardLoader::LoadSkbTemplate(
    /* [in] */ Int32 resourceId)
{
    if (NULL == mContext || 0 == resourceId) {
        return NULL;
    }
    AutoPtr<IResources> r = mResources;
    AutoPtr<IXmlResourceParser> xrp;
    r->GetXml(resourceId, (IXmlResourceParser**)&xrp);

    AutoPtr<KeyCommonAttributes> attrDef = new KeyCommonAttributes(xrp, this);
    AutoPtr<KeyCommonAttributes> attrKey = new KeyCommonAttributes(xrp, this);

    mSkbTemplate = new SkbTemplate(resourceId);
    Int32 lastKeyTypeId = KEYTYPE_ID_LAST;
    Int32 globalColor = 0;
    Int32 globalColorHl = 0;
    Int32 globalColorBalloon = 0;
    // try {
    xrp->Next(&mXmlEventType);
    while (mXmlEventType != IXmlResourceParser::END_DOCUMENT) {
        mNextEventFetched = FALSE;
        if (mXmlEventType == IXmlResourceParser::START_TAG) {
            String attribute;
            xrp->GetName(&attribute);
            if (XMLTAG_SKB_TEMPLATE.Equals(attribute)) {
                AutoPtr<IDrawable> skbBg = GetDrawable(xrp, XMLATTR_SKB_BG, NULL);
                AutoPtr<IDrawable> balloonBg = GetDrawable(xrp,
                        XMLATTR_BALLOON_BG, NULL);
                AutoPtr<IDrawable> popupBg = GetDrawable(xrp, XMLATTR_POPUP_BG,
                        NULL);
                if (NULL == skbBg || NULL == balloonBg
                        || NULL == popupBg) {
                    return NULL;
                }
                mSkbTemplate->SetBackgrounds(skbBg, balloonBg, popupBg);

                Float xMargin = GetFloat(xrp, XMLATTR_KEY_XMARGIN, 0);
                Float yMargin = GetFloat(xrp, XMLATTR_KEY_YMARGIN, 0);
                mSkbTemplate->SetMargins(xMargin, yMargin);

                // Get default global colors.
                globalColor = GetColor(xrp, XMLATTR_COLOR, 0);
                globalColorHl = GetColor(xrp, XMLATTR_COLOR_HIGHLIGHT,
                        0xffffffff);
                globalColorBalloon = GetColor(xrp,
                        XMLATTR_COLOR_BALLOON, 0xffffffff);
            } else if (XMLTAG_KEYTYPE.Equals(attribute)) {
                Int32 id = GetInteger(xrp, XMLATTR_ID, KEYTYPE_ID_LAST);
                AutoPtr<IDrawable> bg = GetDrawable(xrp, XMLATTR_KEYTYPE_BG, NULL);
                AutoPtr<IDrawable> hlBg = GetDrawable(xrp, XMLATTR_KEYTYPE_HLBG,
                        NULL);
                Int32 color = GetColor(xrp, XMLATTR_COLOR, globalColor);
                Int32 colorHl = GetColor(xrp, XMLATTR_COLOR_HIGHLIGHT,
                        globalColorHl);
                Int32 colorBalloon = GetColor(xrp, XMLATTR_COLOR_BALLOON,
                        globalColorBalloon);
                if (id != lastKeyTypeId + 1) {
                    return NULL;
                }
                AutoPtr<SoftKeyType> keyType = mSkbTemplate->CreateKeyType(id,
                        bg, hlBg);
                keyType->SetColors(color, colorHl, colorBalloon);
                if (!mSkbTemplate->AddKeyType(keyType)) {
                    return NULL;
                }
                lastKeyTypeId = id;
            } else if (XMLTAG_KEYICON.Equals(attribute)) {
                Int32 keyCode = GetInteger(xrp, XMLATTR_KEY_CODE, 0);
                AutoPtr<IDrawable> icon = GetDrawable(xrp, XMLATTR_KEY_ICON, NULL);
                AutoPtr<IDrawable> iconPopup = GetDrawable(xrp,
                        XMLATTR_KEY_ICON_POPUP, NULL);
                if (NULL != icon && NULL != iconPopup) {
                    mSkbTemplate->AddDefaultKeyIcons(keyCode, icon,
                            iconPopup);
                }
            } else if (XMLTAG_KEY.Equals(attribute)) {
                Int32 keyId = GetInteger(xrp, XMLATTR_ID, -1);
                if (-1 == keyId) return NULL;

                if (!attrKey->GetAttributes(attrDef)) {
                    return NULL;
                }

                // Update the key position for the key.
                mKeyXPos = GetFloat(xrp, XMLATTR_START_POS_X, 0);
                mKeyYPos = GetFloat(xrp, XMLATTR_START_POS_Y, 0);

                AutoPtr<SoftKey> softKey;
                if (FAILED(GetSoftKey(xrp, attrKey, (SoftKey**)&softKey))) {
                    return NULL;
                }
                if (NULL == softKey) return NULL;
                mSkbTemplate->AddDefaultKey(keyId, softKey);
            }
        }
        // Get the next tag.
        if (!mNextEventFetched) xrp->Next(&mXmlEventType);
    }
    xrp->Close();
    return mSkbTemplate;
    // } catch (XmlPullParserException e) {
    //     // Log.e(TAG, "Ill-formatted keyboard template resource file");
    // } catch (IOException e) {
    //     // Log.e(TAG, "Unable to keyboard template resource file");
    // }
    // //TODO
    // return NULL;
}

AutoPtr<SoftKeyboard> XmlKeyboardLoader::LoadKeyboard(
    /* [in] */ Int32 resourceId,
    /* [in] */ Int32 skbWidth,
    /* [in] */ Int32 skbHeight)
{
    if (NULL == mContext) return NULL;
    AutoPtr<IResources> r = mResources;
    AutoPtr<SkbPool> skbPool = SkbPool::GetInstance();

    AutoPtr<IResources> tmpRes;
    mContext->GetResources((IResources**)&tmpRes);
    AutoPtr<IXmlResourceParser> xrp;
    tmpRes->GetXml(resourceId, (IXmlResourceParser**)&xrp);
    assert(xrp != NULL);

    mSkbTemplate = NULL;
    AutoPtr<SoftKeyboard> softKeyboard;
    AutoPtr<IDrawable> skbBg;
    AutoPtr<IDrawable> popupBg;
    AutoPtr<IDrawable> balloonBg;
    AutoPtr<SoftKey> softKey;

    AutoPtr<KeyCommonAttributes> attrDef = new KeyCommonAttributes(xrp, this);
    AutoPtr<KeyCommonAttributes> attrSkb = new KeyCommonAttributes(xrp, this);
    AutoPtr<KeyCommonAttributes> attrRow = new KeyCommonAttributes(xrp, this);
    AutoPtr<KeyCommonAttributes> attrKeys = new KeyCommonAttributes(xrp, this);
    AutoPtr<KeyCommonAttributes> attrKey = new KeyCommonAttributes(xrp, this);

    mKeyXPos = 0;
    mKeyYPos = 0;
    mSkbWidth = skbWidth;
    mSkbHeight = skbHeight;

    // try {
    mKeyXMargin = 0;
    mKeyYMargin = 0;
    xrp->Next(&mXmlEventType);
    while (mXmlEventType != IXmlResourceParser::END_DOCUMENT) {
        mNextEventFetched = FALSE;
        if (mXmlEventType == IXmlResourceParser::START_TAG) {
            String attr;
            xrp->GetName(&attr);
            // 1. Is it the root element, "keyboard"?
            if (XMLTAG_KEYBOARD.Equals(attr)) {
                // 1.1 Get the keyboard template id.
                Int32 skbTemplateId = 0;
                assert(IAttributeSet::Probe(xrp) != NULL);
                IAttributeSet::Probe(xrp)->GetAttributeResourceValue(String(NULL),
                        XMLATTR_SKB_TEMPLATE, 0, &skbTemplateId);

                // 1.2 Try to get the template from pool. If it is not
                // in, the pool will try to load it.
                mSkbTemplate = skbPool->GetSkbTemplate(skbTemplateId,
                        mContext);

                if (NULL == mSkbTemplate
                        || !attrSkb->GetAttributes(attrDef)) {
                    return NULL;
                }

                Boolean cacheFlag = GetBoolean(xrp,
                        XMLATTR_SKB_CACHE_FLAG, DEFAULT_SKB_CACHE_FLAG);
                Boolean stickyFlag = GetBoolean(xrp,
                        XMLATTR_SKB_STICKY_FLAG,
                        DEFAULT_SKB_STICKY_FLAG);
                Boolean isQwerty = GetBoolean(xrp, XMLATTR_QWERTY,
                        FALSE);
                Boolean isQwertyUpperCase = GetBoolean(xrp,
                        XMLATTR_QWERTY_UPPERCASE, FALSE);

                softKeyboard = new SoftKeyboard(resourceId,
                        mSkbTemplate, mSkbWidth, mSkbHeight);
                softKeyboard->SetFlags(cacheFlag, stickyFlag, isQwerty,
                        isQwertyUpperCase);

                mKeyXMargin = GetFloat(xrp, XMLATTR_KEY_XMARGIN,
                        mSkbTemplate->GetXMargin());
                mKeyYMargin = GetFloat(xrp, XMLATTR_KEY_YMARGIN,
                        mSkbTemplate->GetYMargin());
                skbBg = GetDrawable(xrp, XMLATTR_SKB_BG, NULL);
                popupBg = GetDrawable(xrp, XMLATTR_POPUP_BG, NULL);
                balloonBg = GetDrawable(xrp, XMLATTR_BALLOON_BG, NULL);
                if (NULL != skbBg) {
                    softKeyboard->SetSkbBackground(skbBg);
                }
                if (NULL != popupBg) {
                    softKeyboard->SetPopupBackground(popupBg);
                }
                if (NULL != balloonBg) {
                    softKeyboard->SetKeyBalloonBackground(balloonBg);
                }
                softKeyboard->SetKeyMargins(mKeyXMargin, mKeyYMargin);
            } else if (XMLTAG_ROW.Equals(attr)) {
                if (!attrRow->GetAttributes(attrSkb)) {
                    return NULL;
                }
                // Get the starting positions for the row.
                mKeyXPos = GetFloat(xrp, XMLATTR_START_POS_X, 0);
                mKeyYPos = GetFloat(xrp, XMLATTR_START_POS_Y, mKeyYPos);
                Int32 rowId = GetInteger(xrp, XMLATTR_ROW_ID,
                        SoftKeyboard::KeyRow::ALWAYS_SHOW_ROW_ID);
                softKeyboard->BeginNewRow(rowId, mKeyYPos);
            } else if (XMLTAG_KEYS.Equals(attr)) {
                if (NULL == softKeyboard) return NULL;
                if (!attrKeys->GetAttributes(attrRow)) {
                    return NULL;
                }

                String splitter;
                xrp->GetAttributeValue(String(NULL), XMLATTR_KEY_SPLITTER, &splitter);
                AutoPtr<IPatternHelper> helper;
                CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
                helper->Quote(splitter, &splitter);
                String labels;
                xrp->GetAttributeValue(String(NULL), XMLATTR_KEY_LABELS, &labels);
                String codes;
                xrp->GetAttributeValue(String(NULL), XMLATTR_KEY_CODES, &codes);
                if (NULL == splitter || NULL == labels) {
                    return NULL;
                }
                AutoPtr<ArrayOf<String> > labelArr;
                StringUtils::Split(labels, splitter, (ArrayOf<String>**)&labelArr);
                assert(labelArr != NULL);
                AutoPtr<ArrayOf<String> > codeArr;
                if (NULL != codes) {
                    StringUtils::Split(codes, splitter, (ArrayOf<String>**)&codeArr);
                    assert(codeArr != NULL);
                    if (labelArr->GetLength() != codeArr->GetLength()) {
                        return NULL;
                    }
                }

                for (Int32 i = 0; i < labelArr->GetLength(); i++) {
                    softKey = new SoftKey();
                    Int32 keyCode = 0;
                    if (NULL != codeArr) {
                        StringUtils::ParseInt32((*codeArr)[i], &keyCode);
                    }
                    softKey->SetKeyAttribute(keyCode, (*labelArr)[i],
                            attrKeys->mRepeat, attrKeys->mBalloon);

                    softKey->SetKeyType(mSkbTemplate->GetKeyType(attrKeys->mKeyType), NULL, NULL);

                    Float left, right, top, bottom;
                    left = mKeyXPos;

                    right = left + attrKeys->mKeyWidth;
                    top = mKeyYPos;
                    bottom = top + attrKeys->mKeyHeight;

                    if (right - left < 2 * mKeyXMargin) return NULL;
                    if (bottom - top < 2 * mKeyYMargin) return NULL;

                    softKey->SetKeyDimensions(left, top, right, bottom);
                    softKeyboard->AddSoftKey(softKey);
                    mKeyXPos = right;
                    if ((Int32) mKeyXPos * mSkbWidth > mSkbWidth) {
                        return NULL;
                    }
                }
            } else if (XMLTAG_KEY.Equals(attr)) {
                if (NULL == softKeyboard) {
                    return NULL;
                }
                if (!attrKey->GetAttributes(attrRow)) {
                    return NULL;
                }

                Int32 keyId = GetInteger(xrp, XMLATTR_ID, -1);
                if (keyId >= 0) {
                    softKey = mSkbTemplate->GetDefaultKey(keyId);
                } else {
                    softKey = NULL;
                    if (FAILED(GetSoftKey(xrp, attrKey, (SoftKey**)&softKey))) {
                        return NULL;
                    }
                }
                if (NULL == softKey) return NULL;

                // Update the position for next key.
                mKeyXPos = softKey->mRightF;
                if ((Int32) mKeyXPos * mSkbWidth > mSkbWidth) {
                    return NULL;
                }
                // If the current xml event type becomes a starting tag,
                // it indicates that we have parsed too much to get
                // toggling states, and we started a new row. In this
                // case, the row starting position information should
                // be updated.
                if (mXmlEventType == IXmlResourceParser::START_TAG) {
                    xrp->GetName(&attr);
                    if (XMLTAG_ROW.Equals(attr)) {
                        mKeyYPos += attrRow->mKeyHeight;
                        if ((Int32) mKeyYPos * mSkbHeight > mSkbHeight) {
                            return NULL;
                        }
                    }
                }
                softKeyboard->AddSoftKey(softKey);
            }
        } else if (mXmlEventType == IXmlResourceParser::END_TAG) {
            String attr;
            xrp->GetName(&attr);
            if (XMLTAG_ROW.Equals(attr)) {
                mKeyYPos += attrRow->mKeyHeight;
                if ((Int32) mKeyYPos * mSkbHeight > mSkbHeight) {
                    return NULL;
                }
            }
        }

        // Get the next tag.
        if (!mNextEventFetched) xrp->Next(&mXmlEventType);
    }
    xrp->Close();
    softKeyboard->SetSkbCoreSize(mSkbWidth, mSkbHeight);
    return softKeyboard;
    // } catch (XmlPullParserException e) {
    //     // Log.e(TAG, "Ill-formatted keybaord resource file");
    // } catch (IOException e) {
    //     // Log.e(TAG, "Unable to read keyboard resource file");
    // }
    // //TODO
    // return NULL;
}

ECode XmlKeyboardLoader::GetSoftKey(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ KeyCommonAttributes* attrKey,
    /* [out] */ SoftKey** key) //throws XmlPullParserException, IOException
{
    VALIDATE_NOT_NULL(key);
    *key = NULL;
    assert(xrp != NULL);
    Int32 keyCode = GetInteger(xrp, XMLATTR_KEY_CODE, 0);
    String keyLabel = GetString(xrp, XMLATTR_KEY_LABEL, String(NULL));
    AutoPtr<IDrawable> keyIcon = GetDrawable(xrp, XMLATTR_KEY_ICON, NULL);
    AutoPtr<IDrawable> keyIconPopup = GetDrawable(xrp, XMLATTR_KEY_ICON_POPUP, NULL);
    Int32 popupSkbId = 0;
    assert(IAttributeSet::Probe(xrp) != NULL);
    FAIL_RETURN(IAttributeSet::Probe(xrp)->GetAttributeResourceValue(String(NULL), XMLATTR_KEY_POPUP_SKBID, 0, &popupSkbId));

    if (NULL == keyLabel && NULL == keyIcon) {
        keyIcon = mSkbTemplate->GetDefaultKeyIcon(keyCode);
        keyIconPopup = mSkbTemplate->GetDefaultKeyIconPopup(keyCode);
        if (NULL == keyIcon || NULL == keyIconPopup) {
            return NOERROR;
        }
    }

    // Dimension information must been initialized before
    // getting toggle state, because mKeyYPos may be changed
    // to next row when trying to get toggle state.
    Float left, right, top, bottom;
    left = mKeyXPos;
    right = left + attrKey->mKeyWidth;
    top = mKeyYPos;
    bottom = top + attrKey->mKeyHeight;

    if (right - left < 2 * mKeyXMargin) {
        return NOERROR;
    }
    if (bottom - top < 2 * mKeyYMargin) {
        return NOERROR;
    }

    // Try to find if the next tag is
    // {@link #XMLTAG_TOGGLE_STATE_OF_KEY}, if yes, try to
    // create a toggle key.
    Boolean toggleKey = FALSE;
    FAIL_RETURN(xrp->Next(&mXmlEventType));
    mNextEventFetched = TRUE;

    AutoPtr<SoftKey> softKey;
    if (mXmlEventType == IXmlResourceParser::START_TAG) {
        FAIL_RETURN(xrp->GetName(&mAttrTmp));
        if (mAttrTmp.Equals(XMLTAG_TOGGLE_STATE)) {
            toggleKey = TRUE;
        }
    }
    if (toggleKey) {
        softKey = new SoftKeyToggle();
        if (!((SoftKeyToggle*) softKey.Get())->SetToggleStates(GetToggleStates(
                attrKey, (SoftKeyToggle*) softKey.Get(), keyCode))) {
            return NOERROR;
        }
    } else {
        softKey = new SoftKey();
    }

    // Set the normal state
    softKey->SetKeyAttribute(keyCode, keyLabel, attrKey->mRepeat, attrKey->mBalloon);
    softKey->SetPopupSkbId(popupSkbId);
    softKey->SetKeyType(mSkbTemplate->GetKeyType(attrKey->mKeyType), keyIcon, keyIconPopup);

    softKey->SetKeyDimensions(left, top, right, bottom);
    *key = softKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

AutoPtr<SoftKeyToggle::ToggleState> XmlKeyboardLoader::GetToggleStates(
    /* [in] */ KeyCommonAttributes* attrKey,
    /* [in] */ SoftKeyToggle* softKey,
    /* [in] */ Int32 defKeyCode) //throws XmlPullParserException, IOException
{
    assert(attrKey != NULL);
    AutoPtr<IXmlResourceParser> xrp = attrKey->mXrp;
    assert(xrp != NULL);
    Int32 stateId = GetInteger(xrp, XMLATTR_TOGGLE_STATE_ID, 0);
    if (0 == stateId) return NULL;

    String keyLabel = GetString(xrp, XMLATTR_KEY_LABEL, String(NULL));
    Int32 keyTypeId = GetInteger(xrp, XMLATTR_KEY_TYPE, KEYTYPE_ID_LAST);
    Int32 keyCode;
    if (NULL == keyLabel) {
        keyCode = GetInteger(xrp, XMLATTR_KEY_CODE, defKeyCode);
    } else {
        keyCode = GetInteger(xrp, XMLATTR_KEY_CODE, 0);
    }
    AutoPtr<IDrawable> icon = GetDrawable(xrp, XMLATTR_KEY_ICON, NULL);
    AutoPtr<IDrawable> iconPopup = GetDrawable(xrp, XMLATTR_KEY_ICON_POPUP, NULL);
    if (NULL == icon && NULL == keyLabel) {
        return NULL;
    }
    AutoPtr<SoftKeyToggle::ToggleState> rootState = softKey->CreateToggleState();
    rootState->SetStateId(stateId);
    rootState->mKeyType = NULL;
    if (KEYTYPE_ID_LAST != keyTypeId) {
        rootState->mKeyType = mSkbTemplate->GetKeyType(keyTypeId);
    }
    rootState->mKeyCode = keyCode;
    rootState->mKeyIcon = icon;
    rootState->mKeyIconPopup = iconPopup;
    rootState->mKeyLabel = keyLabel;

    Boolean repeat = GetBoolean(xrp, XMLATTR_KEY_REPEAT, attrKey->mRepeat);
    Boolean balloon = GetBoolean(xrp, XMLATTR_KEY_BALLOON, attrKey->mBalloon);
    rootState->SetStateFlags(repeat, balloon);

    rootState->mNextState = NULL;

    // If there is another toggle state.
    xrp->Next(&mXmlEventType);
    while (mXmlEventType != IXmlResourceParser::START_TAG
            && mXmlEventType != IXmlResourceParser::END_DOCUMENT) {
        xrp->Next(&mXmlEventType);
    }
    if (mXmlEventType == IXmlResourceParser::START_TAG) {
        String attr;
        xrp->GetName(&attr);
        if (attr.Equals(XMLTAG_TOGGLE_STATE)) {
            AutoPtr<SoftKeyToggle::ToggleState> nextState = GetToggleStates(attrKey,
                    softKey, defKeyCode);
            if (NULL == nextState) return NULL;
            rootState->mNextState = nextState;
        }
    }

    return rootState;
}

Int32 XmlKeyboardLoader::GetInteger(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ const String& name,
    /* [in] */ Int32 defValue)
{
    assert(xrp != NULL);
    Int32 resId = 0;
    assert(IAttributeSet::Probe(xrp) != NULL);
    IAttributeSet::Probe(xrp)->GetAttributeResourceValue(String(NULL), name, 0, &resId);
    String s;
    if (resId == 0) {
        xrp->GetAttributeValue(String(NULL), name, &s);
        if (NULL == s) return defValue;
        // try {
        Int32 ret = 0;
        ECode ec = StringUtils::ParseInt32(s, &ret);
        if (FAILED(ec)) {
            return defValue;
        }
        return ret;
        // } catch (NumberFormatException e) {
        //     return defValue;
        // }
    } else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        String str;
        res->GetString(resId, &str);
        Int32 value = 0;
        StringUtils::ParseInt32(str, &value);
        return value;
    }
}

Int32 XmlKeyboardLoader::GetColor(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ const String& name,
    /* [in] */ Int32 defValue)
{
    assert(xrp != NULL);
    Int32 resId = 0;
    assert(IAttributeSet::Probe(xrp) != NULL);
    IAttributeSet::Probe(xrp)->GetAttributeResourceValue(String(NULL), name, 0, &resId);
    String s;
    if (resId == 0) {
        xrp->GetAttributeValue(String(NULL), name, &s);
        if (NULL == s) return defValue;
        // try {
        Int32 ret = 0;
        ECode ec = StringUtils::ParseInt32(s, &ret);
        if (FAILED(ec)) {
            return defValue;
        }
        return ret;
        // } catch (NumberFormatException e) {
        //     return defValue;
        // }
    } else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Int32 color = 0;
        res->GetColor(resId, &color);
        return color;
    }
}

String XmlKeyboardLoader::GetString(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ const String& name,
    /* [in] */ const String& defValue)
{
    assert(xrp != NULL);
    Int32 resId = 0;
    assert(IAttributeSet::Probe(xrp) != NULL);
    IAttributeSet::Probe(xrp)->GetAttributeResourceValue(String(NULL), name, 0, &resId);

    String retValue;
    if (resId == 0) {
        xrp->GetAttributeValue(String(NULL), name, &retValue);
        return retValue;
    } else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetString(resId, &retValue);
        return retValue;
    }
}

Float XmlKeyboardLoader::GetFloat(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ const String& name,
    /* [in] */ Float defValue)
{
    assert(xrp != NULL);
    Int32 resId = 0;
    assert(IAttributeSet::Probe(xrp) != NULL);
    IAttributeSet::Probe(xrp)->GetAttributeResourceValue(String(NULL), name, 0, &resId);
    if (resId == 0) {
        String s;
        xrp->GetAttributeValue(String(NULL), name, &s);
        if (s.IsNullOrEmpty()) return defValue;
        // try {
        Float ret;
        ECode ec = NOERROR;
        if (s.EndWith(String("%p"))) {
            ec = StringUtils::ParseFloat(s.Substring(0, s.GetLength() - 2), &ret);
            if (FAILED(ec)) {
                return defValue;
            }
            ret /= 100;
        } else {
            ec = StringUtils::ParseFloat(s, &ret);
            if (FAILED(ec)) {
                return defValue;
            }
        }
        return ret;
        // } catch (NumberFormatException e) {
        //     return defValue;
        // }
    } else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Float ret = 0.f;
        res->GetDimension(resId, &ret);
        return ret;
    }
}

Boolean XmlKeyboardLoader::GetBoolean(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ const String& name,
    /* [in] */ Boolean defValue)
{
    assert(xrp != NULL);
    String s;
    xrp->GetAttributeValue(String(NULL), name, &s);
    if (NULL == s) return defValue;
    // try {
    // TODO
    // Boolean ret = s.ToBoolean();
    return s.EqualsIgnoreCase("TRUE");
    // } catch (NumberFormatException e) {
           //TODO
    //     return defValue;
    // }
}

AutoPtr<IDrawable> XmlKeyboardLoader::GetDrawable(
    /* [in] */ IXmlResourceParser* xrp,
    /* [in] */ const String& name,
    /* [in] */ IDrawable* defValue)
{
    assert(xrp != NULL);
    Int32 resId = 0;
    assert(IAttributeSet::Probe(xrp) != NULL);
    IAttributeSet::Probe(xrp)->GetAttributeResourceValue(String(NULL), name, 0, &resId);
    if (0 == resId) return defValue;
    AutoPtr<IDrawable> d;
    mResources->GetDrawable(resId, (IDrawable**)&d);
    return d;
}

ECode XmlKeyboardLoader::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Elastos.Droid.Inputmethods.PinyinIME.XmlKeyboardLoader");
    return NOERROR;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
