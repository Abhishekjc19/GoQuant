import React from 'react';

interface StatusIndicatorProps {
  connected: boolean;
  label?: string;
}

const StatusIndicator: React.FC<StatusIndicatorProps> = ({ 
  connected, 
  label = connected ? 'Connected' : 'Disconnected'
}) => {
  return (
    <div className="flex items-center">
      <div 
        className={`
          w-3 
          h-3 
          rounded-full 
          mr-2
          ${connected ? 'bg-green-500' : 'bg-red-500'}
          ${connected ? 'animate-pulse' : ''}
        `}
      />
      <span className="text-sm text-white/80">{label}</span>
    </div>
  );
};

export default StatusIndicator;