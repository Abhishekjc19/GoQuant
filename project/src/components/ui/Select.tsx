import React from 'react';

interface SelectOption {
  value: string;
  label: string;
}

interface SelectProps {
  options: SelectOption[];
  value: string;
  onChange: (value: string) => void;
  label: string;
  className?: string;
}

const Select: React.FC<SelectProps> = ({ 
  options, 
  value, 
  onChange, 
  label, 
  className = '' 
}) => {
  return (
    <div className={`mb-4 ${className}`}>
      <label className="block text-white font-semibold mb-2">
        {label}
      </label>
      <select
        value={value}
        onChange={(e) => onChange(e.target.value)}
        className="
          w-full
          bg-black/30
          text-white
          border border-white/20
          hover:border-yellow-400
          focus:border-yellow-400
          rounded-lg
          py-2
          px-3
          appearance-none
          transition-colors
          focus:outline-none
          cursor-pointer
        "
      >
        {options.map((option) => (
          <option key={option.value} value={option.value}>
            {option.label}
          </option>
        ))}
      </select>
    </div>
  );
};

export default Select;