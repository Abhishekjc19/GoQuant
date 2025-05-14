#pragma once

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace GoQuant {

class WebSocketClient : public QObject {
    Q_OBJECT

public:
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    void connect(const QString &url);
    void disconnect();
    bool isConnected() const;

    using MessageCallback = std::function<void(const nlohmann::json&)>;
    void setMessageCallback(MessageCallback callback);

signals:
    void connected();
    void disconnected();
    void error(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(const QString &message);

private:
    std::unique_ptr<QWebSocket> m_webSocket;
    MessageCallback m_messageCallback;
    bool m_isConnected;
};

} // namespace GoQuant 