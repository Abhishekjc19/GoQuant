// Theme Management
let isDarkMode = true;

function toggleTheme() {
    isDarkMode = !isDarkMode;
    document.body.classList.toggle('light-mode');
    updateThemeIcon();
}

function updateThemeIcon() {
    const themeToggle = document.getElementById('theme-toggle');
    if (themeToggle) {
        themeToggle.innerHTML = isDarkMode ? 
            '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z"/></svg>' :
            '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 12.79A9 9 0 1111.21 3 7 7 0 0021 12.79z"/></svg>';
    }
}

// Header Scroll Effect
function handleHeaderScroll() {
    const header = document.querySelector('.header');
    if (header) {
        if (window.scrollY > 50) {
            header.classList.add('scrolled');
        } else {
            header.classList.remove('scrolled');
        }
    }
}

// Smooth Scroll
function smoothScroll(target) {
    const element = document.querySelector(target);
    if (element) {
        element.scrollIntoView({
            behavior: 'smooth',
            block: 'start'
        });
    }
}

// Animation on Scroll
function animateOnScroll() {
    const elements = document.querySelectorAll('.fade-in, .slide-up');
    elements.forEach(element => {
        const elementTop = element.getBoundingClientRect().top;
        const elementBottom = element.getBoundingClientRect().bottom;
        
        if (elementTop < window.innerHeight && elementBottom > 0) {
            element.style.opacity = '1';
            element.style.transform = 'translateY(0)';
        }
    });
}

// Initialize animations
function initializeAnimations() {
    const elements = document.querySelectorAll('.hero-title, .hero-subtitle, .hero-actions');
    elements.forEach((element, index) => {
        element.style.opacity = '0';
        element.style.transform = 'translateY(20px)';
        element.style.transition = 'opacity 0.5s ease, transform 0.5s ease';
        element.style.transitionDelay = `${index * 0.2}s`;
    });
}

// Show animations after page load
window.addEventListener('load', () => {
    const elements = document.querySelectorAll('.hero-title, .hero-subtitle, .hero-actions');
    elements.forEach(element => {
        element.style.opacity = '1';
        element.style.transform = 'translateY(0)';
    });
});

// WebSocket connection
let ws = null;
let isConnected = false;
let lastUpdateTime = null;
let processingLatency = [];
let performanceStats = {
    totalUpdates: 0,
    totalLatency: 0,
    maxLatency: 0,
    minLatency: Infinity,
    errors: 0,
    reconnections: 0
};

// Configuration
const config = {
    exchange: 'OKX',
    symbol: 'BTC-USDT-SWAP',
    websocketUrl: 'wss://ws.gomarket-cpp.goquant.io/ws/l2-orderbook/okx/BTC-USDT-SWAP',
    volatility: 0.02, // 2% volatility
    feeTier: 'tier1', // Default fee tier
    feeRates: {
        tier1: { maker: 0.0008, taker: 0.0010 },
        tier2: { maker: 0.0007, taker: 0.0009 },
        tier3: { maker: 0.0006, taker: 0.0008 }
    },
    defaultValues: {
        orderSize: 1.0,
        volatility: 2.0,
        feeTier: 'tier1'
    }
};

// Order book data
let orderBook = {
    asks: [],
    bids: [],
    lastUpdate: null
};

// Show error message with animation
function showError(message) {
    const errorElement = document.getElementById('error-message');
    if (errorElement) {
        errorElement.textContent = message;
        errorElement.classList.remove('hidden');
        errorElement.style.animation = 'fadeIn 0.3s ease';
        setTimeout(() => {
            errorElement.style.animation = 'fadeOut 0.3s ease';
            setTimeout(() => {
                errorElement.classList.add('hidden');
            }, 300);
        }, 5000);
    }
    performanceStats.errors++;
    updatePerformanceStats();
}

