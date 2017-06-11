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

set(MPGameIncludeDirectories
	"${MPDir}"
	"${SharedDir}"
	"${GSLIncludeDirectory}"
	)
if(WIN32)
	set(MPGameLibraries "winmm")
endif(WIN32)
set(MPGameDefines ${MPSharedDefines})

set(MPGameGameFiles
	"${MPDir}/game/ai_main.cpp"
	"${MPDir}/game/ai_util.cpp"
	"${MPDir}/game/ai_wpnav.cpp"
	"${MPDir}/game/AnimalNPC.cpp"
	"${MPDir}/game/bg_g2_utils.cpp"
	"${MPDir}/game/bg_misc.cpp"
	"${MPDir}/game/bg_panimate.cpp"
	"${MPDir}/game/bg_pmove.cpp"
	"${MPDir}/game/bg_saber.cpp"
	"${MPDir}/game/bg_saberLoad.cpp"
	"${MPDir}/game/bg_saga.cpp"
	"${MPDir}/game/bg_slidemove.cpp"
	"${MPDir}/game/bg_vehicleLoad.cpp"
	"${MPDir}/game/bg_weapons.cpp"
	"${MPDir}/game/FighterNPC.cpp"
	"${MPDir}/game/g_active.cpp"
	"${MPDir}/game/g_bot.cpp"
	"${MPDir}/game/g_client.cpp"
	"${MPDir}/game/g_cmds.cpp"
	"${MPDir}/game/g_combat.cpp"
	"${MPDir}/game/g_cvar.cpp"
	"${MPDir}/game/g_exphysics.cpp"
	"${MPDir}/game/g_ICARUScb.cpp"
	"${MPDir}/game/g_items.cpp"
	"${MPDir}/game/g_log.cpp"
	"${MPDir}/game/g_main.cpp"
	"${MPDir}/game/g_mem.cpp"
	"${MPDir}/game/g_misc.cpp"
	"${MPDir}/game/g_missile.cpp"
	"${MPDir}/game/g_mover.cpp"
	"${MPDir}/game/g_nav.cpp"
	"${MPDir}/game/g_navnew.cpp"
	"${MPDir}/game/g_object.cpp"
	"${MPDir}/game/g_saga.cpp"
	"${MPDir}/game/g_session.cpp"
	"${MPDir}/game/g_spawn.cpp"
	"${MPDir}/game/g_svcmds.cpp"
	"${MPDir}/game/g_target.cpp"
	"${MPDir}/game/g_team.cpp"
	"${MPDir}/game/g_timer.cpp"
	"${MPDir}/game/g_trigger.cpp"
	"${MPDir}/game/g_turret.cpp"
	"${MPDir}/game/g_turret_G2.cpp"
	"${MPDir}/game/g_utils.cpp"
	"${MPDir}/game/g_vehicles.cpp"
	"${MPDir}/game/g_vehicleTurret.cpp"
	"${MPDir}/game/g_weapon.cpp"
	"${MPDir}/game/NPC.cpp"
	"${MPDir}/game/NPC_AI_Atst.cpp"
	"${MPDir}/game/NPC_AI_Default.cpp"
	"${MPDir}/game/NPC_AI_Droid.cpp"
	"${MPDir}/game/NPC_AI_GalakMech.cpp"
	"${MPDir}/game/NPC_AI_Grenadier.cpp"
	"${MPDir}/game/NPC_AI_Howler.cpp"
	"${MPDir}/game/NPC_AI_ImperialProbe.cpp"
	"${MPDir}/game/NPC_AI_Interrogator.cpp"
	"${MPDir}/game/NPC_AI_Jedi.cpp"
	"${MPDir}/game/NPC_AI_Mark1.cpp"
	"${MPDir}/game/NPC_AI_Mark2.cpp"
	"${MPDir}/game/NPC_AI_MineMonster.cpp"
	"${MPDir}/game/NPC_AI_Rancor.cpp"
	"${MPDir}/game/NPC_AI_Remote.cpp"
	"${MPDir}/game/NPC_AI_Seeker.cpp"
	"${MPDir}/game/NPC_AI_Sentry.cpp"
	"${MPDir}/game/NPC_AI_Sniper.cpp"
	"${MPDir}/game/NPC_AI_Stormtrooper.cpp"
	"${MPDir}/game/NPC_AI_Utils.cpp"
	"${MPDir}/game/NPC_AI_Wampa.cpp"
	"${MPDir}/game/NPC_behavior.cpp"
	"${MPDir}/game/NPC_combat.cpp"
	"${MPDir}/game/NPC_goal.cpp"
	"${MPDir}/game/NPC_misc.cpp"
	"${MPDir}/game/NPC_move.cpp"
	"${MPDir}/game/NPC_reactions.cpp"
	"${MPDir}/game/NPC_senses.cpp"
	"${MPDir}/game/NPC_sounds.cpp"
	"${MPDir}/game/NPC_spawn.cpp"
	"${MPDir}/game/NPC_stats.cpp"
	"${MPDir}/game/NPC_utils.cpp"
	"${MPDir}/game/SpeederNPC.cpp"
	"${MPDir}/game/tri_coll_test.cpp"
	"${MPDir}/game/w_force.cpp"
	"${MPDir}/game/w_saber.cpp"
	"${MPDir}/game/WalkerNPC.cpp"
	"${MPDir}/game/ai.h"
	"${MPDir}/game/ai_main.h"
	"${MPDir}/game/anims.h"
	"${MPDir}/game/b_local.h"
	"${MPDir}/game/b_public.h"
	"${MPDir}/game/bg_moduletype.h"
	"${MPDir}/game/bg_local.h"
	"${MPDir}/game/bg_public.h"
	"${MPDir}/game/bg_saga.h"
	"${MPDir}/game/bg_vehicles.h"
	"${MPDir}/game/bg_weapons.h"
	"${MPDir}/game/chars.h"
	"${MPDir}/game/g_ICARUScb.h"
	"${MPDir}/game/g_local.h"
	"${MPDir}/game/g_nav.h"
	"${MPDir}/game/g_public.h"
	"${MPDir}/game/g_team.h"
	"${MPDir}/game/g_xcvar.h"
	"${MPDir}/game/inv.h"
	"${MPDir}/game/match.h"
	"${MPDir}/game/npc_headers.h"
	"${MPDir}/game/say.h"
	"${MPDir}/game/surfaceflags.h"
	"${MPDir}/game/teams.h"
	"${MPDir}/game/w_saber.h"
	)
source_group("game" FILES ${MPGameGameFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameGameFiles})

add_library(${MPGame} STATIC ${MPGameFiles})

set_target_properties(${MPGame} PROPERTIES COMPILE_DEFINITIONS "${MPGameDefines}")

set_target_properties(${MPGame} PROPERTIES INCLUDE_DIRECTORIES "${MPGameIncludeDirectories}")
set_target_properties(${MPGame} PROPERTIES PROJECT_LABEL "MP Game Library")
# no libraries used
if(MPGameLibraries)
	target_link_libraries(${MPGame} ${MPGameLibraries})
endif(MPGameLibraries)
