#ifndef EMBEDDEDNETWORKTYPES_H
#define EMBEDDEDNETWORKTYPES_H


#include <QByteArray>
#include <QList>
#include <QString>

namespace ew {
namespace network {
const quint8 m_protVer = 1; ///< Версия протокола
enum MessageClassType
{
    MT_STYLE_MESSAGE = 0,
    MT_DOX_MESSAGE
};
enum TransferType
{
    TT_REQUEST,
    TT_ANSWER
};
struct MessageHeader
{
    quint8 protocolVer = 1;
    MessageClassType messageClassType;
    quint32 messageType; ///< Style or Dox Msg
    quint32 dataSize;
    QByteArray data;
    MessageHeader(){data.clear(); protocolVer = m_protVer; messageClassType = MT_STYLE_MESSAGE; messageType = 0; dataSize = 0; }
    MessageHeader & operator=(const MessageHeader & prev);
    MessageHeader(const MessageHeader & prev);
    static QByteArray    serialize( const MessageHeader msg);
    static MessageHeader deserialize(QByteArray & msg);
};
struct BaseMsgGetClickedWidget
{
    QString widgetPath;
    static QByteArray              serialize(const BaseMsgGetClickedWidget & msg);
    static BaseMsgGetClickedWidget deserialize(QByteArray & msg);
};


struct MsgFile
{
    QString fullWidgetName;
    QString relFilePath;
    quint32 sz;
    QByteArray file;

    MsgFile(){sz = 0;}
    MsgFile & operator=(const MsgFile & prev);
    MsgFile(const MsgFile & prev);
    static QByteArray              serialize(const MsgFile & msg);
    static MsgFile deserialize(QByteArray & msg);
};


struct BaseMsgFiles
{
    QList<MsgFile> files;

    BaseMsgFiles(){}
    BaseMsgFiles & operator=(const BaseMsgFiles & prev);
    BaseMsgFiles(const BaseMsgFiles & prev);
    static QByteArray              serialize(const BaseMsgFiles & msg);
    static BaseMsgFiles deserialize(QByteArray & msg);
};


struct BaseMsgGetWidgetScreenShot
{
    QString widgetPath;
    QByteArray screenData;

    BaseMsgGetWidgetScreenShot(){}
    BaseMsgGetWidgetScreenShot & operator=(const BaseMsgGetWidgetScreenShot & prev);
    BaseMsgGetWidgetScreenShot(const BaseMsgGetWidgetScreenShot & prev);
    static QByteArray                 serialize(const BaseMsgGetWidgetScreenShot & msg);
    static BaseMsgGetWidgetScreenShot deserialize(QByteArray & msg);
};


/*************************************************************/
/*              Style Messages                              */
/************************************************************/
enum StyleMessageType
{
    SMT_NULL_MESSAGE = 0,
    //SMT_ENABLE_STYLE_MODE,  //null data
    SMT_GET_FULL_WIDGET_TREE, //StyleMsgGetWidgetTree
    //SMT_GET_CLICKED_WIDGET_TREE, //StyleMsgGetWidgetTree
    SMT_GET_CLICKED_WIDGET,//BaseMsgGetClickedWidget
    SMT_GET_WIDGET_SCREENSHOT,//BaseMsgGetWidgetScreenShot
    SMT_GET_STYLESHEET,//StyleMsgWidgetStylesheet
    SMT_SET_STYLESHEET,//StyleMsgWidgetStylesheet
    SMT_RESET_STYLES//null data
};
struct StyleTreeNode
{
    StyleTreeNode();
    static QByteArray       serialize(const StyleTreeNode & msg);
    static StyleTreeNode    deserialize(QByteArray & msg);
    StyleTreeNode & operator=(const StyleTreeNode & prev);

