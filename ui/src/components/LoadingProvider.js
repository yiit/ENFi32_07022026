/**
 * Loading Provider Component  
 * Global loading state management
 */

import { h, createContext } from 'preact';
import { useState, useContext } from 'preact/hooks';

const LoadingContext = createContext();

export function LoadingProvider({ children }) {
    const [loading, setLoading] = useState(false);
    const [loadingText, setLoadingText] = useState('Loading...');
    const [loadingQueue, setLoadingQueue] = useState([]);

    const showLoading = (text = 'Loading...') => {
        setLoadingText(text);
        setLoadingQueue(prev => [...prev, text]);
        setLoading(true);
    };

    const hideLoading = (text) => {
        setLoadingQueue(prev => {
            const filtered = text ? prev.filter(item => item !== text) : prev.slice(1);
            
            if (filtered.length === 0) {
                setLoading(false);
                setLoadingText('Loading...');
            } else {
                setLoadingText(filtered[filtered.length - 1]);
            }
            
            return filtered;
        });
    };

    const contextValue = {
        loading,
        loadingText,
        showLoading,
        hideLoading,
        isLoading: loadingQueue.length > 0
    };

    return (
        <LoadingContext.Provider value={contextValue}>
            {children}
            
            {/* Global Loading Overlay */}
            {loading && (
                <div className="global-loading-overlay">
                    <div className="loading-content">
                        <div className="loading-spinner">
                            <div className="spinner-ring"></div>
                        </div>
                        <div className="loading-text">{loadingText}</div>
                    </div>
                </div>
            )}
        </LoadingContext.Provider>
    );
}

export const useLoading = () => {
    const context = useContext(LoadingContext);
    if (!context) {
        throw new Error('useLoading must be used within a LoadingProvider');
    }
    return context;
};