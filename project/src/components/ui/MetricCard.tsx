import React from 'react';

interface MetricCardProps {
  label: string;
  value: number | string;
  unit?: string;
  tooltip?: string;
}

const MetricCard: React.FC<MetricCardProps> = ({ 
  label, 
  value, 
  unit = '', 
  tooltip 
}) => {
  return (
    <div 
      className="bg-black/20 p-3 rounded-lg mb-3"
      title={tooltip}
    >
      <div className="text-white/70 text-sm mb-1">{label}</div>
      <div className="flex items-baseline">
        <span className="text-yellow-400 font-bold text-xl mr-1">
          {typeof value === 'number' ? value.toLocaleString('en-US', { 
            minimumFractionDigits: 2,
            maximumFractionDigits: 4
          }) : value}
        </span>
        {unit && <span className="text-white/70 text-xs">{unit}</span>}
      </div>
    </div>
  );
};

export default MetricCard;