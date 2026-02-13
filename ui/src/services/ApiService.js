/**
 * API Service - ESP32 Communication
 * Professional data fetching and state management
 */

class ApiServiceClass {
    constructor() {
        this.baseUrl = window.location.origin;
        this.timeout = 5000;
        this.retryAttempts = 3;
        this.cache = new Map();
        this.eventListeners = new Map();
    }

    // ================================
    // Core HTTP Methods
    // ================================

    async request(endpoint, options = {}) {
        const {
            method = 'GET',
            body,
            headers = {},
            cache = false,
            retry = true
        } = options;

        const url = `${this.baseUrl}${endpoint}`;
        const cacheKey = `${method}:${url}:${JSON.stringify(body)}`;

        // Check cache first
        if (cache && this.cache.has(cacheKey)) {
            const cached = this.cache.get(cacheKey);
            if (Date.now() - cached.timestamp < 30000) { // 30 second cache
                return cached.data;
            }
        }

        let lastError;
        const maxAttempts = retry ? this.retryAttempts : 1;

        for (let attempt = 1; attempt <= maxAttempts; attempt++) {
            try {
                const controller = new AbortController();
                const timeoutId = setTimeout(() => controller.abort(), this.timeout);

                const response = await fetch(url, {
                    method,
                    headers: {
                        'Content-Type': 'application/json',
                        'Cache-Control': 'no-cache',
                        ...headers
                    },
                    body: body ? JSON.stringify(body) : undefined,
                    signal: controller.signal
                });

                clearTimeout(timeoutId);

                if (!response.ok) {
                    throw new Error(`HTTP ${response.status}: ${response.statusText}`);
                }

                const data = await response.json();

                // Cache successful responses
                if (cache) {
                    this.cache.set(cacheKey, {
                        data,
                        timestamp: Date.now()
                    });
                }

                return data;

            } catch (error) {
                lastError = error;
                
                if (attempt < maxAttempts) {
                    // Exponential backoff
                    const delay = Math.min(1000 * Math.pow(2, attempt - 1), 5000);
                    await new Promise(resolve => setTimeout(resolve, delay));
                    console.warn(`API request failed (attempt ${attempt}/${maxAttempts}), retrying...`, error);
                }
            }
        }

        throw lastError;
    }

    // ================================
    // ESP32 API Endpoints
    // ================================

    async checkConnection() {
        try {
            await this.request('/json', { cache: true });
            this.emit('connection', { connected: true });
            return true;
        } catch (error) {
            console.warn('ESP32 connection check failed:', error);
            this.emit('connection', { connected: false });
            return false;
        }
    }

    async getSystemInfo() {
        try {
            const data = await this.request('/json', { cache: true });
            return this.parseSystemInfo(data);
        } catch (error) {
            console.error('Failed to get system info:', error);
            return null;
        }
    }

    async getSensorData() {\n        return this.request('/json');\n    }\n\n    async getDevices() {\n        return this.request('/devices');\n    }\n\n    async getControllers() {\n        return this.request('/controllers');\n    }\n\n    async getConfig() {\n        return this.request('/config');\n    }\n\n    async sendCommand(command) {\n        return this.request('/control', {\n            method: 'POST',\n            body: { cmd: command }\n        });\n    }\n\n    async uploadFile(file, endpoint = '/upload') {\n        const formData = new FormData();\n        formData.append('file', file);\n        \n        const response = await fetch(`${this.baseUrl}${endpoint}`, {\n            method: 'POST',\n            body: formData\n        });\n        \n        if (!response.ok) {\n            throw new Error(`Upload failed: ${response.statusText}`);\n        }\n        \n        return response.json();\n    }\n\n    // ================================\n    // Real-time Data Streaming\n    // ================================\n\n    startRealTimeData(interval = 5000) {\n        if (this.realTimeInterval) {\n            clearInterval(this.realTimeInterval);\n        }\n\n        this.realTimeInterval = setInterval(async () => {\n            try {\n                const data = await this.getSensorData();\n                this.emit('sensorData', data);\n            } catch (error) {\n                console.warn('Real-time data fetch failed:', error);\n                this.emit('error', error);\n            }\n        }, interval);\n    }\n\n    stopRealTimeData() {\n        if (this.realTimeInterval) {\n            clearInterval(this.realTimeInterval);\n            this.realTimeInterval = null;\n        }\n    }\n\n    // ================================\n    // Data Processing\n    // ================================\n\n    parseSystemInfo(rawData) {\n        return {\n            name: rawData.System?.Name || 'ENFi32',\n            uptime: rawData.System?.Uptime || 0,\n            memory: rawData.System?.RAM || 0,\n            load: rawData.System?.Load || 0,\n            version: rawData.System?.Version || 'Unknown',\n            wifi: {\n                ssid: rawData.WiFi?.SSID || '',\n                rssi: rawData.WiFi?.RSSI || 0,\n                ip: rawData.WiFi?.IP || ''\n            },\n            sensors: rawData.Sensors || [],\n            timestamp: Date.now()\n        };\n    }\n\n    // ================================\n    // Event System\n    // ================================\n\n    on(event, callback) {\n        if (!this.eventListeners.has(event)) {\n            this.eventListeners.set(event, []);\n        }\n        this.eventListeners.get(event).push(callback);\n        \n        // Return unsubscribe function\n        return () => this.off(event, callback);\n    }\n\n    off(event, callback) {\n        const listeners = this.eventListeners.get(event);\n        if (listeners) {\n            const index = listeners.indexOf(callback);\n            if (index > -1) {\n                listeners.splice(index, 1);\n            }\n        }\n    }\n\n    emit(event, data) {\n        const listeners = this.eventListeners.get(event);\n        if (listeners) {\n            listeners.forEach(callback => {\n                try {\n                    callback(data);\n                } catch (error) {\n                    console.error('Event listener error:', error);\n                }\n            });\n        }\n    }\n\n    // ================================\n    // Cache Management\n    // ================================\n\n    clearCache() {\n        this.cache.clear();\n    }\n\n    getCacheSize() {\n        return this.cache.size;\n    }\n\n    // ================================\n    // Utility Methods\n    // ================================\n\n    isConnected() {\n        return this.checkConnection();\n    }\n\n    getLastError() {\n        return this.lastError;\n    }\n\n    destroy() {\n        this.stopRealTimeData();\n        this.clearCache();\n        this.eventListeners.clear();\n    }\n}\n\n// Export singleton instance\nexport const ApiService = new ApiServiceClass();\n\n// Export class for testing\nexport { ApiServiceClass };