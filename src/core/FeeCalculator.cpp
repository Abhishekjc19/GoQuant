/**
 * @file FeeCalculator.cpp
 * @brief Implementation of the FeeCalculator class for trading fee calculations
 * 
 * This file contains the implementation of the FeeCalculator class, which handles
 * calculation of trading fees based on exchange-specific fee tiers and trading volumes.
 * Supports multiple exchanges and dynamic fee tier selection based on trading volume.
 * 
 * @author GoQuant Team
 * @version 1.0
 * @date 2024
 */

#include "core/FeeCalculator.h"
#include <algorithm>
#include <stdexcept>

namespace GoQuant {

/**
 * @brief Constructs a new FeeCalculator instance
 * 
 * Initializes the fee calculator with default fee tiers for supported exchanges.
 * Currently supports OKX exchange with multiple VIP tiers.
 */
FeeCalculator::FeeCalculator() {
    initializeFeeTiers();
}

/**
 * @brief Initializes fee tiers for supported exchanges
 * 
 * Sets up the fee structure for different exchanges and their VIP tiers.
 * Each tier includes maker fee, taker fee, and minimum trading volume requirements.
 */
void FeeCalculator::initializeFeeTiers() {
    // OKX fee tiers (as of 2024)
    std::vector<FeeTier> okxTiers = {
        {0.0008, 0.0010, 0.0},           // Regular
        {0.0007, 0.0009, 50000.0},       // VIP1
        {0.0006, 0.0008, 100000.0},      // VIP2
        {0.0005, 0.0007, 500000.0},      // VIP3
        {0.0004, 0.0006, 1000000.0},     // VIP4
        {0.0003, 0.0005, 5000000.0},     // VIP5
        {0.0002, 0.0004, 10000000.0}     // VIP6
    };
    m_feeTiers["OKX"] = std::move(okxTiers);

    // Set default tier
    m_currentTier = m_feeTiers["OKX"][0];
}

/**
 * @brief Sets the current fee tier based on trading volume
 * 
 * Selects the appropriate fee tier for a given exchange based on the trading volume.
 * Higher trading volumes qualify for lower fee tiers.
 * 
 * @param exchange Exchange name (e.g., "OKX")
 * @param tradingVolume Total trading volume in base currency
 * @throws std::invalid_argument if exchange is not supported
 */
void FeeCalculator::setFeeTier(const std::string& exchange, double tradingVolume) {
    auto it = m_feeTiers.find(exchange);
    if (it == m_feeTiers.end()) {
        throw std::invalid_argument("Unsupported exchange: " + exchange);
    }

    const auto& tiers = it->second;
    auto tierIt = std::find_if(tiers.rbegin(), tiers.rend(),
        [tradingVolume](const FeeTier& tier) {
            return tradingVolume >= tier.minVolume;
        });

    if (tierIt != tiers.rend()) {
        m_currentTier = *tierIt;
    } else {
        m_currentTier = tiers[0];  // Default to lowest tier
    }
}

/**
 * @brief Calculates trading fees for an order
 * 
 * Computes the fee amount based on order size and whether it's a maker or taker order.
 * Uses the current fee tier's rates for calculation.
 * 
 * @param orderSize Size of the order in base currency
 * @param isMaker True for maker orders, false for taker orders
 * @return double Fee amount in base currency
 * @throws std::invalid_argument if order size is not positive
 */
double FeeCalculator::calculateFees(double orderSize, bool isMaker) const {
    if (orderSize <= 0.0) {
        throw std::invalid_argument("Order size must be positive");
    }

    double feeRate = isMaker ? m_currentTier.makerFee : m_currentTier.takerFee;
    return orderSize * feeRate;
}

/**
 * @brief Retrieves the current fee tier information
 * 
 * @return const FeeTier& Reference to the current fee tier structure
 */
const FeeCalculator::FeeTier& FeeCalculator::getCurrentFeeTier() const {
    return m_currentTier;
}

} // namespace GoQuant 