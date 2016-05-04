
#include "AbstractPreferences.h"
#include "CLinkedList.h"
#include "CTreeSet.h"
#include "CString.h"
#include "CBase64.h"
#include "AutoLock.h"
#include "XMLParser.h"
#include "EmptyArray.h"
#include "StringUtils.h"

using Libcore::IO::CBase64;
using Libcore::IO::IBase64;
using Libcore::Utility::EmptyArray;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;


DEFINE_CONVERSION_FOR(Elastos::Utility::Prefs::AbstractPreferences, IInterface)


namespace Elastos {
namespace Utility {
namespace Prefs {

const Int64 AbstractPreferences::NodeAddEvent::serialVersionUID = 1L;
const Int64 AbstractPreferences::NodeRemoveEvent::serialVersionUID = 1L;

AutoPtr<IList> AbstractPreferences::sEvents;
//TODO:
Boolean AbstractPreferences::sStaticInit;// = AbstractPreferences::staticInit();
// AutoPtr<EventDispatcher> AbstractPreferences::sDispatcher;

CAR_INTERFACE_IMPL(AbstractPreferences, Preferences, IAbstractPreferences);

Boolean AbstractPreferences::staticInit()
{
    CLinkedList::New((IList**)&sEvents);
    //TODO
    assert(0);
    // sDispatcher = new EventDispatcher("Preference Event Dispatcher");

    /*
     * -----------------------------------------------------------
     * Class initializer
     * -----------------------------------------------------------
     */
    // static {
    //     sDispatcher.setDaemon(TRUE);
    //     sDispatcher.start();
    //     Runtime.getRuntime().addShutdownHook(new Thread() {
    //         @Override
    //         public void run() {
    //             Preferences uroot = Preferences.userRoot();
    //             Preferences sroot = Preferences.systemRoot();
    //             try {
    //                 uroot.flush();
    //             } catch (BackingStoreException e) {
    //                 // ignore
    //             }
    //             try {
    //                 sroot.flush();
    //             } catch (BackingStoreException e) {
    //                 // ignore
    //             }
    //         }
    //     });
    // }

    return TRUE;
}

AbstractPreferences::AbstractPreferences(
    /* [in] */ AbstractPreferences* parent,
    /* [in] */ const String& name)
{
    if ((parent == NULL || name.GetLength() == 0) || name.IndexOf("/") >= 0) {
        // throw new IllegalArgumentException();
        assert(0);
    }
    if (parent == NULL) {
        mRoot = this;
    } else {
        mRoot = parent->mRoot;
    }
    CLinkedList::New((ILinkedList**)&mNodeChangeListeners);
    CLinkedList::New((ILinkedList**)&mPreferenceChangeListeners);
    mIsRemoved = FALSE;
    mNodeName = name;
    mParentPref = parent;
    mUserNode = mRoot->mUserNode;
}

AutoPtr<ArrayOf<AbstractPreferences* > > AbstractPreferences::CachedChildren()
{
    AutoPtr<ArrayOf<AbstractPreferences* > > absps
        = ArrayOf<AbstractPreferences* >::Alloc(mCachedNode.GetSize());

    HashMap<String, AutoPtr<AbstractPreferences> >::Iterator iter = mCachedNode.Begin();
    for (Int32 i = 0; iter != mCachedNode.End(); ++iter, i++) {
        absps->Set(i, iter->mSecond);
    }

    return absps;
}

ECode AbstractPreferences::GetChild(
    /* [in] */ const String& name,
    /* [out] */ AbstractPreferences** apfs)
{
    VALIDATE_NOT_NULL(apfs);
    *apfs = NULL;
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        AutoPtr<AbstractPreferences> result;
        AutoPtr<ArrayOf<String> > childrenNames;
        FAIL_RETURN(GetChildrenNames((ArrayOf<String>**)&childrenNames));
        for (Int32 i = 0; i < childrenNames->GetLength(); i++) {
            if ((*childrenNames)[i].Equals(name)) {
                result = ChildSpi(name);
                break;
            }
        }

        *apfs = result;
        REFCOUNT_ADD(*apfs);
    }

