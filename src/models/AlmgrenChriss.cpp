#include "models/AlmgrenChriss.h"
#include <cmath>
#include <stdexcept>

namespace GoQuant {

AlmgrenChriss::AlmgrenChriss(const Parameters& params)
    : m_params(params)
{
    if (params.volatility <= 0 || params.permanentImpact <= 0 || 
        params.temporaryImpact <= 0 || params.riskAversion <= 0 || 
        params.timeHorizon <= 0) {
        throw std::invalid_argument("Invalid parameters for Almgren-Chriss model");
    }
}

std::vector<double> AlmgrenChriss::calculateOptimalTrajectory(
    double initialPosition,
    double targetPosition,
    int numSteps
) const {
    if (numSteps <= 0) {
        throw std::invalid_argument("Number of steps must be positive");
    }

    std::vector<double> trajectory(numSteps);
    double totalPosition = initialPosition - targetPosition;
    double timeStep = m_params.timeHorizon / numSteps;

    // Calculate optimal trading rate
    double kappa = std::sqrt(m_params.riskAversion * m_params.volatility * m_params.volatility / 
                           (2 * m_params.temporaryImpact));
    
    for (int i = 0; i < numSteps; ++i) {
        double t = i * timeStep;
        trajectory[i] = totalPosition * std::exp(-kappa * t);
    }

    return trajectory;
}

double AlmgrenChriss::calculateMarketImpact(
    double quantity,
    double price,
    double timeHorizon
) const {
    double permanentImpact = calculatePermanentImpact(quantity);
    double temporaryImpact = calculateTemporaryImpact(quantity);
    double riskCost = calculateRiskCost(quantity, timeHorizon);

    return price * (permanentImpact + temporaryImpact + riskCost);
}

double AlmgrenChriss::calculateTotalCost(
    double quantity,
    double price,
    double timeHorizon
) const {
    double marketImpact = calculateMarketImpact(quantity, price, timeHorizon);
    return marketImpact + (quantity * price);
}

double AlmgrenChriss::calculatePermanentImpact(double quantity) const {
    return m_params.permanentImpact * std::sqrt(std::abs(quantity));
}

double AlmgrenChriss::calculateTemporaryImpact(double quantity) const {
    return m_params.temporaryImpact * std::abs(quantity);
}

double AlmgrenChriss::calculateRiskCost(double quantity, double timeHorizon) const {
    return m_params.riskAversion * m_params.volatility * m_params.volatility * 
           std::abs(quantity) * timeHorizon;
}

} // namespace GoQuant 