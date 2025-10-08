/**
 * Edge Detection Web Viewer
 * Displays processed frames from the Android application
 */

interface FrameStats {
    fps: number;
    resolution: { width: number; height: number };
    processingTime: number;
    timestamp: number;
}

class EdgeDetectionViewer {
    private canvas: HTMLCanvasElement;
    private ctx: CanvasRenderingContext2D;
    private statsElement: HTMLElement;
    private currentFrame: HTMLImageElement | null = null;
    private stats: FrameStats;

    constructor() {
        this.canvas = document.getElementById('frameCanvas') as HTMLCanvasElement;
        this.ctx = this.canvas.getContext('2d')!;
        this.statsElement = document.getElementById('stats')!;
        
        this.stats = {
            fps: 0,
            resolution: { width: 0, height: 0 },
            processingTime: 0,
            timestamp: Date.now()
        };

        this.setupEventListeners();
        this.loadSampleFrame();
        this.updateStats();
    }

    private setupEventListeners(): void {
        // File upload
        const fileInput = document.getElementById('fileInput') as HTMLInputElement;
        fileInput.addEventListener('change', (e) => this.handleFileUpload(e));

        // Load sample button
        const loadSampleBtn = document.getElementById('loadSample') as HTMLButtonElement;
        loadSampleBtn.addEventListener('click', () => this.loadSampleFrame());

        // Clear button
        const clearBtn = document.getElementById('clearBtn') as HTMLButtonElement;
        clearBtn.addEventListener('click', () => this.clearCanvas());
    }

    private handleFileUpload(event: Event): void {
        const target = event.target as HTMLInputElement;
        const file = target.files?.[0];
        
        if (file && file.type.startsWith('image/')) {
            const reader = new FileReader();
            reader.onload = (e) => {
                const img = new Image();
                img.onload = () => {
                    this.displayFrame(img);
                };
                img.src = e.target?.result as string;
            };
            reader.readAsDataURL(file);
        }
    }

    private loadSampleFrame(): void {
        // Load a sample edge-detected frame (base64 encoded)
        // This would typically come from the Android app
        const sampleBase64 = this.generateSampleEdgeFrame();
        
        const img = new Image();
        img.onload = () => {
            this.displayFrame(img);
        };
        img.src = sampleBase64;
    }

    private displayFrame(img: HTMLImageElement): void {
        this.currentFrame = img;
        
        // Update canvas size to match image
        this.canvas.width = img.width;
        this.canvas.height = img.height;
        
        // Draw image
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        this.ctx.drawImage(img, 0, 0);
        
        // Update stats
        this.stats.resolution = { width: img.width, height: img.height };
        this.stats.timestamp = Date.now();
        this.stats.fps = 15.0; // Sample FPS
        this.stats.processingTime = 25.5; // Sample processing time
        
        this.updateStats();
    }

    private clearCanvas(): void {
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        this.currentFrame = null;
        this.stats = {
            fps: 0,
            resolution: { width: 0, height: 0 },
            processingTime: 0,
            timestamp: Date.now()
        };
        this.updateStats();
    }

    private updateStats(): void {
        const statsHTML = `
            <div class="stat-item">
                <span class="stat-label">FPS:</span>
                <span class="stat-value">${this.stats.fps.toFixed(1)}</span>
            </div>
            <div class="stat-item">
                <span class="stat-label">Resolution:</span>
                <span class="stat-value">${this.stats.resolution.width} x ${this.stats.resolution.height}</span>
            </div>
            <div class="stat-item">
                <span class="stat-label">Processing Time:</span>
                <span class="stat-value">${this.stats.processingTime.toFixed(2)} ms</span>
            </div>
            <div class="stat-item">
                <span class="stat-label">Last Update:</span>
                <span class="stat-value">${new Date(this.stats.timestamp).toLocaleTimeString()}</span>
            </div>
        `;
        this.statsElement.innerHTML = statsHTML;
    }

    private generateSampleEdgeFrame(): string {
        // Generate a sample edge-detected frame pattern
        const tempCanvas = document.createElement('canvas');
        tempCanvas.width = 640;
        tempCanvas.height = 480;
        const tempCtx = tempCanvas.getContext('2d')!;
        
        // Black background
        tempCtx.fillStyle = '#000000';
        tempCtx.fillRect(0, 0, tempCanvas.width, tempCanvas.height);
        
        // White edges (simulating Canny edge detection)
        tempCtx.strokeStyle = '#FFFFFF';
        tempCtx.lineWidth = 2;
        
        // Draw some sample edges
        for (let i = 0; i < 50; i++) {
            tempCtx.beginPath();
            const x = Math.random() * tempCanvas.width;
            const y = Math.random() * tempCanvas.height;
            const length = Math.random() * 100 + 20;
            const angle = Math.random() * Math.PI * 2;
            
            tempCtx.moveTo(x, y);
            tempCtx.lineTo(
                x + Math.cos(angle) * length,
                y + Math.sin(angle) * length
            );
            tempCtx.stroke();
        }
        
        // Draw some geometric shapes
        tempCtx.strokeRect(50, 50, 150, 100);
        tempCtx.beginPath();
        tempCtx.arc(400, 240, 80, 0, Math.PI * 2);
        tempCtx.stroke();
        
        return tempCanvas.toDataURL('image/png');
    }
}

// Initialize viewer when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    new EdgeDetectionViewer();
    console.log('Edge Detection Viewer initialized');
});