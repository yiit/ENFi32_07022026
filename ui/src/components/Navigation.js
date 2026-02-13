/**
 * Modern Navigation Component
 * Professional Touch-Optimized Menu System
 */

import { h } from 'preact';
import { useState, useEffect } from 'preact/hooks';
import { route } from 'preact-router';

export function Navigation({ isConnected, systemInfo }) {
    const [currentRoute, setCurrentRoute] = useState('/');
    const [menuOpen, setMenuOpen] = useState(false);
    const [theme, setTheme] = useState('auto');

    useEffect(() => {
        // Get current route
        setCurrentRoute(window.location.pathname);
        
        // Load theme
        const savedTheme = localStorage.getItem('enfi32-theme') || 'auto';
        setTheme(savedTheme);
        updateTheme(savedTheme);
    }, []);

    const menuItems = [
        {
            icon: '📊',
            title: 'Dashboard', 
            href: '/dashboard',
            description: 'Real-time system overview'
        },
        {
            icon: '🔧',
            title: 'Devices',
            href: '/devices', 
            description: 'Sensor & actuator management'
        },
        {
            icon: '🎛️',
            title: 'Controllers',
            href: '/controllers',
            description: 'Protocol & communication'
        },
        {
            icon: '🤖',
            title: 'Automation',
            href: '/automation',
            description: 'Rules & automation logic'
        },
        {
            icon: '⚙️',
            title: 'Config',
            href: '/config',
            description: 'System configuration'
        },
        {
            icon: '🛠️',
            title: 'Tools',
            href: '/tools',
            description: 'Utilities & diagnostics'
        }
    ];

    const navigateTo = (href) => {
        route(href);
        setCurrentRoute(href);
        setMenuOpen(false);
    };

    const toggleTheme = () => {
        const themes = ['auto', 'light', 'dark'];
        const currentIndex = themes.indexOf(theme);
        const nextTheme = themes[(currentIndex + 1) % themes.length];
        
        setTheme(nextTheme);
        updateTheme(nextTheme);
        localStorage.setItem('enfi32-theme', nextTheme);
    };

    const updateTheme = (newTheme) => {
        const root = document.documentElement;
        
        if (newTheme === 'dark') {
            root.setAttribute('data-theme', 'dark');
        } else if (newTheme === 'light') {
            root.setAttribute('data-theme', 'light');
        } else {
            root.removeAttribute('data-theme');
        }
    };

    const getThemeIcon = () => {
        switch (theme) {
            case 'light': return '🌕';
            case 'dark': return '🌑';
            default: return '🌗';
        }
    };

    return (
        <nav className="modern-navigation">
            {/* Header */}
            <div className="nav-header">
                <div className="nav-brand" onClick={() => navigateTo('/dashboard')}>
                    <span className="brand-icon">🚀</span>
                    <span className="brand-text">ENFi32</span>
                    <span className="brand-version">v2.0</span>
                </div>
                
                <div className="nav-actions">
                    {/* Theme Toggle */}
                    <button 
                        className="nav-btn theme-toggle"
                        onClick={toggleTheme}
                        title={`Theme: ${theme}`}
                    >
                        {getThemeIcon()}
                    </button>
                    
                    {/* Connection Status */}
                    <div className={`connection-indicator ${isConnected ? 'connected' : 'disconnected'}`}>
                        <span className="connection-dot"></span>
                    </div>
                    
                    {/* Mobile Menu Toggle */}
                    <button 
                        className={`nav-btn menu-toggle ${menuOpen ? 'active' : ''}`}
                        onClick={() => setMenuOpen(!menuOpen)}
                    >
                        <span></span>
                        <span></span>
                        <span></span>
                    </button>
                </div>
            </div>
            
            {/* Menu */}
            <div className={`nav-menu ${menuOpen ? 'open' : ''}`}>
                {menuItems.map((item) => {
                    const isActive = currentRoute === item.href || 
                                   (item.href !== '/' && currentRoute.startsWith(item.href));
                    
                    return (
                        <div
                            key={item.href}
                            className={`nav-item ${isActive ? 'active' : ''}`}
                            onClick={() => navigateTo(item.href)}
                        >
                            <div className="nav-item-content">
                                <span className="nav-icon">{item.icon}</span>
                                <div className="nav-text">
                                    <div className="nav-title">{item.title}</div>
                                    <div className="nav-desc">{item.description}</div>
                                </div>
                            </div>
                            
                            {isActive && <div className="nav-active-indicator"></div>}
                        </div>
                    );
                })}
            </div>
            
            {/* Footer Info */}
            {systemInfo && (
                <div className="nav-footer">
                    <div className="system-stats">
                        <div className="stat">
                            <span className="stat-label">Uptime</span>
                            <span className="stat-value">
                                {formatUptime(systemInfo.uptime)}
                            </span>
                        </div>
                        <div className="stat">
                            <span className="stat-label">Free RAM</span>
                            <span className="stat-value">
                                {formatBytes(systemInfo.memory)}
                            </span>
                        </div>
                        <div className="stat">
                            <span className="stat-label">Load</span>
                            <span className="stat-value">
                                {systemInfo.load}%
                            </span>
                        </div>
                    </div>
                </div>
            )}
        </nav>
    );
}

// Utility functions
function formatUptime(seconds) {
    if (!seconds) return '--';
    
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const mins = Math.floor((seconds % 3600) / 60);
    
    if (days > 0) {
        return `${days}d ${hours}h`;
    } else if (hours > 0) {
        return `${hours}h ${mins}m`;
    } else {
        return `${mins}m`;
    }
}

function formatBytes(bytes) {
    if (!bytes) return '--';
    
    const kb = bytes / 1024;
    if (kb < 1024) {
        return `${kb.toFixed(1)}KB`;
    } else {
        return `${(kb / 1024).toFixed(1)}MB`;
    }
}