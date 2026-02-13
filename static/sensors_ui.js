/**
 * ENFi32 Sensors UI - Sensör Verilerini Göster
 */

class SensorsUI {
    constructor() {
        this.updateInterval = 3000; // 3 saniye
        this.sensors = [];
        this.init();
    }

    init() {
        this.loadSensorData();
        this.startAutoUpdate();
    }

    async loadSensorData() {
        try {
            const response = await fetch('/json');
            const data = await response.json();
            
            this.sensors = data.Sensors || [];
            this.renderSensors();
            
        } catch (error) {
            console.error('Sensör verisi yüklenemedi:', error);
            this.showError();
        }
    }

    renderSensors() {
        const container = document.getElementById('sensors-container');
        if (!container) return;

        if (this.sensors.length === 0) {
            container.innerHTML = `
                <div class="no-sensors">
                    <div class="no-sensors-icon">📊</div>
                    <h3>Henüz sensör bulunamadı</h3>
                    <p>Sensör eklemek için ESPEasy ana panelini kullanın</p>
                </div>
            `;
            return;
        }

        container.innerHTML = '';

        this.sensors.forEach((sensor, index) => {
            if (!sensor.TaskValues) return;

            const sensorCard = this.createSensorCard(sensor, index);
            container.appendChild(sensorCard);
        });
    }

    createSensorCard(sensor, index) {
        const card = document.createElement('div');
        card.className = 'sensor-card';
        
        let sensorIcon = this.getSensorIcon(sensor);
        let taskName = sensor.TaskName || `Görev ${index + 1}`;
        
        let valuesHTML = '';
        sensor.TaskValues.forEach((value, valueIndex) => {
            const quality = this.getValueQuality(value.Value, value.Unit);
            valuesHTML += `
                <div class="sensor-value-row">
                    <span class="value-name">${value.Name}</span>
                    <span class="value-display ${quality}">
                        ${value.Value}${value.Unit ? ' ' + value.Unit : ''}
                    </span>
                </div>
            `;
        });

        card.innerHTML = `
            <div class="sensor-card-header">
                <div class="sensor-icon">${sensorIcon}</div>
                <div class="sensor-title">
                    <h3>${taskName}</h3>
                    <span class="sensor-status">Aktif</span>
                </div>
            </div>
            <div class="sensor-card-body">
                ${valuesHTML}
            </div>
            <div class="sensor-card-footer">
                <span class="last-update">Son güncelleme: ${new Date().toLocaleTimeString('tr-TR')}</span>
            </div>
        `;

        return card;
    }

    getSensorIcon(sensor) {
        const taskName = (sensor.TaskName || '').toLowerCase();
        
        if (taskName.includes('temp') || taskName.includes('sıcaklık')) return '🌡️';
        if (taskName.includes('hum') || taskName.includes('nem')) return '💧';
        if (taskName.includes('press') || taskName.includes('basınç')) return '🌊';
        if (taskName.includes('light') || taskName.includes('ışık')) return '💡';
        if (taskName.includes('motion') || taskName.includes('hareket')) return '🚶';
        if (taskName.includes('gas') || taskName.includes('gaz')) return '💨';
        if (taskName.includes('sound') || taskName.includes('ses')) return '🔊';
        if (taskName.includes('co2')) return '🫧';
        if (taskName.includes('dust') || taskName.includes('toz')) return '☁️';
        if (taskName.includes('volt') || taskName.includes('gerilim')) return '⚡';
        
        return '📊'; // Varsayılan
    }

    getValueQuality(value, unit) {
        // Basit kalite kontrolü
        if (unit === '°C') {
            if (value < 0 || value > 50) return 'warning';
            if (value >= 18 && value <= 25) return 'good';
            return 'normal';
        }
        
        if (unit === '%' && unit.includes('Hum')) {
            if (value < 30 || value > 70) return 'warning';
            if (value >= 40 && value <= 60) return 'good';
            return 'normal';
        }
        
        return 'normal';
    }

    showError() {
        const container = document.getElementById('sensors-container');
        if (container) {
            container.innerHTML = `
                <div class="error-message">
                    <div class="error-icon">❌</div>
                    <h3>Bağlantı Hatası</h3>
                    <p>Sensör verileri yüklenemiyor. Lütfen bağlantınızı kontrol edin.</p>
                    <button onclick="location.reload()" class="btn btn-primary">Yeniden Dene</button>
                </div>
            `;
        }
    }

    startAutoUpdate() {
        setInterval(() => {
            this.loadSensorData();
        }, this.updateInterval);
    }
}

// Sayfa yüklendiğinde başlat
document.addEventListener('DOMContentLoaded', () => {
    window.sensorsUI = new SensorsUI();
    
    // Sensör sayfası için CSS eklentileri
    if (!document.getElementById('sensors-styles')) {
        const style = document.createElement('style');
        style.id = 'sensors-styles';
        style.textContent = `
            .sensors-grid {
                display: grid;
                grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
                gap: 20px;
                padding: 20px;
            }
            
            .sensor-card {
                background: white;
                border-radius: 12px;
                box-shadow: 0 4px 6px rgba(0,0,0,0.1);
                overflow: hidden;
                transition: transform 0.3s ease;
            }
            
            .sensor-card:hover {
                transform: translateY(-4px);
                box-shadow: 0 8px 15px rgba(0,0,0,0.15);
            }
            
            .sensor-card-header {
                background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
                color: white;
                padding: 15px;
                display: flex;
                align-items: center;
                gap: 12px;
            }
            
            .sensor-icon {
                font-size: 24px;
                width: 40px;
                height: 40px;
                background: rgba(255,255,255,0.2);
                border-radius: 8px;
                display: flex;
                align-items: center;
                justify-content: center;
            }
            
            .sensor-title h3 {
                margin: 0;
                font-size: 16px;
                font-weight: 600;
            }
            
            .sensor-status {
                font-size: 12px;
                opacity: 0.8;
            }
            
            .sensor-card-body {
                padding: 15px;
            }
            
            .sensor-value-row {
                display: flex;
                justify-content: space-between;
                align-items: center;
                padding: 8px 0;
                border-bottom: 1px solid #f0f0f0;
            }
            
            .sensor-value-row:last-child {
                border-bottom: none;
            }
            
            .value-name {
                color: #666;
                font-size: 14px;
            }
            
            .value-display {
                font-weight: 600;
                font-size: 16px;
                padding: 4px 8px;
                border-radius: 6px;
            }
            
            .value-display.good { 
                background: #d4edda; 
                color: #155724; 
            }
            .value-display.warning { 
                background: #fff3cd; 
                color: #856404; 
            }
            .value-display.normal { 
                background: #e7f1ff; 
                color: #004085; 
            }
            
            .sensor-card-footer {
                background: #f8f9fa;
                padding: 10px 15px;
                font-size: 12px;
                color: #666;
                text-align: center;
            }
            
            .no-sensors, .error-message {
                text-align: center;
                padding: 60px 20px;
                color: #666;
            }
            
            .no-sensors-icon, .error-icon {
                font-size: 64px;
                margin-bottom: 20px;
            }
        `;
        document.head.appendChild(style);
    }
});