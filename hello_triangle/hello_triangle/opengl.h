#pragma once

#include <cassert>
#include <Windows.h>
#include <GL/gl.h>
#include "glext.h"
#include "wglext.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "math.h"
#include <stdio.h>

bool running;
bool resized;
uint32_t viewport_width;
uint32_t viewport_height;
HWND window_handle;
HDC device_context;
HGLRC opengl_render_context;

#define DECLARE_GL_PROC(pointer_name, function_name) pointer_name function_name
#define LOAD_GL_PROC(pointer_name, function_name) function_name = (pointer_name)wglGetProcAddress(#function_name);

DECLARE_GL_PROC(PFNGLCREATESHADERPROC, glCreateShader);
DECLARE_GL_PROC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
DECLARE_GL_PROC(PFNGLLINKPROGRAMPROC, glLinkProgram);
DECLARE_GL_PROC(PFNGLUSEPROGRAMPROC, glUseProgram);
DECLARE_GL_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
DECLARE_GL_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
DECLARE_GL_PROC(PFNGLUNIFORM4FVPROC, glUniform4fv);
DECLARE_GL_PROC(PFNGLSHADERSOURCEPROC, glShaderSource);
DECLARE_GL_PROC(PFNGLCOMPILESHADERPROC, glCompileShader);
DECLARE_GL_PROC(PFNGLGETSHADERIVPROC, glGetShaderiv);
DECLARE_GL_PROC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
DECLARE_GL_PROC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
DECLARE_GL_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
DECLARE_GL_PROC(PFNGLATTACHSHADERPROC, glAttachShader);
DECLARE_GL_PROC(PFNGLDETACHSHADERPROC, glDetachShader);

void initialize_opengl()
{
    PIXELFORMATDESCRIPTOR pfd = {};

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    device_context = GetDC(window_handle);
    int32_t format_index = ChoosePixelFormat(device_context, &pfd);
    assert(format_index);
    assert(SetPixelFormat(device_context, format_index, &pfd));

    opengl_render_context = wglCreateContext(device_context);
    assert(opengl_render_context);

    assert(wglMakeCurrent(device_context, opengl_render_context));

    LOAD_GL_PROC(PFNGLCREATESHADERPROC, glCreateShader);
    LOAD_GL_PROC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
    LOAD_GL_PROC(PFNGLLINKPROGRAMPROC, glLinkProgram);
    LOAD_GL_PROC(PFNGLSHADERSOURCEPROC, glShaderSource);
    LOAD_GL_PROC(PFNGLCOMPILESHADERPROC, glCompileShader);
    LOAD_GL_PROC(PFNGLGETSHADERIVPROC, glGetShaderiv);
    LOAD_GL_PROC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
    LOAD_GL_PROC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
    LOAD_GL_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
    LOAD_GL_PROC(PFNGLATTACHSHADERPROC, glAttachShader);
    LOAD_GL_PROC(PFNGLDETACHSHADERPROC, glDetachShader);
    LOAD_GL_PROC(PFNGLUSEPROGRAMPROC, glUseProgram);
    LOAD_GL_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    LOAD_GL_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
    LOAD_GL_PROC(PFNGLUNIFORM4FVPROC, glUniform4fv);
}

LRESULT CALLBACK window_callback(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CLOSE: {
        running = 0;
        PostQuitMessage(0);
    } break;
    }

    return DefWindowProc(window_handle, message, wparam, lparam);
}

void initialize_window_and_opengl()
{
    HINSTANCE hinstance = GetModuleHandle(NULL);

    running = 0;

    const char *window_class_name = "triangle_demo_class";
    const char *window_name = "Triangle!";

    SetProcessDPIAware();

    WNDCLASS window_class = {};
    window_class.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    window_class.hInstance = hinstance;
    window_class.lpszClassName = window_class_name;
    window_class.lpfnWndProc = window_callback;

    assert(RegisterClass(&window_class));

    window_handle = CreateWindowEx(0,
        window_class_name,
        window_name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hinstance,
        NULL);

    RECT rect = {};
    GetClientRect(window_handle, &rect);

    viewport_width = rect.right;
    viewport_height = rect.bottom;
    
    assert(window_handle);

    initialize_opengl();

    ShowWindow(window_handle, SW_SHOW);

    running = 1;

    glViewport(0, 0, viewport_width, viewport_height);
}

void catch_input_events()
{
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

void swap_buffers()
{
    SwapBuffers(device_context);
}

bool is_window_open()
{
    return running;
}

const char *load_vertex_shader_code()
{
    static const char *source = R"(

#version 400

vec4 positions[3] = vec4[3]( vec4(-0.5, -0.5, 0.0, 1.0), vec4(0.0, 0.5, 0.0, 1.0), vec4(0.5, -0.5, 0.0, 1.0) );

void main()
{
    gl_Position = positions[gl_VertexID];
}

)";

    return source;
}

const char *load_fragment_shader_code()
{
    static const char *source = R"(

#version 400

out vec4 out_color;

void main()
{
    out_color = vec4(1.0, 0.0, 0.0, 1.0);
}


)";

    return source;
}