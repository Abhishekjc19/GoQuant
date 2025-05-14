import React, { useEffect, useRef } from 'react';
import { OrderBook } from '../types';

interface DepthChartProps {
  orderBook: OrderBook;
  width?: number;
  height?: number;
}

const DepthChart: React.FC<DepthChartProps> = ({ 
  orderBook, 
  width = 240, 
  height = 120 
}) => {
  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    // Clear canvas
    ctx.clearRect(0, 0, width, height);

    // Calculate cumulative volumes
    const cumulativeBids: { price: number; total: number }[] = [];
    const cumulativeAsks: { price: number; total: number }[] = [];
    
    let bidTotal = 0;
    for (const bid of orderBook.bids) {
      bidTotal += bid.size;
      cumulativeBids.push({ price: bid.price, total: bidTotal });
    }
    
    let askTotal = 0;
    for (const ask of orderBook.asks) {
      askTotal += ask.size;
      cumulativeAsks.push({ price: ask.price, total: askTotal });
    }
    
    // Find min/max for scaling
    const minPrice = Math.min(...orderBook.bids.map(b => b.price));
    const maxPrice = Math.max(...orderBook.asks.map(a => a.price));
    const maxVolume = Math.max(
      cumulativeBids.length > 0 ? cumulativeBids[cumulativeBids.length - 1].total : 0,
      cumulativeAsks.length > 0 ? cumulativeAsks[cumulativeAsks.length - 1].total : 0
    );

    // Scale to canvas
    const scaleX = width / (maxPrice - minPrice);
    const scaleY = (height - 10) / maxVolume;

    // Draw depth chart
    if (cumulativeBids.length > 0) {
      // Bids (green)
      ctx.beginPath();
      ctx.moveTo(0, height);
      
      for (let i = 0; i < cumulativeBids.length; i++) {
        const x = (cumulativeBids[i].price - minPrice) * scaleX;
        const y = height - (cumulativeBids[i].total * scaleY);
        ctx.lineTo(x, y);
      }
      
      ctx.lineTo(0, height - (cumulativeBids[cumulativeBids.length - 1].total * scaleY));
      ctx.closePath();
      
      ctx.fillStyle = 'rgba(16, 185, 129, 0.2)';
      ctx.fill();
      
      ctx.strokeStyle = 'rgba(16, 185, 129, 0.8)';
      ctx.lineWidth = 1;
      ctx.stroke();
    }

    if (cumulativeAsks.length > 0) {
      // Asks (red)
      ctx.beginPath();
      ctx.moveTo(width, height);
      
      for (let i = cumulativeAsks.length - 1; i >= 0; i--) {
        const x = (cumulativeAsks[i].price - minPrice) * scaleX;
        const y = height - (cumulativeAsks[i].total * scaleY);
        ctx.lineTo(x, y);
      }
      
      ctx.lineTo(width, height - (cumulativeAsks[0].total * scaleY));
      ctx.closePath();
      
      ctx.fillStyle = 'rgba(239, 68, 68, 0.2)';
      ctx.fill();
      
      ctx.strokeStyle = 'rgba(239, 68, 68, 0.8)';
      ctx.lineWidth = 1;
      ctx.stroke();
    }

    // Draw midpoint line
    const midPrice = (orderBook.bids[0]?.price + orderBook.asks[0]?.price) / 2;
    const midX = (midPrice - minPrice) * scaleX;
    
    ctx.beginPath();
    ctx.moveTo(midX, 0);
    ctx.lineTo(midX, height);
    ctx.strokeStyle = 'rgba(255, 215, 0, 0.5)';
    ctx.lineWidth = 1;
    ctx.stroke();

  }, [orderBook, width, height]);

  return (
    <canvas 
      ref={canvasRef} 
      width={width} 
      height={height}
      className="w-full h-auto bg-black/10 rounded-lg border border-white/10"
    />
  );
};

export default DepthChart;