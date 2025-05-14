#include "core/FeeCalculator.h"
#include <algorithm>
#include <stdexcept>

namespace GoQuant {

FeeCalculator::FeeCalculator() {
    initializeFeeTiers();
}

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

double FeeCalculator::calculateFees(double orderSize, bool isMaker) const {
    if (orderSize <= 0.0) {
        throw std::invalid_argument("Order size must be positive");
    }

    double feeRate = isMaker ? m_currentTier.makerFee : m_currentTier.takerFee;
    return orderSize * feeRate;
}

const FeeCalculator::FeeTier& FeeCalculator::getCurrentFeeTier() const {
    return m_currentTier;
}

} // namespace GoQuant 