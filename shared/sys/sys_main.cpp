/*
===========================================================================
Copyright (C) 2005 - 2015, ioquake3 contributors
Copyright (C) 2013 - 2015, OpenJK contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include <csignal>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <sys/stat.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "qcommon/qcommon.h"
#include "sys_local.h"
#include <SDL.h>
#include "sys_public.h"
#include "con_local.h"

static char binaryPath[ MAX_OSPATH ] = { 0 };
static char installPath[ MAX_OSPATH ] = { 0 };

cvar_t *com_minimized;
cvar_t *com_unfocused;
cvar_t *com_maxfps;
cvar_t *com_maxfpsMinimized;
cvar_t *com_maxfpsUnfocused;

/*
=================
Sys_SetBinaryPath
=================
*/
void Sys_SetBinaryPath(const char *path)
{
	Q_strncpyz(binaryPath, path, sizeof(binaryPath));
}

/*
=================
Sys_BinaryPath
=================
*/
char *Sys_BinaryPath(void)
{
	return binaryPath;
}

/*
=================
Sys_SetDefaultInstallPath
=================
*/
void Sys_SetDefaultInstallPath(const char *path)
{
	Q_strncpyz(installPath, path, sizeof(installPath));
}

/*
=================
Sys_DefaultInstallPath
=================
*/
char *Sys_DefaultInstallPath(void)
{
	if (*installPath)
		return installPath;
	else
		return Sys_Cwd();
}

/*
=================
Sys_DefaultAppPath
=================
*/
char *Sys_DefaultAppPath(void)
{
	return Sys_BinaryPath();
}

/*
==================
Sys_GetClipboardData
==================
*/
char *Sys_GetClipboardData( void ) {
#ifdef DEDICATED
	return NULL;
#else
	if ( !SDL_HasClipboardText() )
		return NULL;

	char *cbText = SDL_GetClipboardText();
	size_t len = strlen( cbText ) + 1;

	char *buf = (char *)Z_Malloc( len, TAG_CLIPBOARD );
	Q_strncpyz( buf, cbText, len );

	SDL_free( cbText );
	return buf;
#endif
}

/*
=================
Sys_ConsoleInput

Handle new console input
=================
*/
char *Sys_ConsoleInput(void)
{
	return CON_Input( );
}

void Sys_Print( const char *msg ) {
	// TTimo - prefix for text that shows up in console but not in notify
	// backported from RTCW
	if ( !Q_strncmp( msg, "[skipnotify]", 12 ) ) {
		msg += 12;
	}
	if ( msg[0] == '*' ) {
		msg += 1;
	}
	ConsoleLogAppend( msg );
	CON_Print( msg );
}

/*
================
Sys_Init

Called after the common systems (cvars, files, etc)
are initialized
================
*/
void Sys_Init( void ) {
	Cmd_AddCommand ("in_restart", IN_Restart);
	Cvar_Get( "arch", OS_STRING " " ARCH_STRING, CVAR_ROM );
	Cvar_Get( "username", Sys_GetCurrentUser(), CVAR_ROM );

	com_unfocused = Cvar_Get( "com_unfocused", "0", CVAR_ROM );
	com_minimized = Cvar_Get( "com_minimized", "0", CVAR_ROM );
#ifdef _JK2EXE
	com_maxfps = Cvar_Get ("com_maxfps", "125", CVAR_ARCHIVE );
#else
	com_maxfps = Cvar_Get( "com_maxfps", "125", CVAR_ARCHIVE, "Maximum frames per second" );
#endif
	com_maxfpsUnfocused = Cvar_Get( "com_maxfpsUnfocused", "0", CVAR_ARCHIVE );
	com_maxfpsMinimized = Cvar_Get( "com_maxfpsMinimized", "50", CVAR_ARCHIVE );
}

static void NORETURN Sys_Exit( int ex ) {
	IN_Shutdown();
#ifndef DEDICATED
	SDL_Quit();
#endif

	NET_Shutdown();

	Sys_PlatformExit();

	Com_ShutdownHunkMemory();
	Com_ShutdownZoneMemory();

	CON_Shutdown();

    exit( ex );
}

