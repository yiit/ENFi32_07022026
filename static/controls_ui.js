/**
 * ENFi32 Controls UI - Sistem Kontrolü
 */

class ControlsUI {
    constructor() {
        this.gpios = [];
        this.defaultGPIOs = [2, 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27];
        this.init();
    }

    init() {
        this.setupEventListeners();
        this.generateGPIOControls();
        this.loadGPIOStates();
    }

    setupEventListeners() {
        // Komut butonları
        document.querySelectorAll('[data-cmd]').forEach(btn => {
            btn.addEventListener('click', async (e) => {
                const cmd = e.target.dataset.cmd;
                const btnElement = e.target;
                
                // Butonu devre dışı bırak
                btnElement.disabled = true;
                btnElement.textContent = btnElement.textContent.replace(btnElement.textContent.split(' ')[0], '⏳');
                
                await this.sendCommand(cmd);
                
                // Butonu tekrar aktif yap
                setTimeout(() => {
                    btnElement.disabled = false;
                    location.reload(); // Sayfa yenile  
                }, 2000);
            });
        });
    }

    generateGPIOControls() {
        const container = document.getElementById('gpio-controls');
        if (!container) return;

        container.innerHTML = '';

        this.defaultGPIOs.forEach(gpio => {
            const gpioControl = this.createGPIOControl(gpio);
            container.appendChild(gpioControl);
        });
    }

    createGPIOControl(gpioNumber) {
        const control = document.createElement('div');
        control.className = 'gpio-control';
        control.innerHTML = `
            <div class="gpio-info">
                <span class="gpio-number">GPIO ${gpioNumber}</span>
                <span class="gpio-status" id="gpio-status-${gpioNumber}">Bilinmiyor</span>
            </div>
            <div class="gpio-buttons">
                <button class="btn btn-sm btn-success" onclick="controlsUI.setGPIO(${gpioNumber}, 1)">
                    ⬆️ HIGH
                </button>
                <button class="btn btn-sm btn-secondary" onclick="controlsUI.setGPIO(${gpioNumber}, 0)">
                    ⬇️ LOW
                </button>
                <button class="btn btn-sm btn-info" onclick="controlsUI.toggleGPIO(${gpioNumber})">
                    🔄 Toggle
                </button>
            </div>
        `;
        return control;
    }

    async loadGPIOStates() {
        // ESPEasy GPIO durumlarını JSON'dan oku
        try {
            const response = await fetch('/json');
            const data = await response.json();
            
            // GPIO durumlarını güncelle (eğer varsa)
            this.defaultGPIOs.forEach(gpio => {
                const statusEl = document.getElementById(`gpio-status-${gpio}`);
                if (statusEl) {
                    // Varsayılan durum
                    statusEl.textContent = 'Giriş';
                    statusEl.className = 'gpio-status input';
                }
            });
        } catch (error) {
            console.error('GPIO durumları yüklenemedi:', error);
        }
    }

    async setGPIO(gpio, state) {
        const cmd = `gpio,${gpio},${state}`;
        await this.sendCommand(cmd);
        
        // Durum güncelleme
        const statusEl = document.getElementById(`gpio-status-${gpio}`);
        if (statusEl) {
            statusEl.textContent = state ? 'HIGH (1)' : 'LOW (0)';
            statusEl.className = `gpio-status ${state ? 'high' : 'low'}`;
        }
        
        this.showToast(`GPIO ${gpio} → ${state ? 'HIGH' : 'LOW'}`, 'success');
    }

    async toggleGPIO(gpio) {
        const cmd = `gpiotoggle,${gpio}`;
        await this.sendCommand(cmd);
        
        this.showToast(`GPIO ${gpio} toggle edildi`, 'info');
        
        // Durum bilgisini güncelle
        setTimeout(() => this.loadGPIOStates(), 500);
    }

    async sendCommand(cmd) {
        try {
            const response = await fetch(`/control?cmd=${encodeURIComponent(cmd)}`);
            
            if (response.ok) {
                console.log(`Komut başarılı: ${cmd}`);
                return true;
            } else {
                throw new Error(`HTTP ${response.status}`);
            }
        } catch (error) {
            console.error('Komut hatası:', error);
            this.showToast(`Komut hatası: ${cmd}`, 'error');
            return false;
        }
    }

    showToast(message, type = 'info') {
        // Basit toast notification
        const toast = document.createElement('div');
        toast.className = `toast toast-${type}`;
        toast.textContent = message;
        
        document.body.appendChild(toast);
        
        setTimeout(() => toast.classList.add('show'), 10);
        
        setTimeout(() => {
            toast.classList.remove('show');
            setTimeout(() => document.body.removeChild(toast), 300);
        }, 3000);
    }

    // Özel komutlar
    async rebootSystem() {
        if (confirm('Sistemi yeniden başlatmak istediğinizden emin misiniz?')) {
            await this.sendCommand('reboot');
            this.showToast('Sistem yeniden başlatılıyor...', 'warning');
        }
    }

    async resetSettings() {
        if (confirm('TÜM ayarları sıfırlamak istediğinizden emin misiniz? Bu işlem geri alınamaz!')) {
            await this.sendCommand('reset');
            this.showToast('Ayarlar sıfırlanıyor...', 'warning');
        }
    }
}

// Sayfa yüklendiğinde başlat
document.addEventListener('DOMContentLoaded', () => {
    window.controlsUI = new ControlsUI();
    
    // Kontrol sayfası için CSS eklentileri
    if (!document.getElementById('controls-styles')) {
        const style = document.createElement('style');
        style.id = 'controls-styles';
        style.textContent = `
            .controls-grid {
                display: grid;
                grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
                gap: 20px;
                padding: 20px;
            }
            
            .gpio-controls {
                display: grid;
                grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
                gap: 15px;
            }
            
            .gpio-control {
                background: #f8f9fa;
                border: 1px solid #dee2e6;
                border-radius: 8px;
                padding: 12px;
            }
            
            .gpio-info {
                display: flex;
                justify-content: space-between;
                align-items: center;
                margin-bottom: 10px;
            }
            
            .gpio-number {
                font-weight: 600;
                color: #495057;
            }
            
            .gpio-status {
                font-size: 12px;
                padding: 2px 6px;
                border-radius: 4px;
                font-weight: 500;
            }
            
            .gpio-status.input {
                background: #e7f1ff;
                color: #004085;
            }
            
            .gpio-status.high {
                background: #d4edda;
                color: #155724;
            }
            
            .gpio-status.low {
                background: #f8d7da;
                color: #721c24;
            }
            
            .gpio-buttons {
                display: flex;
                gap: 6px;
                flex-wrap: wrap;
            }
            
            .btn-sm {
                padding: 4px 8px;
                font-size: 12px;
                border: none;
                border-radius: 4px;
                cursor: pointer;
                flex: 1;
                min-width: 60px;
            }
            
            .btn-large {
                width: 100%;
                padding: 12px;
                margin: 8px 0;
                font-size: 14px;
                border: none;
                border-radius: 8px;
                cursor: pointer;
                font-weight: 600;
            }
            
            .control-buttons {
                display: flex;
                flex-direction: column;
                gap: 8px;
            }
            
            /* Toast styles */
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
            .toast.toast-warning { background: #ffa502; }
        `;
        document.head.appendChild(style);
    }
});