#pragma once

#include <string>

#include <d3dx10.h>
#include <dxgi.h>
#include <d3dx10mesh.h>

#ifndef GDEFINES_H
#define GDEFINES_H

typedef long long			g_int64;
typedef unsigned long long	g_uint64;
typedef int  				g_int32;
typedef unsigned int		g_uint32;
typedef long  				g_long;
typedef unsigned long		g_ulong;
typedef short				g_int16;
typedef unsigned short		g_uint16;
typedef char				g_int8;
typedef unsigned char		g_uint8;

typedef float				g_float;
typedef double				g_double;

typedef bool				g_bool;

typedef char				g_char;

typedef D3DXVECTOR2         g_vec2;
typedef D3DXVECTOR3         g_vec3;
typedef D3DXVECTOR4         g_vec4;

typedef D3DXMATRIX          g_mat4x4;
typedef D3DXPLANE           g_plane;

typedef D3DXCOLOR           g_color;

typedef D3DXQUATERNION      g_quaternion;

typedef RECT                g_rect;

typedef std::string         g_string; 

typedef void(ginput_proc)(void);

#define FREENULL_ARR(x) {if (x != NULL) {delete[]x; x = NULL;}}
#define FREENULL_VAR(x) {if (x != NULL) {delete x; x = NULL;}}
#define FREENULLRELEASE(x) {if (x != NULL) {x->Release(); delete x; x = NULL;}}
#define NULLRELEASE(x) {if (x != NULL) {x->Release(); x = NULL;}}

#endif /* GDEFINES_H */