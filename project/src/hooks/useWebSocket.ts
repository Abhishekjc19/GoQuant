import { useState, useEffect, useRef, useCallback } from 'react';
import { OrderBook, OrderBookEntry } from '../types';
import { MOCK_ORDERBOOK } from '../constants';

interface UseWebSocketReturn {
  connected: boolean;
  latency: number;
  orderBook: OrderBook;
  connect: () => void;
  disconnect: () => void;
}

export default function useWebSocket(url: string): UseWebSocketReturn {
  const [connected, setConnected] = useState(false);
  const [latency, setLatency] = useState(0);
  const [orderBook, setOrderBook] = useState<OrderBook>(MOCK_ORDERBOOK);
  
  const wsRef = useRef<WebSocket | null>(null);
  const lastMessageTime = useRef<number | null>(null);

  // Generate simulated order book updates
  const generateOrderBookUpdate = useCallback(() => {
    const bids: OrderBookEntry[] = [];
    const asks: OrderBookEntry[] = [];
    
    const midPrice = 61254 + (Math.random() * 10 - 5);
    
    // Generate bids (below mid price)
    for (let i = 0; i < 5; i++) {
      const price = midPrice - (i * 0.7) - (Math.random() * 0.5);
      const size = 0.5 + Math.random() * 3;
      bids.push({ price, size });
    }
    
    // Generate asks (above mid price)
    for (let i = 0; i < 5; i++) {
      const price = midPrice + (i * 0.7) + (Math.random() * 0.5);
      const size = 0.5 + Math.random() * 3;
      asks.push({ price, size });
    }
    
    // Sort bids in descending order (highest price first)
    bids.sort((a, b) => b.price - a.price);
    
    // Sort asks in ascending order (lowest price first)
    asks.sort((a, b) => a.price - b.price);
    
    return {
      bids,
      asks,
      timestamp: Date.now()
    };
  }, []);

  // Connect to WebSocket
  const connect = useCallback(() => {
    if (wsRef.current) return;

    try {
      // In a real app, this would connect to the actual WebSocket
      // For simulation, we'll use a timer to simulate updates
      setConnected(true);
      
      // Simulate connection delay
      const connectionInterval = setInterval(() => {
        const now = Date.now();
        lastMessageTime.current = now;
        
        // Generate new order book data
        const newOrderBook = generateOrderBookUpdate();
        setOrderBook(newOrderBook);
        
        // Calculate simulated latency (1-15ms)
        const simulatedLatency = 1 + Math.random() * 14;
        setLatency(simulatedLatency);
      }, 1000);
      
      wsRef.current = { close: () => clearInterval(connectionInterval) } as unknown as WebSocket;
    } catch (error) {
      console.error('WebSocket connection error:', error);
      setConnected(false);
    }
  }, [generateOrderBookUpdate]);

  // Disconnect from WebSocket
  const disconnect = useCallback(() => {
    if (!wsRef.current) return;
    
    wsRef.current.close();
    wsRef.current = null;
    setConnected(false);
    setLatency(0);
  }, []);

  // Clean up on unmount
  useEffect(() => {
    return () => {
      if (wsRef.current) {
        wsRef.current.close();
      }
    };
  }, []);

  return {
    connected,
    latency,
    orderBook,
    connect,
    disconnect
  };
}