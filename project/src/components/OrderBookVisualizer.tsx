import React, { useMemo } from 'react';
import { OrderBook, OrderBookEntry } from '../types';

interface OrderBookVisualizerProps {
  orderBook: OrderBook;
}

const OrderBookVisualizer: React.FC<OrderBookVisualizerProps> = ({ orderBook }) => {
  // Calculate total volumes for normalization
  const maxBidVolume = useMemo(() => {
    return Math.max(...orderBook.bids.map(bid => bid.size));
  }, [orderBook.bids]);

  const maxAskVolume = useMemo(() => {
    return Math.max(...orderBook.asks.map(ask => ask.size));
  }, [orderBook.asks]);

  const maxVolume = Math.max(maxBidVolume, maxAskVolume);

  // Process order book entries with totals
  const processedBids = useMemo(() => {
    return orderBook.bids.map((bid, index, array) => {
      const total = array.slice(0, index + 1).reduce((sum, current) => sum + current.size, 0);
      return { ...bid, total };
    });
  }, [orderBook.bids]);

  const processedAsks = useMemo(() => {
    return orderBook.asks.map((ask, index, array) => {
      const total = array.slice(0, index + 1).reduce((sum, current) => sum + current.size, 0);
      return { ...ask, total };
    });
  }, [orderBook.asks]);

  const renderOrderBookRow = (entry: OrderBookEntry, maxVolume: number, type: 'bid' | 'ask') => {
    const volumePercentage = (entry.size / maxVolume) * 100;
    
    return (
      <div key={entry.price} className="flex items-center h-8 relative">
        <div 
          className={`absolute h-full ${type === 'bid' ? 'bg-green-500/20' : 'bg-red-500/20'}`}
          style={{ width: `${volumePercentage}%`, right: type === 'bid' ? 0 : 'auto', left: type === 'ask' ? 0 : 'auto' }}
        />
        <div className="relative flex w-full justify-between px-2 z-10">
          <span className={`font-mono ${type === 'bid' ? 'text-green-400' : 'text-red-400'}`}>
            {entry.price.toFixed(2)}
          </span>
          <span className="text-white/80 font-mono">{entry.size.toFixed(4)}</span>
        </div>
      </div>
    );
  };

  return (
    <div className="mt-4 bg-black/20 rounded-lg overflow-hidden border border-white/10">
      <div className="flex justify-between p-2 bg-black/30 text-white/70 text-sm">
        <span>Price</span>
        <span>Size</span>
      </div>
      
      <div className="asks border-b border-white/10">
        {processedAsks.map(ask => renderOrderBookRow(ask, maxVolume, 'ask'))}
      </div>
      
      <div className="bids">
        {processedBids.map(bid => renderOrderBookRow(bid, maxVolume, 'bid'))}
      </div>
    </div>
  );
};

export default OrderBookVisualizer;