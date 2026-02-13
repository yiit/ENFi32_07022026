/**
 * Theme Provider Component
 * Global theme management with system preference detection
 */

import { h, createContext } from 'preact';
import { useState, useEffect, useContext } from 'preact/hooks';

const ThemeContext = createContext();

export function ThemeProvider({ children }) {
    const [theme, setTheme] = useState('auto');
    const [isDark, setIsDark] = useState(false);

    useEffect(() => {
        // Load saved theme
        const saved = localStorage.getItem('enfi32-theme') || 'auto';
        setTheme(saved);
        updateThemeState(saved);
        
        // Watch for system theme changes
        const mediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
        
        const handleChange = () => {
            if (theme === 'auto') {
                updateThemeState('auto');
            }
        };
        
        mediaQuery.addListener(handleChange);
        
        return () => mediaQuery.removeListener(handleChange);
    }, [theme]);

    const updateThemeState = (newTheme) => {
        const root = document.documentElement;
        let dark = false;
        
        if (newTheme === 'dark') {
            root.setAttribute('data-theme', 'dark');
            dark = true;
        } else if (newTheme === 'light') {
            root.setAttribute('data-theme', 'light');
            dark = false;
        } else {
            // Auto mode
            root.removeAttribute('data-theme');
            dark = window.matchMedia('(prefers-color-scheme: dark)').matches;
        }
        
        setIsDark(dark);
    };

    const changeTheme = (newTheme) => {
        setTheme(newTheme);
        updateThemeState(newTheme);
        localStorage.setItem('enfi32-theme', newTheme);
    };

    const contextValue = {
        theme,
        isDark,
        changeTheme,
        toggleTheme: () => {
            const themes = ['auto', 'light', 'dark'];
            const currentIndex = themes.indexOf(theme);
            const nextTheme = themes[(currentIndex + 1) % themes.length];
            changeTheme(nextTheme);
        }
    };

    return (
        <ThemeContext.Provider value={contextValue}>
            {children}
        </ThemeContext.Provider>
    );
}

export const useTheme = () => {
    const context = useContext(ThemeContext);
    if (!context) {
        throw new Error('useTheme must be used within a ThemeProvider');
    }
    return context;
};