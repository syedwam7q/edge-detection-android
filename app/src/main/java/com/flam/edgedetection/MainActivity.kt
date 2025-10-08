package com.flam.edgedetection

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.flam.edgedetection.databinding.ActivityMainBinding
import com.flam.edgedetection.gl.GLRenderer
import java.nio.ByteBuffer
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {
    
    private lateinit var binding: ActivityMainBinding
    private lateinit var cameraExecutor: ExecutorService
    private lateinit var glRenderer: GLRenderer
    
    private var isEdgeDetectionMode = true
    private var fpsCounter = FPSCounter()
    
    companion object {
        private const val CAMERA_PERMISSION_CODE = 100
        private const val TAG = "EdgeDetection"
        
        init {
            // Load OpenCV library first (required dependency)
            System.loadLibrary("opencv_java4")
            // Then load our native library
            System.loadLibrary("edgedetection")
        }
    }
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        
        // Initialize OpenCV
        NativeLib.initOpenCV()
        
        // Setup GLSurfaceView
        binding.glSurfaceView.setEGLContextClientVersion(2)
        glRenderer = GLRenderer()
        binding.glSurfaceView.setRenderer(glRenderer)
        binding.glSurfaceView.renderMode = android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY
        
        // Setup camera executor
        cameraExecutor = Executors.newSingleThreadExecutor()
        
        // Setup toggle button
        binding.toggleButton.setOnClickListener {
            isEdgeDetectionMode = !isEdgeDetectionMode
            updateModeText()
        }
        
        updateModeText()
        
        // Check camera permission
        if (checkCameraPermission()) {
            startCamera()
        } else {
            requestCameraPermission()
        }
    }
    
    private fun checkCameraPermission(): Boolean {
        return ContextCompat.checkSelfPermission(
            this, Manifest.permission.CAMERA
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    private fun requestCameraPermission() {
        ActivityCompat.requestPermissions(
            this,
            arrayOf(Manifest.permission.CAMERA),
            CAMERA_PERMISSION_CODE
        )
    }
    
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == CAMERA_PERMISSION_CODE) {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                startCamera()
            } else {
                Toast.makeText(
                    this,
                    getString(R.string.camera_permission_required),
                    Toast.LENGTH_SHORT
                ).show()
                finish()
            }
        }
    }
    
    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        
        cameraProviderFuture.addListener({
            val cameraProvider = cameraProviderFuture.get()
            
            val imageAnalysis = ImageAnalysis.Builder()
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build()
                .also {
                    it.setAnalyzer(cameraExecutor) { imageProxy ->
                        processImage(imageProxy)
                    }
                }
            
            val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA
            
            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(
                    this, cameraSelector, imageAnalysis
                )
            } catch (e: Exception) {
                e.printStackTrace()
            }
            
        }, ContextCompat.getMainExecutor(this))
    }
    
    private fun processImage(imageProxy: ImageProxy) {
        try {
            val buffer = imageProxy.planes[0].buffer
            val bytes = ByteArray(buffer.remaining())
            buffer.get(bytes)
            
            val width = imageProxy.width
            val height = imageProxy.height
            
            // Convert YUV to RGB
            val rgbBytes = NativeLib.yuv420ToRgb(
                bytes,
                width,
                height,
                imageProxy.planes[0].rowStride
            )
            
            // Process frame
            val processedBytes = if (isEdgeDetectionMode) {
                NativeLib.processFrame(rgbBytes, width, height, true)
            } else {
                rgbBytes // Raw feed
            }
            
            // Update texture on GL thread
            glRenderer.updateTexture(processedBytes, width, height)
            binding.glSurfaceView.requestRender()
            
            // Update FPS
            fpsCounter.frame()
            runOnUiThread {
                binding.fpsText.text = getString(R.string.fps_label, fpsCounter.getFPS())
            }
            
        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            imageProxy.close()
        }
    }
    
    private fun updateModeText() {
        binding.modeText.text = if (isEdgeDetectionMode) {
            getString(R.string.mode_edge)
        } else {
            getString(R.string.mode_raw)
        }
    }
    
    override fun onDestroy() {
        super.onDestroy()
        cameraExecutor.shutdown()
    }
    
    // FPS Counter class
    private class FPSCounter {
        private var frameCount = 0
        private var lastTime = System.currentTimeMillis()
        private var fps = 0.0
        
        fun frame() {
            frameCount++
            val currentTime = System.currentTimeMillis()
            val elapsed = currentTime - lastTime
            
            if (elapsed >= 1000) {
                fps = (frameCount * 1000.0) / elapsed
                frameCount = 0
                lastTime = currentTime
            }
        }
        
        fun getFPS(): Double = fps
    }
}