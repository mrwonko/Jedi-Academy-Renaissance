#============================================================================
# Copyright (C) 2013 - 2015, OpenJK contributors
#
# This file is part of the OpenJK source code.
#
# OpenJK is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
#============================================================================

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(MPUIIncludeDirectories
	"${MPDir}"
	"${SharedDir}"
	"${GSLIncludeDirectory}"
	)
if(WIN32)
	set(MPUILibraries "odbc32" "odbccp32") # what are these even?
endif(WIN32)

set(MPUIUIFiles
	"${MPDir}/ui/ui_atoms.cpp"
	"${MPDir}/ui/ui_cvar.cpp"
	"${MPDir}/ui/ui_force.cpp"
	"${MPDir}/ui/ui_gameinfo.cpp"
	"${MPDir}/ui/ui_main.cpp"
	"${MPDir}/ui/ui_saber.cpp"
	"${MPDir}/ui/ui_shared.cpp"
	"${MPDir}/ui/keycodes.h"
	"${MPDir}/ui/menudef.h"
	"${MPDir}/ui/ui_force.h"
	"${MPDir}/ui/ui_local.h"
	"${MPDir}/ui/ui_public.h"
	"${MPDir}/ui/ui_shared.h"
	"${MPDir}/ui/ui_xcvar.h"
	)
source_group("ui" FILES ${MPUIUIFiles})
set(MPUIFiles ${MPUIFiles} ${MPUIUIFiles})

add_library(${MPUI} STATIC ${MPUIFiles})

set_target_properties(${MPUI} PROPERTIES COMPILE_DEFINITIONS "${MPUIDefines}")

set_target_properties(${MPUI} PROPERTIES INCLUDE_DIRECTORIES "${MPUIIncludeDirectories}")
set_target_properties(${MPUI} PROPERTIES PROJECT_LABEL "MP UI Library")
# no libraries used
if(MPUILibraries)
	target_link_libraries(${MPUI} ${MPUILibraries})
endif(MPUILibraries)
