package com.flam.edgedetection.gl

import android.opengl.GLES20
import android.opengl.GLSurfaceView
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

/**
 * OpenGL ES 2.0 Renderer for displaying processed camera frames
 */
class GLRenderer : GLSurfaceView.Renderer {
    
    private val vertices = floatArrayOf(
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 0.0f
    )
    
    private val indices = shortArrayOf(
        0, 1, 2,
        0, 2, 3
    )
    
    private lateinit var vertexBuffer: FloatBuffer
    private lateinit var indexBuffer: ByteBuffer
    
    private var program = 0
    private var textureId = 0
    
    private var imageData: ByteBuffer? = null
    private var imageWidth = 0
    private var imageHeight = 0
    private var needsTextureUpdate = false
    
    private val vertexShaderCode = """
        attribute vec3 aPosition;
        attribute vec2 aTexCoord;
        varying vec2 vTexCoord;
        
        void main() {
            gl_Position = vec4(aPosition, 1.0);
            vTexCoord = aTexCoord;
        }
    """.trimIndent()
    
    private val fragmentShaderCode = """
        precision mediump float;
        varying vec2 vTexCoord;
        uniform sampler2D uTexture;
        
        void main() {
            gl_FragColor = texture2D(uTexture, vTexCoord);
        }
    """.trimIndent()
    
    init {
        // Initialize buffers
        vertexBuffer = ByteBuffer.allocateDirect(vertices.size * 4).run {
            order(ByteOrder.nativeOrder())
            asFloatBuffer().apply {
                put(vertices)
                position(0)
            }
        }
        
        indexBuffer = ByteBuffer.allocateDirect(indices.size * 2).apply {
            order(ByteOrder.nativeOrder())
            indices.forEach { put(it.toByte()) }
            position(0)
        }
    }
    
    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f)
        
        // Create shader program
        val vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode)
        val fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode)
        
        program = GLES20.glCreateProgram().also {
            GLES20.glAttachShader(it, vertexShader)
            GLES20.glAttachShader(it, fragmentShader)
            GLES20.glLinkProgram(it)
        }
        
        // Generate texture
        val textures = IntArray(1)
        GLES20.glGenTextures(1, textures, 0)
        textureId = textures[0]
        
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE)
    }
    
    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)
    }
    
    override fun onDrawFrame(gl: GL10?) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        
        // Update texture if needed
        synchronized(this) {
            if (needsTextureUpdate && imageData != null) {
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)
                GLES20.glTexImage2D(
                    GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGB,
                    imageWidth, imageHeight, 0,
                    GLES20.GL_RGB, GLES20.GL_UNSIGNED_BYTE,
                    imageData
                )
                needsTextureUpdate = false
            }
        }
        
        // Use shader program
        GLES20.glUseProgram(program)
        
        // Get attribute locations
        val positionHandle = GLES20.glGetAttribLocation(program, "aPosition")
        val texCoordHandle = GLES20.glGetAttribLocation(program, "aTexCoord")
        val textureHandle = GLES20.glGetUniformLocation(program, "uTexture")
        
        // Enable vertex arrays
        GLES20.glEnableVertexAttribArray(positionHandle)
        GLES20.glEnableVertexAttribArray(texCoordHandle)
        
        // Set vertex attributes
        vertexBuffer.position(0)
        GLES20.glVertexAttribPointer(positionHandle, 3, GLES20.GL_FLOAT, false, 20, vertexBuffer)
        
        vertexBuffer.position(3)
        GLES20.glVertexAttribPointer(texCoordHandle, 2, GLES20.GL_FLOAT, false, 20, vertexBuffer)
        
        // Bind texture
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)
        GLES20.glUniform1i(textureHandle, 0)
        
        // Draw
        GLES20.glDrawElements(
            GLES20.GL_TRIANGLES, indices.size,
            GLES20.GL_UNSIGNED_BYTE, indexBuffer
        )
        
        // Disable vertex arrays
        GLES20.glDisableVertexAttribArray(positionHandle)
        GLES20.glDisableVertexAttribArray(texCoordHandle)
    }
    
    fun updateTexture(data: ByteArray, width: Int, height: Int) {
        synchronized(this) {
            imageData = ByteBuffer.allocateDirect(data.size).apply {
                put(data)
                position(0)
            }
            imageWidth = width
            imageHeight = height
            needsTextureUpdate = true
        }
    }
    
    private fun loadShader(type: Int, shaderCode: String): Int {
        return GLES20.glCreateShader(type).also { shader ->
            GLES20.glShaderSource(shader, shaderCode)
            GLES20.glCompileShader(shader)
        }
    }
}