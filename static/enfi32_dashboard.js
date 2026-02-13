/**
 * ENFi32 Professional Dashboard - Modern IoT Interface
 * Advanced Chart.js Integration & Real-time Data Visualization
 */

class ENFi32Dashboard {
    constructor() {
        this.updateInterval = 5000; // 5 seconds
        this.dataHistory = {
            temperature: [],
            humidity: [],
            wifi: [],
            power: [],
            timestamps: []
        };
        this.maxDataPoints = 50;
        this.charts = {};
        this.isConnected = false;
        
        this.init();
    }

    async init() {
        console.log('🚀 Initializing ENFi32 Professional Dashboard');
        
        // Initialize charts if Chart.js is available
        if (typeof Chart !== 'undefined') {
            this.initializeCharts();
        }
        
        // Start data fetching
        this.startDataUpdates();
        
        // Setup event listeners
        this.setupEventListeners();
        
        // Initial data load
        await this.fetchSystemData();
        
        console.log('✅ Dashboard initialized successfully');
    }

    // Advanced Chart.js initialization with professional styling
    initializeCharts() {
        // Common chart options for professional appearance
        const commonOptions = {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: true,
                    position: 'top',
                    labels: {
                        usePointStyle: true,
                        padding: 15,
                        font: { size: 11 }
                    }
                }
            },
            scales: {
                x: {
                    grid: { color: 'rgba(255,255,255,0.1)' },
                    ticks: { color: '#8B949E', font: { size: 10 } }
                },
                y: {
                    grid: { color: 'rgba(255,255,255,0.1)' },
                    ticks: { color: '#8B949E', font: { size: 10 } }
                }
            },
            animation: { duration: 750 }
        };

        // Temperature Chart
        const tempCanvas = document.getElementById('temperature-chart');
        if (tempCanvas) {
            this.charts.temperature = new Chart(tempCanvas, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Temperature (°C)',
                        data: [],
                        borderColor: '#FF6B6B',
                        backgroundColor: 'rgba(255, 107, 107, 0.1)',
                        borderWidth: 2,
                        fill: true,
                        tension: 0.4
                    }]
                },
                options: {
                    ...commonOptions,
                    scales: {
                        ...commonOptions.scales,
                        y: { 
                            ...commonOptions.scales.y,
                            min: 0, max: 50,
                            title: { display: true, text: '°C', color: '#8B949E' }
                        }
                    }
                }
            });
        }

        // WiFi Signal Chart
        const wifiCanvas = document.getElementById('wifi-chart');
        if (wifiCanvas) {
            this.charts.wifi = new Chart(wifiCanvas, {
                type: 'doughnut',
                data: {
                    labels: ['Signal Strength', 'Loss'],
                    datasets: [{
                        data: [80, 20],
                        backgroundColor: ['#4ECDC4', '#2C3E50'],
                        borderWidth: 0
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: { display: false }
                    }
                }
            });
        }

        // Power Consumption Chart
        const powerCanvas = document.getElementById('power-chart');
        if (powerCanvas) {
            this.charts.power = new Chart(powerCanvas, {
                type: 'bar',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Power (W)',
                        data: [],
                        backgroundColor: '#45B7D1',
                        borderColor: '#3A9BC1',
                        borderWidth: 1
                    }]
                },
                options: {
                    ...commonOptions,
                    scales: {
                        ...commonOptions.scales,
                        y: { 
                            ...commonOptions.scales.y,
                            min: 0,
                            title: { display: true, text: 'Watts', color: '#8B949E' }
                        }
                    }
                }
            });
        }
    }

    setupAutoRefresh() {
        setInterval(() => {
            this.fetchSystemData();
        }, this.updateInterval);
    }

    setupEventListeners() {
        // Dark mode toggle setup
        this.initializeDarkMode();
        
        // Refresh button
        const refreshBtn = document.getElementById('refresh-btn');
        if (refreshBtn) {
            refreshBtn.addEventListener('click', () => {
                this.refreshData();
                this.showToast('Data refreshed!', 'success');
            });
        }

        // Auto refresh toggle
        const autoRefreshToggle = document.getElementById('auto-refresh');
        if (autoRefreshToggle) {
            autoRefreshToggle.addEventListener('change', (e) => {
                this.isAutoRefresh = e.target.checked;
                if (this.isAutoRefresh) {
                    this.setupAutoRefresh();
                }
            });
        }

        // Control buttons
        this.setupControlButtons();
        
        // Touch event optimizations
        this.setupTouchOptimizations();
    }

    setupControlButtons() {
        const controlBtns = document.querySelectorAll('.control-btn');
        controlBtns.forEach(btn => {
            btn.addEventListener('click', (e) => {
                const command = e.target.dataset.command;
                if (command) {
                    this.sendCommand(command);
                }
            });
        });
    }

    // Professional data management methods
    async fetchSystemData() {
        try {
            const response = await fetch('/json');
            if (!response.ok) throw new Error('Network response was not ok');
            
            const data = await response.json();
            this.updateDashboard(data);
            this.updateConnectionStatus(true);
            
        } catch (error) {
            console.error('Data fetch error:', error);
            this.updateConnectionStatus(false);
        }
    }

    updateDashboard(data) {
        const timestamp = new Date().toLocaleTimeString();
        
        // Update metric cards with animation
        this.updateMetricCard('temp-value', data.sensors?.temperature || '--', '°C');
        this.updateMetricCard('humidity-value', data.sensors?.humidity || '--', '%');
        this.updateMetricCard('memory-value', data.system?.memory || '--', 'KB');
        this.updateMetricCard('uptime-value', data.system?.uptime || '--', 'h');
        this.updateMetricCard('wifi-strength', data.wifi?.signal || '--', 'dBm');
        
        // Update charts with new data
        this.updateCharts(data, timestamp);
        
        // Store data in history
        this.storeDataHistory(data, timestamp);
    }

    updateMetricCard(elementId, value, unit) {
        const element = document.getElementById(elementId);
        if (element) {
            element.textContent = value;
            // Professional animation
            element.style.transform = 'scale(1.05)';
            element.style.transition = 'all 0.2s ease';
            setTimeout(() => {
                element.style.transform = 'scale(1)';
            }, 200);
        }
    }

    updateCharts(data, timestamp) {
        // Update temperature chart with smooth animation
        if (this.charts.temperature && data.sensors?.temperature) {
            const chart = this.charts.temperature;
            chart.data.labels.push(timestamp);
            chart.data.datasets[0].data.push(data.sensors.temperature);
            
            // Keep only recent data points
            if (chart.data.labels.length > this.maxDataPoints) {
                chart.data.labels.shift();
                chart.data.datasets[0].data.shift();
            }
            chart.update('none');
        }

        // Update WiFi signal strength chart
        if (this.charts.wifi && data.wifi?.signal) {
            const signal = Math.abs(data.wifi.signal);
            const quality = signal > 70 ? 90 : signal > 50 ? 70 : 50;
            
            this.charts.wifi.data.datasets[0].data = [quality, 100 - quality];
            this.charts.wifi.update('none');
        }

        // Update power consumption chart
        if (this.charts.power && data.system?.power) {
            const chart = this.charts.power;
            chart.data.labels.push(timestamp);
            chart.data.datasets[0].data.push(data.system.power);
            
            if (chart.data.labels.length > 20) {
                chart.data.labels.shift();
                chart.data.datasets[0].data.shift();
            }
            chart.update('none');
        }
    }

    storeDataHistory(data, timestamp) {
        this.dataHistory.timestamps.push(timestamp);
        this.dataHistory.temperature.push(data.sensors?.temperature || null);
        this.dataHistory.humidity.push(data.sensors?.humidity || null);
        this.dataHistory.wifi.push(data.wifi?.signal || null);
        this.dataHistory.power.push(data.system?.power || null);
        
        // Maintain history limit
        Object.keys(this.dataHistory).forEach(key => {
            if (this.dataHistory[key].length > this.maxDataPoints) {
                this.dataHistory[key].shift();
            }
        });
    }

    updateConnectionStatus(connected) {
        this.isConnected = connected;
        const indicator = document.getElementById('connection-status');
        if (indicator) {
            indicator.className = connected ? 'status-online' : 'status-offline';
            indicator.textContent = connected ? 'Online' : 'Offline';
        }
        
        // Professional status animation
        if (indicator) {
            indicator.style.animation = 'none';
            void indicator.offsetHeight; // Trigger reflow
            indicator.style.animation = 'statusPulse 0.5s ease';
        }
    }

    startDataUpdates() {
        this.setupAutoRefresh();
        // Immediate first update
        this.fetchSystemData();
    }

    async refreshData() {
        try {
            // Update status indicator
            this.updateStatus('loading');
            
            // Fetch data from ESPEasy JSON API
            const response = await fetch('/json', {
                method: 'GET',
                headers: {
                    'Cache-Control': 'no-cache'
                }
            });
            
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            const data = await response.json();
            this.updateSensorData(data);
            this.updateSystemInfo(data);
            this.updateStatus('online');
            
        } catch (error) {
            console.error('Error fetching data:', error);
            this.updateStatus('offline');
            this.showToast('Failed to fetch data', 'error');
        }
    }

    updateSensorData(data) {
        if (!data.Sensors) return;

        data.Sensors.forEach((sensor, index) => {
            // Temperature
            if (sensor.Temperature !== undefined) {
                this.updateElement(`temp-${index}`, sensor.Temperature, '°C');
            }
            
            // Humidity
            if (sensor.Humidity !== undefined) {
                this.updateElement(`hum-${index}`, sensor.Humidity, '%');
            }
            
            // Pressure
            if (sensor.Pressure !== undefined) {
                this.updateElement(`pres-${index}`, sensor.Pressure, ' hPa');
            }
            
            // Generic TaskValues
            if (sensor.TaskValues) {
                sensor.TaskValues.forEach((value, valueIndex) => {
                    this.updateElement(`sensor-${index}-${valueIndex}`, value.Value, value.Unit);
                });
            }
        });
    }

    updateSystemInfo(data) {
        // System info updates
        if (data.System) {
            this.updateElement('cpu-load', data.System.Load, '%');
            this.updateElement('free-memory', data.System.RAM, ' bytes');
            this.updateElement('wifi-rssi', data.WiFi?.RSSI, ' dBm');
            this.updateElement('uptime', this.formatUptime(data.System?.Uptime), '');
        }
    }

    updateElement(elementId, value, unit = '') {
        const element = document.getElementById(elementId);
        if (element && value !== undefined) {
            element.textContent = `${value}${unit}`;
            // Add animation class
            element.classList.add('value-updated');
            setTimeout(() => {
                element.classList.remove('value-updated');
            }, 1000);
        }
    }

    updateStatus(status) {
        const indicator = document.getElementById('status-indicator');
        if (indicator) {
            indicator.className = `status-indicator status-${status}`;
        }

        const statusText = document.getElementById('status-text');
        if (statusText) {
            const statusMessages = {
                online: 'Connected',
                offline: 'Disconnected',
                loading: 'Loading...'
            };
            statusText.textContent = statusMessages[status] || status;
        }
    }

    async sendCommand(command) {
        try {
            const response = await fetch(`/control?cmd=${encodeURIComponent(command)}`, {
                method: 'GET'
            });
            
            if (response.ok) {
                this.showToast(`Command sent: ${command}`, 'success');
                // Refresh data after command
                setTimeout(() => this.refreshData(), 1000);
            } else {
                throw new Error('Command failed');
            }
        } catch (error) {
            console.error('Error sending command:', error);
            this.showToast('Command failed', 'error');
        }
    }

    showToast(message, type = 'info') {
        // Create toast notification
        const toast = document.createElement('div');
        toast.className = `toast toast-${type}`;
        toast.textContent = message;
        toast.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            padding: 12px 20px;
            border-radius: 8px;
            color: white;
            font-weight: bold;
            z-index: 1000;
            opacity: 0;
            transition: opacity 0.3s ease;
        `;
        
        // Set colors based on type
        const colors = {
            success: '#00ff88',
            error: '#ff4757',
            info: '#3742fa'
        };
        toast.style.background = colors[type] || colors.info;
        
        document.body.appendChild(toast);
        
        // Animate in
        setTimeout(() => toast.style.opacity = '1', 100);
        
        // Auto remove
        setTimeout(() => {
            toast.style.opacity = '0';
            setTimeout(() => document.body.removeChild(toast), 300);
        }, 3000);
    }

    formatUptime(seconds) {
        if (!seconds) return '--';
        
        const days = Math.floor(seconds / 86400);
        const hours = Math.floor((seconds % 86400) / 3600);
        const mins = Math.floor((seconds % 3600) / 60);
        
        if (days > 0) {
            return `${days}d ${hours}h ${mins}m`;
        } else if (hours > 0) {
            return `${hours}h ${mins}m`;
        } else {
            return `${mins}m`;
        }
    }
    
    // ========================================
    // DARK MODE & TOUCH INTERFACE METHODS
    // ========================================
    
    initializeDarkMode() {
        // Check for saved theme or default to auto
        const savedTheme = localStorage.getItem('enfi32-theme') || 'auto';
        this.setTheme(savedTheme);
        
        // Create theme toggle button
        this.createThemeToggle();
        
        // Listen for system theme changes
        if (window.matchMedia) {
            const mediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
            mediaQuery.addListener(() => {
                if (localStorage.getItem('enfi32-theme') === 'auto') {
                    this.updateThemeDisplay();
                }
            });
        }
    }
    
    createThemeToggle() {
        // Remove existing toggle if present
        const existing = document.getElementById('theme-toggle-wrapper');
        if (existing) existing.remove();
        
        // Create theme toggle wrapper
        const wrapper = document.createElement('div');
        wrapper.id = 'theme-toggle-wrapper';
        wrapper.className = 'theme-toggle-wrapper';
        
        // Create toggle button
        const button = document.createElement('button');
        button.className = 'theme-toggle touch-btn';
        button.id = 'theme-toggle';
        button.innerHTML = '🌙';
        button.title = 'Toggle Dark Mode';
        
        // Add click handler
        button.addEventListener('click', () => {
            this.toggleTheme();
        });
        
        // Add theme label
        const label = document.createElement('span');
        label.textContent = 'Theme';
        label.style.fontSize = '12px';
        label.style.color = 'var(--text-secondary)';
        
        wrapper.appendChild(button);
        wrapper.appendChild(label);
        document.body.appendChild(wrapper);
    }
    
    setTheme(theme) {
        const root = document.documentElement;
        const themeToggle = document.getElementById('theme-toggle');
        
        if (theme === 'dark') {
            root.setAttribute('data-theme', 'dark');
            if (themeToggle) themeToggle.innerHTML = '☀️';
        } else if (theme === 'light') {
            root.setAttribute('data-theme', 'light'); 
            if (themeToggle) themeToggle.innerHTML = '🌙';
        } else {
            // Auto mode - follow system preference
            root.removeAttribute('data-theme');
            if (themeToggle) themeToggle.innerHTML = '🔄';
        }
        
        localStorage.setItem('enfi32-theme', theme);
        this.updateThemeDisplay();
    }
    
    toggleTheme() {
        const current = localStorage.getItem('enfi32-theme') || 'auto';
        let next;
        
        switch (current) {
            case 'auto': next = 'light'; break;
            case 'light': next = 'dark'; break;
            case 'dark': next = 'auto'; break;
            default: next = 'auto';
        }
        
        this.setTheme(next);
        this.showToast(`Theme: ${next}`, 'info');
    }
    
    updateThemeDisplay() {
        // Update chart colors if charts exist
        if (this.charts && Object.keys(this.charts).length > 0) {
            this.updateChartsForTheme();
        }
    }
    
    updateChartsForTheme() {
        const isDark = this.isDarkMode();
        const textColor = isDark ? '#e2e8f0' : '#374151';
        const gridColor = isDark ? 'rgba(255,255,255,0.1)' : 'rgba(0,0,0,0.1)';
        
        Object.values(this.charts).forEach(chart => {
            if (chart && chart.options && chart.options.scales) {
                // Update scale colors
                if (chart.options.scales.x) {
                    chart.options.scales.x.grid.color = gridColor;
                    chart.options.scales.x.ticks.color = textColor;
                }
                if (chart.options.scales.y) {
                    chart.options.scales.y.grid.color = gridColor;
                    chart.options.scales.y.ticks.color = textColor;
                }
                // Update legend colors
                if (chart.options.plugins && chart.options.plugins.legend) {
                    chart.options.plugins.legend.labels.color = textColor;
                }
                chart.update('none');
            }
        });
    }
    
    isDarkMode() {
        const theme = localStorage.getItem('enfi32-theme');
        if (theme === 'dark') return true;
        if (theme === 'light') return false;
        // Auto mode - check system preference
        return window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
    }
    
    setupTouchOptimizations() {
        // Add touch-friendly classes to interactive elements
        document.querySelectorAll('button, .control-btn, .nav-link, .card').forEach(el => {
            el.classList.add('touch-btn');
        });
        
        // Optimize scroll behavior for touch devices
        if ('ontouchstart' in window) {
            document.body.style.webkitOverflowScrolling = 'touch';
            
            // Add touch feedback
            document.addEventListener('touchstart', (e) => {
                if (e.target.classList.contains('touch-btn')) {
                    e.target.style.transform = 'scale(0.95)';
                }
            }, { passive: true });
            
            document.addEventListener('touchend', (e) => {
                if (e.target.classList.contains('touch-btn')) {
                    setTimeout(() => {
                        e.target.style.transform = '';
                    }, 150);
                }
            }, { passive: true });
        }
    }
}

// Initialize dashboard when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    // Add professional CSS animations and styles
    const style = document.createElement('style');
    style.textContent = `
        .value-updated {
            animation: valueUpdate 1s ease;
        }
        
        @keyframes valueUpdate {
            0% { transform: scale(1); color: inherit; }
            50% { transform: scale(1.05); color: #00ff88; }
            100% { transform: scale(1); color: inherit; }
        }
        
        @keyframes statusPulse {
            0% { transform: scale(1); }
            50% { transform: scale(1.1); }
            100% { transform: scale(1); }
        }
        
        .metric-card {
            transition: all 0.3s ease;
        }
        
        .metric-card:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(0, 255, 136, 0.15);
        }
        
        .chart-container {
            position: relative;
            overflow: hidden;
        }
        
        .chart-container::before {
            content: '';
            position: absolute;
            top: -50%;
            left: -50%;
            width: 200%;
            height: 200%;
            background: linear-gradient(45deg, transparent, rgba(255, 255, 255, 0.05), transparent);
            transform: translateX(-100%);
            transition: transform 0.8s ease;
        }
        
        .chart-container:hover::before {
            transform: translateX(100%);
        }
        
        .device-item {
            transition: all 0.2s ease;
            border-left: 3px solid transparent;
        }
        
        .device-item:hover {
            border-left-color: #00ff88;
            background: rgba(0, 255, 136, 0.05);
        }
        
        .status-online {
            color: #00ff88;
            animation: statusPulse 2s infinite;
        }
        
        .status-offline {
            color: #ff4757;
            animation: none;
        }
        
        .loading-shimmer {
            background: linear-gradient(90deg, #f0f0f0 25%, #e0e0e0 50%, #f0f0f0 75%);
            background-size: 200% 100%;
            animation: loading-shimmer 1.5s infinite;
        }
        
        @keyframes loading-shimmer {
            0% { background-position: -200% 0; }
            100% { background-position: 200% 0; }
        }
        
        .professional-glow {
            box-shadow: 0 0 20px rgba(0, 255, 136, 0.3);
            transition: box-shadow 0.3s ease;
        }
        
        .fade-in {
            animation: fadeIn 0.5s ease-in-out;
        }
        
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }
    `;
    document.head.appendChild(style);
    
    // Add professional loading effects
    const addLoadingShimmer = () => {
        document.querySelectorAll('.metric-value').forEach(el => {
            el.classList.add('loading-shimmer');
        });
    };
    
    const removeLoadingShimmer = () => {
        document.querySelectorAll('.metric-value').forEach(el => {
            el.classList.remove('loading-shimmer');
        });
    };
    
    // Initialize dashboard with professional features
    window.enfi32Dashboard = new ENFi32Dashboard();
    
    // Add fade-in effect to all cards
    setTimeout(() => {
        document.querySelectorAll('.metric-card, .chart-container').forEach(el => {
            el.classList.add('fade-in');
        });
    }, 100);
    
    console.log('🚀 ENFi32 Professional Dashboard initialized with advanced features');
});