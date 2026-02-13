/**
 * ENFi32 Simple UI - Ana Dashboard
 * Proje yapısını bozmadan sade UI çözümü
 */

class SimpleUI {
    constructor() {
        this.updateInterval = 5000; // 5 saniye
        this.isAutoRefresh = true;
        this.init();
    }

    init() {
        this.setupEventListeners();
        this.startAutoRefresh();
        this.loadData();
        this.showToast('Dashboard yüklendi', 'success');
    }

    setupEventListeners() {
        // Yenile butonu
        const refreshBtn = document.getElementById('refresh-btn');
        if (refreshBtn) {
            refreshBtn.addEventListener('click', () => {
                this.loadData();
                this.showToast('Veriler yenilendi', 'success');
            });
        }

        // Otomatik yenileme
        const autoRefresh = document.getElementById('auto-refresh');
        if (autoRefresh) {
            autoRefresh.addEventListener('change', (e) => {
                this.isAutoRefresh = e.target.checked;
                if (this.isAutoRefresh) {
                    this.startAutoRefresh();
                }
            });
        }

        // Kontrol butonları
        document.querySelectorAll('[data-cmd]').forEach(btn => {
            btn.addEventListener('click', (e) => {
                const cmd = e.target.dataset.cmd;
                this.sendCommand(cmd);
            });
        });
    }

    startAutoRefresh() {
        if (this.refreshTimer) {
            clearInterval(this.refreshTimer);
        }

        if (this.isAutoRefresh) {
            this.refreshTimer = setInterval(() => {
                this.loadData();
            }, this.updateInterval);
        }
    }

    async loadData() {
        try {
            this.updateStatus('loading');
            
            // ESPEasy JSON API'sini kullan
            const response = await fetch('/json');
            const data = await response.json();
            
            this.updateSystemInfo(data);
            this.updateSensors(data);
            this.updateStatus('online');
            this.updateTimestamp();
            
        } catch (error) {
            console.error('Veri yükleme hatası:', error);
            this.updateStatus('offline');
            this.showToast('Veri yüklenemedi', 'error');
        }
    }

    updateSystemInfo(data) {
        // Sistem bilgileri güncelle
        const updateElement = (id, value, suffix = '') => {
            const el = document.getElementById(id);
            if (el && value !== undefined) {
                el.textContent = value + suffix;
                this.animateUpdate(el);
            }
        };

        if (data.System) {
            updateElement('uptime', this.formatUptime(data.System.Uptime));
            updateElement('free-ram', this.formatBytes(data.System.RAM));
            updateElement('cpu-load', data.System.Load, '%');
        }

        if (data.WiFi) {
            updateElement('wifi-ssid', data.WiFi.SSID || 'Bağlı değil');
            updateElement('wifi-rssi', data.WiFi.RSSI, ' dBm');
            updateElement('ip-address', data.WiFi.IP || '--');
        }
    }

    updateSensors(data) {
        const sensorGrid = document.getElementById('sensor-grid');
        if (!sensorGrid || !data.Sensors) return;

        sensorGrid.innerHTML = '';

        data.Sensors.forEach((sensor, index) => {
            if (!sensor.TaskValues) return;

            sensor.TaskValues.forEach((value, valueIndex) => {
                const sensorCard = document.createElement('div');
                sensorCard.className = 'sensor-item';
                sensorCard.innerHTML = `
                    <div class="sensor-icon">🌡️</div>
                    <div class="sensor-info">
                        <div class="sensor-name">${value.Name || 'Sensör ' + (index + 1)}</div>
                        <div class="sensor-value">${value.Value} ${value.Unit || ''}</div>
                    </div>
                `;
                sensorGrid.appendChild(sensorCard);
            });
        });

        if (data.Sensors.length === 0) {
            sensorGrid.innerHTML = '<div class="no-data">Sensör verisi bulunamadı</div>';
        }
    }

    updateStatus(status) {
        const statusBadge = document.getElementById('status-badge');
        if (statusBadge) {
            statusBadge.className = `status-badge status-${status}`;
            
            const statusText = {
                online: 'Çevrimiçi',
                offline: 'Çevrimdışı',
                loading: 'Yükleniyor...'
            };
            statusBadge.textContent = statusText[status] || status;
        }
    }

    updateTimestamp() {
        const timestampEl = document.getElementById('last-update');
        if (timestampEl) {
            timestampEl.textContent = new Date().toLocaleTimeString('tr-TR');
        }
    }

    async sendCommand(cmd) {
        try {
            const response = await fetch(`/control?cmd=${encodeURIComponent(cmd)}`);
            
            if (response.ok) {
                this.showToast(`Komut gönderildi: ${cmd}`, 'success');
                // Kısa bir süre sonra veriyi yenile
                setTimeout(() => this.loadData(), 2000);
            } else {
                throw new Error('Komut gönderilemedi');
            }
        } catch (error) {
            console.error('Komut hatası:', error);
            this.showToast('Komut gönderilemedi', 'error');
        }
    }

    animateUpdate(element) {
        element.style.transform = 'scale(1.05)';
        element.style.transition = 'transform 0.2s ease';
        
        setTimeout(() => {
            element.style.transform = 'scale(1)';
        }, 200);
    }

    showToast(message, type = 'info') {
        const toast = document.createElement('div');
        toast.className = `toast toast-${type}`;
        toast.textContent = message;
        
        document.body.appendChild(toast);
        
        // Animasyon
        setTimeout(() => toast.classList.add('show'), 10);
        
        // Otomatik kaldır
        setTimeout(() => {
            toast.classList.remove('show');
            setTimeout(() => document.body.removeChild(toast), 300);
        }, 3000);
    }

    formatUptime(seconds) {
        if (!seconds) return '--';
        
        const days = Math.floor(seconds / 86400);
        const hours = Math.floor((seconds % 86400) / 3600);
        const mins = Math.floor((seconds % 3600) / 60);
        
        if (days > 0) {
            return `${days}g ${hours}s ${mins}d`;
        } else if (hours > 0) {
            return `${hours}s ${mins}d`;
        } else {
            return `${mins} dakika`;
        }
    }

    formatBytes(bytes) {
        if (!bytes) return '--';
        
        if (bytes < 1024) return bytes + ' B';
        if (bytes < 1048576) return Math.round(bytes / 1024) + ' KB';
        return Math.round(bytes / 1048576) + ' MB';
    }
}

// Sayfa yüklendiğinde başlat
document.addEventListener('DOMContentLoaded', () => {
    window.simpleUI = new SimpleUI();
    
    // Toast CSS'i ekle
    if (!document.getElementById('toast-styles')) {
        const style = document.createElement('style');
        style.id = 'toast-styles';
        style.textContent = `
            .toast {
                position: fixed;
                top: 20px;
                right: 20px;
                padding: 12px 20px;
                border-radius: 8px;
                color: white;
                font-weight: 500;
                z-index: 1000;
                opacity: 0;
                transform: translateX(100%);
                transition: all 0.3s ease;
            }
            .toast.show {
                opacity: 1;
                transform: translateX(0);
            }
            .toast.toast-success { background: #00d4aa; }
            .toast.toast-error { background: #ff6b6b; }
            .toast.toast-info { background: #4ecdc4; }
        `;
        document.head.appendChild(style);
    }
});