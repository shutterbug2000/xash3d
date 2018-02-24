/*
cl_perfgraph.c -- perfomance monitor(client drawing)
Copyright (C) 2018 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#if defined(XASH_PERFMON) && !defined(XASH_DEDICATED)

#include "common.h"
#include "client.h"
#include "perf.h"
#include "gl_local.h"

convar_t *perf_graph;
convar_t *perf_diagram_width;
convar_t *perf_diagram_height;

_inline void CL_ColorForString( const char *str, rgba_t rgba )
{
	uint ulRGB = Com_HashKey( str, 1 << 24 );
	rgba[0] = (ulRGB & 0xFF0000) >> 16;
	rgba[1] = (ulRGB & 0xFF00) >> 8;
	rgba[2] = (ulRGB & 0xFF) >> 0;
	rgba[3] = 255; // alpha
}

void CL_DrawFunctionGraph( qboolean once )
{
	int x, y, maxFrames;
	int h;
	perf_frame_t *frame;
	perf_list_t *func;

	PERF_START;

	x = 64;
	y = 64;
	Con_DrawCharacterLen( 'M', NULL, &h );

	maxFrames = host.framecount - 1 - (int)perf_diagram_width->value;
	if( maxFrames < 0 )
		maxFrames = 0;

	frame = Perf_GetFrame( host.framecount - 1 );

	for( func = frame->head; func && func->f.name; func = func->graphnext )
	{
		rgba_t rgba;
		int _x;

		CL_ColorForString( func->f.name, rgba );
		_x = x + h * func->stackidx;

		pglColor4ubv( rgba );

		GL_SetRenderMode( kRenderNormal );
		R_DrawStretchPic( _x, y, h, h, 0, 0, 1, 1, cls.fillImage );

		Con_DrawString( _x + h * 1.5, y, func->f.name, rgba );

		y += h;
	}

	PERF_END;

}

void CL_DrawFunctionBar( int x, int y, float scaleHeight, double start, perf_func_t *func )
{
	rgba_t rgba;
	int h;

	CL_ColorForString( func->name, rgba );

	h = func->end * scaleHeight - func->start * scaleHeight;
	y = y - h;
	y = y - (func->start * scaleHeight - start * scaleHeight);

	pglColor4ubv( rgba );

	GL_SetRenderMode( kRenderNormal );
	R_DrawStretchPic( x, y, 1, h, 0, 0, 1, 1, cls.fillImage );
}

void CL_DrawFunctionDiagramma( )
{
	perf_frame_t *frame;
	int i, maxFrames, y;
	float scaleHeight;
	float frametime, worstFrameTime = 0; // TODO: get from perf API

	PERF_START;

	maxFrames = host.framecount - 1 - (int)perf_diagram_width->value;
	if( maxFrames < 0 )
		maxFrames = 0;
	y = glw_state.desktopHeight - 64;

	for( i = host.framecount - 1; i > maxFrames; i-- )
	{
		frame = Perf_GetFrame( i );
		// get time of host_frame
		frametime = frame->head->f.end - frame->head->f.start;

		if( frametime > worstFrameTime )
			worstFrameTime = frametime;
	}

	if( worstFrameTime )
		scaleHeight = perf_diagram_height->value / worstFrameTime;
	else scaleHeight = 3200.0f; // enough to fill height if 10 FPS is worst case

	// we can't read latest frame, it's not ready yet!
	for( i = host.framecount - 1; i > maxFrames; i-- )
	{
		perf_list_t *func;
		int x;
		frame = Perf_GetFrame( i );

		x = glw_state.desktopWidth - 64 - ( host.framecount - i );

		for( func = frame->head->graphnext; func && func->f.name; func = func->graphnext )
			CL_DrawFunctionBar( x, y, scaleHeight, frame->head->f.start, &func->f );
	}

	PERF_END;
}

void CL_DrawPerf( void )
{
	if( !host.developer )
		return;

	if( !host.framecount )
		return;

	PERF_START;

	switch( (int)perf_graph->value )
	{
	// case 4:
		// CL_DrawFunctionDiagramma( );
		// intentional fallthrough
	case 3:
		CL_DrawFunctionGraph( true );
		break;
	case 2:
		CL_DrawFunctionDiagramma( );
		// intentional fallthrough
	case 1:
		CL_DrawFunctionGraph( false );
		break;
	default:;
	}

	PERF_END;
}

void CL_InitPerf( void )
{
	perf_graph = Cvar_Get( "perf_graph", "2", 0, "draw performance graph" );
	perf_diagram_width = Cvar_Get( "perf_diagram_width", "512", FCVAR_ARCHIVE, "how much of previous frames should be displayed" );
	perf_diagram_height = Cvar_Get( "perf_diagram_height", "320", FCVAR_ARCHIVE, "performance diagram height" );
}

#endif // defined(XASH_PERFMON) && !defined(XASH_DEDICATED)