// Show loading state with animation
function setLoading(isLoading) {
    const loadingIndicator = document.getElementById('loading-indicator');
    if (loadingIndicator) {
        if (isLoading) {
            loadingIndicator.classList.remove('hidden');
            loadingIndicator.style.animation = 'fadeIn 0.3s ease';
        } else {
            loadingIndicator.style.animation = 'fadeOut 0.3s ease';
            setTimeout(() => {
                loadingIndicator.classList.add('hidden');
            }, 300);
        }
    }
}

// Update order book display with animations
function updateOrderBook() {
    const asksList = document.getElementById('asks-list');
    const bidsList = document.getElementById('bids-list');
    const lastUpdateTime = document.getElementById('last-update-time');

    if (!asksList || !bidsList) {
        console.error('Order book elements not found');
        return;
    }

    // Update asks with animation
    asksList.innerHTML = orderBook.asks.map((level, index) => `
        <div class="order-level" style="animation: fadeIn 0.3s ease ${index * 0.05}s">
            <span>${level.price.toFixed(2)}</span>
            <span>${level.quantity.toFixed(4)}</span>
        </div>
    `).join('');

    // Update bids with animation
    bidsList.innerHTML = orderBook.bids.map((level, index) => `
        <div class="order-level" style="animation: fadeIn 0.3s ease ${index * 0.05}s">
            <span>${level.price.toFixed(2)}</span>
            <span>${level.quantity.toFixed(4)}</span>
        </div>
    `).join('');

    // Update last update time
    if (lastUpdateTime && orderBook.lastUpdate) {
        lastUpdateTime.textContent = orderBook.lastUpdate.toLocaleTimeString();
        lastUpdateTime.style.animation = 'fadeIn 0.3s ease';
    }
}

// Connect to WebSocket with retry logic
function connectWebSocket() {
    try {
        setLoading(true);
        ws = new WebSocket(config.websocketUrl);
        
        ws.onopen = () => {
            console.log('WebSocket connected');
            isConnected = true;
            const statusElement = document.getElementById('connection-status');
            if (statusElement) {
                statusElement.textContent = 'Connected';
                statusElement.className = 'status-connected';
                statusElement.style.animation = 'fadeIn 0.3s ease';
            }
            setLoading(false);
        };
        
        ws.onclose = () => {
            console.log('WebSocket disconnected');
            isConnected = false;
            const statusElement = document.getElementById('connection-status');
            if (statusElement) {
                statusElement.textContent = 'Disconnected';
                statusElement.className = 'status-disconnected';
                statusElement.style.animation = 'fadeIn 0.3s ease';
            }
            performanceStats.reconnections++;
            updatePerformanceStats();
            // Attempt to reconnect after 5 seconds
            setTimeout(connectWebSocket, 5000);
        };
        
        ws.onerror = (error) => {
            console.error('WebSocket error:', error);
            const statusElement = document.getElementById('connection-status');
            if (statusElement) {
                statusElement.textContent = 'Error';
                statusElement.className = 'status-error';
                statusElement.style.animation = 'fadeIn 0.3s ease';
            }
            showError('WebSocket connection error. Attempting to reconnect...');
            setLoading(false);
        };
        
        ws.onmessage = (event) => {
            try {
                const startTime = performance.now();
                const data = JSON.parse(event.data);
                
                // Validate data structure
                if (!data.asks || !data.bids || !Array.isArray(data.asks) || !Array.isArray(data.bids)) {
                    throw new Error('Invalid order book data structure');
                }
                
                // Update order book with animation
                orderBook.asks = data.asks.map(([price, quantity]) => ({
                    price: parseFloat(price),
                    quantity: parseFloat(quantity)
                })).filter(level => !isNaN(level.price) && !isNaN(level.quantity));
                
                orderBook.bids = data.bids.map(([price, quantity]) => ({
                    price: parseFloat(price),
                    quantity: parseFloat(quantity)
                })).filter(level => !isNaN(level.price) && !isNaN(level.quantity));
                
                orderBook.lastUpdate = new Date(data.timestamp);
                
                // Calculate processing latency
                const endTime = performance.now();
                const latency = endTime - startTime;
                processingLatency.push(latency);
                if (processingLatency.length > 100) processingLatency.shift();
                
                // Update performance stats
                performanceStats.totalUpdates++;
                performanceStats.totalLatency += latency;
                performanceStats.maxLatency = Math.max(performanceStats.maxLatency, latency);
                performanceStats.minLatency = Math.min(performanceStats.minLatency, latency);
                
                // Update UI with animations
                updateOrderBook();
                updateResults();
                updatePerformanceStats();
            } catch (error) {
                console.error('Error processing WebSocket message:', error);
                showError('Error processing market data');
            }
        };
    } catch (error) {
        console.error('Error creating WebSocket connection:', error);
        showError('Failed to establish WebSocket connection');
        setLoading(false);
    }
}

