
#ifndef __CMONKEYSOURCENETWORKVIEWS_H__
#define __CMONKEYSOURCENETWORKVIEWS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_CMonkeySourceNetworkViews.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IServiceManager;

using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::AccessibilityService::IUiTestAutomationBridge;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceNetworkViews)
{
private:
    CAR_OBJECT_DECL()

    /* Interface for view queries */
    class ViewIntrospectionCommand : public ElRefBase {
    public:
        /**
         * Get the response to the query
         * @return the response to the query
         */
         virtual CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret) = 0;
    };

public:
    static AutoPtr<IIPackageManager> InitStatics();

    /**
     * Registers the event listener for AccessibilityEvents.
     * Also sets up a communication connection so we can query the
     * accessibility service.
     */
     static CARAPI Setup();

     class ListViewsCommand : public ElRefBase, public IListViewsCommand {
        public:
            //listviews
            CAR_INTERFACE_DECL()

            CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
     };

     /**
     * A command that allows for querying of views. It takes an id type, the requisite ids,
     * and the command for querying the view.
     */
     class QueryViewCommand : public ElRefBase, public IQueryViewCommand
     {
     public:
        //queryview [id type] [id(s)] [command]
        //queryview viewid button1 gettext
        //queryview accessibilityids 12 5 getparent
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
     };

     /**
     * A command that returns the accessibility ids of the root view.
     */
     class GetRootViewCommand : public ElRefBase, public IGetRootViewCommand
     {
        // getrootview
     public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
     };

      /**
     * A command that returns the accessibility ids of the views that contain the given text.
     * It takes a string of text and returns the accessibility ids of the nodes that contain the
     * text as a list of integers separated by spaces.
     */
     class GetViewsWithTextCommand : public ElRefBase, public IGetViewsWithTextCommand
     {
        // getviewswithtext [text]
        // getviewswithtext "some text here"
     public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
     };

     /**
     * Command to retrieve the location of the given node.
     * Returns the x, y, width and height of the view, separated by spaces.
     */
     class GetLocation : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] getlocation
        //queryview viewid button1 getlocation
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

     /**
     * Command to retrieve the text of the given node
     */
     class GetText : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] gettext
        //queryview viewid button1 gettext
     public:
        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

      /**
     * Command to retrieve the class name of the given node
     */
     class GetClass : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] getclass
        //queryview viewid button1 getclass
     public:
        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

     /**
     * Command to retrieve the checked status of the given node
     */
     class GetChecked : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] getchecked
        //queryview viewid button1 getchecked
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

      /**
     * Command to retrieve whether the given node is enabled
     */
    class GetEnabled : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] getenabled
        //queryview viewid button1 getenabled
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

     /**
     * Command to retrieve whether the given node is selected
     */
     class GetSelected : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] getselected
        //queryview viewid button1 getselected
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

    /**
     * Command to set the selected status of the given node. Takes a boolean value as its only
     * argument.
     */
     class SetSelected : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] setselected [boolean]
        //queryview viewid button1 setselected true
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

     /**
     * Command to get whether the given node is focused.
     */
     class GetFocused : public ViewIntrospectionCommand
     {
        //queryview [id type] [id] getfocused
        //queryview viewid button1 getfocused
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

     /**
     * Command to set the focus status of the given node. Takes a boolean value
     * as its only argument.
     */
     class SetFocused : public ViewIntrospectionCommand
     {
       //queryview [id type] [id] setfocused [boolean]
        //queryview viewid button1 setfocused false
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

     /**
     * Command to get the accessibility ids of the given node. Returns the accessibility ids as a
     * space separated pair of integers with window id coming first, followed by the accessibility
     * view id.
     */
     class GetAccessibilityIds : public ViewIntrospectionCommand
     {
       //queryview [id type] [id] getaccessibilityids
        //queryview viewid button1 getaccessibilityids
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

     /**
     * Command to get the accessibility ids of the parent of the given node. Returns the
     * accessibility ids as a space separated pair of integers with window id coming first followed
     * by the accessibility view id.
     */
     class GetParent : public ViewIntrospectionCommand
     {
       //queryview [id type] [id] getaccessibilityids
        //queryview viewid button1 getaccessibilityids
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

      /**
     * Command to get the accessibility ids of the children of the given node. Returns the
     * children's ids as a space separated list of integer pairs. Each of the pairs consists of the
     * window id, followed by the accessibility id.
     */
    class GetChildren : public ViewIntrospectionCommand
     {
       //queryview [id type] [id] getchildren
        //queryview viewid button1 getchildren
     public:

        CARAPI Query(
            /* [in] */ IAccessibilityNodeInfo *node,
            /* [in] */ List<String> *args,
            /* [out] */IMonkeyCommandReturn **ret);
     };

    /**
     * Get the ID class for the given package.
     * This will cause issues if people reload a package with different
     * resource identifiers, but don't restart the Monkey server.
     *
     * @param packageName The package that we want to retrieve the ID class for
     * @return The ID class for the given package
     */
     /*Todo*/
     /*
     private static Class<?> getIdClass(String packageName, String sourceDir)
            throws ClassNotFoundException {
        // This kind of reflection is expensive, so let's only do it
        // if we need to
        Class<?> klass = sClassMap.get(packageName);
        if (klass == null) {
            DexClassLoader classLoader = new DexClassLoader(
                    sourceDir, "/data/local/tmp",
                    null, ClassLoader.getSystemClassLoader());
            klass = classLoader.loadClass(packageName + ".R$id");
            sClassMap.put(packageName, klass);
        }
        return klass;
    }*/

    static CARAPI_(String) GetPositionFromNode(IAccessibilityNodeInfo *node);

    /**
     * Converts a resource identifier into it's generated integer ID
     *
     * @param stringId the string identifier
     * @return the generated integer identifier.
     */
    static CARAPI_(Int32) GetId(
        /* [in] */ const String& stringId,
        /* [in] */ IAccessibilityEvent *event);

    static IAccessibilityNodeInfo* GetNodeByAccessibilityIds(
        /* [in] */ const String& windowString,
        /* [in] */ const String& viewString);

    static IAccessibilityNodeInfo* GetNodeByViewId(
        /* [in] */ const String& viewId,
        /* [in] */ IAccessibilityEvent *event);

    /**
     * Command to list all possible view ids for the given application.
     * This lists all view ids regardless if they are on screen or not.
     */

protected:
    static AutoPtr<IUiTestAutomationBridge> sUiTestAutomationBridge;

private:
    static AutoPtr<IIPackageManager> sPm;
    //Todo:
    //Class<?> klass = sClassMap.get(packageName);
    static AutoPtr<HashMap<String, void*> >  sClassMap;
    static const String REMOTE_ERROR;
    static const String CLASS_NOT_FOUND;
    static const String NO_ACCESSIBILITY_EVENT;
    static const String NO_NODE;
    static const String NO_CONNECTION;
    static AutoPtr<HashMap<String, AutoPtr<ViewIntrospectionCommand> > > COMMAND_MAP;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCENETWORKVIEWS_H__