    return NOERROR;
}

Boolean AbstractPreferences::IsRemoved()
{
    AutoLock lock(mLock);
    return mIsRemoved;
}

ECode AbstractPreferences::GetAbsolutePath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    if (mParentPref == NULL) {
        *path = String("/");
        return NOERROR;
    } else if (mParentPref == mRoot) {
        *path = String("/") + mNodeName;
        return NOERROR;
    }

    String tmp;
    mParentPref->GetAbsolutePath(&tmp);
    *path = tmp + String("/") + mNodeName;
    return NOERROR;
}

ECode AbstractPreferences::GetChildrenNames(
    /* [out, callee] */ ArrayOf<String>** values) /*throws BackingStoreException*/
{
    VALIDATE_NOT_NULL(values);
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        AutoPtr<ITreeSet> result;
        CTreeSet::New((ITreeSet**)&result);

        HashMap<String, AutoPtr<AbstractPreferences> >::Iterator iter = mCachedNode.Begin();
        Boolean modified = FALSE;
        for (Int32 i = 0; iter != mCachedNode.End(); ++iter, i++) {
            AutoPtr<ICharSequence> name;
            CString::New(iter->mFirst, (ICharSequence**)&name);
            result->Add(name, &modified);
        }

        AutoPtr<ArrayOf<String> > names;
        FAIL_RETURN(ChildrenNamesSpi((ArrayOf<String>**)&names));
        for (Int32 i = 0; i < names->GetLength(); i++) {
            AutoPtr<ICharSequence> name;
            CString::New((*names)[i], (ICharSequence**)&name);
            result->Add(name, &modified);
        }

        Int32 size = 0;
        result->GetSize(&size);
        *values = ArrayOf<String>::Alloc(size);
        String str;
        AutoPtr<IIterator> iter2;
        result->GetIterator((IIterator**)&iter2);
        Boolean has = FALSE;
        Int32 pos = 0;
        while (iter2->HasNext(&has), has) {
            AutoPtr<IInterface> value;
            iter2->GetNext((IInterface**)&value);
            ICharSequence::Probe(value)->ToString(&str);
            (*values)->Set(pos, str);
            pos++;
        }
        REFCOUNT_ADD(*values);
    }

    return NOERROR;
}

ECode AbstractPreferences::Clear() /*throws BackingStoreException */
{
    synchronized (mLock) {
        AutoPtr<ArrayOf<String> > keys;
        FAIL_RETURN(GetKeys((ArrayOf<String>**)&keys));
        String key;
        for (Int32 i = 0; i < keys->GetLength(); i++) {
            FAIL_RETURN(Remove((*keys)[i]));
        }
    }
    return NOERROR;
}

