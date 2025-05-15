/**
 * @file WebSocketClient.h
 * @brief Header file for the WebSocketClient class
 * 
 * This file defines the WebSocketClient class for handling WebSocket connections
 * to cryptocurrency exchange APIs and processing real-time market data streams.
 * 
 * @author GoQuant Team
 * @version 1.0
 * @date 2024
 */

#pragma once

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace GoQuant {

/**
 * @brief WebSocket client for real-time market data streaming
 * 
 * This class provides a Qt-based WebSocket client implementation for connecting
 * to cryptocurrency exchange WebSocket APIs. It handles connection management,
 * message processing, and error handling.
 */
class WebSocketClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a new WebSocketClient instance
     * 
     * @param parent Parent QObject for Qt signal/slot system
     */
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    /**
     * @brief Establishes WebSocket connection to the specified URL
     * 
     * @param url WebSocket server URL to connect to
     */
    void connect(const QString &url);

    /**
     * @brief Closes the WebSocket connection
     */
    void disconnect();

    /**
     * @brief Checks if the client is currently connected
     * 
     * @return bool True if connected, false otherwise
     */
    bool isConnected() const;

    /**
     * @brief Callback function type for processing received messages
     * 
     * The callback receives parsed JSON messages from the WebSocket connection.
     */
    using MessageCallback = std::function<void(const nlohmann::json&)>;

    /**
     * @brief Sets the callback function for handling incoming messages
     * 
     * @param callback Function to be called when a message is received
     */
    void setMessageCallback(MessageCallback callback);

signals:
    /// Emitted when the WebSocket connection is established
    void connected();
    /// Emitted when the WebSocket connection is closed
    void disconnected();
    /// Emitted when a WebSocket error occurs
    void error(const QString &error);

private slots:
    /**
     * @brief Handles successful WebSocket connection
     */
    void onConnected();

    /**
     * @brief Handles WebSocket disconnection
     */
    void onDisconnected();

    /**
     * @brief Handles WebSocket errors
     * 
     * @param error Socket error code
     */
    void onError(QAbstractSocket::SocketError error);

    /**
     * @brief Processes incoming WebSocket text messages
     * 
     * @param message Received text message
     */
    void onTextMessageReceived(const QString &message);

private:
    std::unique_ptr<QWebSocket> m_webSocket;  ///< WebSocket connection instance
    MessageCallback m_messageCallback;         ///< Callback for message processing
    bool m_isConnected;                        ///< Connection state flag
};

} // namespace GoQuant 