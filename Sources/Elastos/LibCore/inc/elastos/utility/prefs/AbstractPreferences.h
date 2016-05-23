
#ifndef __ELASTOS_UTILITY_PREFS_ABSTRACTPREFERENCES_H__
#define __ELASTOS_UTILITY_PREFS_ABSTRACTPREFERENCES_H__

#include "Preferences.h"
#include "NodeChangeEvent.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Utility {
namespace Prefs {

class XMLParser;

class AbstractPreferences
    : public Preferences
    , public IAbstractPreferences
{
public:
    // class EventDispatcher extends Thread {
    //     EventDispatcher(String name){
    //         super(name);
    //     }

    //     @Override
    //     public void run() {
    //         while (true) {
    //             EventObject event;
    //             try {
    //                 event = getEventObject();
    //             } catch (InterruptedException e) {
    //                 e.printStackTrace();
    //                 continue;
    //             }
    //             AbstractPreferences pref = (AbstractPreferences) event.getSource();
    //             if (event instanceof NodeAddEvent) {
    //                 dispatchNodeAdd((NodeChangeEvent) event,
    //                         pref.nodeChangeListeners);
    //             } else if (event instanceof NodeRemoveEvent) {
    //                 dispatchNodeRemove((NodeChangeEvent) event,
    //                         pref.nodeChangeListeners);
    //             } else if (event instanceof PreferenceChangeEvent) {
    //                 dispatchPrefChange((PreferenceChangeEvent) event,
    //                         pref.preferenceChangeListeners);
    //             }
    //         }
    //     }

    //     private EventObject getEventObject() throws InterruptedException {
    //         {    AutoLock syncLock(events);
    //             if (events.isEmpty()) {
    //                 events.wait();
    //             }
    //             EventObject event = events.get(0);
    //             events.remove(0);
    //             return event;
    //         }
    //     }

    //     private void dispatchPrefChange(PreferenceChangeEvent event,
    //             List<EventListener> preferenceChangeListeners) {
    //         {    AutoLock syncLock(preferenceChangeListeners);
    //             for (EventListener preferenceChangeListener : preferenceChangeListeners) {
    //                 ((PreferenceChangeListener) preferenceChangeListener).preferenceChange(event);
    //             }
    //         }
    //     }

    //     private void dispatchNodeRemove(NodeChangeEvent event,
    //             List<EventListener> nodeChangeListeners) {
    //         {    AutoLock syncLock(nodeChangeListeners);
    //             for (EventListener nodeChangeListener : nodeChangeListeners) {
    //                 ((NodeChangeListener) nodeChangeListener).childRemoved(event);
    //             }
    //         }
    //     }

    //     private void dispatchNodeAdd(NodeChangeEvent event,
    //             List<EventListener> nodeChangeListeners) {
    //         {    AutoLock syncLock(nodeChangeListeners);
    //             for (EventListener nodeChangeListener : nodeChangeListeners) {
    //                 NodeChangeListener ncl = (NodeChangeListener) nodeChangeListener;
    //                 ncl.childAdded(event);
    //             }
    //         }
    //     }
    // };

    class NodeAddEvent
        : public NodeChangeEvent
    {
    public:

        CARAPI constructor(
            /* [in] */ Preferences* p,
            /* [in] */ Preferences* c);

    private:
        //The base class is NOT serializable, so this class isn't either.
        static const Int64 serialVersionUID;
    };

    class NodeRemoveEvent
        : public NodeChangeEvent
    {
    public:

    private:
        //The base class is NOT serializable, so this class isn't either.
        static const Int64 serialVersionUID;
    };

public:
    CAR_INTERFACE_DECL()
    /*
     * -----------------------------------------------------------
     * Methods inherited from Preferences
     * -----------------------------------------------------------
     */
    // @Override
    CARAPI GetAbsolutePath(
        /* [out] */ String* path);

    // @Override
    CARAPI GetChildrenNames(
        /* [out, callee] */ ArrayOf<String>** names) /*throws BackingStoreException*/;

    // @Override
    CARAPI Clear() /*throws BackingStoreException */;

    // @Override
    CARAPI ExportNode(
        /* [in] */ IOutputStream* ostream) /*throws IOException, BackingStoreException */;

    // @Override
    CARAPI ExportSubtree(
        /* [in] */ IOutputStream* ostream) /*throws IOException, BackingStoreException*/;

    // @Override
    CARAPI Flush() /*throws BackingStoreException */;

    // @Override
    CARAPI Get(
        /* [in] */ const String& key,
        /* [in] */ const String& deflt,
        /* [out] */ String* value);

    // @Override
    CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean deflt,
        /* [out] */ Boolean* value);

    // @Override
    CARAPI GetByteArray(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* deflt,
        /* [out] */ ArrayOf<Byte>** values);

    // @Override
    CARAPI GetDouble(
        /* [in] */ const String& key,
        /* [in] */ Double deflt,
        /* [out] */ Double* value);

    // @Override
    CARAPI GetFloat(
        /* [in] */ const String& key,
        /* [in] */ Float deflt,
        /* [out] */ Float* value);