ECode AbstractPreferences::ExportNode(
    /* [in] */ IOutputStream* ostream) /*throws IOException, BackingStoreException */
{
    if (ostream == NULL) {
        // throw new NullPointerException("ostream == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(CheckState());
    return XMLParser::ExportPrefs(this, ostream, FALSE);
}

ECode AbstractPreferences::ExportSubtree(
    /* [in] */ IOutputStream* ostream) /*throws IOException, BackingStoreException*/
{
    if (ostream == NULL) {
        // throw new NullPointerException("ostream == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(CheckState());
    return XMLParser::ExportPrefs(this, ostream, TRUE);
}

ECode AbstractPreferences::Flush() /*throws BackingStoreException */
{
    synchronized (mLock) {
        FAIL_RETURN(FlushSpi());
    }

    AutoPtr<ArrayOf<AbstractPreferences* > > cc = CachedChildren();
    Int32 i;
    for (i = 0; i < cc->GetLength(); i++) {
        FAIL_RETURN((*cc)[i]->Flush());
    }

    return NOERROR;
}

ECode AbstractPreferences::Get(
    /* [in] */ const String& key,
    /* [in] */ const String& deflt,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    if (key == NULL) {
        // throw new NullPointerException("key == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    String result;
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        // try {
        result = GetSpi(key);
        // } catch (Exception e) {
        //     // ignored
        // }
    }
    if (result == NULL) {
        *value = deflt;
    } else {
        *value = result;
    }
    return NOERROR;
}

ECode AbstractPreferences::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean deflt,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = deflt;
    String result;
    FAIL_RETURN(Get(key, String(NULL), &result));
    if (result == NULL) {
        return NOERROR;
    }

    if (String("TRUE").EqualsIgnoreCase(result)) {
        *value = TRUE;
        return NOERROR;
    } else if (String("FALSE").EqualsIgnoreCase(result)) {
        *value = FALSE;
        return NOERROR;
    }

    return NOERROR;
}

ECode AbstractPreferences::GetByteArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* deflt,
    /* [out] */ ArrayOf<Byte>** values)
{
    VALIDATE_NOT_NULL(values);
    *values = deflt;
    String svalue;
    ECode ec = Get(key, String(NULL), &svalue);
    if (FAILED(ec)) {
        REFCOUNT_ADD(*values);
        return ec;
    }

    if (svalue == NULL) {
        REFCOUNT_ADD(*values);
        return NOERROR;
    }
    if (svalue.GetLength() == 0) {
        *values = EmptyArray::BYTE;
        REFCOUNT_ADD(*values);
        return NOERROR;
    }
    // try {
    AutoPtr<ArrayOf<Byte> > bavalue;
    //TODO
    assert(0);
    // byte[] bavalue = svalue.getBytes(StandardCharsets.US_ASCII);
    if (bavalue->GetLength() % 4 != 0) {
        REFCOUNT_ADD(*values);
        return NOERROR;
    }

    AutoPtr<Libcore::IO::IBase64> base;
    CBase64::AcquireSingleton((Libcore::IO::IBase64**)&base);
    return base->Decode(bavalue, values);
    // } catch (Exception e) {
    //     return deflt;
    // }
}

ECode AbstractPreferences::GetDouble(
    /* [in] */ const String& key,
    /* [in] */ Double deflt,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);
    String result;
    Get(key, String(NULL), &result);
    if (result == NULL) {
        *value = deflt;
        return NOERROR;
    }
    // try {
    *value = StringUtils::ParseDouble(result);
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     return deflt;
    // }
}

ECode AbstractPreferences::GetFloat(
    /* [in] */ const String& key,
    /* [in] */ Float deflt,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    String result;
    Get(key, String(NULL), &result);
    if (result == NULL) {
        *value = deflt;
        return NOERROR;
    }
    // try {
    *value = StringUtils::ParseFloat(result);
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     return deflt;
    // }
}

ECode AbstractPreferences::GetInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 deflt,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    String result;
    Get(key, String(NULL), &result);
    if (result == NULL) {
        *value = deflt;
        return deflt;
    }

    // try {
    *value = StringUtils::ParseInt32(result);
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     return deflt;
    // }
}

ECode AbstractPreferences::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 deflt,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    String result;
    Get(key, String(NULL), &result);
    if (result == NULL) {
        *value = deflt;
        return NOERROR;
    }
    // try {
    *value = StringUtils::ParseInt64(result);
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     return deflt;
    // }
}

ECode AbstractPreferences::IsUserNode(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IPreferences> pfs;
    Preferences::UserRoot((IPreferences**)&pfs);
    *value = ((IPreferences*)mRoot->Probe(EIID_IPreferences) == pfs.Get());
    return NOERROR;
}

ECode AbstractPreferences::GetKeys(
    /* [out, callee] */ ArrayOf<String>** keys) /*throws BackingStoreException*/
{
    VALIDATE_NOT_NULL(keys);
    ECode ec = NOERROR;
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        ec = KeysSpi(keys);
    }
    return ec;
}

ECode AbstractPreferences::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mNodeName;
    return NOERROR;
}