// Almgren-Chriss model for market impact
function calculateAlmgrenChrissImpact(quantity, isBuy) {
    try {
        const levels = isBuy ? orderBook.asks : orderBook.bids;
        if (!levels || levels.length === 0) return 0.0;
        
        const midPrice = (levels[0].price + levels[0].price) / 2.0;
        const volatility = config.volatility;
        const timeHorizon = 1.0; // 1 hour
        const riskAversion = 0.1;
        
        // Temporary impact
        const eta = 0.1; // Linear impact parameter
        const tempImpact = eta * quantity / timeHorizon;
        
        // Permanent impact
        const gamma = 0.1; // Permanent impact parameter
        const permImpact = gamma * quantity;
        
        // Risk term
        const riskTerm = Math.sqrt(volatility * volatility * timeHorizon * riskAversion);
        
        return (tempImpact + permImpact + riskTerm) / midPrice;
    } catch (error) {
        console.error('Error calculating market impact:', error);
        return 0.0;
    }
}

// Calculate expected slippage using linear regression
function calculateExpectedSlippage(quantity, isBuy) {
    try {
        const levels = isBuy ? orderBook.asks : orderBook.bids;
        if (!levels || levels.length === 0) return 0.0;
        
        let totalQuantity = 0;
        let weightedPrice = 0;
        
        for (const level of levels) {
            if (totalQuantity >= quantity) break;
            const executedQuantity = Math.min(quantity - totalQuantity, level.quantity);
            weightedPrice += level.price * executedQuantity;
            totalQuantity += executedQuantity;
        }
        
        if (totalQuantity === 0) return 0.0;
        
        const averagePrice = weightedPrice / quantity;
        const midPrice = (levels[0].price + levels[0].price) / 2.0;
        return Math.abs(averagePrice - midPrice) / midPrice;
    } catch (error) {
        console.error('Error calculating slippage:', error);
        return 0.0;
    }
}

// Calculate maker/taker proportion using logistic regression
function calculateMakerTakerProportion(quantity, isBuy) {
    try {
        const levels = isBuy ? orderBook.asks : orderBook.bids;
        if (!levels || levels.length === 0) return { maker: 0.5, taker: 0.5 };
        
        // Simple logistic function based on order size relative to average level size
        const avgLevelSize = levels.reduce((sum, level) => sum + level.quantity, 0) / levels.length;
        const x = quantity / avgLevelSize;
        const makerProb = 1 / (1 + Math.exp(x - 2)); // Sigmoid function
        
        return {
            maker: makerProb,
            taker: 1 - makerProb
        };
    } catch (error) {
        console.error('Error calculating maker/taker proportion:', error);
        return { maker: 0.5, taker: 0.5 };
    }
}

// Calculate fees based on fee tier
function calculateFees(orderSize, isMaker) {
    try {
        const feeRate = isMaker ? 
            config.feeRates[config.feeTier].maker : 
            config.feeRates[config.feeTier].taker;
        return orderSize * feeRate;
    } catch (error) {
        console.error('Error calculating fees:', error);
        return 0.0;
    }
}