    // @Override
    CARAPI GetInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 deflt,
        /* [out] */ Int32* value);

    // @Override
    CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 deflt,
        /* [out] */ Int64* value);

    // @Override
    CARAPI IsUserNode(
        /* [out] */ Boolean* value);

    // @Override
    CARAPI GetKeys(
        /* [out, callee] */ ArrayOf<String>** keys) /*throws BackingStoreException*/;

    // @Override
    CARAPI GetName(
        /* [out] */ String* name);

    // @Override
    CARAPI GetNode(
        /* [in] */ const String& name,
        /* [out] */ IPreferences** pfs);

    // @Override
    CARAPI IsNodeExists(
        /* [in] */ const String& name,
        /* [out] */ Boolean* exist) /*throws BackingStoreException*/;

    // @Override
    CARAPI GetParent(
        /* [out] */IPreferences** pf);

    // @Override
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    // @Override
    CARAPI PutBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean value);

    // @Override
    CARAPI PutByteArray(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value);

    // @Override
    CARAPI PutDouble(
        /* [in] */ const String& key,
        /* [in] */ Double value);

    // @Override
    CARAPI PutFloat(
        /* [in] */ const String& key,
        /* [in] */ Float value);

    // @Override
    CARAPI PutInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 value);

    // @Override
    CARAPI PutInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 value);

    // @Override
    CARAPI Remove(
        /* [in] */ const String& key);

    // @Override
    CARAPI RemoveNode() /*throws BackingStoreException */;

    CARAPI RemoveNodeImpl()/* throws BackingStoreException */;

    // @Override
    CARAPI AddNodeChangeListener(
        /* [in] */ INodeChangeListener* ncl);

    // @Override
    CARAPI AddPreferenceChangeListener(
        /* [in] */ IPreferenceChangeListener* pcl);

    // @Override
    CARAPI RemoveNodeChangeListener(
        /* [in] */ INodeChangeListener* ncl);

    // @Override
    CARAPI RemovePreferenceChangeListener(
        /* [in] */ IPreferenceChangeListener* pcl);

    // @Override
    CARAPI Sync() /*throws BackingStoreException*/;

    // @Override
    CARAPI ToString(
        /* [out] */ String* value);

protected:
    /*
     * -----------------------------------------------------------
     * Constructors
     * -----------------------------------------------------------
     */
    /**
     * Constructs a new {@code AbstractPreferences} instance using the given
     * parent node and node name.
     *
     * @param parent
     *            the parent node of the new node or {@code null} to indicate
     *            that the new node is a root node.
     * @param name
     *            the name of the new node or an empty string to indicate that
     *            this node is called "root".
     * @throws IllegalArgumentException
     *             if the name contains a slash character or is empty if {@code
     *             parent} is not {@code null}.
     */
    AbstractPreferences(
        /* [in] */ AbstractPreferences* parent,
        /* [in] */ const String& name);

    /*
     * -----------------------------------------------------------
     * Methods
     * -----------------------------------------------------------
     */
    /**
     * Returns an array of all cached child nodes.
     *
     * @return the array of cached child nodes.
     */
    CARAPI_(AutoPtr<ArrayOf<AbstractPreferences* > >) CachedChildren();

    /**
     * Returns the child node with the specified name or {@code null} if it
     * doesn't exist. Implementers can assume that the name supplied to this
     * method will be a valid node name string (conforming to the node naming
     * format) and will not correspond to a node that has been cached or
     * removed.
     *
     * @param name
     *            the name of the desired child node.
     * @return the child node with the given name or {@code null} if it doesn't
     *         exist.
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    CARAPI GetChild(
        /* [in] */ const String& name,
        /* [out] */ AbstractPreferences** apfs) /*throws BackingStoreException */;

    /**
     * Returns whether this node has been removed by invoking the method {@code
     * removeNode()}.
     *
     * @return {@code true}, if this node has been removed, {@code false}
     *         otherwise.
     */
    CARAPI_(Boolean) IsRemoved();

    /**
     * Flushes changes of this node to the backing store. This method should
     * only flush this node and should not include the descendant nodes. Any
     * implementation that wants to provide functionality to flush all nodes
     * at once should override the method {@link #flush() flush()}.
     *
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    virtual CARAPI FlushSpi() = 0 /*throws BackingStoreException*/;

    /**
     * Returns the names of all of the child nodes of this node or an empty
     * array if this node has no children. The names of cached children are not
     * required to be returned.
     *
     * @return the names of this node's children.
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    virtual CARAPI ChildrenNamesSpi(
        /* [out, callee] */ ArrayOf<String>** namesSpi) = 0/* throws BackingStoreException*/;

    /**
     * Returns the child preference node with the given name, creating it
     * if it does not exist. The caller of this method should ensure that the
     * given name is valid and that this node has not been removed or cached.
     * If the named node has just been removed, the implementation
     * of this method must create a new one instead of reactivating the removed
     * one.
     * <p>
     * The new creation is not required to be persisted immediately until the
     * flush method will be invoked.
     * </p>
     *
     * @param name
     *            the name of the child preference to be returned.
     * @return the child preference node.
     */
    virtual CARAPI_(AutoPtr<AbstractPreferences>) ChildSpi(
        /* [in] */ const String& name) = 0;

    /**
     * Puts the given key-value pair into this node. Caller of this method
     * should ensure that both of the given values are valid and that this
     * node has not been removed.
     *
     * @param name
     *            the given preference key.
     * @param value
     *            the given preference value.
     */
    virtual CARAPI PutSpi(
        /* [in] */ const String& name,
        /* [in] */ const String& value) = 0;

    /**
     * Gets the preference value mapped to the given key. The caller of this
     * method should ensure that the given key is valid and that this node has
     * not been removed. This method should not throw any exceptions but if it
     * does, the caller will ignore the exception, regarding it as a {@code
     * null} return value.
     *
     * @param key
     *            the given key to be searched for.
     * @return the preference value mapped to the given key.
     */
    virtual CARAPI_(String) GetSpi(
        /* [in] */ const String& key) = 0;


    /**
     * Returns an array of all preference keys of this node or an empty array if
     * no preferences have been found. The caller of this method should ensure
     * that this node has not been removed.
     *
     * @return the array of all preference keys.
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    virtual CARAPI KeysSpi(
        /* [out] */ ArrayOf<String>** spi) = 0/*throws BackingStoreException*/;

    /**
     * Removes this node from the preference hierarchy tree. The caller of this
     * method should ensure that this node has no child nodes, which means the
     * method {@link Preferences#removeNode() Preferences.removeNode()} should
     * invoke this method multiple-times in bottom-up pattern. The removal is
     * not required to be persisted until after it is flushed.
     *
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    virtual CARAPI RemoveNodeSpi() = 0/*throws BackingStoreException*/;

    /**
     * Removes the preference with the specified key. The caller of this method
     * should ensure that the given key is valid and that this node has not been
     * removed.
     *
     * @param key
     *            the key of the preference that is to be removed.
     */
    virtual CARAPI RemoveSpi(
        /* [in] */ const String& key) = 0;

    /**
     * Synchronizes this node with the backing store. This method should only
     * synchronize this node and should not include the descendant nodes. An
     * implementation that wants to provide functionality to synchronize all
     * nodes at once should override the method {@link #sync() sync()}.
     *
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    virtual CARAPI SyncSpi() = 0/*throws BackingStoreException*/;

