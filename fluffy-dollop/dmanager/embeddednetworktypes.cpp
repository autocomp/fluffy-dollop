#include "dataserializer.h"
#include "embeddednetworktypes.h"



using namespace ew::network;
ew::network::StyleTreeNode::StyleTreeNode()
{
    id = 0;
    parentId = 0;
    hasStylesheet = false;
}


QByteArray ew::network::StyleTreeNode::serialize(const ew::network::StyleTreeNode &msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.id, ar);
    DataSerializer::serializeRAW(msg.parentId, ar);
    DataSerializer::serializeRAW(msg.className, ar);
    DataSerializer::serializeRAW(msg.objectName, ar);
    DataSerializer::serializeRAW(msg.title, ar);
    DataSerializer::serializeRAW(msg.icon, ar);
    DataSerializer::serializeRAW(msg.hasStylesheet, ar);

    return ar;
}


ew::network::StyleTreeNode ew::network::StyleTreeNode::deserialize(QByteArray &msg)
{
    StyleTreeNode node;

    node.id = DataSerializer::deserializeUINT64RAW(msg);
    node.parentId = DataSerializer::deserializeUINT64RAW(msg);
    node.className = DataSerializer::deserializeQStringRAW(msg);
    node.objectName = DataSerializer::deserializeQStringRAW(msg);
    node.title = DataSerializer::deserializeQStringRAW(msg);
    node.icon = DataSerializer::deserializeQByteArrayRAW(msg);
    node.hasStylesheet = DataSerializer::deserializeBOOLRAW(msg);
    return node;
}


StyleTreeNode &StyleTreeNode::operator=(const StyleTreeNode &prev)
{
    id = prev.id;
    parentId = prev.parentId;
    icon = prev.icon;
    className  = prev.className;
    objectName = prev.objectName;
    title = prev.title;
    hasStylesheet = prev.hasStylesheet;


    return *this;
}


StyleTreeNode::StyleTreeNode(const StyleTreeNode &prev)
{
    id = prev.id;
    parentId = prev.parentId;
    icon = prev.icon;
    className  = prev.className;
    objectName = prev.objectName;
    title = prev.title;
    hasStylesheet = prev.hasStylesheet;
}


QByteArray StyleMsgGetWidgetTree::serialize(const StyleMsgGetWidgetTree &msg)
{
    QByteArray ar;
    quint64 elemSize = msg.rootNodes.count();

    DataSerializer::serializeRAW(elemSize, ar);
    foreach (StyleTreeNode node, msg.rootNodes)
    {
        ar.append(StyleTreeNode::serialize(node));
    }

    return ar;
}


StyleMsgGetWidgetTree StyleMsgGetWidgetTree::deserialize(QByteArray &msg)
{
    QByteArray arr = msg;
    StyleMsgGetWidgetTree treeMsg;
    quint64 elemSize = DataSerializer::deserializeUINT64RAW(arr);

    QList<StyleTreeNode> rootNodes;

    for(quint64 i = 0; i < elemSize; i++)
    {
        auto node = StyleTreeNode::deserialize(arr);
        rootNodes.append(node);
    }

    treeMsg.rootNodes = rootNodes;

    return treeMsg;
}


QByteArray BaseMsgGetClickedWidget::serialize(const BaseMsgGetClickedWidget &msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.widgetPath, ar);
    return ar;
}


BaseMsgGetClickedWidget BaseMsgGetClickedWidget::deserialize(QByteArray &msg)
{
    BaseMsgGetClickedWidget msgClick;

    msgClick.widgetPath = DataSerializer::deserializeQStringRAW(msg);
    return msgClick;
}


BaseMsgGetWidgetScreenShot &BaseMsgGetWidgetScreenShot::operator=(const BaseMsgGetWidgetScreenShot &prev)
{
    widgetPath = prev.widgetPath;
    screenData = prev.screenData;
    return *this;
}


BaseMsgGetWidgetScreenShot::BaseMsgGetWidgetScreenShot(const BaseMsgGetWidgetScreenShot &prev)
{
    widgetPath = prev.widgetPath;
    screenData = prev.screenData;
}


