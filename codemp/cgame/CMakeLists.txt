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

set(MPCGameIncludeDirectories
	"${MPDir}"
	"${SharedDir}"
	"${GSLIncludeDirectory}"
	)
if(WIN32)
	set(MPCGameLibraries "odbc32" "odbccp32") # what are these even?
endif(WIN32)
set(MPCGameDefines ${MPSharedDefines})

set(MPCGameCgameFiles
	"${MPDir}/cgame/cg_consolecmds.cpp"
	"${MPDir}/cgame/cg_cvar.cpp"
	"${MPDir}/cgame/cg_draw.cpp"
	"${MPDir}/cgame/cg_drawtools.cpp"
	"${MPDir}/cgame/cg_effects.cpp"
	"${MPDir}/cgame/cg_ents.cpp"
	"${MPDir}/cgame/cg_event.cpp"
	"${MPDir}/cgame/cg_info.cpp"
	"${MPDir}/cgame/cg_light.cpp"
	"${MPDir}/cgame/cg_localents.cpp"
	"${MPDir}/cgame/cg_main.cpp"
	"${MPDir}/cgame/cg_marks.cpp"
	"${MPDir}/cgame/cg_newDraw.cpp"
	"${MPDir}/cgame/cg_players.cpp"
	"${MPDir}/cgame/cg_playerstate.cpp"
	"${MPDir}/cgame/cg_predict.cpp"
	"${MPDir}/cgame/cg_saga.cpp"
	"${MPDir}/cgame/cg_scoreboard.cpp"
	"${MPDir}/cgame/cg_servercmds.cpp"
	"${MPDir}/cgame/cg_snapshot.cpp"
	"${MPDir}/cgame/cg_spawn.cpp"
	"${MPDir}/cgame/cg_turret.cpp"
	"${MPDir}/cgame/cg_view.cpp"
	"${MPDir}/cgame/cg_weaponinit.cpp"
	"${MPDir}/cgame/cg_weapons.cpp"
	"${MPDir}/cgame/fx_blaster.cpp"
	"${MPDir}/cgame/fx_bowcaster.cpp"
	"${MPDir}/cgame/fx_bryarpistol.cpp"
	"${MPDir}/cgame/fx_demp2.cpp"
	"${MPDir}/cgame/fx_disruptor.cpp"
	"${MPDir}/cgame/fx_flechette.cpp"
	"${MPDir}/cgame/fx_force.cpp"
	"${MPDir}/cgame/fx_heavyrepeater.cpp"
	"${MPDir}/cgame/fx_rocketlauncher.cpp"
	"${MPDir}/cgame/animtable.h"
	"${MPDir}/cgame/animtable.cpp"
	"${MPDir}/cgame/cg_local.h"
	"${MPDir}/cgame/cg_public.h"
	"${MPDir}/cgame/cg_xcvar.h"
	"${MPDir}/cgame/fx_local.h"
	)
source_group("cgame" FILES ${MPCGameCgameFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameCgameFiles})

add_library(${MPCGame} STATIC ${MPCGameFiles})

set_target_properties(${MPCGame} PROPERTIES COMPILE_DEFINITIONS "${MPCGameDefines}")

set_target_properties(${MPCGame} PROPERTIES INCLUDE_DIRECTORIES "${MPCGameIncludeDirectories}")
set_target_properties(${MPCGame} PROPERTIES PROJECT_LABEL "MP Client Game Library")
# no libraries used
if(MPCGameLibraries)
	target_link_libraries(${MPCGame} ${MPCGameLibraries})
endif(MPCGameLibraries)