private:
    static CARAPI_(Boolean) staticInit();

    CARAPI ValidateName(
        /* [in] */ const String& name);

    CARAPI NodeImpl(
        /* [in] */ const String& path,
        /* [in] */ Boolean createNew,
        /* [out] */ AbstractPreferences** apfs) /*throws BackingStoreException*/;

    CARAPI GetNodeFromBackend(
        /* [in] */ Boolean createNew,
        /* [in] */ AbstractPreferences* currentNode,
        /* [in] */ const String& name,
        /* [out] */ AbstractPreferences** apfs) /*throws BackingStoreException */;

    CARAPI CheckState();

    CARAPI_(void) NotifyChildAdded(
        /* [in] */ Preferences* child);

    CARAPI_(void) NotifyChildRemoved(
        /* [in] */ Preferences* child);

    CARAPI_(void) NotifyPreferenceChange(
        /* [in] */ const String& key,
        /* [in] */ const String& newValue);

protected:
    /*
     * -----------------------------------------------------------
     * Instance fields (package-private)
     * -----------------------------------------------------------
     */
    /** true if this node is in user preference hierarchy */
    Boolean mUserNode;

    /*
     * -----------------------------------------------------------
     * Instance fields (private)
     * -----------------------------------------------------------
     */
    /**
     * The object used to lock this node.
     */
    Object mLock;

    /**
     * This field is true if this node is created while it doesn't exist in the
     * backing store. This field's default value is false, and it is checked
     * when the node creation is completed, and if it is true, the node change
     * event will be fired for this node's parent.
     */
    Boolean mNewNode;

private:
    /*
     * -----------------------------------------------------------
     * Class fields
     * -----------------------------------------------------------
     */
    /** the unhandled events collection */
    // private static final List<EventObject> events = new LinkedList<EventObject>();
    static AutoPtr<IList> sEvents;
    /** the event dispatcher thread */
    // static AutoPtr<EventDispatcher> sDispatcher;

    static Boolean sStaticInit;

    /** cached child nodes */
    HashMap<String, AutoPtr<AbstractPreferences> > mCachedNode;

    //the collections of listeners
    AutoPtr<ILinkedList> mNodeChangeListeners;
    AutoPtr<ILinkedList> mPreferenceChangeListeners;

    //this node's name
    String mNodeName;

    //handler to this node's parent
    AutoPtr<AbstractPreferences> mParentPref;

    //true if this node has been removed
    Boolean mIsRemoved;

    //handler to this node's root node
    AutoPtr<AbstractPreferences> mRoot;

    friend class XMLParser;
};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_ABSTRACTPREFERENCES_H__
