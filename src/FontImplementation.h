#pragma once
#include "ShaderProgram.h"

#define FONTSTASH_IMPLEMENTATION // Expands implementation
#include "fontstash.h"

typedef struct GLFONScontext {
	GLuint tex;
	GLuint VAO, VBOvert, VBOtex;
	int width, height;
	Shader shaderprogram;
} GLFONScontext;

static int glfons__renderCreate(void* userPtr, int width, int height)
{
	GLFONScontext* gl = (GLFONScontext*)userPtr;

	// Create may be called multiple times, delete existing texture.
	if (gl->tex != 0) {
		glDeleteTextures(1, &gl->tex);
		gl->tex = 0;
	}

	glGenTextures(1, &gl->tex);

	if (!gl->tex)
		return 0;

	gl->width = width;
	gl->height = height;

	// The bitmap generated from the glyph is a grayscale 8-bit image where each color is represented by a single byte.
	glBindTexture(GL_TEXTURE_2D, gl->tex);

	// Generated bitmap uses GL_RED as the texture's internal format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gl->width, gl->height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return 1;
}

static int glfons__renderResize(void* userPtr, int width, int height)
{
	// Reuse create to resize too.
	return glfons__renderCreate(userPtr, width, height);
}

static void glfons__renderUpdate(void* userPtr, int* rect, const unsigned char* data)
{
	GLFONScontext* gl = (GLFONScontext*)userPtr;
	int w = rect[2] - rect[0];
	int h = rect[3] - rect[1];

	if (gl->tex == 0)
		return;

	glActiveTexture(GL_TEXTURE0);

	// original pixmap size in pixels, not the size after zooming!
	glBindTexture(GL_TEXTURE_2D, gl->tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // specifies the alignment requirements for the start of each pixel row in memory
	glPixelStorei(GL_UNPACK_ROW_LENGTH, gl->width); // defines the number of pixels in a row.
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, rect[0]); // specifies the number of pixels to skip on the left side of the image (left side of the image)
	glPixelStorei(GL_UNPACK_SKIP_ROWS, rect[1]); // specifies the number of rows or scanlines in the image to skip (bottom of the image)

	// remember? generated bitmap uses GL_RED as the texture's internal format so we pass GL_RED and GL_UNSIGNED_BYTE
	glTexSubImage2D(GL_TEXTURE_2D, 0, rect[0], rect[1], w, h, GL_RED, GL_UNSIGNED_BYTE, data);

	// Restore PixelStorei
	glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}

static void glfons__renderDraw(void* userPtr, const float* verts, const float* tcoords, const unsigned int* colors, int nverts)
{
	GLFONScontext* gl = (GLFONScontext*)userPtr;

	ShaderUseProgram(gl->shaderprogram);

	float* dRGBA = decodeRGBA(*colors);
	setVec4(gl->shaderprogram, "textColor", glm::vec4(dRGBA[0], dRGBA[1], dRGBA[2], dRGBA[3]));
	//LOG_INFO("r {} g {} b {} a {}", dRGBA[0], dRGBA[1], dRGBA[2], dRGBA[3]);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(gl->VAO); // bind vertex array object

	if (gl->tex == 0)
		return;

	glBindTexture(GL_TEXTURE_2D, gl->tex);

	glBindBuffer(GL_ARRAY_BUFFER, gl->VBOvert); // bind vertex buffer texture vertexes
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * nverts * 2, verts);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0); //vertex attr

	glBindBuffer(GL_ARRAY_BUFFER, gl->VBOtex); // bind vertex buffer texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * nverts * 2, tcoords);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0); //tcoord attr

	glDrawArrays(GL_TRIANGLES, 0, nverts); // draw vertices 

	glBindBuffer(GL_ARRAY_BUFFER, 0); // reset buffer
	glBindVertexArray(0); // reset vertex array object
}

static void glfons__renderDelete(void* userPtr)
{
	GLFONScontext* gl = (GLFONScontext*)userPtr;

	if (gl->tex != 0)
		glDeleteTextures(1, &gl->tex);

	gl->tex = 0;
	free(gl);
}

//////////////////////////////////////////////////////////////////////////

// fontStash Renderer backend
FONScontext* glfonsCreate(int screenwidth, int screenheight, int flags)
{
	FONSparams params;
	GLFONScontext* gl = (GLFONScontext*)malloc(sizeof(GLFONScontext));

	if (gl == nullptr)
		return nullptr;

	memset(gl, 0, sizeof(GLFONScontext));

	glm::mat4 projection = glm::ortho(0.0f, (float)screenwidth, (float)screenheight, 0.0f);

	// vertex shader
	gl->shaderprogram.vs_code = R"CODE(
				#version 330 core
				layout (location = 0) in vec2 vertex;
				layout (location = 1) in vec2 tcoord;
				out vec2 TexCoords;

				uniform mat4 projection;

				void main()
				{
					gl_Position = projection * vec4(vertex, 0.0, 1.0);
					TexCoords = tcoord;
				} 
)CODE";

	// fragment shader takes in vec4 color array -> RGBA
	gl->shaderprogram.ps_code = R"CODE(
				#version 330 core
				in vec2 TexCoords;
				out vec4 color;

				uniform sampler2D text;
				uniform vec4 textColor;

				void main()
				{    
				    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
				    color = vec4(textColor) * sampled;
				}  
)CODE";

	ShaderCreateProgram(gl->shaderprogram);
	ShaderUseProgram(gl->shaderprogram);

	setMat4(gl->shaderprogram, "projection", projection);

	glGenVertexArrays(1, &gl->VAO); // generate vertex array objects
	glGenBuffers(1, &gl->VBOvert);  // generate vertex buffer texture vertexes
	glGenBuffers(1, &gl->VBOtex);  // generate vertex buffer texture coordinates

	glBindVertexArray(gl->VAO);

	//Because we'll be updating the content of the VBO's memory quite often we'll allocate the memory with GL_DYNAMIC_DRAW.
	// The 2D quad requires 1024 vertices of 2 floats each, so we reserve 4 * 1024 * 2 = 8192 of memory. 
	glBindBuffer(GL_ARRAY_BUFFER, gl->VBOvert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * FONS_VERTEX_COUNT * 2, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, gl->VBOtex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * FONS_VERTEX_COUNT * 2, NULL, GL_DYNAMIC_DRAW);

	memset(&params, 0, sizeof(params));
	params.width = screenwidth;
	params.height = screenheight;
	params.flags = (unsigned char)flags;
	params.renderCreate = glfons__renderCreate;
	params.renderResize = glfons__renderResize;
	params.renderUpdate = glfons__renderUpdate;
	params.renderDraw = glfons__renderDraw;
	params.renderDelete = glfons__renderDelete;
	params.userPtr = gl;

	return fonsCreateInternal(&params);
}
