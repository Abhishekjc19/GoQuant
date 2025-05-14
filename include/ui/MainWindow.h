#pragma once

#include <QMainWindow>
#include <QTimer>
#include <memory>
#include "../core/WebSocketClient.h"
#include "../core/OrderBookProcessor.h"
#include "../models/AlmgrenChriss.h"

namespace GoQuant {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketError(const QString &error);
    void onOrderBookUpdated(const OrderBook& orderBook);
    void onMarketImpactUpdated(double impact);
    void onSlippageUpdated(double slippage);
    void onMakerTakerProportionUpdated(double proportion);
    void updatePerformanceMetrics();

private:
    // UI Components
    void setupUi();
    void createInputPanel();
    void createOutputPanel();
    void createStatusBar();
    
    // Data processing
    void processOrderBookData(const nlohmann::json& data);
    void updateMetrics();
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    
    // Member variables
    std::unique_ptr<WebSocketClient> m_webSocket;
    std::unique_ptr<OrderBookProcessor> m_orderBookProcessor;
    std::unique_ptr<AlmgrenChriss> m_marketImpactModel;
    
    QTimer m_performanceTimer;
    
    // UI state
    bool m_isConnected;
    double m_lastProcessingTime;
    double m_lastUiUpdateTime;
    
    // Input parameters
    QString m_selectedExchange;
    QString m_selectedAsset;
    double m_orderQuantity;
    double m_volatility;
    double m_feeTier;
    
    // Output parameters
    double m_expectedSlippage;
    double m_expectedFees;
    double m_expectedMarketImpact;
    double m_netCost;
    double m_makerTakerProportion;
    double m_internalLatency;
};

} // namespace GoQuant 