#include "core/WebSocketClient.h"
#include <QDebug>

namespace GoQuant {

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
    , m_webSocket(new QWebSocket())
    , m_isConnected(false)
{
    connect(m_webSocket.get(), &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_webSocket.get(), &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_webSocket.get(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &WebSocketClient::onError);
    connect(m_webSocket.get(), &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);
}

WebSocketClient::~WebSocketClient()
{
    disconnect();
}

void WebSocketClient::connect(const QString &url)
{
    if (!m_isConnected) {
        m_webSocket->open(QUrl(url));
    }
}

void WebSocketClient::disconnect()
{
    if (m_isConnected) {
        m_webSocket->close();
    }
}

bool WebSocketClient::isConnected() const
{
    return m_isConnected;
}

void WebSocketClient::setMessageCallback(MessageCallback callback)
{
    m_messageCallback = std::move(callback);
}

void WebSocketClient::onConnected()
{
    m_isConnected = true;
    emit connected();
}

void WebSocketClient::onDisconnected()
{
    m_isConnected = false;
    emit disconnected();
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorMessage = m_webSocket->errorString();
    qDebug() << "WebSocket error:" << errorMessage;
    emit this->error(errorMessage);
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    try {
        auto json = nlohmann::json::parse(message.toStdString());
        if (m_messageCallback) {
            m_messageCallback(json);
        }
    } catch (const std::exception& e) {
        qDebug() << "Error parsing WebSocket message:" << e.what();
        emit error(QString("Error parsing message: %1").arg(e.what()));
    }
}

} // namespace GoQuant 