QByteArray BaseMsgGetWidgetScreenShot::serialize(const BaseMsgGetWidgetScreenShot &msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.widgetPath, ar);
    DataSerializer::serializeRAW(msg.screenData, ar);
    return ar;
}


BaseMsgGetWidgetScreenShot BaseMsgGetWidgetScreenShot::deserialize(QByteArray &msg)
{
    BaseMsgGetWidgetScreenShot msgScreen;

    msgScreen.widgetPath = DataSerializer::deserializeQStringRAW(msg);
    msgScreen.screenData = DataSerializer::deserializeQByteArrayRAW(msg);
    return msgScreen;
}


StyleMsgWidgetStylesheet &StyleMsgWidgetStylesheet::operator=(const StyleMsgWidgetStylesheet &prev)
{
    widgetPath = prev.widgetPath;
    widgetStylesheet = prev.widgetStylesheet;
    return *this;
}


StyleMsgWidgetStylesheet::StyleMsgWidgetStylesheet(const StyleMsgWidgetStylesheet &prev)
{
    widgetPath = prev.widgetPath;
    widgetStylesheet = prev.widgetStylesheet;
}


QByteArray StyleMsgWidgetStylesheet::serialize(const StyleMsgWidgetStylesheet &msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.widgetPath, ar);
    DataSerializer::serializeRAW(msg.widgetStylesheet, ar);
    return ar;
}


StyleMsgWidgetStylesheet StyleMsgWidgetStylesheet::deserialize(QByteArray &msg)
{
    StyleMsgWidgetStylesheet msgScreen;

    msgScreen.widgetPath = DataSerializer::deserializeQStringRAW(msg);
    msgScreen.widgetStylesheet = DataSerializer::deserializeQStringRAW(msg);
    return msgScreen;
}


MessageHeader &MessageHeader::operator=(const MessageHeader &prev)
{
    protocolVer = prev.protocolVer;
    messageType = prev.messageType;
    messageClassType = prev.messageClassType;
    data = prev.data;
    dataSize = prev.dataSize;

    return *this;
}


MessageHeader::MessageHeader(const MessageHeader &prev)
{
    protocolVer = prev.protocolVer;
    messageType = prev.messageType;
    messageClassType = prev.messageClassType;
    data = prev.data;
    dataSize = prev.dataSize;
}


QByteArray MessageHeader::serialize(const MessageHeader msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.protocolVer, ar);
    DataSerializer::serializeRAW((quint32)msg.messageClassType, ar);
    DataSerializer::serializeRAW((quint32)msg.messageType, ar);
    DataSerializer::serializeRAW(msg.dataSize, ar);
    DataSerializer::serializeRAW(msg.data, ar);
    return ar;
}


MessageHeader MessageHeader::deserialize(QByteArray &msg)
{
    MessageHeader head;

    head.protocolVer = DataSerializer::deserializeUINT8RAW(msg);
    head.messageClassType = (MessageClassType)DataSerializer::deserializeUINT32RAW(msg);
    head.messageType = DataSerializer::deserializeUINT32RAW(msg);
    head.dataSize = DataSerializer::deserializeUINT32RAW(msg);
    head.data = DataSerializer::deserializeQByteArrayRAW(msg);
    return head;
}


ew::network::DocTreeNode::DocTreeNode()
{
    hasDoc = false;
    hasHeader = true;
    unique = true;
}


QByteArray ew::network::DocTreeNode::serialize(const ew::network::DocTreeNode &msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.tag, ar);
    DataSerializer::serializeRAW(msg.parentTag, ar);
    DataSerializer::serializeRAW(msg.className, ar);
    DataSerializer::serializeRAW(msg.objectName, ar);
    DataSerializer::serializeRAW(msg.title, ar);
    DataSerializer::serializeRAW(msg.icon, ar);
    DataSerializer::serializeRAW(msg.hasDoc, ar);
    DataSerializer::serializeRAW(msg.hasHeader, ar);
    DataSerializer::serializeRAW(msg.unique, ar);

    return ar;
}


