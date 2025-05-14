import React, { useState, useEffect } from 'react';
import { Activity, BarChart3 } from 'lucide-react';

// Components
import GlassCard from './components/ui/GlassCard';
import Select from './components/ui/Select';
import Input from './components/ui/Input';
import Slider from './components/ui/Slider';
import Button from './components/ui/Button';
import StatusIndicator from './components/ui/StatusIndicator';
import MetricCard from './components/ui/MetricCard';
import OrderBookVisualizer from './components/OrderBookVisualizer';
import DepthChart from './components/DepthChart';

// Hooks
import useWebSocket from './hooks/useWebSocket';
import useTradeMetrics from './hooks/useTradeMetrics';

// Types & Constants
import { 
  EXCHANGES, 
  ASSET_PAIRS, 
  ORDER_TYPES, 
  FEE_TIERS,
  WEBSOCKET_URL
} from './constants';

function App() {
  // State for input controls
  const [exchange, setExchange] = useState(EXCHANGES[0].value);
  const [assetPair, setAssetPair] = useState(ASSET_PAIRS[0].value);
  const [orderType, setOrderType] = useState(ORDER_TYPES[0].value);
  const [quantity, setQuantity] = useState('100');
  const [volatility, setVolatility] = useState(30);
  const [feeTier, setFeeTier] = useState(FEE_TIERS[0].value);
  
  // Get current fee tier object
  const currentFeeTier = FEE_TIERS.find(tier => tier.value === feeTier) || FEE_TIERS[0];

  // WebSocket connection for order book data
  const { 
    connected, 
    latency: wsLatency, 
    orderBook, 
    connect, 
    disconnect 
  } = useWebSocket(WEBSOCKET_URL);

  // Calculate trade metrics
  const metrics = useTradeMetrics({
    orderBook,
    quantity: parseFloat(quantity) || 0,
    feeTier: currentFeeTier,
    orderType: orderType as 'market' | 'limit',
    volatility
  });

  // Connect/disconnect WebSocket based on UI state
  useEffect(() => {
    if (connected) {
      // Reconnect if exchange or asset pair changes
      disconnect();
      setTimeout(connect, 500);
    }
  }, [exchange, assetPair, connected, connect, disconnect]);

  // Handle connect button click
  const handleConnectClick = () => {
    if (connected) {
      disconnect();
    } else {
      connect();
    }
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-black to-gray-900 text-white">
      <div className="container mx-auto px-4 py-8">
        {/* Header */}
        <header className="flex items-center justify-between mb-8">
          <div className="flex items-center gap-2">
            <Activity className="h-8 w-8 text-yellow-400" />
            <h1 className="text-2xl font-bold">GoQuant Trade Simulator</h1>
          </div>
          <StatusIndicator connected={connected} />
        </header>

        {/* Main content */}
        <div className="grid grid-cols-1 lg:grid-cols-12 gap-6">
          {/* Left panel - Input section */}
          <div className="lg:col-span-5">
            <GlassCard className="h-full">
              <h2 className="text-xl font-semibold mb-6 flex items-center gap-2">
                <BarChart3 className="h-5 w-5 text-yellow-400" />
                Trade Parameters
              </h2>

              <Select
                label="Exchange"
                value={exchange}
                onChange={setExchange}
                options={EXCHANGES}
              />

              <Select
                label="Asset Pair"
                value={assetPair}
                onChange={setAssetPair}
                options={ASSET_PAIRS}
              />

              <Select
                label="Order Type"
                value={orderType}
                onChange={setOrderType}
                options={ORDER_TYPES}
              />

              <Input
                type="number"
                label="Quantity (USD)"
                value={quantity}
                onChange={setQuantity}
                min={1}
              />

              <Slider
                label="Market Volatility"
                value={volatility}
                onChange={setVolatility}
                min={0}
                max={100}
              />

              <Select
                label="Fee Tier"
                value={feeTier}
                onChange={setFeeTier}
                options={FEE_TIERS}
              />

              <div className="mt-6">
                <Button
                  onClick={handleConnectClick}
                  className="w-full"
                >
                  {connected ? 'Disconnect' : 'Connect WebSocket'}
                </Button>
              </div>

              <div className="mt-6">
                <h3 className="text-white/70 mb-2 text-sm">Fee Structure</h3>
                <div className="grid grid-cols-2 gap-2 bg-black/20 p-3 rounded-lg">
                  <div>
                    <div className="text-white/70 text-xs">Maker</div>
                    <div className="text-yellow-400 font-semibold">{currentFeeTier.maker}%</div>
                  </div>
                  <div>
                    <div className="text-white/70 text-xs">Taker</div>
                    <div className="text-yellow-400 font-semibold">{currentFeeTier.taker}%</div>
                  </div>
                </div>
              </div>
            </GlassCard>
          </div>

          {/* Center panel - Order book visualization */}
          <div className="lg:col-span-3">
            <GlassCard className="h-full">
              <h2 className="text-xl font-semibold mb-4">Order Book</h2>
              
              <DepthChart orderBook={orderBook} />
              
              <div className="mt-4">
                <div className="flex justify-between text-white/70 text-sm mb-2">
                  <span>Best Bid</span>
                  <span>Best Ask</span>
                </div>
                <div className="flex justify-between">
                  <span className="text-green-400 font-mono">
                    {orderBook.bids[0]?.price.toFixed(2) || '-'}
                  </span>
                  <span className="text-red-400 font-mono">
                    {orderBook.asks[0]?.price.toFixed(2) || '-'}
                  </span>
                </div>
                <div className="text-white/50 text-xs text-center mt-1">
                  Spread: {orderBook.asks[0] && orderBook.bids[0] ? 
                    (orderBook.asks[0].price - orderBook.bids[0].price).toFixed(2) : '-'}
                </div>
              </div>

              <OrderBookVisualizer orderBook={orderBook} />
            </GlassCard>
          </div>

          {/* Right panel - Output section */}
          <div className="lg:col-span-4">
            <GlassCard className="h-full">
              <h2 className="text-xl font-semibold mb-6">Trade Metrics</h2>

              <MetricCard 
                label="Slippage" 
                value={metrics.slippage} 
                unit="%" 
                tooltip="Price difference due to order size and market depth"
              />
              
              <MetricCard 
                label="Fees" 
                value={metrics.fees} 
                unit="USD" 
                tooltip="Trading fees based on selected fee tier"
              />
              
              <MetricCard 
                label="Market Impact" 
                value={metrics.marketImpact} 
                unit="%" 
                tooltip="Estimated price movement caused by this trade (Almgren-Chriss model)"
              />
              
              <MetricCard 
                label="Net Cost" 
                value={metrics.netCost} 
                unit="%" 
                tooltip="Total trading cost (slippage + fees + impact)"
              />
              
              <div className="mt-4 mb-3">
                <h3 className="text-white/70 text-sm mb-2">Maker/Taker Split</h3>
                <div className="h-4 bg-black/30 rounded-full overflow-hidden">
                  <div 
                    className="h-full bg-yellow-400"
                    style={{ width: `${metrics.makerPercentage}%` }}
                  />
                </div>
                <div className="flex justify-between text-xs mt-1">
                  <span className="text-white/70">Maker: {metrics.makerPercentage.toFixed(1)}%</span>
                  <span className="text-white/70">Taker: {metrics.takerPercentage.toFixed(1)}%</span>
                </div>
              </div>
              
              <MetricCard 
                label="Processing Latency" 
                value={connected ? wsLatency.toFixed(2) : "-"} 
                unit="ms" 
                tooltip="Time to process each tick of order book data"
              />

              <div className="mt-6 p-3 bg-yellow-400/10 border border-yellow-400/30 rounded-lg">
                <h3 className="text-yellow-400 font-semibold mb-1">Analysis Summary</h3>
                <p className="text-white/80 text-sm">
                  {connected ? (
                    <>
                      Trading {quantity || '0'} USD of {assetPair} on {exchange.toUpperCase()} 
                      with a {orderType} order has an estimated cost of {metrics.netCost.toFixed(2)}%.
                      {metrics.slippage > 0.5 && ' Consider breaking the order into smaller chunks to reduce slippage.'}
                    </>
                  ) : (
                    'Connect to WebSocket to see real-time trade analysis.'
                  )}
                </p>
              </div>
            </GlassCard>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;