// Update performance stats display
function updatePerformanceStats() {
    const statsElement = document.getElementById('performance-stats');
    if (!statsElement) return;

    const avgLatency = performanceStats.totalUpdates > 0 ?
        performanceStats.totalLatency / performanceStats.totalUpdates : 0;

    statsElement.innerHTML = `
        <div class="result-card">
            <strong>Total Updates:</strong> ${performanceStats.totalUpdates}
        </div>
        <div class="result-card">
            <strong>Average Latency:</strong> ${avgLatency.toFixed(2)}ms
        </div>
        <div class="result-card">
            <strong>Max Latency:</strong> ${performanceStats.maxLatency.toFixed(2)}ms
        </div>
        <div class="result-card">
            <strong>Min Latency:</strong> ${performanceStats.minLatency === Infinity ? '-' : performanceStats.minLatency.toFixed(2) + 'ms'}
        </div>
        <div class="result-card">
            <strong>Errors:</strong> ${performanceStats.errors}
        </div>
        <div class="result-card">
            <strong>Reconnections:</strong> ${performanceStats.reconnections}
        </div>
    `;
}

// Update results with animations
function updateResults() {
    try {
        const orderSize = parseFloat(document.getElementById('order-size').value) || 0;
        const isBuy = document.getElementById('order-type').value === 'buy';
        
        // Calculate all metrics
        const marketImpact = calculateAlmgrenChrissImpact(orderSize, isBuy);
        const slippage = calculateExpectedSlippage(orderSize, isBuy);
        const { maker, taker } = calculateMakerTakerProportion(orderSize, isBuy);
        const makerFee = calculateFees(orderSize, true);
        const takerFee = calculateFees(orderSize, false);
        const netCost = (marketImpact + slippage) * orderSize + (makerFee + takerFee);
        
        // Calculate average processing latency
        const avgLatency = processingLatency.length > 0 ?
            processingLatency.reduce((a, b) => a + b) / processingLatency.length : 0;
        
        // Update UI elements with animations
        const elements = {
            'market-impact': `<strong>Market Impact (Almgren-Chriss):</strong> ${(marketImpact * 100).toFixed(2)}%`,
            'slippage': `<strong>Expected Slippage:</strong> ${(slippage * 100).toFixed(2)}%`,
            'fees': `<strong>Fees for ${orderSize} BTC:</strong><br>Maker fee: ${makerFee.toFixed(8)} BTC<br>Taker fee: ${takerFee.toFixed(8)} BTC`,
            'net-cost': `<strong>Net Cost:</strong> ${netCost.toFixed(8)} BTC`,
            'maker-taker': `<strong>Maker/Taker Proportion:</strong><br>Maker: ${(maker * 100).toFixed(1)}%<br>Taker: ${(taker * 100).toFixed(1)}%`,
            'latency': `<strong>Average Processing Latency:</strong> ${avgLatency.toFixed(2)}ms`
        };
        
        // Update each element with animation
        Object.entries(elements).forEach(([id, content], index) => {
            const element = document.getElementById(id);
            if (element) {
                element.style.animation = 'none';
                element.offsetHeight; // Trigger reflow
                element.innerHTML = content;
                element.style.animation = `fadeIn 0.3s ease ${index * 0.1}s`;
            }
        });
    } catch (error) {
        console.error('Error updating results:', error);
        showError('Error updating results');
    }
}

// Reset to default values
function resetToDefaults() {
    try {
        const orderSizeInput = document.getElementById('order-size');
        const volatilityInput = document.getElementById('volatility');
        const feeTierSelect = document.getElementById('fee-tier');
        
        if (orderSizeInput) orderSizeInput.value = config.defaultValues.orderSize;
        if (volatilityInput) volatilityInput.value = config.defaultValues.volatility;
        if (feeTierSelect) feeTierSelect.value = config.defaultValues.feeTier;
        
        config.volatility = config.defaultValues.volatility / 100;
        config.feeTier = config.defaultValues.feeTier;
        
        updateResults();
    } catch (error) {
        console.error('Error resetting to defaults:', error);
        showError('Error resetting to defaults');
    }
}

