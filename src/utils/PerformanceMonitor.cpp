#include "utils/PerformanceMonitor.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace GoQuant {

PerformanceMonitor::PerformanceMonitor(QObject *parent)
    : QObject(parent)
{
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::recordMetric(const std::string& name, double value) {
    Metric metric{name, value, std::chrono::system_clock::now()};
    
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& history = m_metrics[name];
    history.values.push_back(metric);
    maintainHistory(history);
    
    emit metricUpdated(name, value);
}

void PerformanceMonitor::recordLatency(const std::string& operation, double milliseconds) {
    Metric metric{operation, milliseconds, std::chrono::system_clock::now()};
    
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& history = m_latencies[operation];
    history.values.push_back(metric);
    maintainHistory(history);
    
    emit latencyUpdated(operation, milliseconds);
}

double PerformanceMonitor::getAverageMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_metrics.find(name);
    if (it == m_metrics.end() || it->second.values.empty()) {
        return 0.0;
    }

    const auto& values = it->second.values;
    double sum = std::accumulate(values.begin(), values.end(), 0.0,
        [](double acc, const Metric& m) { return acc + m.value; });
    return sum / values.size();
}

double PerformanceMonitor::getMinMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_metrics.find(name);
    if (it == m_metrics.end() || it->second.values.empty()) {
        return 0.0;
    }

    const auto& values = it->second.values;
    return std::min_element(values.begin(), values.end(),
        [](const Metric& a, const Metric& b) { return a.value < b.value; })->value;
}

double PerformanceMonitor::getMaxMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_metrics.find(name);
    if (it == m_metrics.end() || it->second.values.empty()) {
        return 0.0;
    }

    const auto& values = it->second.values;
    return std::max_element(values.begin(), values.end(),
        [](const Metric& a, const Metric& b) { return a.value < b.value; })->value;
}

double PerformanceMonitor::getPercentileMetric(const std::string& name, double percentile) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_metrics.find(name);
    if (it == m_metrics.end() || it->second.values.empty()) {
        return 0.0;
    }

    return calculatePercentile(it->second.values, percentile);
}

double PerformanceMonitor::getAverageLatency(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_latencies.find(operation);
    if (it == m_latencies.end() || it->second.values.empty()) {
        return 0.0;
    }

    const auto& values = it->second.values;
    double sum = std::accumulate(values.begin(), values.end(), 0.0,
        [](double acc, const Metric& m) { return acc + m.value; });
    return sum / values.size();
}

double PerformanceMonitor::getMinLatency(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_latencies.find(operation);
    if (it == m_latencies.end() || it->second.values.empty()) {
        return 0.0;
    }

    const auto& values = it->second.values;
    return std::min_element(values.begin(), values.end(),
        [](const Metric& a, const Metric& b) { return a.value < b.value; })->value;
}

double PerformanceMonitor::getMaxLatency(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_latencies.find(operation);
    if (it == m_latencies.end() || it->second.values.empty()) {
        return 0.0;
    }

    const auto& values = it->second.values;
    return std::max_element(values.begin(), values.end(),
        [](const Metric& a, const Metric& b) { return a.value < b.value; })->value;
}

double PerformanceMonitor::getPercentileLatency(const std::string& operation, double percentile) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_latencies.find(operation);
    if (it == m_latencies.end() || it->second.values.empty()) {
        return 0.0;
    }

    return calculatePercentile(it->second.values, percentile);
}

void PerformanceMonitor::clearHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_metrics.clear();
    m_latencies.clear();
}

void PerformanceMonitor::maintainHistory(MetricHistory& history) {
    while (history.values.size() > MetricHistory::MAX_HISTORY_SIZE) {
        history.values.pop_front();
    }
}

double PerformanceMonitor::calculatePercentile(const std::deque<Metric>& values, double percentile) const {
    if (values.empty()) {
        return 0.0;
    }

    if (percentile < 0.0 || percentile > 100.0) {
        throw std::invalid_argument("Percentile must be between 0 and 100");
    }

    std::vector<double> sortedValues;
    sortedValues.reserve(values.size());
    for (const auto& metric : values) {
        sortedValues.push_back(metric.value);
    }
    std::sort(sortedValues.begin(), sortedValues.end());

    size_t index = static_cast<size_t>(percentile / 100.0 * (sortedValues.size() - 1));
    return sortedValues[index];
}

} // namespace GoQuant 