import React from 'react';

interface SliderProps {
  label: string;
  value: number;
  onChange: (value: number) => void;
  min: number;
  max: number;
  step?: number;
  className?: string;
}

const Slider: React.FC<SliderProps> = ({
  label,
  value,
  onChange,
  min,
  max,
  step = 1,
  className = '',
}) => {
  return (
    <div className={`mb-4 ${className}`}>
      <div className="flex justify-between items-center mb-2">
        <label className="text-white font-semibold">{label}</label>
        <span className="text-yellow-400 font-mono">{value}</span>
      </div>
      <input
        type="range"
        min={min}
        max={max}
        step={step}
        value={value}
        onChange={(e) => onChange(Number(e.target.value))}
        className="
          w-full
          appearance-none
          h-2
          bg-black/30
          rounded-full
          outline-none
          cursor-pointer
          accent-yellow-400
        "
        style={{
          background: `linear-gradient(to right, #FFD700 0%, #FFD700 ${(value - min) / (max - min) * 100}%, rgba(255, 255, 255, 0.2) ${(value - min) / (max - min) * 100}%, rgba(255, 255, 255, 0.2) 100%)`
        }}
      />
    </div>
  );
};

export default Slider;