ECode AbstractPreferences::GetNode(
    /* [in] */ const String& _name,
    /* [out] */ IPreferences** pfs)
{
    VALIDATE_NOT_NULL(pfs);
    AutoPtr<AbstractPreferences> startNode;
    String name(_name);
    synchronized (mLock) {
        CheckState();
        ValidateName(name);
        if (name.IsEmpty()) {
            *pfs = this;
            REFCOUNT_ADD(*pfs);
            return NOERROR;
        } else if (String("/").Equals(name)) {
            *pfs = (IPreferences*)mRoot->Probe(EIID_IPreferences);
            REFCOUNT_ADD(*pfs);
            return NOERROR;
        }
        if (name.StartWith("/")) {
            startNode = mRoot;
            name = name.Substring(1);
        } else {
            startNode = this;
        }
    }
    // try {
    AutoPtr<AbstractPreferences> tmp;
    ECode ec = startNode->NodeImpl(name, TRUE, (AbstractPreferences**)&tmp);

    if (ec == (Int32)E_BACKING_STORE_EXCEPTION) {
        *pfs = NULL;
        return NOERROR;
    }
    *pfs = (IPreferences*)tmp->Probe(EIID_IPreferences);
    REFCOUNT_ADD(*pfs);
    return NOERROR;
    // } catch (BackingStoreException e) {
    //     // should not happen
    //     return NULL;
    // }
}

ECode AbstractPreferences::ValidateName(
    /* [in] */ const String& name)
{
    if (name.EndWith("/") && name.GetLength() > 1) {
        // throw new IllegalArgumentException("Name cannot end with '/'");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (name.IndexOf("//") >= 0) {
        // throw new IllegalArgumentException("Name cannot contain consecutive '/' characters");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode AbstractPreferences::NodeImpl(
    /* [in] */ const String& path,
    /* [in] */ Boolean createNew,
    /* [out] */ AbstractPreferences** apfs)
{
    VALIDATE_NOT_NULL(apfs);
    *apfs = NULL;
    AutoPtr<ArrayOf<String> > names;
    StringUtils::Split(path, "/", (ArrayOf<String>**)&names);
    AutoPtr<AbstractPreferences> currentNode = this;
    AutoPtr<AbstractPreferences> temp;
    for (Int32 i = 0; i < names->GetLength(); i++) {
        String name = (*names)[i];
        {
            AutoLock lock(currentNode->mLock);
            temp = currentNode->mCachedNode[name];
            if (temp == NULL) {
                FAIL_RETURN(GetNodeFromBackend(createNew, currentNode, name, (AbstractPreferences**)&temp));
            }
        }
        currentNode = temp;
        if (currentNode == NULL) {
            break;
        }
    }
    *apfs = currentNode;
    REFCOUNT_ADD(*apfs);
    return NOERROR;
}

ECode AbstractPreferences::GetNodeFromBackend(
    /* [in] */ Boolean createNew,
    /* [in] */ AbstractPreferences* currentNode,
    /* [in] */ const String& name,
    /* [out] */ AbstractPreferences** apfs)
{
    VALIDATE_NOT_NULL(apfs);
    *apfs = NULL;
    if (name.GetLength() > MAX_NAME_LENGTH) {
        // throw new IllegalArgumentException("Name '" + name + "' too long");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<AbstractPreferences> temp;
    if (createNew) {
        temp = currentNode->ChildSpi(name);
        currentNode->mCachedNode[name] = temp;
        Int32 size = 0;
        if (temp->mNewNode && (ICollection::Probe(currentNode->mNodeChangeListeners)->GetSize(&size),size) > 0) {
            currentNode->NotifyChildAdded(temp);
        }
    } else {
        FAIL_RETURN(currentNode->GetChild(name, (AbstractPreferences**)&temp));
    }
    *apfs = temp;
    REFCOUNT_ADD(*apfs);
    return NOERROR;
}

ECode AbstractPreferences::IsNodeExists(
    /* [in] */ const String& _name,
    /* [out] */ Boolean* exist)
{
    VALIDATE_NOT_NULL(exist);
    *exist = FALSE;
    String name(_name);
    if (name == NULL) {
        // throw new NullPointerException("name == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<AbstractPreferences> startNode;
    synchronized (mLock) {
        if (IsRemoved()) {
            if (name.IsEmpty()) {
                return NOERROR;
            }
            // throw new IllegalStateException("This node has been removed");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        FAIL_RETURN(ValidateName(name));
        if (name.IsEmpty() || String("/").Equals(name)) {
            *exist = TRUE;
            return NOERROR;
        }
        if (name.StartWith("/")) {
            startNode = mRoot;
            name = name.Substring(1);
        } else {
            startNode = this;
        }
    }
    // try {
    AutoPtr<AbstractPreferences> tmp;
    ECode ec = startNode->NodeImpl(name, FALSE, (AbstractPreferences**)&tmp);
    if (ec == (Int32)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        return NOERROR;
    }
    AutoPtr<IPreferences> result = (IPreferences*)tmp->Probe(EIID_IPreferences);
    *exist = (result != NULL);
    return NOERROR;
    // } catch(IllegalArgumentException e) {
    //     return FALSE;
    // }
}

ECode AbstractPreferences::GetParent(
    /* [out] */IPreferences** pf)
{
    VALIDATE_NOT_NULL(pf);
    *pf = NULL;
    FAIL_RETURN(CheckState());
    *pf = (IPreferences*)mParentPref->Probe(EIID_IPreferences);
    REFCOUNT_ADD(*pf);
    return NOERROR;
}

ECode AbstractPreferences::CheckState()
{
    if (IsRemoved()) {
        // throw new IllegalStateException("This node has been removed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode AbstractPreferences::Put(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (key == NULL) {
        // throw new NullPointerException("key == NULL");
        return E_NULL_POINTER_EXCEPTION;
    } else if (value == NULL) {
        // throw new NullPointerException("value == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (key.GetLength() > MAX_KEY_LENGTH || value.GetLength() > MAX_VALUE_LENGTH) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        PutSpi(key, value);
    }
    NotifyPreferenceChange(key, value);
    return NOERROR;
}

ECode AbstractPreferences::PutBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    return Put(key, StringUtils::BooleanToString(value));
}

ECode AbstractPreferences::PutByteArray(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value)
{
    AutoPtr<Libcore::IO::IBase64> base;
    CBase64::AcquireSingleton((Libcore::IO::IBase64**)&base);
    String str;
    base->Encode(value, &str);
    return Put(key, str);
}

ECode AbstractPreferences::PutDouble(
    /* [in] */ const String& key,
    /* [in] */ Double value)
{
    return Put(key, StringUtils::ToString(value));
}

ECode AbstractPreferences::PutFloat(
    /* [in] */ const String& key,
    /* [in] */ Float value)
{
    return Put(key, StringUtils::ToString(value));
}

ECode AbstractPreferences::PutInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    return Put(key, StringUtils::ToString(value));
}

ECode AbstractPreferences::PutInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 value)
{
    return Put(key, StringUtils::ToString(value));
}

ECode AbstractPreferences::Remove(
    /* [in] */ const String& key)
{
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        RemoveSpi(key);
    }
    NotifyPreferenceChange(key, String(NULL));
    return NOERROR;
}