ew::network::DocTreeNode ew::network::DocTreeNode::deserialize(QByteArray &msg)
{
    DocTreeNode node;

    node.tag = DataSerializer::deserializeQStringRAW(msg);
    node.parentTag = DataSerializer::deserializeQStringRAW(msg);
    node.className = DataSerializer::deserializeQStringRAW(msg);
    node.objectName = DataSerializer::deserializeQStringRAW(msg);
    node.title = DataSerializer::deserializeQStringRAW(msg);
    node.icon = DataSerializer::deserializeQByteArrayRAW(msg);
    node.hasDoc = DataSerializer::deserializeBOOLRAW(msg);
    node.hasHeader = DataSerializer::deserializeBOOLRAW(msg);
    node.unique = DataSerializer::deserializeBOOLRAW(msg);

    return node;
}


DocTreeNode &DocTreeNode::operator=(const DocTreeNode &prev)
{
    tag = prev.tag;
    parentTag = prev.parentTag;
    icon = prev.icon;
    className  = prev.className;
    objectName = prev.objectName;
    title = prev.title;
    hasDoc = prev.hasDoc;
    hasHeader = prev.hasHeader;
    unique = prev.unique;
    return *this;
}


DocTreeNode::DocTreeNode(const DocTreeNode &prev)
{
    tag = prev.tag;
    parentTag = prev.parentTag;
    icon = prev.icon;
    className  = prev.className;
    objectName = prev.objectName;
    title = prev.title;
    hasDoc = prev.hasDoc;
    hasHeader = prev.hasHeader;
    unique = prev.unique;
}


QByteArray DocMsgGetWidgetTree::serialize(const DocMsgGetWidgetTree &msg)
{
    QByteArray ar;
    quint64 elemSize = msg.rootNodes.count();

    DataSerializer::serializeRAW(elemSize, ar);
    foreach (DocTreeNode node, msg.rootNodes)
    {
        ar.append(DocTreeNode::serialize(node));
    }

    return ar;
}


DocMsgGetWidgetTree DocMsgGetWidgetTree::deserialize(QByteArray &msg)
{
    DocMsgGetWidgetTree treeMsg;
    quint64 elemSize = DataSerializer::deserializeUINT64RAW(msg);

    for(quint64 i = 0; i < elemSize; i++)
    {
        treeMsg.rootNodes.append(DocTreeNode::deserialize(msg));
    }

    return treeMsg;
}


DocMsgWidgetDoc &DocMsgWidgetDoc::operator=(const DocMsgWidgetDoc &prev)
{
    widgetPath = prev.widgetPath;
    widgetFullDoc = prev.widgetFullDoc;
    widgetShortDoc = prev.widgetShortDoc;
    header = prev.header;
    files = prev.files;
    return *this;
}


DocMsgWidgetDoc::DocMsgWidgetDoc(const DocMsgWidgetDoc &prev)
{
    widgetPath = prev.widgetPath;
    widgetFullDoc = prev.widgetFullDoc;
    widgetShortDoc = prev.widgetShortDoc;
    header = prev.header;
    files = prev.files;
}


QByteArray DocMsgWidgetDoc::serialize(const DocMsgWidgetDoc &msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.widgetPath, ar);
    DataSerializer::serializeRAW(msg.header, ar);
    DataSerializer::serializeRAW(msg.widgetShortDoc, ar);
    DataSerializer::serializeRAW(msg.widgetFullDoc, ar);
    ar.append(BaseMsgFiles::serialize(msg.files));

    return ar;
}


DocMsgWidgetDoc DocMsgWidgetDoc::deserialize(QByteArray &msg)
{
    DocMsgWidgetDoc msgScreen;

    msgScreen.widgetPath = DataSerializer::deserializeQStringRAW(msg);
    msgScreen.header = DataSerializer::deserializeQStringRAW(msg);
    msgScreen.widgetShortDoc = DataSerializer::deserializeQStringRAW(msg);
    msgScreen.widgetFullDoc = DataSerializer::deserializeQStringRAW(msg);
    msgScreen.files = BaseMsgFiles::deserialize(msg);

    return msgScreen;
}

