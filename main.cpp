#include <cassert>
#include <Windows.h>
#include <GL/gl.h>
#include "glext.h"
#include "wglext.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "game_math.hpp"



static bool running;
static bool resized;
static uint32_t viewport_width;
static uint32_t viewport_height;
static HWND window_handle;
static HDC device_context;
static HGLRC opengl_render_context;
static uint32_t pipeline;
static uint32_t v_loc;
static uint32_t p_loc;
static vector3_t position, direction;
static matrix4_t view;
static matrix4_t projection;
static real32_t dt;

static bool moved[6] = {};

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

void initialize_demo()
{
    const char* vertex_shader = R"(

#version 400

vec4 positions[3] = vec4[3]( vec4(-0.5, -0.5, 0.0, 1.0), vec4(0.0, 0.5, 0.0, 1.0), vec4(0.5, -0.5, 0.0, 1.0) );

uniform mat4 view;
uniform mat4 projection;

void main()
{
gl_Position = projection * view * positions[gl_VertexID];
})";

    const char* fragment_shader = R"(

#version 400

out vec4 out_color;

void main()
{
out_color = vec4(1.0, 0.0, 0.0, 1.0);
})";

    pipeline = glCreateProgram();

    uint32_t vsh = glCreateShader(GL_VERTEX_SHADER);
    int32_t vsh_source_length = (int32_t)strlen(vertex_shader);
    glShaderSource(vsh, 1, &vertex_shader, &vsh_source_length);
    glCompileShader(vsh);

    GLint result = GL_FALSE;
    glGetShaderiv(vsh, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        // Get info
        OutputDebugString("Shader failed to compile");
        assert(0);
    }

    glAttachShader(pipeline, vsh);


    uint32_t fsh = glCreateShader(GL_FRAGMENT_SHADER);
    int32_t fsh_source_length = (int32_t)strlen(fragment_shader);
    glShaderSource(fsh, 1, &fragment_shader, &fsh_source_length);
    glCompileShader(fsh);

    glGetShaderiv(fsh, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        // Get info
        OutputDebugString("Shader failed to compile");
        assert(0);
    }

    glAttachShader(pipeline, fsh);
    

    
    glLinkProgram(pipeline);

    glGetProgramiv(pipeline, GL_LINK_STATUS, &result);
    if (!result)
    {
        // Get info
        OutputDebugString("Program failed to link");
        assert(0);
    }

    glDetachShader(pipeline, vsh);
    glDetachShader(pipeline, fsh);

    glUseProgram(pipeline);

    v_loc = glGetUniformLocation(pipeline, "view");
    p_loc = glGetUniformLocation(pipeline, "projection");



    position = vector3_t(0.0f, 0.0f, 1.0f);
    direction = vector3_t(0.0f, 0.0f, -1.0f);

    view = look_at(position, position + direction, vector3_t(0, 1, 0));
    projection = perspective(90.0f * (3.1415 / 180.0f), (real32_t)viewport_width / (real32_t)viewport_height, 0.1f, 100.0f);
}

void tick_demo()
{
    if (moved[0])
    {
        position += direction * dt * 5.0f;
    }
    if (moved[1])
    {
        position -= cross(direction, vector3_t(0, 1, 0)) * dt * 5.0f;
    }
    if (moved[2])
    {
        position -= direction * dt * 5.0f;
    }
    if (moved[3])
    {
        position += cross(direction, vector3_t(0, 1, 0)) * dt * 5.0f;
    }
    if (moved[4])
    {
        position += vector3_t(0, 1, 0) * dt * 5.0f;
    }
    if (moved[5])
    {
        position -= vector3_t(0, 1, 0) * dt * 5.0f;
    }
    
    glViewport(0, 0, viewport_width, viewport_height);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    view = look_at(position, position + direction, vector3_t(0, 1, 0));
    projection = perspective(90.0f * (3.1415 / 180.0f), (real32_t)viewport_width / (real32_t)viewport_height, 0.1f, 100.0f);

    glUniformMatrix4fv(p_loc, 1, GL_FALSE, &projection.col[0].v[0]);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &view.col[0].v[0]);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

LRESULT window_callback(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {

    case WM_SIZE: {

        resized = 1;
        viewport_width = LOWORD(lparam);
        viewport_height = HIWORD(lparam);

    } break;

    case WM_KEYDOWN: {

        switch(wparam)
        {
            // W
        case 0x57: {
            moved[0] = 1;
        } break;
            
            // A
        case 0x41: {
            moved[1] = 1;
        } break;
            
            // S
        case 0x53: {
            moved[2] = 1;
        } break;
            
            // D
        case 0x44: {
            moved[3] = 1;
        } break;

            // Space
        case VK_SPACE: {
            moved[4] = 1;
        } break;

            // Space
        case VK_SHIFT: {
            moved[5] = 1;
        } break;
            
        }
        
    } break;

    case WM_KEYUP: {

        switch(wparam)
        {
            // W
        case 0x57: {
            moved[0] = 0;
        } break;
            
            // A
        case 0x41: {
            moved[1] = 0;
        } break;
            
            // S
        case 0x53: {
            moved[2] = 0;
        } break;
            
            // D
        case 0x44: {
            moved[3] = 0;
        } break;

            // Space
        case VK_SPACE: {
            moved[4] = 0;
        } break;

            // Space
        case VK_SHIFT: {
            moved[5] = 0;
        } break;
            
        }
        
    } break;

    case WM_CLOSE: {
        running = 0;
        PostQuitMessage(0);
    } break;

    }

    return DefWindowProc(window_handle, message, wparam, lparam);
}

static real32_t measure_time_difference(LARGE_INTEGER begin_time, LARGE_INTEGER end_time, LARGE_INTEGER frequency)
{
    return real32_t(end_time.QuadPart - begin_time.QuadPart) / real32_t(frequency.QuadPart);
}

void main()
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

    assert(window_handle);

    initialize_opengl();
    initialize_demo();

    ShowWindow(window_handle, SW_SHOW);

    running = 1;

    uint32_t sleep_granularity_milliseconds = 1;
    bool success = (timeBeginPeriod(sleep_granularity_milliseconds) == TIMERR_NOERROR);

    LARGE_INTEGER clock_frequency;
    QueryPerformanceFrequency(&clock_frequency);
    
    while (running)
    {
        // Frame
        LARGE_INTEGER tick_start;
        QueryPerformanceCounter(&tick_start);
        
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        tick_demo();

        SwapBuffers(device_context);

        LARGE_INTEGER tick_end;
        QueryPerformanceCounter(&tick_end);
        real32_t new_dt = measure_time_difference(tick_start, tick_end, clock_frequency);

        dt = new_dt;
    }
}