// Toggle documentation modal
function toggleDocumentation() {
    const modal = document.getElementById('docs-modal');
    if (modal) {
        if (modal.classList.contains('hidden')) {
            modal.classList.remove('hidden');
            document.body.style.overflow = 'hidden'; // Prevent background scrolling
            modal.style.display = 'flex';
            modal.style.opacity = '0';
            // Trigger reflow
            modal.offsetHeight;
            modal.style.opacity = '1';
        } else {
            modal.style.opacity = '0';
            setTimeout(() => {
                modal.classList.add('hidden');
                modal.style.display = 'none';
                document.body.style.overflow = ''; // Restore scrolling
            }, 300);
        }
    }
}

// Close modal when clicking outside
function handleModalClick(event) {
    const modal = document.getElementById('docs-modal');
    const modalContent = document.querySelector('.modal-content');
    if (modal && !modalContent.contains(event.target) && event.target === modal) {
        toggleDocumentation();
    }
}

// Close modal with Escape key
function handleEscapeKey(event) {
    if (event.key === 'Escape') {
        const modal = document.getElementById('docs-modal');
        if (modal && !modal.classList.contains('hidden')) {
            toggleDocumentation();
        }
    }
}

// Initialize the simulator
document.addEventListener('DOMContentLoaded', () => {
    try {
        // Initialize animations
        initializeAnimations();
        
        // Set up event listeners
        const calculateBtn = document.getElementById('calculate-btn');
        if (calculateBtn) {
            calculateBtn.addEventListener('click', updateResults);
        }
        
        const resetBtn = document.getElementById('reset-btn');
        if (resetBtn) {
            resetBtn.addEventListener('click', resetToDefaults);
        }
        
        const volatilityInput = document.getElementById('volatility');
        if (volatilityInput) {
            volatilityInput.addEventListener('input', (e) => {
                config.volatility = parseFloat(e.target.value) / 100;
                updateResults();
            });
        }
        
        const feeTierSelect = document.getElementById('fee-tier');
        if (feeTierSelect) {
            feeTierSelect.addEventListener('change', (e) => {
                config.feeTier = e.target.value;
                updateResults();
            });
        }
        
        // Documentation modal event listeners
        const showDocsBtn = document.getElementById('show-docs');
        if (showDocsBtn) {
            showDocsBtn.addEventListener('click', (e) => {
                e.preventDefault();
                toggleDocumentation();
            });
        }
        
        const closeDocsBtn = document.getElementById('close-docs');
        if (closeDocsBtn) {
            closeDocsBtn.addEventListener('click', toggleDocumentation);
        }
        
        // Add click outside listener for modal
        const modal = document.getElementById('docs-modal');
        if (modal) {
            modal.addEventListener('click', handleModalClick);
        }
        
        // Add escape key listener
        document.addEventListener('keydown', handleEscapeKey);
        
        const themeToggle = document.getElementById('theme-toggle');
        if (themeToggle) {
            themeToggle.addEventListener('click', toggleTheme);
        }
        
        // Set up scroll effects
        window.addEventListener('scroll', () => {
            handleHeaderScroll();
            animateOnScroll();
        });
        
        // Set up smooth scroll for navigation
        document.querySelectorAll('a[href^="#"]').forEach(anchor => {
            anchor.addEventListener('click', (e) => {
                e.preventDefault();
                smoothScroll(anchor.getAttribute('href'));
            });
        });
        
        // Connect to WebSocket
        connectWebSocket();
        
        // Initial calculations
        updateResults();
        updatePerformanceStats();
    } catch (error) {
        console.error('Error initializing simulator:', error);
        showError('Error initializing simulator');
    }
}); 