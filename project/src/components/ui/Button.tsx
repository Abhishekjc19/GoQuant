import React from 'react';

interface ButtonProps {
  children: React.ReactNode;
  onClick?: () => void;
  variant?: 'primary' | 'secondary';
  className?: string;
  disabled?: boolean;
}

const Button: React.FC<ButtonProps> = ({
  children,
  onClick,
  variant = 'primary',
  className = '',
  disabled = false,
}) => {
  const baseClasses = `px-4 
    py-2 
    rounded-lg 
    font-semibold 
    transition-all 
    duration-200
    focus:outline-none`;
  
  const variantClasses = {
    primary: "bg-yellow-400 text-black hover:bg-yellow-300 active:bg-yellow-500",
    secondary: "bg-white/10 text-white border border-white/20 hover:bg-white/20 active:bg-white/5"
  };

  const disabledClasses = "opacity-50 cursor-not-allowed";

  return (
    <button
      onClick={onClick}
      disabled={disabled}
      className={`
        ${baseClasses} 
        ${variantClasses[variant]} 
        ${disabled ? disabledClasses : ''}
        ${className}
      `}
    >
      {children}
    </button>
  );
};

export default Button;