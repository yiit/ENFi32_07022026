/**
 * ENFi32 Modern UI - Main Application Entry
 * Professional Preact-based IoT Dashboard
 */

import { h, render, Fragment } from 'preact';
import { Router } from 'preact-router';
import { useState, useEffect } from 'preact/hooks';

// Core Components
import { Navigation } from './components/Navigation';
import { ThemeProvider } from './components/ThemeProvider';
import { LoadingProvider } from './components/LoadingProvider';

// Pages
import { DashboardPage } from './pages/DashboardPage';
import { DevicesPage } from './pages/DevicesPage';
import { ControllersPage } from './pages/ControllersPage';
import { AutomationPage } from './pages/AutomationPage';
import { ConfigPage } from './pages/ConfigPage';
import { ToolsPage } from './pages/ToolsPage';
import { NotFoundPage } from './pages/NotFoundPage';

// Services
import { ApiService } from './services/ApiService';
import { SettingsService } from './services/SettingsService';

// Styles
import './styles/modern-ui.css';
import './styles/components.css';
import './styles/responsive.css';

// Main Application Component
function App() {
    const [isConnected, setIsConnected] = useState(false);
    const [systemInfo, setSystemInfo] = useState(null);
    const [loading, setLoading] = useState(true);

    // Initialize application
    useEffect(() => {
        initializeApp();
    }, []);

    const initializeApp = async () => {
        try {
            console.log('🚀 ENFi32 Modern UI Starting...');
            
            // Load settings
            await SettingsService.loadSettings();
            
            // Check ESP32 connection
            const connected = await ApiService.checkConnection();
            setIsConnected(connected);
            
            if (connected) {
                // Load system information
                const sysInfo = await ApiService.getSystemInfo();
                setSystemInfo(sysInfo);
                console.log('✅ Connected to ESP32:', sysInfo);
            }
            
        } catch (error) {
            console.error('❌ App initialization failed:', error);
        } finally {
            setLoading(false);
        }
    };

    const handleRouteChange = (e) => {
        // Analytics or logging could go here
        console.log('Route changed to:', e.url);
    };

    if (loading) {
        return (
            <div className="app-loading">
                <div className="loading-content">
                    <div className="loading-spinner"></div>
                    <p>Initializing Modern UI...</p>
                </div>
            </div>
        );
    }

    return (
        <ThemeProvider>
            <LoadingProvider>
                <div className="enfi32-app">
                    {/* Connection Status Bar */}
                    <div className={`connection-status ${isConnected ? 'connected' : 'disconnected'}`}>
                        <div className="status-indicator">
                            <span className={`status-dot ${isConnected ? 'online' : 'offline'}`}></span>
                            <span className="status-text">
                                {isConnected ? '🔗 Connected to ESP32' : '❌ ESP32 Disconnected'}
                            </span>
                        </div>
                        
                        {systemInfo && (
                            <div className="system-info">
                                <span>📟 {systemInfo.name || 'ENFi32'}</span>
                                <span>📶 {systemInfo.wifi?.rssi || '--'}dBm</span>
                                <span>💾 {systemInfo.system?.memory || '--'}KB</span>
                            </div>
                        )}
                    </div>

                    {/* Navigation */}
                    <Navigation isConnected={isConnected} systemInfo={systemInfo} />
                    
                    {/* Main Content Router */}
                    <main className="app-content">
                        <Router onChange={handleRouteChange}>
                            <DashboardPage path="/" default />
                            <DashboardPage path="/dashboard" />
                            <DevicesPage path="/devices" />
                            <DevicesPage path="/devices/:deviceId" />
                            <ControllersPage path="/controllers" />
                            <AutomationPage path="/automation" />
                            <AutomationPage path="/rules" />
                            <ConfigPage path="/config" />
                            <ConfigPage path="/config/:section" />
                            <ToolsPage path="/tools" />
                            <ToolsPage path="/tools/:tool" />
                            <NotFoundPage default />
                        </Router>
                    </main>
                    
                    {/* Global Notifications */}
                    <div id="notifications" className="notifications-container"></div>
                </div>
            </LoadingProvider>
        </ThemeProvider>
    );
}

// Application Bootstrap
function bootstrap() {
    // Remove any existing preloader
    const preloader = document.getElementById('preloader');
    if (preloader) {
        setTimeout(() => {
            preloader.classList.add('fade-out');
            setTimeout(() => preloader.remove(), 500);
        }, 100);
    }

    // Render application
    const appContainer = document.getElementById('app');
    if (appContainer) {
        render(<App />, appContainer);
        console.log('🎉 ENFi32 Modern UI Loaded Successfully');
    } else {
        console.error('❌ App container not found');
    }
}

// Start application when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', bootstrap);
} else {
    bootstrap();
}

// Development hot reload support
if (module.hot) {
    module.hot.accept();
}

// Export for debugging
window.ENFi32 = {
    ApiService,
    SettingsService,
    version: '2.0.0'
};

export default App;