ECode AbstractPreferences::RemoveNode() /*throws BackingStoreException */
{
    if (mRoot.Get() == this) {
        // throw new UnsupportedOperationException("Cannot remove root node");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    AutoPtr<Object> lock = &mParentPref->mLock;
    synchronized (lock) {
        FAIL_RETURN(RemoveNodeImpl());
    }
    return NOERROR;
}

ECode AbstractPreferences::RemoveNodeImpl()/* throws BackingStoreException */
{
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        AutoPtr<ArrayOf<String> > childrenNames;
        FAIL_RETURN(ChildrenNamesSpi((ArrayOf<String>**)&childrenNames));
        for (Int32 i = 0; i < childrenNames->GetLength(); i++) {
            String childrenName = (*childrenNames)[i];
            AutoPtr<AbstractPreferences> apf = mCachedNode[childrenName];
            if (apf == NULL) {
                AutoPtr<AbstractPreferences> child = ChildSpi(childrenName);
                mCachedNode[childrenName] = child;
            }
        }

        HashMap<String, AutoPtr<AbstractPreferences> >::Iterator ator = mCachedNode.Begin();
        for (; ator != mCachedNode.End(); ++ator) {
            FAIL_RETURN(ator->mSecond->RemoveNodeImpl());
        }

        FAIL_RETURN(RemoveNodeSpi());
        mIsRemoved = TRUE;
        mParentPref->mCachedNode.Erase(mNodeName);
    }
    Int32 size = 0;
    ICollection::Probe(mParentPref->mNodeChangeListeners)->GetSize(&size);
    if (size > 0) {
        mParentPref->NotifyChildRemoved(this);
    }
    return NOERROR;
}