#if !defined(DEDICATED)
static void Sys_ErrorDialog( const char *error )
{
	time_t rawtime;
	char timeStr[32] = {}; // should really only reach ~19 chars
	char crashLogPath[MAX_OSPATH];

	time( &rawtime );
	strftime( timeStr, sizeof( timeStr ), "%Y-%m-%d_%H-%M-%S", localtime( &rawtime ) ); // or gmtime
	Com_sprintf( crashLogPath, sizeof( crashLogPath ),
					"%s%ccrashlog-%s.txt",
					Sys_DefaultHomePath(), PATH_SEP, timeStr );

	Sys_Mkdir( Sys_DefaultHomePath() );

	FILE *fp = fopen( crashLogPath, "w" );
	if ( fp )
	{
		ConsoleLogWriteOut( fp );
		fclose( fp );

		const char *errorMessage = va( "%s\n\nThe crash log was written to %s", error, crashLogPath );
		if ( SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", errorMessage, NULL ) < 0 )
		{
			fprintf( stderr, "%s", errorMessage );
		}
	}
	else
	{
		// Getting pretty desperate now
		ConsoleLogWriteOut( stderr );
		fflush( stderr );

		const char *errorMessage = va( "%s\nCould not write the crash log file, but we printed it to stderr.\n"
										"Try running the game using a command line interface.", error );
		if ( SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", errorMessage, NULL ) < 0 )
		{
			// We really have hit rock bottom here :(
			fprintf( stderr, "%s", errorMessage );
		}
	}
}
#endif

void NORETURN QDECL Sys_Error( const char *error, ... )
{
	va_list argptr;
	char    string[1024];

	va_start (argptr,error);
	Q_vsnprintf (string, sizeof(string), error, argptr);
	va_end (argptr);

	Sys_Print( string );

	// Only print Sys_ErrorDialog for client binary. The dedicated
	// server binary is meant to be a command line program so you would
	// expect to see the error printed.
#if !defined(DEDICATED)
	Sys_ErrorDialog( string );
#endif

	Sys_Exit( 3 );
}

void NORETURN Sys_Quit (void) {
    Sys_Exit(0);
}

/*
============
Sys_FileTime

returns -1 if not present
============
*/
time_t Sys_FileTime( const char *path )
{
	struct stat buf;

	if ( stat( path, &buf ) == -1 )
		return -1;

	return buf.st_mtime;
}

/*
=================
Sys_SigHandler
=================
*/
void Sys_SigHandler( int signal )
{
	static qboolean signalcaught = qfalse;

	if( signalcaught )
	{
		fprintf( stderr, "DOUBLE SIGNAL FAULT: Received signal %d, exiting...\n",
			signal );
	}
	else
	{
		signalcaught = qtrue;
		//VM_Forced_Unload_Start();
#ifndef DEDICATED
		CL_Shutdown();
		//CL_Shutdown(va("Received signal %d", signal), qtrue, qtrue);
#endif
		SV_Shutdown(va("Received signal %d", signal) );
		//VM_Forced_Unload_Done();
	}

	if( signal == SIGTERM || signal == SIGINT )
		Sys_Exit( 1 );
	else
		Sys_Exit( 2 );
}

#ifdef MACOS_X
/*
 =================
 Sys_StripAppBundle

 Discovers if passed dir is suffixed with the directory structure of a Mac OS X
 .app bundle. If it is, the .app directory structure is stripped off the end and
 the result is returned. If not, dir is returned untouched.
 =================
 */
char *Sys_StripAppBundle( char *dir )
{
	static char cwd[MAX_OSPATH];

	Q_strncpyz(cwd, dir, sizeof(cwd));
	if(strcmp(Sys_Basename(cwd), "MacOS"))
		return dir;
	Q_strncpyz(cwd, Sys_Dirname(cwd), sizeof(cwd));
	if(strcmp(Sys_Basename(cwd), "Contents"))
		return dir;
	Q_strncpyz(cwd, Sys_Dirname(cwd), sizeof(cwd));
	if(!strstr(Sys_Basename(cwd), ".app"))
		return dir;
	Q_strncpyz(cwd, Sys_Dirname(cwd), sizeof(cwd));
	return cwd;
}
#endif

#ifndef DEFAULT_BASEDIR
#	ifdef MACOS_X
#		define DEFAULT_BASEDIR Sys_StripAppBundle(Sys_BinaryPath())
#	else
#		define DEFAULT_BASEDIR Sys_BinaryPath()
#	endif
#endif

int main ( int argc, char* argv[] )
{
	int		i;
	char	commandLine[ MAX_STRING_CHARS ] = { 0 };

	Sys_PlatformInit();
	CON_Init();

	// get the initial time base
	Sys_Milliseconds();

#ifdef MACOS_X
	// This is passed if we are launched by double-clicking
	if ( argc >= 2 && Q_strncmp ( argv[1], "-psn", 4 ) == 0 )
		argc = 1;
#endif

	Sys_SetBinaryPath( Sys_Dirname( argv[ 0 ] ) );
	Sys_SetDefaultInstallPath( DEFAULT_BASEDIR );

	// Concatenate the command line for passing to Com_Init
	for( i = 1; i < argc; i++ )
	{
		const bool containsSpaces = (strchr(argv[i], ' ') != NULL);
		if (containsSpaces)
			Q_strcat( commandLine, sizeof( commandLine ), "\"" );

		Q_strcat( commandLine, sizeof( commandLine ), argv[ i ] );

		if (containsSpaces)
			Q_strcat( commandLine, sizeof( commandLine ), "\"" );

		Q_strcat( commandLine, sizeof( commandLine ), " " );
	}

	Com_Init (commandLine);

	NET_Init();

	// main game loop
	while (1)
	{
		if ( com_busyWait->integer )
		{
			bool shouldSleep = false;

#if !defined(_JK2EXE)
			if ( com_dedicated->integer )
			{
				shouldSleep = true;
			}
#endif

			if ( com_minimized->integer )
			{
				shouldSleep = true;
			}

			if ( shouldSleep )
			{
				Sys_Sleep( 5 );
			}
		}

		// run the game
		Com_Frame();
	}

	// never gets here
	return 0;
}
