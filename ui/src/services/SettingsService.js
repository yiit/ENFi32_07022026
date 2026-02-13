/**
 * Settings Service - Configuration Management
 * Professional local settings and preferences
 */

class SettingsServiceClass {
    constructor() {
        this.settings = new Map();
        this.defaults = new Map();
        this.listeners = new Map();
        this.storageKey = 'enfi32-settings';
        this.loaded = false;
    }

    // ================================
    // Core Settings Management
    // ================================

    async loadSettings() {
        try {
            // Load from localStorage
            const stored = localStorage.getItem(this.storageKey);
            if (stored) {
                const parsed = JSON.parse(stored);
                Object.entries(parsed).forEach(([key, value]) => {
                    this.settings.set(key, value);
                });
            }

            // Set defaults if not present
            this.setDefaults();
            this.loaded = true;
            
            console.log('✅ Settings loaded:', this.getAllSettings());
            return true;
        } catch (error) {
            console.error('❌ Failed to load settings:', error);
            this.setDefaults();
            this.loaded = true;
            return false;
        }
    }

    setDefaults() {
        const defaults = {
            // Theme settings
            'ui.theme': 'auto',
            'ui.animations': true,
            'ui.compactMode': false,
            
            // Dashboard settings
            'dashboard.autoRefresh': true,
            'dashboard.refreshInterval': 5000,
            'dashboard.showAdvanced': false,
            'dashboard.defaultView': 'overview',
            
            // Chart settings
            'charts.enabled': true,
            'charts.maxDataPoints': 50,
            'charts.animationDuration': 750,
            'charts.autoScale': true,
            
            // Device settings
            'devices.autoDiscover': true,
            'devices.showOffline': true,
            'devices.groupByType': false,
            
            // Notification settings
            'notifications.enabled': true,
            'notifications.position': 'top-right',
            'notifications.duration': 5000,
            'notifications.sound': false,
            
            // Advanced settings
            'advanced.debugMode': false,
            'advanced.apiTimeout': 5000,
            'advanced.maxRetries': 3,
            'advanced.cacheEnabled': true,
            
            // Touch/Mobile settings
            'mobile.hapticFeedback': true,
            'mobile.gestureNavigation': true,
            'mobile.fullscreen': false
        };

        defaults.forEach((value, key) => {
            this.defaults.set(key, value);
            if (!this.settings.has(key)) {
                this.settings.set(key, value);
            }
        });
    }

    saveSettings() {
        try {
            const settingsObj = {};
            this.settings.forEach((value, key) => {
                settingsObj[key] = value;
            });
            
            localStorage.setItem(this.storageKey, JSON.stringify(settingsObj));
            console.log('💾 Settings saved');
            return true;
        } catch (error) {
            console.error('❌ Failed to save settings:', error);
            return false;
        }
    }

    // ================================
    // Get/Set Methods
    // ================================

    get(key, defaultValue = null) {
        if (!this.loaded) {
            console.warn('Settings not loaded yet, using default for:', key);
            return this.defaults.get(key) || defaultValue;
        }
        
        return this.settings.get(key) ?? this.defaults.get(key) ?? defaultValue;
    }

    set(key, value) {
        const oldValue = this.settings.get(key);
        this.settings.set(key, value);
        
        // Auto-save
        this.saveSettings();
        
        // Emit change event
        this.emit(key, { oldValue, newValue: value });
        this.emit('*', { key, oldValue, newValue: value });
        
        return true;
    }

    // ================================
    // Bulk Operations
    // ================================

    getAll() {
        const result = {};
        this.settings.forEach((value, key) => {
            result[key] = value;
        });
        return result;
    }

    setMultiple(settings) {
        let changed = false;
        
        Object.entries(settings).forEach(([key, value]) => {
            if (this.settings.get(key) !== value) {
                this.settings.set(key, value);
                changed = true;
                this.emit(key, { oldValue: this.settings.get(key), newValue: value });
            }
        });
        
        if (changed) {\n            this.saveSettings();\n            this.emit('*', { type: 'bulk', settings });\n        }\n        \n        return changed;\n    }\n\n    reset(key = null) {\n        if (key) {\n            // Reset single setting\n            const defaultValue = this.defaults.get(key);\n            if (defaultValue !== undefined) {\n                this.set(key, defaultValue);\n                return true;\n            }\n            return false;\n        } else {\n            // Reset all settings\n            this.settings.clear();\n            this.setDefaults();\n            this.saveSettings();\n            this.emit('*', { type: 'reset' });\n            return true;\n        }\n    }\n\n    // ================================\n    // Category Helpers\n    // ================================\n\n    getCategory(category) {\n        const result = {};\n        const prefix = `${category}.`;\n        \n        this.settings.forEach((value, key) => {\n            if (key.startsWith(prefix)) {\n                const shortKey = key.substring(prefix.length);\n                result[shortKey] = value;\n            }\n        });\n        \n        return result;\n    }\n\n    setCategory(category, settings) {\n        const updates = {};\n        Object.entries(settings).forEach(([key, value]) => {\n            updates[`${category}.${key}`] = value;\n        });\n        \n        return this.setMultiple(updates);\n    }\n\n    // ================================\n    // UI Helper Methods\n    // ================================\n\n    getUI() {\n        return this.getCategory('ui');\n    }\n\n    getDashboard() {\n        return this.getCategory('dashboard');\n    }\n\n    getCharts() {\n        return this.getCategory('charts');\n    }\n\n    getNotifications() {\n        return this.getCategory('notifications');\n    }\n\n    // ================================\n    // Event System\n    // ================================\n\n    on(key, callback) {\n        if (!this.listeners.has(key)) {\n            this.listeners.set(key, []);\n        }\n        this.listeners.get(key).push(callback);\n        \n        // Return unsubscribe function\n        return () => this.off(key, callback);\n    }\n\n    off(key, callback) {\n        const listeners = this.listeners.get(key);\n        if (listeners) {\n            const index = listeners.indexOf(callback);\n            if (index > -1) {\n                listeners.splice(index, 1);\n            }\n        }\n    }\n\n    emit(key, data) {\n        const listeners = this.listeners.get(key);\n        if (listeners) {\n            listeners.forEach(callback => {\n                try {\n                    callback(data);\n                } catch (error) {\n                    console.error('Settings listener error:', error);\n                }\n            });\n        }\n    }\n\n    // ================================\n    // Import/Export\n    // ================================\n\n    export() {\n        return {\n            version: '2.0.0',\n            timestamp: Date.now(),\n            settings: this.getAll()\n        };\n    }\n\n    import(data) {\n        try {\n            if (data.settings) {\n                this.setMultiple(data.settings);\n                console.log('✅ Settings imported successfully');\n                return true;\n            }\n            return false;\n        } catch (error) {\n            console.error('❌ Failed to import settings:', error);\n            return false;\n        }\n    }\n\n    // ================================\n    // Utility Methods\n    // ================================\n\n    getAllSettings() {\n        return this.getAll();\n    }\n\n    getSize() {\n        return this.settings.size;\n    }\n\n    clear() {\n        this.settings.clear();\n        this.saveSettings();\n        this.emit('*', { type: 'clear' });\n    }\n\n    destroy() {\n        this.clear();\n        this.listeners.clear();\n    }\n}\n\n// Export singleton instance\nexport const SettingsService = new SettingsServiceClass();\n\n// Export class for testing\nexport { SettingsServiceClass };