ECode AbstractPreferences::AddNodeChangeListener(
    /* [in] */ INodeChangeListener* ncl)
{
    if (ncl == NULL) {
        // throw new NullPointerException("ncl == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(CheckState());
    synchronized (mNodeChangeListeners) {
        IList::Probe(mNodeChangeListeners)->Add(ncl);
    }
    return NOERROR;
}

ECode AbstractPreferences::AddPreferenceChangeListener(
    /* [in] */ IPreferenceChangeListener* pcl)
{
    if (pcl == NULL) {
        // throw new NullPointerException("pcl == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(CheckState());
    synchronized (mPreferenceChangeListeners) {
        IList::Probe(mPreferenceChangeListeners)->Add(pcl);
    }
    return NOERROR;
}

ECode AbstractPreferences::RemoveNodeChangeListener(
    /* [in] */ INodeChangeListener* ncl)
{
    FAIL_RETURN(CheckState());
    synchronized (mNodeChangeListeners) {
        Int32 pos;
        IList::Probe(mNodeChangeListeners)->IndexOf(ncl, &pos);
        if (pos == -1) {
            // throw new IllegalArgumentException();
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IInterface> tmp;
        IList::Probe(mNodeChangeListeners)->Remove(pos, (IInterface**)&tmp);
    }
    return NOERROR;
}

ECode AbstractPreferences::RemovePreferenceChangeListener(
    /* [in] */ IPreferenceChangeListener* pcl)
{
    FAIL_RETURN(CheckState());
    synchronized (mPreferenceChangeListeners) {
        Int32 pos;
        IList::Probe(mPreferenceChangeListeners)->IndexOf(pcl, &pos);
        if (pos == -1) {
            // throw new IllegalArgumentException();
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IInterface> tmp;
        IList::Probe(mPreferenceChangeListeners)->Remove(pos, (IInterface**)&tmp);
    }
    return NOERROR;
}

ECode AbstractPreferences::Sync() /*throws BackingStoreException*/
{
    synchronized (mLock) {
        FAIL_RETURN(CheckState());
        FAIL_RETURN(SyncSpi());
    }
    AutoPtr<ArrayOf<AbstractPreferences* > > children = CachedChildren();
    for (Int32 i = 0; i < children->GetLength(); i++) {
        AutoPtr<AbstractPreferences> child = (*children)[i];
        FAIL_RETURN(child->Sync());
    }
    return NOERROR;
}

ECode AbstractPreferences::ToString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    Boolean userNode = FALSE;
    IsUserNode(&userNode);
    String path;
    GetAbsolutePath(&path);
    if (userNode) {
        *value = String("User") + String(" Preference Node: ") + path;
    } else {
        *value = String("System") + String(" Preference Node: ") + path;
    }
    return NOERROR;
}

void AbstractPreferences::NotifyChildAdded(
    /* [in] */ Preferences* child)
{
    AutoPtr<NodeChangeEvent> nce = new NodeAddEvent();
    nce->constructor(this, child);
    synchronized (sEvents) {
        sEvents->Add(nce->Probe(EIID_IInterface));
        ISynchronize::Probe(sEvents)->NotifyAll();
    }
}

void AbstractPreferences::NotifyChildRemoved(
    /* [in] */ Preferences* child)
{
    AutoPtr<NodeChangeEvent> nce = new NodeRemoveEvent();
    nce->constructor(this, child);
    synchronized (sEvents) {
        sEvents->Add(nce->Probe(EIID_IInterface));
        ISynchronize::Probe(sEvents)->NotifyAll();
    }
}

void AbstractPreferences::NotifyPreferenceChange(
    /* [in] */ const String& key,
    /* [in] */ const String& newValue)
{
    //TODO
    assert(0);
    // AutoPtr<PreferenceChangeEvent> pce = new PreferenceChangeEvent(this, key, newValue);
    // synchronized (sEvents) {
    //     sEvents->Add(nce->Probe(EIID_IInterface));
    //     ISynchronize::Probe(sEvents)->NotifyAll();
    // }
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
