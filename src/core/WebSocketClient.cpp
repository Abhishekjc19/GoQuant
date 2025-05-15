/**
 * @file WebSocketClient.cpp
 * @brief Implementation of the WebSocketClient class for real-time market data streaming
 * 
 * This file contains the implementation of the WebSocketClient class, which provides
 * a Qt-based WebSocket client for connecting to cryptocurrency exchange WebSocket APIs
 * and handling real-time market data streams.
 * 
 * @author GoQuant Team
 * @version 1.0
 * @date 2024
 */

#include "core/WebSocketClient.h"
#include <QDebug>

namespace GoQuant {

/**
 * @brief Constructs a new WebSocketClient instance
 * 
 * Initializes the WebSocket client and sets up signal/slot connections for
 * handling connection events and message reception.
 * 
 * @param parent Parent QObject for Qt signal/slot system
 */
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

/**
 * @brief Destructor for WebSocketClient
 * 
 * Ensures proper cleanup by disconnecting from the WebSocket server
 * before destruction.
 */
WebSocketClient::~WebSocketClient()
{
    disconnect();
}

/**
 * @brief Establishes WebSocket connection to the specified URL
 * 
 * @param url WebSocket server URL to connect to
 */
void WebSocketClient::connect(const QString &url)
{
    if (!m_isConnected) {
        m_webSocket->open(QUrl(url));
    }
}

/**
 * @brief Closes the WebSocket connection
 * 
 * Gracefully disconnects from the WebSocket server if currently connected.
 */
void WebSocketClient::disconnect()
{
    if (m_isConnected) {
        m_webSocket->close();
    }
}

/**
 * @brief Checks if the client is currently connected
 * 
 * @return bool True if connected, false otherwise
 */
bool WebSocketClient::isConnected() const
{
    return m_isConnected;
}

/**
 * @brief Sets the callback function for handling incoming messages
 * 
 * @param callback Function to be called when a message is received
 */
void WebSocketClient::setMessageCallback(MessageCallback callback)
{
    m_messageCallback = std::move(callback);
}

/**
 * @brief Handles successful WebSocket connection
 * 
 * Updates connection state and emits the connected signal.
 */
void WebSocketClient::onConnected()
{
    m_isConnected = true;
    emit connected();
}

/**
 * @brief Handles WebSocket disconnection
 * 
 * Updates connection state and emits the disconnected signal.
 */
void WebSocketClient::onDisconnected()
{
    m_isConnected = false;
    emit disconnected();
}

/**
 * @brief Handles WebSocket errors
 * 
 * Logs the error message and emits the error signal.
 * 
 * @param error Socket error code
 */
void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorMessage = m_webSocket->errorString();
    qDebug() << "WebSocket error:" << errorMessage;
    emit this->error(errorMessage);
}

/**
 * @brief Processes incoming WebSocket text messages
 * 
 * Parses received JSON messages and forwards them to the registered callback.
 * Emits error signal if message parsing fails.
 * 
 * @param message Received text message
 */
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