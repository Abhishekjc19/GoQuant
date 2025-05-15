/**
 * @file OrderBookProcessor.cpp
 * @brief Implementation of the OrderBookProcessor class for real-time order book analysis
 * 
 * This file contains the implementation of the OrderBookProcessor class, which handles
 * real-time processing of order book data, including market impact calculation,
 * slippage estimation, and maker/taker proportion analysis.
 * 
 * @author GoQuant Team
 * @version 1.0
 * @date 2024
 */

#include "core/OrderBookProcessor.h"
#include "models/RegressionModels.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace GoQuant {

/**
 * @brief Constructs a new OrderBookProcessor instance
 * 
 * Initializes the order book processor with a pre-allocated history buffer
 * to store historical order book snapshots for analysis.
 * 
 * @param parent Parent QObject for Qt signal/slot system
 */
OrderBookProcessor::OrderBookProcessor(QObject *parent)
    : QObject(parent)
{
    m_orderBookHistory.reserve(HISTORY_SIZE);
}

OrderBookProcessor::~OrderBookProcessor() = default;

/**
 * @brief Processes incoming order book data
 * 
 * Parses and validates incoming order book data in JSON format, updates the current
 * order book state, and emits signals for various market metrics.
 * 
 * @param data JSON object containing order book data
 * @throws std::runtime_error if data parsing fails
 */
void OrderBookProcessor::processOrderBook(const nlohmann::json& data) {
    try {
        OrderBook newOrderBook;
        newOrderBook.timestamp = data["timestamp"].get<std::string>();
        newOrderBook.exchange = data["exchange"].get<std::string>();
        newOrderBook.symbol = data["symbol"].get<std::string>();

        // Process asks
        const auto& asks = data["asks"];
        newOrderBook.asks.reserve(asks.size());
        for (const auto& ask : asks) {
            OrderBookLevel level;
            level.price = std::stod(ask[0].get<std::string>());
            level.quantity = std::stod(ask[1].get<std::string>());
            newOrderBook.asks.push_back(level);
        }

        // Process bids
        const auto& bids = data["bids"];
        newOrderBook.bids.reserve(bids.size());
        for (const auto& bid : bids) {
            OrderBookLevel level;
            level.price = std::stod(bid[0].get<std::string>());
            level.quantity = std::stod(bid[1].get<std::string>());
            newOrderBook.bids.push_back(level);
        }

        // Update order book
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_currentOrderBook = std::move(newOrderBook);
            m_orderBookHistory.push_back(m_currentOrderBook);
            maintainHistory();
        }

        // Emit signals
        emit orderBookUpdated(m_currentOrderBook);
        emit marketImpactUpdated(calculateMarketImpact(100.0, true));
        emit slippageUpdated(calculateSlippage(100.0, true));
        emit makerTakerProportionUpdated(calculateMakerTakerProportion());

    } catch (const std::exception& e) {
        throw std::runtime_error("Error processing order book: " + std::string(e.what()));
    }
}

/**
 * @brief Retrieves the most recent order book snapshot
 * 
 * @return OrderBook Current order book state
 */
OrderBook OrderBookProcessor::getLatestOrderBook() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentOrderBook;
}

/**
 * @brief Calculates market impact for a given order size
 * 
 * Estimates the price impact of executing an order of specified size by
 * calculating the weighted average price across multiple price levels.
 * 
 * @param quantity Order size in base currency
 * @param isBuy True for buy orders, false for sell orders
 * @return double Market impact as a percentage of mid price
 */
double OrderBookProcessor::calculateMarketImpact(double quantity, bool isBuy) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    const auto& levels = isBuy ? m_currentOrderBook.asks : m_currentOrderBook.bids;
    if (levels.empty()) {
        return 0.0;
    }

    double remainingQuantity = quantity;
    double weightedPrice = 0.0;
    double totalQuantity = 0.0;

    for (const auto& level : levels) {
        if (remainingQuantity <= 0.0) break;

        double executedQuantity = std::min(remainingQuantity, level.quantity);
        weightedPrice += level.price * executedQuantity;
        totalQuantity += executedQuantity;
        remainingQuantity -= executedQuantity;
    }

    if (totalQuantity == 0.0) {
        return 0.0;
    }

    double averagePrice = weightedPrice / totalQuantity;
    double midPrice = (levels[0].price + levels[0].price) / 2.0;
    return std::abs(averagePrice - midPrice) / midPrice;
}

/**
 * @brief Calculates slippage for a given order size
 * 
 * Estimates the price slippage that would occur when executing an order
 * of specified size, taking into account available liquidity at each price level.
 * 
 * @param quantity Order size in base currency
 * @param isBuy True for buy orders, false for sell orders
 * @return double Slippage as a percentage of mid price, or infinity if insufficient liquidity
 */
double OrderBookProcessor::calculateSlippage(double quantity, bool isBuy) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    const auto& levels = isBuy ? m_currentOrderBook.asks : m_currentOrderBook.bids;
    if (levels.empty()) {
        return 0.0;
    }

    double remainingQuantity = quantity;
    double totalCost = 0.0;

    for (const auto& level : levels) {
        if (remainingQuantity <= 0.0) break;

        double executedQuantity = std::min(remainingQuantity, level.quantity);
        totalCost += level.price * executedQuantity;
        remainingQuantity -= executedQuantity;
    }

    if (remainingQuantity > 0.0) {
        // Not enough liquidity
        return std::numeric_limits<double>::infinity();
    }

    double averagePrice = totalCost / quantity;
    double midPrice = (levels[0].price + levels[0].price) / 2.0;
    return std::abs(averagePrice - midPrice) / midPrice;
}

/**
 * @brief Calculates the proportion of maker vs taker orders
 * 
 * Analyzes order book history to estimate the ratio of maker to taker orders
 * based on price movement patterns.
 * 
 * @return double Proportion of maker orders (0.0 to 1.0)
 */
double OrderBookProcessor::calculateMakerTakerProportion() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_orderBookHistory.empty()) {
        return 0.5;  // Default to 50/50 if no history
    }

    // Calculate the proportion of maker orders based on order book changes
    size_t makerCount = 0;
    size_t totalCount = 0;

    for (size_t i = 1; i < m_orderBookHistory.size(); ++i) {
        const auto& prev = m_orderBookHistory[i-1];
        const auto& curr = m_orderBookHistory[i];

        // Compare ask levels
        for (size_t j = 0; j < std::min(prev.asks.size(), curr.asks.size()); ++j) {
            if (prev.asks[j].price != curr.asks[j].price) {
                totalCount++;
                if (curr.asks[j].price > prev.asks[j].price) {
                    makerCount++;  // Price increase suggests maker order
                }
            }
        }

        // Compare bid levels
        for (size_t j = 0; j < std::min(prev.bids.size(), curr.bids.size()); ++j) {
            if (prev.bids[j].price != curr.bids[j].price) {
                totalCount++;
                if (curr.bids[j].price < prev.bids[j].price) {
                    makerCount++;  // Price decrease suggests maker order
                }
            }
        }
    }

    return totalCount > 0 ? static_cast<double>(makerCount) / totalCount : 0.5;
}

/**
 * @brief Maintains the order book history within size limits
 * 
 * Removes oldest entries when history exceeds HISTORY_SIZE to prevent
 * unbounded memory growth.
 */
void OrderBookProcessor::maintainHistory() {
    while (m_orderBookHistory.size() > HISTORY_SIZE) {
        m_orderBookHistory.pop_front();
    }
}

} // namespace GoQuant 