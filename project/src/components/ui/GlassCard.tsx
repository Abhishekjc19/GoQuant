import React, { ReactNode } from 'react';

interface GlassCardProps {
  children: ReactNode;
  className?: string;
}

const GlassCard: React.FC<GlassCardProps> = ({ children, className = '' }) => {
  return (
    <div className={`
      bg-black/10
      backdrop-blur-md
      border border-white/20
      rounded-xl
      shadow-lg
      p-6
      ${className}
    `}>
      {children}
    </div>
  );
};

export default GlassCard;