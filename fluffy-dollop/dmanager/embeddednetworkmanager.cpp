#include "dataserializer.h"
#include "docdebugger.h"
#include "embeddedapp.h"
#include "embeddednetworkmanager.h"
#include "styledebugger.h"
#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>


using namespace ew;
using namespace ew::network;

EmbeddedNetworkManager::EmbeddedNetworkManager(QObject *parent) : QObject(parent)
{
    m_pServer = 0;
    m_pClient = 0;
    m_pConnectedSocket = 0;
}


EmbeddedNetworkManager *EmbeddedNetworkManager::instance()
{
    static EmbeddedNetworkManager manager;

    return &manager;
}


void EmbeddedNetworkManager::init(EmbeddedNetworkManager::WorkMode wm, quint16 port)
{
    m_port = port;
    m_mode = wm;

    switch(wm)
    {
      case WM_CLIENT:
      {
          if(nullptr != m_pClient)
          {
              return;
          }

          if(nullptr != m_pServer)
          {
              delete m_pServer;
              m_pServer = 0;
          }

          m_pClient = new QTcpSocket(this);

          connect(m_pClient, SIGNAL(connected()), this, SLOT(slotConnectedWithServer()));
          connect(m_pClient, SIGNAL(readyRead()), this, SLOT(slotDataAvailableFromServer()));
      } break;
      case WM_SERVER:
      {
          if(nullptr != m_pServer)
          {
              return;
          }

          if(nullptr != m_pClient)
          {
              delete m_pClient;
              m_pClient = 0;
          }

          m_pServer = new QTcpServer(this);
          bool res = m_pServer->listen(QHostAddress::Any, m_port);

          if(!res)
          {
              qWarning() << "EmbeddedNetworkManager::init(): listen error: " << m_pServer->errorString();
          }

          connect(m_pServer, SIGNAL(newConnection()), this, SLOT(slotClientConnect()));

          connect(StyleDebugger::instance(), SIGNAL(signalWidgetClicked(QString))
                  , this, SLOT(slotWidgetSelected(QString)));
          //connect(DocDebugger::instance(), SIGNAL(signalWidgetClicked(QString))
          //        , this, SLOT(slotWidgetSelected(QString)));
      } break;
      default:
          break;
    }
}





void EmbeddedNetworkManager::sendMessage(MessageClassType mt, quint32 messageType, QByteArray data)
{
    if(WM_CLIENT == m_mode)
    {
        while(QTcpSocket::ConnectedState == m_pClient->state())
        {
            qApp->processEvents();
        }

        if(QTcpSocket::ConnectedState != m_pClient->state())
        {
            m_pClient->connectToHost("localhost", m_port);
            m_pClient->waitForConnected(10000);
        }

        MessageHeader head;
        head.data = data;
        head.dataSize = head.data.size();
        head.messageClassType = mt;
        head.messageType = messageType;
        head.protocolVer = m_protVer;
        QByteArray arr = MessageHeader::serialize(head);
        int sz = m_pClient->write(arr);

        while(sz != arr.size())
        {
            qWarning() << "EmbeddedNetworkManager::sendMessage: check sended data size. Write " << sz << " from " << arr.size();
            arr = arr.right(arr.size()-sz);
            sz = m_pClient->write(arr);
            if(-1 == sz )
            {
                qWarning()<<tr("* Something gone wrong: Check target application");
                return;
            }
        }

        m_pClient->waitForBytesWritten(3000);
    }
    else
    {
        if(m_pConnectedSocket->isOpen())
        {
            MessageHeader head;
            head.data = data;
            head.dataSize = head.data.size();
            head.messageClassType = mt;
            head.messageType = messageType;
            head.protocolVer = m_protVer;
            QByteArray arr = MessageHeader::serialize(head);
            bool sz = m_pClient->write(arr);

            while(sz != arr.size())
            {
                qWarning() << "EmbeddedNetworkManager::sendMessage: check sended data size. Write " << sz << " from " << arr.size();
                arr = arr.right(arr.size()-sz);
                sz = m_pClient->write(arr);
            }

            m_pClient->waitForBytesWritten(3000);
        }
    }
}


void EmbeddedNetworkManager::slotClientConnect()
{
    if(nullptr != m_pConnectedSocket)
    {
        m_pConnectedSocket->close();
        delete m_pConnectedSocket;

        if(!m_pConnectedSocket->isValid() && m_pConnectedSocket->isOpen())
        {
            m_pConnectedSocket = m_pServer->nextPendingConnection();
            connect(m_pConnectedSocket, SIGNAL(readyRead()), this, SLOT(slotClientReceivedData()));
        }
    }
    else
    {
        m_pConnectedSocket = m_pServer->nextPendingConnection();
        connect(m_pConnectedSocket, SIGNAL(readyRead()), this, SLOT(slotClientReceivedData()));
    }
}





void EmbeddedNetworkManager::slotConnectedWithServer()
{
}