MsgFile &MsgFile::operator=(const MsgFile &prev)
{
    fullWidgetName = prev.fullWidgetName;
    relFilePath = prev.relFilePath;
    sz = prev.sz;
    file = prev.file;

    return *this;
}

MsgFile::MsgFile(const MsgFile &prev)
{
    fullWidgetName = prev.fullWidgetName;
    relFilePath = prev.relFilePath;
    sz = prev.sz;
    file = prev.file;
}

QByteArray MsgFile::serialize(const MsgFile &msg)
{
    QByteArray ar;

        DataSerializer::serializeRAW(msg.fullWidgetName, ar);
    DataSerializer::serializeRAW(msg.relFilePath, ar);
    DataSerializer::serializeRAW(msg.sz, ar);
    DataSerializer::serializeRAW(msg.file, ar);

    return ar;
}

MsgFile MsgFile::deserialize(QByteArray &msg)
{
    MsgFile msgFile;

        msgFile.fullWidgetName = DataSerializer::deserializeQStringRAW(msg);
    msgFile.relFilePath = DataSerializer::deserializeQStringRAW(msg);
    msgFile.sz = DataSerializer::deserializeUINT32RAW(msg);
    msgFile.file = DataSerializer::deserializeQByteArrayRAW(msg);

    return msgFile;
}


BaseMsgFiles &BaseMsgFiles::operator=(const BaseMsgFiles &prev)
{
    files = prev.files;

    return *this;
}

BaseMsgFiles::BaseMsgFiles(const BaseMsgFiles &prev)
{
    files = prev.files;
}

QByteArray BaseMsgFiles::serialize(const BaseMsgFiles &msg)
{
    QByteArray ar;

    quint32 count = msg.files.size();
    DataSerializer::serializeRAW(count, ar);

    for(quint32 i =0; i<count; i++)
    {
        ar.append(MsgFile::serialize(msg.files.at(i)));
    }

    return ar;
}

BaseMsgFiles BaseMsgFiles::deserialize(QByteArray &msg)
{
    BaseMsgFiles files;

    quint32 count = DataSerializer::deserializeUINT32RAW(msg);

    for(quint32 i =0; i<count; i++)
    {
        files.files.append(MsgFile::deserialize(msg));
    }

    return files;
}

UserTreeNode::UserTreeNode()
{

}

QByteArray UserTreeNode::serialize(const UserTreeNode &msg)
{
    QByteArray ar;

    DataSerializer::serializeRAW(msg.tag, ar);
    DataSerializer::serializeRAW(msg.parentTag, ar);
    DataSerializer::serializeRAW(msg.title, ar);

    return ar;
}

UserTreeNode UserTreeNode::deserialize(QByteArray &msg)
{
    UserTreeNode node;

    node.tag = DataSerializer::deserializeQStringRAW(msg);
    node.parentTag = DataSerializer::deserializeQStringRAW(msg);
    node.title = DataSerializer::deserializeQStringRAW(msg);
    return node;
}

UserTreeNode &UserTreeNode::operator=(const UserTreeNode &prev)
{
    tag = prev.tag;
    parentTag = prev.parentTag;
    title = prev.title;
    return *this;
}

UserTreeNode::UserTreeNode(const UserTreeNode &prev)
{
    tag = prev.tag;
    parentTag = prev.parentTag;
    title = prev.title;
}

QByteArray DocMsgGetUserTree::serialize(const DocMsgGetUserTree &msg)
{
    QByteArray ar;
    quint64 elemSize = msg.rootNodes.count();

    DataSerializer::serializeRAW(elemSize, ar);
    foreach (UserTreeNode node, msg.rootNodes)
    {
        ar.append(UserTreeNode::serialize(node));
    }

    return ar;
}

DocMsgGetUserTree DocMsgGetUserTree::deserialize(QByteArray &msg)
{
    DocMsgGetUserTree treeMsg;
    quint64 elemSize = DataSerializer::deserializeUINT64RAW(msg);

    for(quint64 i = 0; i < elemSize; i++)
    {
        treeMsg.rootNodes.append(UserTreeNode::deserialize(msg));
    }

    return treeMsg;
}

