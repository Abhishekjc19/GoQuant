#include "models/RegressionModels.h"
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace GoQuant {

// Linear Regression Implementation
void RegressionModels::LinearRegression::fit(const std::vector<DataPoint>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Empty dataset for linear regression");
    }

    // Calculate means
    double meanX = 0.0, meanY = 0.0;
    for (const auto& point : data) {
        meanX += point.x;
        meanY += point.y;
    }
    meanX /= data.size();
    meanY /= data.size();

    // Calculate slope and intercept
    double numerator = 0.0, denominator = 0.0;
    for (const auto& point : data) {
        double xDiff = point.x - meanX;
        double yDiff = point.y - meanY;
        numerator += xDiff * yDiff;
        denominator += xDiff * xDiff;
    }

    if (denominator == 0.0) {
        throw std::runtime_error("Cannot fit linear regression: zero variance in x");
    }

    m_slope = numerator / denominator;
    m_intercept = meanY - m_slope * meanX;

    // Calculate R-squared
    double ssTotal = 0.0, ssResidual = 0.0;
    for (const auto& point : data) {
        double predicted = predict(point.x);
        ssTotal += std::pow(point.y - meanY, 2);
        ssResidual += std::pow(point.y - predicted, 2);
    }
    m_rSquared = 1.0 - (ssResidual / ssTotal);
}

double RegressionModels::LinearRegression::predict(double x) const {
    return m_slope * x + m_intercept;
}

double RegressionModels::LinearRegression::getRSquared() const {
    return m_rSquared;
}

// Quantile Regression Implementation
RegressionModels::QuantileRegression::QuantileRegression(double quantile)
    : m_quantile(quantile), m_slope(0.0), m_intercept(0.0) {
    if (quantile <= 0.0 || quantile >= 1.0) {
        throw std::invalid_argument("Quantile must be between 0 and 1");
    }
}

void RegressionModels::QuantileRegression::fit(const std::vector<DataPoint>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Empty dataset for quantile regression");
    }

    // Sort data by x
    std::vector<DataPoint> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end(),
              [](const DataPoint& a, const DataPoint& b) { return a.x < b.x; });

    // Find quantile index
    size_t quantileIndex = static_cast<size_t>(m_quantile * (sortedData.size() - 1));
    
    // Calculate slope and intercept
    m_slope = (sortedData[quantileIndex + 1].y - sortedData[quantileIndex].y) /
              (sortedData[quantileIndex + 1].x - sortedData[quantileIndex].x);
    m_intercept = sortedData[quantileIndex].y - m_slope * sortedData[quantileIndex].x;
}

double RegressionModels::QuantileRegression::predict(double x) const {
    return m_slope * x + m_intercept;
}

// Logistic Regression Implementation
void RegressionModels::LogisticRegression::fit(
    const std::vector<DataPoint>& data,
    const std::vector<bool>& labels) {
    if (data.size() != labels.size()) {
        throw std::invalid_argument("Data and labels must have same size");
    }

    // Simple gradient descent implementation
    const double learningRate = 0.01;
    const int maxIterations = 1000;
    m_slope = 0.0;
    m_intercept = 0.0;

    for (int iter = 0; iter < maxIterations; ++iter) {
        double slopeGrad = 0.0, interceptGrad = 0.0;

        for (size_t i = 0; i < data.size(); ++i) {
            double prediction = predictProbability(data[i].x);
            double error = prediction - (labels[i] ? 1.0 : 0.0);
            slopeGrad += error * data[i].x;
            interceptGrad += error;
        }

        m_slope -= learningRate * slopeGrad / data.size();
        m_intercept -= learningRate * interceptGrad / data.size();
    }
}

double RegressionModels::LogisticRegression::predictProbability(double x) const {
    return sigmoid(m_slope * x + m_intercept);
}

bool RegressionModels::LogisticRegression::predict(double x) const {
    return predictProbability(x) >= 0.5;
}

double RegressionModels::LogisticRegression::sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

// Slippage Estimator Implementation
RegressionModels::SlippageEstimator::SlippageEstimator() {
    m_historicalData.reserve(MAX_HISTORY_SIZE);
}

void RegressionModels::SlippageEstimator::update(
    const std::deque<DataPoint>& historicalData) {
    m_historicalData = historicalData;
    if (m_historicalData.size() > MAX_HISTORY_SIZE) {
        m_historicalData.resize(MAX_HISTORY_SIZE);
    }

    std::vector<DataPoint> data(m_historicalData.begin(), m_historicalData.end());
    m_linearModel.fit(data);
    m_quantileModel.fit(data);
}

double RegressionModels::SlippageEstimator::estimateSlippage(double orderSize) const {
    double linearPrediction = m_linearModel.predict(orderSize);
    double quantilePrediction = m_quantileModel.predict(orderSize);
    return (linearPrediction + quantilePrediction) / 2.0;
}

double RegressionModels::SlippageEstimator::getConfidence() const {
    return m_linearModel.getRSquared();
}

// Maker/Taker Predictor Implementation
RegressionModels::MakerTakerPredictor::MakerTakerPredictor() {
    m_historicalData.reserve(MAX_HISTORY_SIZE);
    m_makerLabels.reserve(MAX_HISTORY_SIZE);
}

void RegressionModels::MakerTakerPredictor::update(
    const std::deque<DataPoint>& historicalData,
    const std::deque<bool>& makerLabels) {
    m_historicalData = historicalData;
    m_makerLabels = makerLabels;

    if (m_historicalData.size() > MAX_HISTORY_SIZE) {
        m_historicalData.resize(MAX_HISTORY_SIZE);
        m_makerLabels.resize(MAX_HISTORY_SIZE);
    }

    std::vector<DataPoint> data(m_historicalData.begin(), m_historicalData.end());
    std::vector<bool> labels(m_makerLabels.begin(), m_makerLabels.end());
    m_model.fit(data, labels);
}

double RegressionModels::MakerTakerPredictor::predictMakerProportion(double orderSize) const {
    return m_model.predictProbability(orderSize);
}

} // namespace GoQuant 