void EmbeddedNetworkManager::slotWidgetSelected(QString widgetFullName)
{
    QByteArray resArr;
    MessageHeader head;

    head.messageClassType = m_lastMessageClassType;
    head.messageType = m_lastMessageType;
    head.protocolVer = m_protVer;



    if(MT_STYLE_MESSAGE == m_lastMessageClassType)
    {
        switch(m_lastMessageType)
        {
          case SMT_GET_CLICKED_WIDGET://StyleMsgGetClickedWidget
          {
              BaseMsgGetClickedWidget msg;
              msg.widgetPath = widgetFullName;
              resArr = BaseMsgGetClickedWidget::serialize(msg);
          } break;
          case SMT_GET_WIDGET_SCREENSHOT://StyleMsgGetWidgetScreenShot
          case SMT_GET_STYLESHEET://StyleMsgWidgetStylesheet
          case SMT_SET_STYLESHEET://StyleMsgWidgetStylesheet
          case SMT_GET_FULL_WIDGET_TREE: //StyleMsgGetWidgetTree
          case SMT_RESET_STYLES://null data
          case SMT_NULL_MESSAGE:
          default:
          {
              return;
          } break;
        }
    }

    if(MT_DOX_MESSAGE == m_lastMessageClassType)
    {
        switch(m_lastMessageType)
        {
          case DMT_GET_CLICKED_WIDGET://StyleMsgGetClickedWidget
          {
              BaseMsgGetClickedWidget msg;
              msg.widgetPath = widgetFullName;
              resArr = BaseMsgGetClickedWidget::serialize(msg);
          } break;
          case DMT_GET_WIDGET_SCREENSHOT://StyleMsgGetWidgetScreenShot
          case DMT_GET_DOC://StyleMsgWidgetStylesheet
          case DMT_SET_DOC://StyleMsgWidgetStylesheet
          case DMT_GET_FULL_WIDGET_TREE: //StyleMsgGetWidgetTree
          case DMT_DELETE_DOC://null data
          case DMT_NULL_MESSAGE:
          default:
          {
              return;
          } break;
        }
    }

    head.data = resArr;
    head.dataSize = resArr.size();

    QByteArray headArr = MessageHeader::serialize(head);

    m_pConnectedSocket->write(headArr);
    m_pConnectedSocket->waitForBytesWritten();
    m_pConnectedSocket->close();
    delete m_pConnectedSocket;
    m_pConnectedSocket = 0;


    return;
}

void EmbeddedNetworkManager::slotDataAvailableFromServer() //slot ReadyRead server;
{
    if(m_pClient)
    {
        QByteArray fullArr = m_pClient->readAll();
        QByteArray arrCopy = fullArr;
        MessageHeader head = MessageHeader::deserialize(arrCopy);

        while((fullArr.size() < 10) || (head.dataSize != (quint64)head.data.size()))
        {
            //quint64 ssz = (quint64)head.data.size();
            qApp->processEvents();
            fullArr = fullArr.append(m_pClient->readAll());
            arrCopy = fullArr;
            head = MessageHeader::deserialize(arrCopy);
        }

        head = MessageHeader::deserialize(fullArr);

        if( MT_STYLE_MESSAGE == head.messageClassType)
        {
            StyleDebugger::instance()->setReceivedMessage((StyleMessageType)head.messageType, head.data);
        }

        if( MT_DOX_MESSAGE == head.messageClassType)
        {
            //DocDebugger::instance()->setReceivedMessage((DocMessageType)head.messageType, head.data);
        }
    }
}

void EmbeddedNetworkManager::slotClientReceivedData() // slot readyread client
{
    if(m_pConnectedSocket)
    {
        QByteArray arr = m_pConnectedSocket->readAll();
        QByteArray arrCopy = arr;
        MessageHeader head = MessageHeader::deserialize(arrCopy);

        while(arr.size() < 10 || head.dataSize != (quint32)head.data.size())
        {
            qApp->processEvents();
            arr = arr.append(m_pConnectedSocket->readAll());
            arrCopy = arr;
            head = MessageHeader::deserialize(arrCopy);
        }

        if(head.dataSize != (quint32)head.data.size())
        {
            qWarning() << "EmbeddedNetworkManager::slotClientConnect: datasize mismatch";
            m_pConnectedSocket->close();
            m_pConnectedSocket = 0;
        }

        if(head.protocolVer != m_protVer)
        {
            qWarning() << "EmbeddedNetworkManager::slotClientConnect: protocol ver mismatch";
            m_pConnectedSocket->close();
            m_pConnectedSocket = 0;
        }


        m_lastMessageType = head.messageType;
        m_lastMessageClassType = head.messageClassType;


        bool asyncAnswer = false;

        switch(head.messageClassType)
        {
        case MT_STYLE_MESSAGE:
        {
            arr = StyleDebugger::instance()->getStyleMsgAnswer((StyleMessageType)head.messageType, head.data, asyncAnswer);
        } break;
        case MT_DOX_MESSAGE:
        {
            //DocMessageType answMsgType = (DocMessageType)head.messageType;
            //arr = DocDebugger::instance()->getDocMsgAnswer((DocMessageType)head.messageType, head.data, asyncAnswer, answMsgType);
            //head.messageType = answMsgType;
        } break;
        default:
            break;
        }

        if(asyncAnswer)
        {
            return;
        }

        if(arr.isEmpty())
        {
            m_pConnectedSocket->close();
            m_pConnectedSocket = 0;
            return;
        }
        else
        {
            head.data = arr;
            head.dataSize = arr.size();
            QByteArray mesArr = MessageHeader::serialize(head);
            int sendSize = 2048;
            int i = 0;

            if(mesArr.size() > sendSize)
            {
                while(i < mesArr.size())
                {
                    i += m_pConnectedSocket->write(mesArr.mid(i, sendSize));
                    /*bool res = */m_pConnectedSocket->waitForBytesWritten();
                }
            }
            else
            {
                i += m_pConnectedSocket->write(mesArr.mid(i, sendSize));
                /*bool res = */m_pConnectedSocket->waitForBytesWritten();
            }

            m_pConnectedSocket->close();
            delete m_pConnectedSocket;
            m_pConnectedSocket = 0;
        }
    }
}