    StyleTreeNode(const StyleTreeNode & prev);
    quint64 id;
    quint64 parentId;
    QByteArray icon;
    QString className;
    QString objectName;
    QString title;
    bool hasStylesheet;
};
struct StyleMsgGetWidgetTree
{
    QList<StyleTreeNode> rootNodes;
    static QByteArray            serialize(const StyleMsgGetWidgetTree & msg);
    static StyleMsgGetWidgetTree deserialize(QByteArray & msg);
};
struct StyleMsgWidgetStylesheet
{
    QString widgetPath;
    QString widgetStylesheet;

    StyleMsgWidgetStylesheet(){}
    StyleMsgWidgetStylesheet & operator=(const StyleMsgWidgetStylesheet & prev);
    StyleMsgWidgetStylesheet(const StyleMsgWidgetStylesheet & prev);

    static QByteArray               serialize(const StyleMsgWidgetStylesheet & msg);
    static StyleMsgWidgetStylesheet deserialize(QByteArray & msg);
};



/*************************************************************/
/*              Doc Messages                                 */
/************************************************************/
enum DocMessageType
{
    DMT_NULL_MESSAGE = 0,
    DMT_ENABLE_DOX_MODE,  //null data
    DMT_GET_FULL_WIDGET_TREE, //DocMsgGetWidgetTree
    DMT_GET_CLICKED_WIDGET_TREE, //BaseMsgGetWidgetTree
    DMT_GET_CLICKED_WIDGET,//BaseMsgGetClickedWidget
    DMT_GET_WIDGET_SCREENSHOT,//BaseMsgGetWidgetScreenShot
    DMT_GET_DOC,//DocMsgWidgetDoc
    DMT_SET_DOC,//DocMsgWidgetDoc
    DMT_GET_FILES,//BaseMsgFilesS
    DMT_SET_FILES,//BaseMsgFiles
    DMT_DELETE_DOC,//DocMsgWidgetDoc
    DMT_SELECT_WIDGET,//BaseMsgGetClickedWidget
    DMT_SET_USER_TREE,//DocMsgGetUserTree
    DMT_GET_USER_TREE//DocMsgGetUserTree
};

struct UserTreeNode
{
    UserTreeNode();
    static QByteArray     serialize(const UserTreeNode & msg);
    static UserTreeNode    deserialize(QByteArray & msg);
    UserTreeNode & operator=(const UserTreeNode & prev);
    UserTreeNode(const UserTreeNode &prev);

    QString tag;
    QString parentTag;
    QString title;
};
struct DocMsgGetUserTree
{
    QList<UserTreeNode> rootNodes;
    static QByteArray          serialize(const DocMsgGetUserTree & msg);
    static DocMsgGetUserTree deserialize(QByteArray & msg);
};

struct DocTreeNode
{
    DocTreeNode();
    static QByteArray     serialize(const DocTreeNode & msg);
    static DocTreeNode    deserialize(QByteArray & msg);
    DocTreeNode & operator=(const DocTreeNode & prev);

    DocTreeNode(const DocTreeNode &prev);
    QString tag;
    QString parentTag;
    QByteArray icon;
    QString className;
    QString objectName;
    QString title;
    bool unique;
    bool hasDoc;
    bool hasHeader;
};
struct DocMsgGetWidgetTree
{
    QList<DocTreeNode> rootNodes;
    static QByteArray          serialize(const DocMsgGetWidgetTree & msg);
    static DocMsgGetWidgetTree deserialize(QByteArray & msg);
};

struct DocMsgWidgetDoc
{
    QString widgetPath;
    QString header;
    QString widgetShortDoc;
    QString widgetFullDoc;
    BaseMsgFiles files;


    DocMsgWidgetDoc(){widgetPath = ""; header = ""; widgetFullDoc = ""; widgetShortDoc = "";}

    DocMsgWidgetDoc & operator=(const DocMsgWidgetDoc & prev);

    DocMsgWidgetDoc(const DocMsgWidgetDoc & prev);
    static QByteArray      serialize(const DocMsgWidgetDoc & msg);
    static DocMsgWidgetDoc deserialize(QByteArray & msg);
};
}
}
#endif // EMBEDDEDNETWORKTYPES_H
