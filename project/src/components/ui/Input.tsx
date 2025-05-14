import React from 'react';

interface InputProps {
  type?: string;
  label: string;
  value: string | number;
  onChange: (value: string) => void;
  min?: number;
  max?: number;
  step?: number;
  className?: string;
}

const Input: React.FC<InputProps> = ({
  type = 'text',
  label,
  value,
  onChange,
  min,
  max,
  step,
  className = '',
}) => {
  return (
    <div className={`mb-4 ${className}`}>
      <label className="block text-white font-semibold mb-2">
        {label}
      </label>
      <input
        type={type}
        value={value}
        onChange={(e) => onChange(e.target.value)}
        min={min}
        max={max}
        step={step}
        className="
          w-full
          bg-black/30
          text-white
          border border-white/20
          hover:border-yellow-400
          focus:border-yellow-400
          focus:outline-none
          rounded-lg
          py-2
          px-3
          transition-colors
        "
      />
    </div>
  );
};

export default Input;