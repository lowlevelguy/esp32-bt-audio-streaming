// Global state
let devices = [];

// DOM elements
const deviceList = document.getElementById('device-list');
const loading = document.getElementById('loading');
const statusDiv = document.getElementById('status');
const statusText = document.getElementById('status-text');
const ipAddress = document.getElementById('ip-address');

// Fetch IP address on load
fetch('/api/status')
    .then(res => res.json())
    .then(data => {
        ipAddress.textContent = data.ip;
    });

// Scan for Bluetooth devices
async function scanDevices() {
    showLoading();
    
    try {
        const response = await fetch('/api/scan');
        devices = await response.json();
        renderDeviceList();
    } catch (error) {
        showError('Failed to scan for devices');
    } finally {
        hideLoading();
    }
}

// Connect to a device
async function connectToDevice(deviceId) {
    showStatus('Connecting to speaker...');
    
    try {
        const response = await fetch(`/api/connect?id=${deviceId}`);
        const result = await response.json();
        
        if (response.ok) {
            showStatus('Connected successfully!');
            
            // Update UI
            const connectBtn = document.querySelector(`[data-id="${deviceId}"]`);
            if (connectBtn) {
                connectBtn.innerHTML = '<i class="fas fa-check"></i> Connected';
                connectBtn.disabled = true;
                connectBtn.classList.add('connected');
            }
            
            // Show success message
            setTimeout(() => {
                hideStatus();
                alert('Speaker connected! The system will now reboot in streaming mode.');
            }, 2000);
        } else {
            showStatus('Connection failed: ' + (result.error || 'Unknown error'));
        }
    } catch (error) {
        showStatus('Connection error');
    }
}

// Render device list
function renderDeviceList() {
    if (devices.length === 0) {
        deviceList.innerHTML = `
            <div class="empty-state">
                <i class="fas fa-search"></i>
                <p>No Bluetooth speakers found</p>
                <p class="small">Make sure your speaker is in pairing mode and nearby</p>
            </div>
        `;
        return;
    }
    
    deviceList.innerHTML = devices.map(device => `
        <div class="device-item">
            <div class="device-info">
                <i class="fas fa-speaker device-icon"></i>
                <span class="device-name">${device.name || 'Unknown Device'}</span>
            </div>
            <button class="btn-connect" onclick="connectToDevice(${device.id})" data-id="${device.id}">
                <i class="fas fa-link"></i> Connect
            </button>
        </div>
    `).join('');
}

// UI helpers
function showLoading() {
    loading.classList.remove('hidden');
    deviceList.classList.add('hidden');
}

function hideLoading() {
    loading.classList.add('hidden');
    deviceList.classList.remove('hidden');
}

function showStatus(message) {
    statusText.textContent = message;
    statusDiv.classList.remove('hidden');
}

function hideStatus() {
    statusDiv.classList.add('hidden');
}

function showError(message) {
    deviceList.innerHTML = `
        <div class="empty-state">
            <i class="fas fa-exclamation-triangle"></i>
            <p>${message}</p>
            <button class="btn" onclick="scanDevices()">Try Again</button>
        </div>
    `;
}

// Initial scan
window.addEventListener('DOMContentLoaded', scanDevices);