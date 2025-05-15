/**
 * @file main.cpp
 * @brief Main entry point for the GoQuant trading system
 * 
 * This file implements the main trading system loop, including order book processing,
 * fee calculations, and performance monitoring. It sets up the core components and
 * connects their signals for real-time market data processing.
 * 
 * @author GoQuant Team
 * @version 1.0
 * @date 2024
 */

#include "core/OrderBookProcessor.h"
#include "core/FeeCalculator.h"
#include "models/RegressionModels.h"
#include "utils/PerformanceMonitor.h"
#include <QCoreApplication>
#include <QTimer>
#include <iostream>
#include <thread>
#include <chrono>

using namespace GoQuant;

/**
 * @brief Simulates order book updates with sample market data
 * 
 * This function creates a sample order book update with mock data for testing
 * and demonstration purposes. In a production environment, this would be
 * replaced with real market data feeds.
 * 
 * @param processor Reference to the OrderBookProcessor instance
 */
void simulateOrderBook(OrderBookProcessor& processor) {
    nlohmann::json orderBookData = {
        {"timestamp", "2024-03-20T10:00:00Z"},
        {"exchange", "OKX"},
        {"symbol", "BTC-USDT"},
        {"asks", {
            {"50000.0", "1.5"},
            {"50001.0", "2.0"},
            {"50002.0", "1.0"}
        }},
        {"bids", {
            {"49999.0", "2.0"},
            {"49998.0", "1.5"},
            {"49997.0", "1.0"}
        }}
    };

    processor.processOrderBook(orderBookData);
}

/**
 * @brief Main entry point for the trading system
 * 
 * Initializes and connects all core components of the trading system:
 * - Order book processor for market data
 * - Fee calculator for transaction costs
 * - Performance monitor for system metrics
 * - Regression models for market analysis
 * 
 * Sets up a timer-based update loop that processes market data every second.
 * 
 * @param argc Command line argument count
 * @param argv Command line argument values
 * @return int Application exit code
 */
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Create instances
    OrderBookProcessor orderBookProcessor;
    FeeCalculator feeCalculator;
    PerformanceMonitor performanceMonitor;
    RegressionModels::SlippageEstimator slippageEstimator;
    RegressionModels::MakerTakerPredictor makerTakerPredictor;

    // Connect signals
    QObject::connect(&orderBookProcessor, &OrderBookProcessor::orderBookUpdated,
        [](const OrderBook& book) {
            std::cout << "Order book updated for " << book.symbol << std::endl;
        });

    QObject::connect(&orderBookProcessor, &OrderBookProcessor::marketImpactUpdated,
        [](double impact) {
            std::cout << "Market impact: " << impact * 100 << "%" << std::endl;
        });

    QObject::connect(&orderBookProcessor, &OrderBookProcessor::slippageUpdated,
        [](double slippage) {
            std::cout << "Slippage: " << slippage * 100 << "%" << std::endl;
        });

    QObject::connect(&orderBookProcessor, &OrderBookProcessor::makerTakerProportionUpdated,
        [](double proportion) {
            std::cout << "Maker/Taker proportion: " << proportion * 100 << "%" << std::endl;
        });

    // Set up periodic updates
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        simulateOrderBook(orderBookProcessor);
        
        // Calculate and display fees
        double orderSize = 1.0; // 1 BTC
        double makerFee = feeCalculator.calculateFees(orderSize, true);
        double takerFee = feeCalculator.calculateFees(orderSize, false);
        std::cout << "Fees for " << orderSize << " BTC:" << std::endl;
        std::cout << "  Maker fee: " << makerFee << " BTC" << std::endl;
        std::cout << "  Taker fee: " << takerFee << " BTC" << std::endl;

        // Record performance metrics
        performanceMonitor.recordMetric("order_book_depth", 
            orderBookProcessor.getLatestOrderBook().asks.size() + 
            orderBookProcessor.getLatestOrderBook().bids.size());
        performanceMonitor.recordLatency("order_book_update", 50.0); // Simulated latency
    });

    // Start updates every second
    timer.start(1000);

    std::cout << "Trade simulator running. Press Ctrl+C to exit." << std::endl;
    return app.exec();
} 