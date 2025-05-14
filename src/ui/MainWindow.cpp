#include "ui/MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QStatusBar>
#include <QMessageBox>
#include <QDebug>

namespace GoQuant {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_webSocket(new WebSocketClient(this))
    , m_orderBookProcessor(new OrderBookProcessor(this))
    , m_isConnected(false)
    , m_lastProcessingTime(0.0)
    , m_lastUiUpdateTime(0.0)
    , m_orderQuantity(100.0)
    , m_volatility(0.0)
    , m_feeTier(0.0)
    , m_expectedSlippage(0.0)
    , m_expectedFees(0.0)
    , m_expectedMarketImpact(0.0)
    , m_netCost(0.0)
    , m_makerTakerProportion(0.0)
    , m_internalLatency(0.0)
{
    setupUi();
    setupConnections();
    startPerformanceMonitoring();
}

MainWindow::~MainWindow()
{
    stopPerformanceMonitoring();
}

void MainWindow::setupUi()
{
    setWindowTitle("GoQuant Trade Simulator");
    resize(1200, 800);

    // Create central widget and main layout
    auto centralWidget = new QWidget(this);
    auto mainLayout = new QHBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    // Create input panel
    createInputPanel();
    mainLayout->addWidget(createInputPanel());

    // Create output panel
    createOutputPanel();
    mainLayout->addWidget(createOutputPanel());

    // Create status bar
    createStatusBar();
}

QWidget* MainWindow::createInputPanel()
{
    auto panel = new QGroupBox("Input Parameters", this);
    auto layout = new QVBoxLayout(panel);

    // Exchange selection
    auto exchangeLayout = new QHBoxLayout();
    exchangeLayout->addWidget(new QLabel("Exchange:"));
    auto exchangeCombo = new QComboBox();
    exchangeCombo->addItem("OKX");
    exchangeLayout->addWidget(exchangeCombo);
    layout->addLayout(exchangeLayout);

    // Asset selection
    auto assetLayout = new QHBoxLayout();
    assetLayout->addWidget(new QLabel("Asset:"));
    auto assetCombo = new QComboBox();
    assetCombo->addItem("BTC-USDT-SWAP");
    assetLayout->addWidget(assetCombo);
    layout->addLayout(assetLayout);

    // Order quantity
    auto quantityLayout = new QHBoxLayout();
    quantityLayout->addWidget(new QLabel("Quantity (USD):"));
    auto quantityEdit = new QLineEdit();
    quantityEdit->setText("100");
    quantityLayout->addWidget(quantityEdit);
    layout->addLayout(quantityLayout);

    // Volatility
    auto volatilityLayout = new QHBoxLayout();
    volatilityLayout->addWidget(new QLabel("Volatility:"));
    auto volatilityEdit = new QLineEdit();
    volatilityEdit->setText("0.0");
    volatilityLayout->addWidget(volatilityEdit);
    layout->addLayout(volatilityLayout);

    // Fee tier
    auto feeLayout = new QHBoxLayout();
    feeLayout->addWidget(new QLabel("Fee Tier:"));
    auto feeEdit = new QLineEdit();
    feeEdit->setText("0.0");
    feeLayout->addWidget(feeEdit);
    layout->addLayout(feeLayout);

    // Connect button
    auto connectButton = new QPushButton("Connect");
    layout->addWidget(connectButton);

    // Connect signals
    connect(connectButton, &QPushButton::clicked, [this]() {
        if (!m_isConnected) {
            m_webSocket->connect("wss://ws.gomarket-cpp.goquant.io/ws/l2-orderbook/okx/BTC-USDT-SWAP");
        } else {
            m_webSocket->disconnect();
        }
    });

    return panel;
}

QWidget* MainWindow::createOutputPanel()
{
    auto panel = new QGroupBox("Output Parameters", this);
    auto layout = new QVBoxLayout(panel);

    // Expected Slippage
    auto slippageLayout = new QHBoxLayout();
    slippageLayout->addWidget(new QLabel("Expected Slippage:"));
    auto slippageValue = new QLabel("0.0");
    slippageLayout->addWidget(slippageValue);
    layout->addLayout(slippageLayout);

    // Expected Fees
    auto feesLayout = new QHBoxLayout();
    feesLayout->addWidget(new QLabel("Expected Fees:"));
    auto feesValue = new QLabel("0.0");
    feesLayout->addWidget(feesValue);
    layout->addLayout(feesLayout);

    // Expected Market Impact
    auto impactLayout = new QHBoxLayout();
    impactLayout->addWidget(new QLabel("Expected Market Impact:"));
    auto impactValue = new QLabel("0.0");
    impactLayout->addWidget(impactValue);
    layout->addLayout(impactLayout);

    // Net Cost
    auto costLayout = new QHBoxLayout();
    costLayout->addWidget(new QLabel("Net Cost:"));
    auto costValue = new QLabel("0.0");
    costLayout->addWidget(costValue);
    layout->addLayout(costLayout);

    // Maker/Taker Proportion
    auto proportionLayout = new QHBoxLayout();
    proportionLayout->addWidget(new QLabel("Maker/Taker Proportion:"));
    auto proportionValue = new QLabel("0.0");
    proportionLayout->addWidget(proportionValue);
    layout->addLayout(proportionLayout);

    // Internal Latency
    auto latencyLayout = new QHBoxLayout();
    latencyLayout->addWidget(new QLabel("Internal Latency:"));
    auto latencyValue = new QLabel("0.0");
    latencyLayout->addWidget(latencyValue);
    layout->addLayout(latencyLayout);

    return panel;
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Disconnected");
}

void MainWindow::setupConnections()
{
    connect(m_webSocket, &WebSocketClient::connected, this, &MainWindow::onWebSocketConnected);
    connect(m_webSocket, &WebSocketClient::disconnected, this, &MainWindow::onWebSocketDisconnected);
    connect(m_webSocket, &WebSocketClient::error, this, &MainWindow::onWebSocketError);

    connect(m_orderBookProcessor, &OrderBookProcessor::orderBookUpdated,
            this, &MainWindow::onOrderBookUpdated);
    connect(m_orderBookProcessor, &OrderBookProcessor::marketImpactUpdated,
            this, &MainWindow::onMarketImpactUpdated);
    connect(m_orderBookProcessor, &OrderBookProcessor::slippageUpdated,
            this, &MainWindow::onSlippageUpdated);
    connect(m_orderBookProcessor, &OrderBookProcessor::makerTakerProportionUpdated,
            this, &MainWindow::onMakerTakerProportionUpdated);

    m_webSocket->setMessageCallback([this](const nlohmann::json& data) {
        processOrderBookData(data);
    });
}

void MainWindow::startPerformanceMonitoring()
{
    connect(&m_performanceTimer, &QTimer::timeout,
            this, &MainWindow::updatePerformanceMetrics);
    m_performanceTimer.start(1000); // Update every second
}

void MainWindow::stopPerformanceMonitoring()
{
    m_performanceTimer.stop();
}

void MainWindow::onWebSocketConnected()
{
    m_isConnected = true;
    statusBar()->showMessage("Connected");
}

void MainWindow::onWebSocketDisconnected()
{
    m_isConnected = false;
    statusBar()->showMessage("Disconnected");
}

void MainWindow::onWebSocketError(const QString &error)
{
    QMessageBox::warning(this, "WebSocket Error", error);
}

void MainWindow::onOrderBookUpdated(const OrderBook& orderBook)
{
    // Update UI with new order book data
    updateMetrics();
}

void MainWindow::onMarketImpactUpdated(double impact)
{
    m_expectedMarketImpact = impact;
    updateMetrics();
}

void MainWindow::onSlippageUpdated(double slippage)
{
    m_expectedSlippage = slippage;
    updateMetrics();
}

void MainWindow::onMakerTakerProportionUpdated(double proportion)
{
    m_makerTakerProportion = proportion;
    updateMetrics();
}

void MainWindow::processOrderBookData(const nlohmann::json& data)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    m_orderBookProcessor->processOrderBook(data);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    m_lastProcessingTime = std::chrono::duration<double>(endTime - startTime).count();
}

void MainWindow::updateMetrics()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update UI with latest metrics
    // TODO: Update UI elements with current values
    
    auto endTime = std::chrono::high_resolution_clock::now();
    m_lastUiUpdateTime = std::chrono::duration<double>(endTime - startTime).count();
}

void MainWindow::updatePerformanceMetrics()
{
    m_internalLatency = m_lastProcessingTime + m_lastUiUpdateTime;
    // TODO: Update latency display in UI
}

} // namespace GoQuant 