/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
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

// cg_main.c -- initialization and primary entry point for cgame
#include "cg_local.h"

#include "qcommon/qcommon.h"
#include "ui/ui_shared.h"
// display context for new ui stuff
displayContextDef_t cgDC;

extern int cgSiegeRoundState;
extern int cgSiegeRoundTime;
/*
Ghoul2 Insert Start
*/
void CG_InitItems(void);
/*
Ghoul2 Insert End
*/

void CG_InitJetpackGhoul2(void);
void CG_CleanJetpackGhoul2(void);

void CG_Init( int serverMessageNum, int serverCommandSequence, int clientNum );
void CG_Shutdown( void );

void CG_CalcEntityLerpPositions( centity_t *cent );
void CG_ROFF_NotetrackCallback( centity_t *cent, const char *notetrack);

void UI_CleanupGhoul2(void);

static int	C_PointContents(void);
static void C_GetLerpOrigin(void);
static void C_GetLerpData(void);
static void C_Trace(void);
static void C_G2Trace(void);
static void C_G2Mark(void);
static int	CG_RagCallback(int callType);
static void C_ImpactMark(void);

extern autoMapInput_t cg_autoMapInput; //cg_view.c
extern int cg_autoMapInputTime;
extern vec3_t cg_autoMapAngle;

void CG_MiscEnt(void);
void CG_DoCameraShake( vec3_t origin, float intensity, int radius, int time );

//do we have any force powers that we would normally need to cycle to?
qboolean CG_NoUseableForce(void)
{
	int i = FP_HEAL;
	while (i < NUM_FORCE_POWERS)
	{
		if (i != FP_SABERTHROW &&
			i != FP_SABER_OFFENSE &&
			i != FP_SABER_DEFENSE &&
			i != FP_LEVITATION)
		{ //valid selectable power
			if (cg.predictedPlayerState.fd.forcePowersKnown & (1 << i))
			{ //we have it
				return qfalse;
			}
		}
		i++;
	}

	//no useable force powers, I guess.
	return qtrue;
}

static int C_PointContents( void ) {
	TCGPointContents *data = &cg.sharedBuffer.pointContents;
	return CG_PointContents( data->mPoint, data->mPassEntityNum );
}

static void C_GetLerpOrigin( void ) {
	TCGVectorData *data = &cg.sharedBuffer.vectorData;
	VectorCopy( cg_entities[data->mEntityNum].lerpOrigin, data->mPoint );
}

// only used by FX system to pass to getboltmat
static void C_GetLerpData( void ) {
	TCGGetBoltData *data = &cg.sharedBuffer.getBoltData;

	VectorCopy( cg_entities[data->mEntityNum].lerpOrigin, data->mOrigin );
	VectorCopy( cg_entities[data->mEntityNum].modelScale, data->mScale );
	VectorCopy( cg_entities[data->mEntityNum].lerpAngles, data->mAngles );
	if ( cg_entities[data->mEntityNum].currentState.eType == ET_PLAYER ) {
		// normal player
		data->mAngles[PITCH] = 0.0f;
		data->mAngles[ROLL] = 0.0f;
	}
	else if ( cg_entities[data->mEntityNum].currentState.eType == ET_NPC ) {
		// an NPC
		Vehicle_t *pVeh = cg_entities[data->mEntityNum].m_pVehicle;
		if ( !pVeh ) {
			// for vehicles, we may or may not want to 0 out pitch and roll
			data->mAngles[PITCH] = 0.0f;
			data->mAngles[ROLL] = 0.0f;
		}
		else if ( pVeh->m_pVehicleInfo->type == VH_SPEEDER ) {
			// speeder wants no pitch but a roll
			data->mAngles[PITCH] = 0.0f;
		}
		else if ( pVeh->m_pVehicleInfo->type != VH_FIGHTER ) {
			// fighters want all angles
			data->mAngles[PITCH] = 0.0f;
			data->mAngles[ROLL] = 0.0f;
		}
	}
}

static void C_Trace( void ) {
	TCGTrace *td = &cg.sharedBuffer.trace;
	CG_Trace( &td->mResult, td->mStart, td->mMins, td->mMaxs, td->mEnd, td->mSkipNumber, td->mMask );
}

static void C_G2Trace( void ) {
	TCGTrace *td = &cg.sharedBuffer.trace;
	CG_G2Trace( &td->mResult, td->mStart, td->mMins, td->mMaxs, td->mEnd, td->mSkipNumber, td->mMask );
}

static void C_G2Mark( void ) {
	TCGG2Mark *td = &cg.sharedBuffer.g2Mark;
	trace_t tr;
	vec3_t end;

	VectorMA( td->start, 64.0f, td->dir, end );
	CG_G2Trace( &tr, td->start, NULL, NULL, end, ENTITYNUM_NONE, MASK_PLAYERSOLID );

	if ( tr.entityNum < ENTITYNUM_WORLD && cg_entities[tr.entityNum].ghoul2 ) {
		// hit someone with a ghoul2 instance, let's project the decal on them then.
		centity_t *cent = &cg_entities[tr.entityNum];

	//	CG_TestLine( tr.endpos, end, 2000, 0x0000ff, 1 );

		CG_AddGhoul2Mark( td->shader, td->size, tr.endpos, end, tr.entityNum, cent->lerpOrigin, cent->lerpAngles[YAW],
			cent->ghoul2, cent->modelScale, Q_irand( 2000, 4000 ) );
		// I'm making fx system decals have a very short lifetime.
	}
}

static void CG_DebugBoxLines( vec3_t mins, vec3_t maxs, int duration ) {
	vec3_t start, end, vert;
	float x = maxs[0] - mins[0];
	float y = maxs[1] - mins[1];

	start[2] = maxs[2];
	vert[2] = mins[2];

	vert[0] = mins[0];
	vert[1] = mins[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	vert[0] = mins[0];
	vert[1] = maxs[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	vert[0] = maxs[0];
	vert[1] = mins[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	vert[0] = maxs[0];
	vert[1] = maxs[1];
	start[0] = vert[0];
	start[1] = vert[1];
	CG_TestLine(start, vert, duration, 0x00000ff, 1);

	// top of box
	VectorCopy(maxs, start);
	VectorCopy(maxs, end);
	start[0] -= x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	end[0] = start[0];
	end[1] -= y;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	start[1] = end[1];
	start[0] += x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	CG_TestLine(start, maxs, duration, 0x00000ff, 1);
	// bottom of box
	VectorCopy(mins, start);
	VectorCopy(mins, end);
	start[0] += x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	end[0] = start[0];
	end[1] += y;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	start[1] = end[1];
	start[0] -= x;
	CG_TestLine(start, end, duration, 0x00000ff, 1);
	CG_TestLine(start, mins, duration, 0x00000ff, 1);
}

//handle ragdoll callbacks, for events and debugging -rww
static int CG_RagCallback(int callType)
{
	switch(callType)
	{
	case RAG_CALLBACK_DEBUGBOX:
		{
			ragCallbackDebugBox_t *callData = &cg.sharedBuffer.rcbDebugBox;

			CG_DebugBoxLines(callData->mins, callData->maxs, callData->duration);
		}
		break;
	case RAG_CALLBACK_DEBUGLINE:
		{
			ragCallbackDebugLine_t *callData = &cg.sharedBuffer.rcbDebugLine;

			CG_TestLine(callData->start, callData->end, callData->time, callData->color, callData->radius);
		}
		break;
	case RAG_CALLBACK_BONESNAP:
		{
			ragCallbackBoneSnap_t *callData = &cg.sharedBuffer.rcbBoneSnap;
			centity_t *cent = &cg_entities[callData->entNum];
			int snapSound = cg_trap->S_RegisterSound(va("sound/player/bodyfall_human%i.wav", Q_irand(1, 3)));

			cg_trap->S_StartSound(cent->lerpOrigin, callData->entNum, CHAN_AUTO, snapSound);
		}
	case RAG_CALLBACK_BONEIMPACT:
		break;
	case RAG_CALLBACK_BONEINSOLID:
#if 0
		{
			ragCallbackBoneInSolid_t *callData = &cg.sharedBuffer.rcbBoneInSolid;

			if (callData->solidCount > 16)
			{ //don't bother if we're just tapping into solidity, we'll probably recover on our own
				centity_t *cent = &cg_entities[callData->entNum];
				vec3_t slideDir;

				VectorSubtract(cent->lerpOrigin, callData->bonePos, slideDir);
				VectorAdd(cent->ragOffsets, slideDir, cent->ragOffsets);

				cent->hasRagOffset = qtrue;
			}
		}
#endif
		break;
	case RAG_CALLBACK_TRACELINE:
		{
			ragCallbackTraceLine_t *callData = &cg.sharedBuffer.rcbTraceLine;

			CG_Trace(&callData->tr, callData->start, callData->mins, callData->maxs,
				callData->end, callData->ignore, callData->mask);
		}
		break;
	default:
		Com_Error(ERR_DROP, "Invalid callType in CG_RagCallback");
		break;
	}

	return 0;
}

static void C_ImpactMark( void ) {
	TCGImpactMark *data = &cg.sharedBuffer.impactMark;

//	CG_ImpactMark( (int)arg0, (const float *)arg1, (const float *)arg2, (float)arg3, (float)arg4, (float)arg5, (float)arg6,
//		(float)arg7, qtrue, (float)arg8, qfalse );

	CG_ImpactMark( data->mHandle, data->mPoint, data->mAngle, data->mRotation, data->mRed, data->mGreen, data->mBlue,
		data->mAlphaStart, qtrue, data->mSizeStart, qfalse );
}

void CG_MiscEnt( void ) {
	int i, modelIndex;
	TCGMiscEnt *data = &cg.sharedBuffer.miscEnt;
	cg_staticmodel_t *staticmodel;

	if( cgs.numMiscStaticModels >= MAX_STATIC_MODELS ) {
		cg_trap->Error( ERR_DROP, "^1MAX_STATIC_MODELS(%i) hit", MAX_STATIC_MODELS );
	}

	modelIndex = cg_trap->R_RegisterModel(data->mModel);
	if (modelIndex == 0) {
		cg_trap->Error( ERR_DROP, "client_model failed to load model '%s'", data->mModel );
		return;
	}

	staticmodel = &cgs.miscStaticModels[cgs.numMiscStaticModels++];
	staticmodel->model = modelIndex;
	AnglesToAxis( data->mAngles, staticmodel->axes );
	for ( i = 0; i < 3; i++ ) {
		VectorScale( staticmodel->axes[i], data->mScale[i], staticmodel->axes[i] );
	}

	VectorCopy( data->mOrigin, staticmodel->org );
	staticmodel->zoffset = 0.f;

	if( staticmodel->model ) {
		vec3_t mins, maxs;

		cg_trap->R_ModelBounds( staticmodel->model, mins, maxs );

		VectorScaleVector(mins, data->mScale, mins);
		VectorScaleVector(maxs, data->mScale, maxs);

		staticmodel->radius = RadiusFromBounds( mins, maxs );
	} else {
		staticmodel->radius = 0;
	}
}

/*
Ghoul2 Insert Start
*/
/*
void CG_ResizeG2Bolt(boltInfo_v *bolt, int newCount)
{
	bolt->resize(newCount);
}

void CG_ResizeG2Surface(surfaceInfo_v *surface, int newCount)
{
	surface->resize(newCount);
}

void CG_ResizeG2Bone(boneInfo_v *bone, int newCount)
{
	bone->resize(newCount);
}

void CG_ResizeG2(CGhoul2Info_v *ghoul2, int newCount)
{
	ghoul2->resize(newCount);
}

void CG_ResizeG2TempBone(mdxaBone_v *tempBone, int newCount)
{
	tempBone->resize(newCount);
}
*/
/*
Ghoul2 Insert End
*/
cg_t				cg;
cgs_t				cgs;
centity_t			cg_entities[MAX_GENTITIES];

centity_t			*cg_permanents[MAX_GENTITIES]; //rwwRMG - added
int					cg_numpermanents = 0;

weaponInfo_t		cg_weapons[MAX_WEAPONS];
itemInfo_t			cg_items[MAX_ITEMS];

int CG_CrosshairPlayer( void ) {
	if ( cg.time > (cg.crosshairClientTime + 1000) )
		return -1;

	if ( cg.crosshairClientNum >= MAX_CLIENTS )
		return -1;

	return cg.crosshairClientNum;
}

int CG_LastAttacker( void ) {
	if ( !cg.attackerTime )
		return -1;

	return cg.snap->ps.persistant[PERS_ATTACKER];
}

/*
================
CG_Argv
================
*/
const char *CG_Argv( int arg ) {
	static char	buffer[MAX_STRING_CHARS] = {0};

	cg_trap->Cmd_Argv( arg, buffer, sizeof( buffer ) );

	return buffer;
}


//========================================================================

/*
=================
CG_RegisterItemSounds

The server says this item is used on this level
=================
*/
static void CG_RegisterItemSounds( int itemNum ) {
	gitem_t			*item;
	char			data[MAX_QPATH];
	char			*s, *start;
	int				len;

	item = &bg_itemlist[ itemNum ];

	if( item->pickup_sound ) {
		cg_trap->S_RegisterSound( item->pickup_sound );
	}

	// parse the space seperated precache string for other media
	s = item->sounds;
	if (!s || !s[0])
		return;

	while (*s) {
		start = s;
		while (*s && *s != ' ') {
			s++;
		}

		len = s-start;
		if (len >= MAX_QPATH || len < 5) {
			cg_trap->Error( ERR_DROP, "PrecacheItem: %s has bad precache string",
				item->classname);
			return;
		}
		memcpy (data, start, len);
		data[len] = 0;
		if ( *s ) {
			s++;
		}

		cg_trap->S_RegisterSound( data );
	}

	// parse the space seperated precache string for other media
	s = item->precaches;
	if (!s || !s[0])
		return;

	while (*s) {
		start = s;
		while (*s && *s != ' ') {
			s++;
		}

		len = s-start;
		if (len >= MAX_QPATH || len < 5) {
			cg_trap->Error( ERR_DROP, "PrecacheItem: %s has bad precache string",
				item->classname);
			return;
		}
		memcpy (data, start, len);
		data[len] = 0;
		if ( *s ) {
			s++;
		}

		if ( !strcmp(data+len-3, "efx" )) {
			cg_trap->FX_RegisterEffect( data );
		}
	}
}

static void CG_AS_Register(void)
{
	const char *soundName;
	int i;

//	CG_LoadingString( "ambient sound sets" );

	//Load the ambient sets
#if 0 //as_preCacheMap was game-side.. that is evil.
	cg_trap->AS_AddPrecacheEntry( "#clear" );	// ;-)
	//FIXME: Don't ask... I had to get around a really nasty MS error in the templates with this...
	namePrecache_m::iterator	pi;
	STL_ITERATE( pi, as_preCacheMap )
	{
		cgi_AS_AddPrecacheEntry( ((*pi).first).c_str() );
	}
#else
	cg_trap->AS_AddPrecacheEntry( "#clear" );

	for ( i = 1 ; i < MAX_AMBIENT_SETS ; i++ ) {
		soundName = CG_ConfigString( CS_AMBIENT_SET+i );
		if ( !soundName || !soundName[0] )
		{
			break;
		}

		cg_trap->AS_AddPrecacheEntry(soundName);
	}
	soundName = CG_ConfigString( CS_GLOBAL_AMBIENT_SET );
	if (soundName && soundName[0] && Q_stricmp(soundName, "default"))
	{ //global soundset
		cg_trap->AS_AddPrecacheEntry(soundName);
	}
#endif

	cg_trap->AS_ParseSets();
}

//a global weather effect (rain, snow, etc)
void CG_ParseWeatherEffect(const char *str)
{
	char *sptr = (char *)str;
	sptr++; //pass the '*'
	cg_trap->R_WorldEffectCommand(sptr);
}

extern int cgSiegeRoundBeganTime;
void CG_ParseSiegeState(const char *str)
{
	int i = 0;
	int j = 0;
//	int prevState = cgSiegeRoundState;
	char b[1024];

	while (str[i] && str[i] != '|')
	{
		b[j] = str[i];
		i++;
		j++;
	}
	b[j] = 0;
	cgSiegeRoundState = atoi(b);

	if (str[i] == '|')
	{
		j = 0;
		i++;
		while (str[i])
		{
			b[j] = str[i];
			i++;
			j++;
		}
		b[j] = 0;
//		if (cgSiegeRoundState != prevState)
		{ //it changed
			cgSiegeRoundTime = atoi(b);
			if (cgSiegeRoundState == 0 || cgSiegeRoundState == 2)
			{
				cgSiegeRoundBeganTime = cgSiegeRoundTime;
			}
		}
	}
	else
	{
	    cgSiegeRoundTime = cg.time;
	}
}

/*
=================
CG_RegisterSounds

called during a precache command
=================
*/
void CG_PrecacheNPCSounds(const char *str);
void CG_ParseSiegeObjectiveStatus(const char *str);
extern int cg_beatingSiegeTime;
extern int cg_siegeWinTeam;
static void CG_RegisterSounds( void ) {
	int		i;
	char	items[MAX_ITEMS+1];
	char	name[MAX_QPATH];
	const char	*soundName;

	CG_AS_Register();

//	CG_LoadingString( "sounds" );

	cg_trap->S_RegisterSound( "sound/weapons/melee/punch1.mp3" );
	cg_trap->S_RegisterSound( "sound/weapons/melee/punch2.mp3" );
	cg_trap->S_RegisterSound( "sound/weapons/melee/punch3.mp3" );
	cg_trap->S_RegisterSound( "sound/weapons/melee/punch4.mp3" );
	cg_trap->S_RegisterSound("sound/movers/objects/saber_slam");

	cg_trap->S_RegisterSound("sound/player/bodyfall_human1.wav");
	cg_trap->S_RegisterSound("sound/player/bodyfall_human2.wav");
	cg_trap->S_RegisterSound("sound/player/bodyfall_human3.wav");

	//test effects
	cg_trap->FX_RegisterEffect("effects/mp/test_sparks.efx");
	cg_trap->FX_RegisterEffect("effects/mp/test_wall_impact.efx");

	cgs.media.oneMinuteSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM004" );
	cgs.media.fiveMinuteSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM005" );
	cgs.media.oneFragSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM001" );
	cgs.media.twoFragSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM002" );
	cgs.media.threeFragSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM003");
	cgs.media.count3Sound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM035" );
	cgs.media.count2Sound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM036" );
	cgs.media.count1Sound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM037" );
	cgs.media.countFightSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM038" );

	cgs.media.hackerIconShader			= cg_trap->R_RegisterShaderNoMip("gfx/mp/c_icon_tech");

	cgs.media.redSaberGlowShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/red_glow" );
	cgs.media.redSaberCoreShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/red_line" );
	cgs.media.orangeSaberGlowShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/orange_glow" );
	cgs.media.orangeSaberCoreShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/orange_line" );
	cgs.media.yellowSaberGlowShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/yellow_glow" );
	cgs.media.yellowSaberCoreShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/yellow_line" );
	cgs.media.greenSaberGlowShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/green_glow" );
	cgs.media.greenSaberCoreShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/green_line" );
	cgs.media.blueSaberGlowShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/blue_glow" );
	cgs.media.blueSaberCoreShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/blue_line" );
	cgs.media.purpleSaberGlowShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/purple_glow" );
	cgs.media.purpleSaberCoreShader		= cg_trap->R_RegisterShader( "gfx/effects/sabers/purple_line" );
	cgs.media.saberBlurShader			= cg_trap->R_RegisterShader( "gfx/effects/sabers/saberBlur" );
	cgs.media.swordTrailShader			= cg_trap->R_RegisterShader( "gfx/effects/sabers/swordTrail" );

	cgs.media.forceCoronaShader			= cg_trap->R_RegisterShaderNoMip( "gfx/hud/force_swirl" );

	cgs.media.yellowDroppedSaberShader	= cg_trap->R_RegisterShader("gfx/effects/yellow_glow");

	cgs.media.rivetMarkShader			= cg_trap->R_RegisterShader( "gfx/damage/rivetmark" );

	cg_trap->R_RegisterShader( "gfx/effects/saberFlare" );

	cg_trap->R_RegisterShader( "powerups/ysalimarishell" );

	cg_trap->R_RegisterShader( "gfx/effects/forcePush" );

	cg_trap->R_RegisterShader( "gfx/misc/red_dmgshield" );
	cg_trap->R_RegisterShader( "gfx/misc/red_portashield" );
	cg_trap->R_RegisterShader( "gfx/misc/blue_dmgshield" );
	cg_trap->R_RegisterShader( "gfx/misc/blue_portashield" );

	cg_trap->R_RegisterShader( "models/map_objects/imp_mine/turret_chair_dmg.tga" );

	for (i=1 ; i<9 ; i++)
	{
		cg_trap->S_RegisterSound(va("sound/weapons/saber/saberhup%i.wav", i));
	}

	for (i=1 ; i<10 ; i++)
	{
		cg_trap->S_RegisterSound(va("sound/weapons/saber/saberblock%i.wav", i));
	}

	for (i=1 ; i<4 ; i++)
	{
		cg_trap->S_RegisterSound(va("sound/weapons/saber/bounce%i.wav", i));
	}

	cg_trap->S_RegisterSound( "sound/weapons/saber/enemy_saber_on.wav" );
	cg_trap->S_RegisterSound( "sound/weapons/saber/enemy_saber_off.wav" );

	cg_trap->S_RegisterSound( "sound/weapons/saber/saberhum1.wav" );
	cg_trap->S_RegisterSound( "sound/weapons/saber/saberon.wav" );
	cg_trap->S_RegisterSound( "sound/weapons/saber/saberoffquick.wav" );
	cg_trap->S_RegisterSound( "sound/weapons/saber/saberhitwall1" );
	cg_trap->S_RegisterSound( "sound/weapons/saber/saberhitwall2" );
	cg_trap->S_RegisterSound( "sound/weapons/saber/saberhitwall3" );
	cg_trap->S_RegisterSound("sound/weapons/saber/saberhit.wav");
	cg_trap->S_RegisterSound("sound/weapons/saber/saberhit1.wav");
	cg_trap->S_RegisterSound("sound/weapons/saber/saberhit2.wav");
	cg_trap->S_RegisterSound("sound/weapons/saber/saberhit3.wav");

	cg_trap->S_RegisterSound("sound/weapons/saber/saber_catch.wav");

	cgs.media.teamHealSound = cg_trap->S_RegisterSound("sound/weapons/force/teamheal.wav");
	cgs.media.teamRegenSound = cg_trap->S_RegisterSound("sound/weapons/force/teamforce.wav");

	cg_trap->S_RegisterSound("sound/weapons/force/heal.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/speed.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/see.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/rage.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/lightning");
	cg_trap->S_RegisterSound("sound/weapons/force/lightninghit1");
	cg_trap->S_RegisterSound("sound/weapons/force/lightninghit2");
	cg_trap->S_RegisterSound("sound/weapons/force/lightninghit3");
	cg_trap->S_RegisterSound("sound/weapons/force/drain.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/jumpbuild.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/distract.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/distractstop.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/pull.wav");
	cg_trap->S_RegisterSound("sound/weapons/force/push.wav");

	for (i=1 ; i<3 ; i++)
	{
		cg_trap->S_RegisterSound(va("sound/weapons/thermal/bounce%i.wav", i));
	}

	cg_trap->S_RegisterSound("sound/movers/switches/switch2.wav");
	cg_trap->S_RegisterSound("sound/movers/switches/switch3.wav");
	cg_trap->S_RegisterSound("sound/ambience/spark5.wav");
	cg_trap->S_RegisterSound("sound/chars/turret/ping.wav");
	cg_trap->S_RegisterSound("sound/chars/turret/startup.wav");
	cg_trap->S_RegisterSound("sound/chars/turret/shutdown.wav");
	cg_trap->S_RegisterSound("sound/chars/turret/move.wav");
	cg_trap->S_RegisterSound("sound/player/pickuphealth.wav");
	cg_trap->S_RegisterSound("sound/player/pickupshield.wav");

	cg_trap->S_RegisterSound("sound/effects/glassbreak1.wav");

	cg_trap->S_RegisterSound( "sound/weapons/rocket/tick.wav" );
	cg_trap->S_RegisterSound( "sound/weapons/rocket/lock.wav" );

	cg_trap->S_RegisterSound("sound/weapons/force/speedloop.wav");

	cg_trap->S_RegisterSound("sound/weapons/force/protecthit.mp3"); //PDSOUND_PROTECTHIT
	cg_trap->S_RegisterSound("sound/weapons/force/protect.mp3"); //PDSOUND_PROTECT
	cg_trap->S_RegisterSound("sound/weapons/force/absorbhit.mp3"); //PDSOUND_ABSORBHIT
	cg_trap->S_RegisterSound("sound/weapons/force/absorb.mp3"); //PDSOUND_ABSORB
	cg_trap->S_RegisterSound("sound/weapons/force/jump.mp3"); //PDSOUND_FORCEJUMP
	cg_trap->S_RegisterSound("sound/weapons/force/grip.mp3"); //PDSOUND_FORCEGRIP

	if ( cgs.gametype >= GT_TEAM || com_buildScript->integer ) {

#ifdef JK2AWARDS
		cgs.media.captureAwardSound = cg_trap->S_RegisterSound( "sound/teamplay/flagcapture_yourteam.wav" );
#endif
		cgs.media.redLeadsSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM046");
		cgs.media.blueLeadsSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM045");
		cgs.media.teamsTiedSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM032" );

		cgs.media.redScoredSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM044");
		cgs.media.blueScoredSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM043" );

		if ( cgs.gametype == GT_CTF || com_buildScript->integer ) {
			cgs.media.redFlagReturnedSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM042" );
			cgs.media.blueFlagReturnedSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM041" );
			cgs.media.redTookFlagSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM040" );
			cgs.media.blueTookFlagSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM039" );
		}
		if ( cgs.gametype == GT_CTY /*|| com_buildScript->integer*/ ) {
			cgs.media.redYsalReturnedSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM050" );
			cgs.media.blueYsalReturnedSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM049" );
			cgs.media.redTookYsalSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM048" );
			cgs.media.blueTookYsalSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM047" );
		}
	}

	cgs.media.drainSound = cg_trap->S_RegisterSound("sound/weapons/force/drained.mp3");

	cgs.media.happyMusic = cg_trap->S_RegisterSound("music/goodsmall.mp3");
	cgs.media.dramaticFailure = cg_trap->S_RegisterSound("music/badsmall.mp3");

	//PRECACHE ALL MUSIC HERE (don't need to precache normally because it's streamed off the disk)
	if (com_buildScript->integer)
	{
		cg_trap->S_StartBackgroundTrack( "music/mp/duel.mp3", "music/mp/duel.mp3", qfalse );
	}

	cg.loadLCARSStage = 1;

	cgs.media.selectSound = cg_trap->S_RegisterSound( "sound/weapons/change.wav" );

	cgs.media.teleInSound = cg_trap->S_RegisterSound( "sound/player/telein.wav" );
	cgs.media.teleOutSound = cg_trap->S_RegisterSound( "sound/player/teleout.wav" );
	cgs.media.respawnSound = cg_trap->S_RegisterSound( "sound/items/respawn1.wav" );

	cg_trap->S_RegisterSound( "sound/movers/objects/objectHit.wav" );

	cgs.media.talkSound = cg_trap->S_RegisterSound( "sound/player/talk.wav" );
	cgs.media.landSound = cg_trap->S_RegisterSound( "sound/player/land1.wav");
	cgs.media.fallSound = cg_trap->S_RegisterSound( "sound/player/fallsplat.wav");

	cgs.media.crackleSound = cg_trap->S_RegisterSound( "sound/effects/energy_crackle.wav" );
#ifdef JK2AWARDS
	cgs.media.impressiveSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM025" );
	cgs.media.excellentSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM053" );
	cgs.media.deniedSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM017" );
	cgs.media.humiliationSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM019" );
	cgs.media.defendSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM024" );
#endif

	/*
	cgs.media.takenLeadSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM051");
	cgs.media.tiedLeadSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM032");
	cgs.media.lostLeadSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM052");
	*/

	cgs.media.rollSound					= cg_trap->S_RegisterSound( "sound/player/roll1.wav");

	cgs.media.noforceSound				= cg_trap->S_RegisterSound( "sound/weapons/force/noforce" );

	cgs.media.watrInSound				= cg_trap->S_RegisterSound( "sound/player/watr_in.wav");
	cgs.media.watrOutSound				= cg_trap->S_RegisterSound( "sound/player/watr_out.wav");
	cgs.media.watrUnSound				= cg_trap->S_RegisterSound( "sound/player/watr_un.wav");

	cgs.media.explosionModel			= cg_trap->R_RegisterModel ( "models/map_objects/mp/sphere.md3" );
	cgs.media.surfaceExplosionShader	= cg_trap->R_RegisterShader( "surfaceExplosion" );

	cgs.media.disruptorShader			= cg_trap->R_RegisterShader( "gfx/effects/burn");

	if (com_buildScript->integer)
	{
		cg_trap->R_RegisterShader( "gfx/effects/turretflashdie" );
	}

	cgs.media.solidWhite = cg_trap->R_RegisterShader( "gfx/effects/solidWhite_cull" );

	cg_trap->R_RegisterShader("gfx/misc/mp_light_enlight_disable");
	cg_trap->R_RegisterShader("gfx/misc/mp_dark_enlight_disable");

	cg_trap->R_RegisterModel ( "models/map_objects/mp/sphere.md3" );
	cg_trap->R_RegisterModel("models/items/remote.md3");

	cgs.media.holocronPickup = cg_trap->S_RegisterSound( "sound/player/holocron.wav" );

	// Zoom
	cgs.media.zoomStart = cg_trap->S_RegisterSound( "sound/interface/zoomstart.wav" );
	cgs.media.zoomLoop	= cg_trap->S_RegisterSound( "sound/interface/zoomloop.wav" );
	cgs.media.zoomEnd	= cg_trap->S_RegisterSound( "sound/interface/zoomend.wav" );

	for (i=0 ; i<4 ; i++) {
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/stone_step%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_STONEWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/stone_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_STONERUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/metal_step%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_METALWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/metal_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_METALRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/pipe_step%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_PIPEWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/pipe_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_PIPERUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/water_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_SPLASH][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/water_walk%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_WADE][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/water_wade_0%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_SWIM][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/snow_step%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_SNOWWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/snow_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_SNOWRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/sand_walk%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_SANDWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/sand_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_SANDRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/grass_step%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_GRASSWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/grass_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_GRASSRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/dirt_step%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_DIRTWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/dirt_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_DIRTRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/mud_walk%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_MUDWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/mud_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_MUDRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/gravel_walk%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_GRAVELWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/gravel_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_GRAVELRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/rug_step%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_RUGWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/rug_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_RUGRUN][i] = cg_trap->S_RegisterSound (name);

		Com_sprintf (name, sizeof(name), "sound/player/footsteps/wood_walk%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_WOODWALK][i] = cg_trap->S_RegisterSound (name);
		Com_sprintf (name, sizeof(name), "sound/player/footsteps/wood_run%i.wav", i+1);
		cgs.media.footsteps[FOOTSTEP_WOODRUN][i] = cg_trap->S_RegisterSound (name);
	}

	// only register the items that the server says we need
	Q_strncpyz(items, CG_ConfigString(CS_ITEMS), sizeof(items));

	for ( i = 1 ; i < bg_numItems ; i++ ) {
		if ( items[ i ] == '1' || com_buildScript->integer ) {
			CG_RegisterItemSounds( i );
		}
	}

	for ( i = 1 ; i < MAX_SOUNDS ; i++ ) {
		soundName = CG_ConfigString( CS_SOUNDS+i );
		if ( !soundName[0] ) {
			break;
		}
		if ( soundName[0] == '*' )
		{
			if (soundName[1] == '$')
			{ //an NPC soundset
				CG_PrecacheNPCSounds(soundName);
			}
			continue;	// custom sound
		}
		cgs.gameSounds[i] = cg_trap->S_RegisterSound( soundName );
	}

	for ( i = 1 ; i < MAX_FX ; i++ ) {
		soundName = CG_ConfigString( CS_EFFECTS+i );
		if ( !soundName[0] ) {
			break;
		}

		if (soundName[0] == '*')
		{ //it's a special global weather effect
			CG_ParseWeatherEffect(soundName);
			cgs.gameEffects[i] = 0;
		}
		else
		{
			cgs.gameEffects[i] = cg_trap->FX_RegisterEffect( soundName );
		}
	}

	// register all the server specified icons
	for ( i = 1; i < MAX_ICONS; i ++ )
	{
		const char* iconName;

		iconName = CG_ConfigString ( CS_ICONS + i );
		if ( !iconName[0] )
		{
			break;
		}

		cgs.gameIcons[i] = cg_trap->R_RegisterShaderNoMip ( iconName );
	}

	soundName = CG_ConfigString(CS_SIEGE_STATE);

	if (soundName[0])
	{
		CG_ParseSiegeState(soundName);
	}

	soundName = CG_ConfigString(CS_SIEGE_WINTEAM);

	if (soundName[0])
	{
		cg_siegeWinTeam = atoi(soundName);
	}

	if (cgs.gametype == GT_SIEGE)
	{
		CG_ParseSiegeObjectiveStatus(CG_ConfigString(CS_SIEGE_OBJECTIVES));
		cg_beatingSiegeTime = atoi(CG_ConfigString(CS_SIEGE_TIMEOVERRIDE));
		if ( cg_beatingSiegeTime )
		{
			CG_SetSiegeTimerCvar ( cg_beatingSiegeTime );
		}
	}

	cg.loadLCARSStage = 2;

	// FIXME: only needed with item
	cgs.media.deploySeeker = cg_trap->S_RegisterSound ("sound/chars/seeker/misc/hiss");
	cgs.media.medkitSound = cg_trap->S_RegisterSound ("sound/items/use_bacta.wav");

	cgs.media.winnerSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM006" );
	cgs.media.loserSound = cg_trap->S_RegisterSound( "sound/chars/protocol/misc/40MOM010" );
}


//-------------------------------------
// CG_RegisterEffects
//
// Handles precaching all effect files
//	and any shader, model, or sound
//	files an effect may use.
//-------------------------------------
static void CG_RegisterEffects( void )
{
	/*
	const char	*effectName;
	int			i;

	for ( i = 1 ; i < MAX_FX ; i++ )
	{
		effectName = CG_ConfigString( CS_EFFECTS + i );

		if ( !effectName[0] )
		{
			break;
		}

		cg_trap->FX_RegisterEffect( effectName );
	}
	*/
	//the above was redundant as it's being done in CG_RegisterSounds

	// Set up the glass effects mini-system.
	CG_InitGlass();

	//footstep effects
	cgs.effects.footstepMud = cg_trap->FX_RegisterEffect( "materials/mud" );
	cgs.effects.footstepSand = cg_trap->FX_RegisterEffect( "materials/sand" );
	cgs.effects.footstepSnow = cg_trap->FX_RegisterEffect( "materials/snow" );
	cgs.effects.footstepGravel = cg_trap->FX_RegisterEffect( "materials/gravel" );
	//landing effects
	cgs.effects.landingMud = cg_trap->FX_RegisterEffect( "materials/mud_large" );
	cgs.effects.landingSand = cg_trap->FX_RegisterEffect( "materials/sand_large" );
	cgs.effects.landingDirt = cg_trap->FX_RegisterEffect( "materials/dirt_large" );
	cgs.effects.landingSnow = cg_trap->FX_RegisterEffect( "materials/snow_large" );
	cgs.effects.landingGravel = cg_trap->FX_RegisterEffect( "materials/gravel_large" );
	//splashes
	cgs.effects.waterSplash = cg_trap->FX_RegisterEffect( "env/water_impact" );
	cgs.effects.lavaSplash = cg_trap->FX_RegisterEffect( "env/lava_splash" );
	cgs.effects.acidSplash = cg_trap->FX_RegisterEffect( "env/acid_splash" );
}

//===================================================================================

extern char *forceHolocronModels[];
int CG_HandleAppendedSkin(char *modelName);
void CG_CacheG2AnimInfo(char *modelName);
/*
=================
CG_RegisterGraphics

This function may execute for a couple of minutes with a slow disk.
=================
*/
static void CG_RegisterGraphics( void ) {
	int			i;
	int			breakPoint;
	char		items[MAX_ITEMS+1];

	static char		*sb_nums[11] = {
		"gfx/2d/numbers/zero",
		"gfx/2d/numbers/one",
		"gfx/2d/numbers/two",
		"gfx/2d/numbers/three",
		"gfx/2d/numbers/four",
		"gfx/2d/numbers/five",
		"gfx/2d/numbers/six",
		"gfx/2d/numbers/seven",
		"gfx/2d/numbers/eight",
		"gfx/2d/numbers/nine",
		"gfx/2d/numbers/minus",
	};

	static char		*sb_t_nums[11] = {
		"gfx/2d/numbers/t_zero",
		"gfx/2d/numbers/t_one",
		"gfx/2d/numbers/t_two",
		"gfx/2d/numbers/t_three",
		"gfx/2d/numbers/t_four",
		"gfx/2d/numbers/t_five",
		"gfx/2d/numbers/t_six",
		"gfx/2d/numbers/t_seven",
		"gfx/2d/numbers/t_eight",
		"gfx/2d/numbers/t_nine",
		"gfx/2d/numbers/t_minus",
	};

	static char		*sb_c_nums[11] = {
		"gfx/2d/numbers/c_zero",
		"gfx/2d/numbers/c_one",
		"gfx/2d/numbers/c_two",
		"gfx/2d/numbers/c_three",
		"gfx/2d/numbers/c_four",
		"gfx/2d/numbers/c_five",
		"gfx/2d/numbers/c_six",
		"gfx/2d/numbers/c_seven",
		"gfx/2d/numbers/c_eight",
		"gfx/2d/numbers/c_nine",
		"gfx/2d/numbers/t_minus", //?????
	};

	// clear any references to old media
	memset( &cg.refdef, 0, sizeof( cg.refdef ) );
	cg_trap->R_ClearScene();

	CG_LoadingString( cgs.mapname );

	cg_trap->R_LoadWorld( cgs.mapname );

	// precache status bar pics
//	CG_LoadingString( "game media" );

	for ( i=0 ; i<11 ; i++) {
		cgs.media.numberShaders[i] = cg_trap->R_RegisterShader( sb_nums[i] );
	}

	cg.loadLCARSStage = 3;

	for ( i=0; i < 11; i++ )
	{
		cgs.media.numberShaders[i]			= cg_trap->R_RegisterShaderNoMip( sb_nums[i] );
		cgs.media.smallnumberShaders[i]		= cg_trap->R_RegisterShaderNoMip( sb_t_nums[i] );
		cgs.media.chunkyNumberShaders[i]	= cg_trap->R_RegisterShaderNoMip( sb_c_nums[i] );
	}

	cg_trap->R_RegisterShaderNoMip ( "gfx/mp/pduel_icon_lone" );
	cg_trap->R_RegisterShaderNoMip ( "gfx/mp/pduel_icon_double" );

	cgs.media.balloonShader = cg_trap->R_RegisterShader( "gfx/mp/chat_icon" );
	cgs.media.vchatShader = cg_trap->R_RegisterShader( "gfx/mp/vchat_icon" );

	cgs.media.deferShader = cg_trap->R_RegisterShaderNoMip( "gfx/2d/defer.tga" );

	cgs.media.radarShader			= cg_trap->R_RegisterShaderNoMip ( "gfx/menus/radar/radar.png" );
	cgs.media.siegeItemShader		= cg_trap->R_RegisterShaderNoMip ( "gfx/menus/radar/goalitem" );
	cgs.media.mAutomapPlayerIcon	= cg_trap->R_RegisterShader( "gfx/menus/radar/arrow_w" );
	cgs.media.mAutomapRocketIcon	= cg_trap->R_RegisterShader( "gfx/menus/radar/rocket" );

	cgs.media.wireframeAutomapFrame_left = cg_trap->R_RegisterShader( "gfx/mp_automap/mpauto_frame_left" );
	cgs.media.wireframeAutomapFrame_right = cg_trap->R_RegisterShader( "gfx/mp_automap/mpauto_frame_right" );
	cgs.media.wireframeAutomapFrame_top = cg_trap->R_RegisterShader( "gfx/mp_automap/mpauto_frame_top" );
	cgs.media.wireframeAutomapFrame_bottom = cg_trap->R_RegisterShader( "gfx/mp_automap/mpauto_frame_bottom" );

	cgs.media.lagometerShader = cg_trap->R_RegisterShaderNoMip("gfx/2d/lag" );
	cgs.media.connectionShader = cg_trap->R_RegisterShaderNoMip( "gfx/2d/net" );

	cg_trap->FX_InitSystem(&cg.refdef);
	CG_RegisterEffects();

	cgs.media.boltShader = cg_trap->R_RegisterShader( "gfx/misc/blueLine" );

	cgs.effects.turretShotEffect = cg_trap->FX_RegisterEffect( "turret/shot" );
	cgs.effects.mEmplacedDeadSmoke = cg_trap->FX_RegisterEffect("emplaced/dead_smoke.efx");
	cgs.effects.mEmplacedExplode = cg_trap->FX_RegisterEffect("emplaced/explode.efx");
	cgs.effects.mTurretExplode = cg_trap->FX_RegisterEffect("turret/explode.efx");
	cgs.effects.mSparkExplosion = cg_trap->FX_RegisterEffect("sparks/spark_explosion.efx");
	cgs.effects.mTripmineExplosion = cg_trap->FX_RegisterEffect("tripMine/explosion.efx");
	cgs.effects.mDetpackExplosion = cg_trap->FX_RegisterEffect("detpack/explosion.efx");
	cgs.effects.mFlechetteAltBlow = cg_trap->FX_RegisterEffect("flechette/alt_blow.efx");
	cgs.effects.mStunBatonFleshImpact = cg_trap->FX_RegisterEffect("stunBaton/flesh_impact.efx");
	cgs.effects.mAltDetonate = cg_trap->FX_RegisterEffect("demp2/altDetonate.efx");
	cgs.effects.mSparksExplodeNoSound = cg_trap->FX_RegisterEffect("sparks/spark_exp_nosnd");
	cgs.effects.mTripMineLaser = cg_trap->FX_RegisterEffect("tripMine/laser.efx");
	cgs.effects.mEmplacedMuzzleFlash = cg_trap->FX_RegisterEffect( "effects/emplaced/muzzle_flash" );
	cgs.effects.mConcussionAltRing = cg_trap->FX_RegisterEffect("concussion/alt_ring");

	cgs.effects.mHyperspaceStars = cg_trap->FX_RegisterEffect("ships/hyperspace_stars");
	cgs.effects.mBlackSmoke = cg_trap->FX_RegisterEffect( "volumetric/black_smoke" );
	cgs.effects.mShipDestDestroyed = cg_trap->FX_RegisterEffect("effects/ships/dest_destroyed.efx");
	cgs.effects.mShipDestBurning = cg_trap->FX_RegisterEffect("effects/ships/dest_burning.efx");
	cgs.effects.mBobaJet = cg_trap->FX_RegisterEffect("effects/boba/jet.efx");


	cgs.effects.itemCone = cg_trap->FX_RegisterEffect("mp/itemcone.efx");
	cgs.effects.mTurretMuzzleFlash = cg_trap->FX_RegisterEffect("effects/turret/muzzle_flash.efx");
	cgs.effects.mSparks = cg_trap->FX_RegisterEffect("sparks/spark_nosnd.efx"); //sparks/spark.efx
	cgs.effects.mSaberCut = cg_trap->FX_RegisterEffect("saber/saber_cut.efx");
	cgs.effects.mSaberBlock = cg_trap->FX_RegisterEffect("saber/saber_block.efx");
	cgs.effects.mSaberBloodSparks = cg_trap->FX_RegisterEffect("saber/blood_sparks_mp.efx");
	cgs.effects.mSaberBloodSparksSmall = cg_trap->FX_RegisterEffect("saber/blood_sparks_25_mp.efx");
	cgs.effects.mSaberBloodSparksMid = cg_trap->FX_RegisterEffect("saber/blood_sparks_50_mp.efx");
	cgs.effects.mSpawn = cg_trap->FX_RegisterEffect("mp/spawn.efx");
	cgs.effects.mJediSpawn = cg_trap->FX_RegisterEffect("mp/jedispawn.efx");
	cgs.effects.mBlasterDeflect = cg_trap->FX_RegisterEffect("blaster/deflect.efx");
	cgs.effects.mBlasterSmoke = cg_trap->FX_RegisterEffect("blaster/smoke_bolton");
	cgs.effects.mForceConfustionOld = cg_trap->FX_RegisterEffect("force/confusion_old.efx");

	cgs.effects.forceLightning		= cg_trap->FX_RegisterEffect( "effects/force/lightning.efx" );
	cgs.effects.forceLightningWide	= cg_trap->FX_RegisterEffect( "effects/force/lightningwide.efx" );
	cgs.effects.forceDrain		= cg_trap->FX_RegisterEffect( "effects/mp/drain.efx" );
	cgs.effects.forceDrainWide	= cg_trap->FX_RegisterEffect( "effects/mp/drainwide.efx" );
	cgs.effects.forceDrained	= cg_trap->FX_RegisterEffect( "effects/mp/drainhit.efx");

	cgs.effects.mDisruptorDeathSmoke = cg_trap->FX_RegisterEffect("disruptor/death_smoke");

	for ( i = 0 ; i < NUM_CROSSHAIRS ; i++ ) {
		cgs.media.crosshairShader[i] = cg_trap->R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a'+i) );
	}

	cg.loadLCARSStage = 4;

	cgs.media.backTileShader = cg_trap->R_RegisterShader( "gfx/2d/backtile" );

	//precache the fpls skin
	//cg_trap->R_RegisterSkin("models/players/kyle/model_fpls2.skin");

	cgs.media.itemRespawningPlaceholder = cg_trap->R_RegisterShader("powerups/placeholder");
	cgs.media.itemRespawningRezOut = cg_trap->R_RegisterShader("powerups/rezout");

	cgs.media.playerShieldDamage = cg_trap->R_RegisterShader("gfx/misc/personalshield");
	cgs.media.protectShader = cg_trap->R_RegisterShader("gfx/misc/forceprotect");
	cgs.media.forceSightBubble = cg_trap->R_RegisterShader("gfx/misc/sightbubble");
	cgs.media.forceShell = cg_trap->R_RegisterShader("powerups/forceshell");
	cgs.media.sightShell = cg_trap->R_RegisterShader("powerups/sightshell");

	cgs.media.itemHoloModel = cg_trap->R_RegisterModel("models/map_objects/mp/holo.md3");

	if (cgs.gametype == GT_HOLOCRON || com_buildScript->integer)
	{
		for ( i=0; i < NUM_FORCE_POWERS; i++ )
		{
			if (forceHolocronModels[i] &&
				forceHolocronModels[i][0])
			{
				cg_trap->R_RegisterModel(forceHolocronModels[i]);
			}
		}
	}

	if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTY || com_buildScript->integer ) {
		if (com_buildScript->integer)
		{
			cg_trap->R_RegisterModel( "models/flags/r_flag.md3" );
			cg_trap->R_RegisterModel( "models/flags/b_flag.md3" );
			cg_trap->R_RegisterModel( "models/flags/r_flag_ysal.md3" );
			cg_trap->R_RegisterModel( "models/flags/b_flag_ysal.md3" );
		}

		if (cgs.gametype == GT_CTF)
		{
			cgs.media.redFlagModel = cg_trap->R_RegisterModel( "models/flags/r_flag.md3" );
			cgs.media.blueFlagModel = cg_trap->R_RegisterModel( "models/flags/b_flag.md3" );
		}
		else if(cgs.gametype == GT_CTY)
		{
			cgs.media.redFlagModel = cg_trap->R_RegisterModel( "models/flags/r_flag_ysal.md3" );
			cgs.media.blueFlagModel = cg_trap->R_RegisterModel( "models/flags/b_flag_ysal.md3" );
		}

		cg_trap->R_RegisterShaderNoMip( "gfx/hud/mpi_rflag_x" );
		cg_trap->R_RegisterShaderNoMip( "gfx/hud/mpi_bflag_x" );

		cg_trap->R_RegisterShaderNoMip( "gfx/hud/mpi_rflag_ys" );
		cg_trap->R_RegisterShaderNoMip( "gfx/hud/mpi_bflag_ys" );

		cg_trap->R_RegisterShaderNoMip( "gfx/hud/mpi_rflag" );
		cg_trap->R_RegisterShaderNoMip( "gfx/hud/mpi_bflag" );

		cg_trap->R_RegisterShaderNoMip("gfx/2d/net.tga");
	}

	if ( cgs.gametype >= GT_TEAM || com_buildScript->integer ) {
		cgs.media.teamRedShader = cg_trap->R_RegisterShader( "sprites/team_red" );
		cgs.media.teamBlueShader = cg_trap->R_RegisterShader( "sprites/team_blue" );
		//cgs.media.redQuadShader = cg_trap->R_RegisterShader("powerups/blueflag" );
		cgs.media.teamStatusBar = cg_trap->R_RegisterShader( "gfx/2d/colorbar.tga" );
	}
	else if ( cgs.gametype == GT_JEDIMASTER )
	{
		cgs.media.teamRedShader = cg_trap->R_RegisterShader( "sprites/team_red" );
	}

	if (cgs.gametype == GT_POWERDUEL || com_buildScript->integer)
	{
		cgs.media.powerDuelAllyShader = cg_trap->R_RegisterShader("gfx/mp/pduel_icon_double");//cg_trap->R_RegisterShader("gfx/mp/pduel_gameicon_ally");
	}

	cgs.media.heartShader			= cg_trap->R_RegisterShaderNoMip( "ui/assets/statusbar/selectedhealth.tga" );

	cgs.media.ysaliredShader		= cg_trap->R_RegisterShader( "powerups/ysaliredshell");
	cgs.media.ysaliblueShader		= cg_trap->R_RegisterShader( "powerups/ysaliblueshell");
	cgs.media.ysalimariShader		= cg_trap->R_RegisterShader( "powerups/ysalimarishell");
	cgs.media.boonShader			= cg_trap->R_RegisterShader( "powerups/boonshell");
	cgs.media.endarkenmentShader	= cg_trap->R_RegisterShader( "powerups/endarkenmentshell");
	cgs.media.enlightenmentShader	= cg_trap->R_RegisterShader( "powerups/enlightenmentshell");
	cgs.media.invulnerabilityShader = cg_trap->R_RegisterShader( "powerups/invulnerabilityshell");

#ifdef JK2AWARDS
	cgs.media.medalImpressive		= cg_trap->R_RegisterShaderNoMip( "medal_impressive" );
	cgs.media.medalExcellent		= cg_trap->R_RegisterShaderNoMip( "medal_excellent" );
	cgs.media.medalGauntlet			= cg_trap->R_RegisterShaderNoMip( "medal_gauntlet" );
	cgs.media.medalDefend			= cg_trap->R_RegisterShaderNoMip( "medal_defend" );
	cgs.media.medalAssist			= cg_trap->R_RegisterShaderNoMip( "medal_assist" );
	cgs.media.medalCapture			= cg_trap->R_RegisterShaderNoMip( "medal_capture" );
#endif

	// Binocular interface
	cgs.media.binocularCircle		= cg_trap->R_RegisterShader( "gfx/2d/binCircle" );
	cgs.media.binocularMask			= cg_trap->R_RegisterShader( "gfx/2d/binMask" );
	cgs.media.binocularArrow		= cg_trap->R_RegisterShader( "gfx/2d/binSideArrow" );
	cgs.media.binocularTri			= cg_trap->R_RegisterShader( "gfx/2d/binTopTri" );
	cgs.media.binocularStatic		= cg_trap->R_RegisterShader( "gfx/2d/binocularWindow" );
	cgs.media.binocularOverlay		= cg_trap->R_RegisterShader( "gfx/2d/binocularNumOverlay" );

	cg.loadLCARSStage = 5;

	// Chunk models
	//FIXME: jfm:? bother to conditionally load these if an ent has this material type?
	for ( i = 0; i < NUM_CHUNK_MODELS; i++ )
	{
		cgs.media.chunkModels[CHUNK_METAL2][i]	= cg_trap->R_RegisterModel( va( "models/chunks/metal/metal1_%i.md3", i+1 ) ); //_ /switched\ _
		cgs.media.chunkModels[CHUNK_METAL1][i]	= cg_trap->R_RegisterModel( va( "models/chunks/metal/metal2_%i.md3", i+1 ) ); //  \switched/
		cgs.media.chunkModels[CHUNK_ROCK1][i]	= cg_trap->R_RegisterModel( va( "models/chunks/rock/rock1_%i.md3", i+1 ) );
		cgs.media.chunkModels[CHUNK_ROCK2][i]	= cg_trap->R_RegisterModel( va( "models/chunks/rock/rock2_%i.md3", i+1 ) );
		cgs.media.chunkModels[CHUNK_ROCK3][i]	= cg_trap->R_RegisterModel( va( "models/chunks/rock/rock3_%i.md3", i+1 ) );
		cgs.media.chunkModels[CHUNK_CRATE1][i]	= cg_trap->R_RegisterModel( va( "models/chunks/crate/crate1_%i.md3", i+1 ) );
		cgs.media.chunkModels[CHUNK_CRATE2][i]	= cg_trap->R_RegisterModel( va( "models/chunks/crate/crate2_%i.md3", i+1 ) );
		cgs.media.chunkModels[CHUNK_WHITE_METAL][i]	= cg_trap->R_RegisterModel( va( "models/chunks/metal/wmetal1_%i.md3", i+1 ) );
	}

	cgs.media.chunkSound			= cg_trap->S_RegisterSound("sound/weapons/explosions/glasslcar");
	cgs.media.grateSound			= cg_trap->S_RegisterSound( "sound/effects/grate_destroy" );
	cgs.media.rockBreakSound		= cg_trap->S_RegisterSound("sound/effects/wall_smash");
	cgs.media.rockBounceSound[0]	= cg_trap->S_RegisterSound("sound/effects/stone_bounce");
	cgs.media.rockBounceSound[1]	= cg_trap->S_RegisterSound("sound/effects/stone_bounce2");
	cgs.media.metalBounceSound[0]	= cg_trap->S_RegisterSound("sound/effects/metal_bounce");
	cgs.media.metalBounceSound[1]	= cg_trap->S_RegisterSound("sound/effects/metal_bounce2");
	cgs.media.glassChunkSound		= cg_trap->S_RegisterSound("sound/weapons/explosions/glassbreak1");
	cgs.media.crateBreakSound[0]	= cg_trap->S_RegisterSound("sound/weapons/explosions/crateBust1" );
	cgs.media.crateBreakSound[1]	= cg_trap->S_RegisterSound("sound/weapons/explosions/crateBust2" );

/*
Ghoul2 Insert Start
*/
	CG_InitItems();
/*
Ghoul2 Insert End
*/
	memset( cg_weapons, 0, sizeof( cg_weapons ) );

	// only register the items that the server says we need
	Q_strncpyz(items, CG_ConfigString(CS_ITEMS), sizeof(items));

	for ( i = 1 ; i < bg_numItems ; i++ ) {
		if ( items[ i ] == '1' || com_buildScript->integer ) {
			CG_LoadingItem( i );
			CG_RegisterItemVisuals( i );
		}
	}

	cg.loadLCARSStage = 6;

	cgs.media.glassShardShader	= cg_trap->R_RegisterShader( "gfx/misc/test_crackle" );

	// doing one shader just makes it look like a shell.  By using two shaders with different bulge offsets and different texture scales, it has a much more chaotic look
	cgs.media.electricBodyShader			= cg_trap->R_RegisterShader( "gfx/misc/electric" );
	cgs.media.electricBody2Shader			= cg_trap->R_RegisterShader( "gfx/misc/fullbodyelectric2" );

	cgs.media.fsrMarkShader					= cg_trap->R_RegisterShader( "footstep_r" );
	cgs.media.fslMarkShader					= cg_trap->R_RegisterShader( "footstep_l" );
	cgs.media.fshrMarkShader				= cg_trap->R_RegisterShader( "footstep_heavy_r" );
	cgs.media.fshlMarkShader				= cg_trap->R_RegisterShader( "footstep_heavy_l" );

	cgs.media.refractionShader				= cg_trap->R_RegisterShader("effects/refraction");

	cgs.media.cloakedShader					= cg_trap->R_RegisterShader( "gfx/effects/cloakedShader" );

	// wall marks
	cgs.media.shadowMarkShader	= cg_trap->R_RegisterShader( "markShadow" );
	cgs.media.wakeMarkShader	= cg_trap->R_RegisterShader( "wake" );

	cgs.media.viewPainShader					= cg_trap->R_RegisterShader( "gfx/misc/borgeyeflare" );
	cgs.media.viewPainShader_Shields			= cg_trap->R_RegisterShader( "gfx/mp/dmgshader_shields" );
	cgs.media.viewPainShader_ShieldsAndHealth	= cg_trap->R_RegisterShader( "gfx/mp/dmgshader_shieldsandhealth" );

	// register the inline models
	breakPoint = cgs.numInlineModels = cg_trap->CM_NumInlineModels();
	for ( i = 1 ; i < cgs.numInlineModels ; i++ ) {
		char	name[10];
		vec3_t			mins, maxs;
		int				j;

		Com_sprintf( name, sizeof(name), "*%i", i );
		cgs.inlineDrawModel[i] = cg_trap->R_RegisterModel( name );
		if (!cgs.inlineDrawModel[i])
		{
			breakPoint = i;
			break;
		}

		cg_trap->R_ModelBounds( cgs.inlineDrawModel[i], mins, maxs );
		for ( j = 0 ; j < 3 ; j++ ) {
			cgs.inlineModelMidpoints[i][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );
		}
	}

	cg.loadLCARSStage = 7;

	// register all the server specified models
	for (i=1 ; i<MAX_MODELS ; i++) {
		const char		*cModelName;
		char modelName[MAX_QPATH];

		cModelName = CG_ConfigString( CS_MODELS+i );
		if ( !cModelName[0] ) {
			break;
		}

		strcpy(modelName, cModelName);
		if (strstr(modelName, ".glm") || modelName[0] == '$')
		{ //Check to see if it has a custom skin attached.
			CG_HandleAppendedSkin(modelName);
			CG_CacheG2AnimInfo(modelName);
		}

		if (modelName[0] != '$' && modelName[0] != '@')
		{ //don't register vehicle names and saber names as models.
			cgs.gameModels[i] = cg_trap->R_RegisterModel( modelName );
		}
		else
		{//FIXME: register here so that stuff gets precached!!!
			cgs.gameModels[i] = 0;
		}
	}
	cg.loadLCARSStage = 8;
/*
Ghoul2 Insert Start
*/


//	CG_LoadingString( "BSP instances" );

	for(i = 1; i < MAX_SUB_BSP; i++)
	{
		const char		*bspName = 0;
		vec3_t			mins, maxs;
		int				j;
		int				sub = 0;
		char			temp[MAX_QPATH];

		bspName = CG_ConfigString( CS_BSP_MODELS+i );
		if ( !bspName[0] )
		{
			break;
		}

		cg_trap->CM_LoadMap( bspName, qtrue );
		cgs.inlineDrawModel[breakPoint] = cg_trap->R_RegisterModel( bspName );
		cg_trap->R_ModelBounds( cgs.inlineDrawModel[breakPoint], mins, maxs );
		for ( j = 0 ; j < 3 ; j++ )
		{
			cgs.inlineModelMidpoints[breakPoint][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );
		}
		breakPoint++;
		for(sub=1;sub<MAX_MODELS;sub++)
		{
			Com_sprintf(temp, MAX_QPATH, "*%d-%d", i, sub);
			cgs.inlineDrawModel[breakPoint] = cg_trap->R_RegisterModel( temp );
			if (!cgs.inlineDrawModel[breakPoint])
			{
				break;
			}
			cg_trap->R_ModelBounds( cgs.inlineDrawModel[breakPoint], mins, maxs );
			for ( j = 0 ; j < 3 ; j++ )
			{
				cgs.inlineModelMidpoints[breakPoint][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );
			}
			breakPoint++;
		}
	}

	/*
	CG_LoadingString("skins");
	// register all the server specified models
	for (i=1 ; i<MAX_CHARSKINS ; i++) {
		const char		*modelName;

		modelName = CG_ConfigString( CS_CHARSKINS+i );
		if ( !modelName[0] ) {
			break;
		}
		cgs.skins[i] = cg_trap->R_RegisterSkin( modelName );
	}
	*/
	//rww - removed and replaced with CS_G2BONES. For custom skins
	//the new method is to append a * after an indexed model name and
	//then append the skin name after that (note that this is only
	//used for NPCs)

//	CG_LoadingString("weapons");

	CG_InitG2Weapons();

/*
Ghoul2 Insert End
*/
	cg.loadLCARSStage = 9;


	// new stuff
	cgs.media.patrolShader = cg_trap->R_RegisterShaderNoMip("ui/assets/statusbar/patrol.tga");
	cgs.media.assaultShader = cg_trap->R_RegisterShaderNoMip("ui/assets/statusbar/assault.tga");
	cgs.media.campShader = cg_trap->R_RegisterShaderNoMip("ui/assets/statusbar/camp.tga");
	cgs.media.followShader = cg_trap->R_RegisterShaderNoMip("ui/assets/statusbar/follow.tga");
	cgs.media.defendShader = cg_trap->R_RegisterShaderNoMip("ui/assets/statusbar/defend.tga");
	cgs.media.retrieveShader = cg_trap->R_RegisterShaderNoMip("ui/assets/statusbar/retrieve.tga");
	cgs.media.escortShader = cg_trap->R_RegisterShaderNoMip("ui/assets/statusbar/escort.tga");
	cgs.media.cursor = cg_trap->R_RegisterShaderNoMip( "menu/art/3_cursor2" );
	cgs.media.sizeCursor = cg_trap->R_RegisterShaderNoMip( "ui/assets/sizecursor.tga" );
	cgs.media.selectCursor = cg_trap->R_RegisterShaderNoMip( "ui/assets/selectcursor.tga" );

	cgs.media.halfShieldModel	= cg_trap->R_RegisterModel ( "models/weaphits/testboom.md3" );
	cgs.media.halfShieldShader	= cg_trap->R_RegisterShader( "halfShieldShell" );

	cg_trap->FX_RegisterEffect("force/force_touch");
}

const char *CG_GetStringEdString(char *refSection, char *refName)
{
	static char text[2][1024];	//just incase it's nested
	static int		index = 0;

	index ^= 1;
	cg_trap->SE_GetStringTextString(va("%s_%s", refSection, refName), text[index], sizeof(text[0]));
	return text[index];
}

int	CG_GetClassCount(team_t team,int siegeClass );
int CG_GetTeamNonScoreCount(team_t team);

void CG_SiegeCountCvars( void )
{
	int classGfx[6];

	cg_trap->Cvar_Set( "ui_tm1_cnt",va("%d",CG_GetTeamNonScoreCount(TEAM_RED )));
	cg_trap->Cvar_Set( "ui_tm2_cnt",va("%d",CG_GetTeamNonScoreCount(TEAM_BLUE )));
	cg_trap->Cvar_Set( "ui_tm3_cnt",va("%d",CG_GetTeamNonScoreCount(TEAM_SPECTATOR )));

	// This is because the only way we can match up classes is by the gfx handle.
	classGfx[0] = cg_trap->R_RegisterShaderNoMip("gfx/mp/c_icon_infantry");
	classGfx[1] = cg_trap->R_RegisterShaderNoMip("gfx/mp/c_icon_heavy_weapons");
	classGfx[2] = cg_trap->R_RegisterShaderNoMip("gfx/mp/c_icon_demolitionist");
	classGfx[3] = cg_trap->R_RegisterShaderNoMip("gfx/mp/c_icon_vanguard");
	classGfx[4] = cg_trap->R_RegisterShaderNoMip("gfx/mp/c_icon_support");
	classGfx[5] = cg_trap->R_RegisterShaderNoMip("gfx/mp/c_icon_jedi_general");

	cg_trap->Cvar_Set( "ui_tm1_c0_cnt",va("%d",CG_GetClassCount(TEAM_RED,classGfx[0])));
	cg_trap->Cvar_Set( "ui_tm1_c1_cnt",va("%d",CG_GetClassCount(TEAM_RED,classGfx[1])));
	cg_trap->Cvar_Set( "ui_tm1_c2_cnt",va("%d",CG_GetClassCount(TEAM_RED,classGfx[2])));
	cg_trap->Cvar_Set( "ui_tm1_c3_cnt",va("%d",CG_GetClassCount(TEAM_RED,classGfx[3])));
	cg_trap->Cvar_Set( "ui_tm1_c4_cnt",va("%d",CG_GetClassCount(TEAM_RED,classGfx[4])));
	cg_trap->Cvar_Set( "ui_tm1_c5_cnt",va("%d",CG_GetClassCount(TEAM_RED,classGfx[5])));

	cg_trap->Cvar_Set( "ui_tm2_c0_cnt",va("%d",CG_GetClassCount(TEAM_BLUE,classGfx[0])));
	cg_trap->Cvar_Set( "ui_tm2_c1_cnt",va("%d",CG_GetClassCount(TEAM_BLUE,classGfx[1])));
	cg_trap->Cvar_Set( "ui_tm2_c2_cnt",va("%d",CG_GetClassCount(TEAM_BLUE,classGfx[2])));
	cg_trap->Cvar_Set( "ui_tm2_c3_cnt",va("%d",CG_GetClassCount(TEAM_BLUE,classGfx[3])));
	cg_trap->Cvar_Set( "ui_tm2_c4_cnt",va("%d",CG_GetClassCount(TEAM_BLUE,classGfx[4])));
	cg_trap->Cvar_Set( "ui_tm2_c5_cnt",va("%d",CG_GetClassCount(TEAM_BLUE,classGfx[5])));

}

/*
=======================
CG_BuildSpectatorString

=======================
*/
void CG_BuildSpectatorString(void) {
	int i;
	cg.spectatorList[0] = 0;

	// Count up the number of players per team and per class
	CG_SiegeCountCvars();

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_SPECTATOR ) {
			Q_strcat(cg.spectatorList, sizeof(cg.spectatorList), va("%s     ", cgs.clientinfo[i].name));
		}
	}
	i = strlen(cg.spectatorList);
	if (i != cg.spectatorLen) {
		cg.spectatorLen = i;
		cg.spectatorWidth = -1;
	}
}


/*
===================
CG_RegisterClients
===================
*/
static void CG_RegisterClients( void ) {
	int		i;

	CG_LoadingClient(cg.clientNum);
	CG_NewClientInfo(cg.clientNum, qfalse);

	for (i=0 ; i<MAX_CLIENTS ; i++) {
		const char		*clientInfo;

		if (cg.clientNum == i) {
			continue;
		}

		clientInfo = CG_ConfigString( CS_PLAYERS+i );
		if ( !clientInfo[0]) {
			continue;
		}
		CG_LoadingClient( i );
		CG_NewClientInfo( i, qfalse);
	}
	CG_BuildSpectatorString();
}

//===========================================================================

/*
=================
CG_ConfigString
=================
*/
const char *CG_ConfigString( int index ) {
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		cg_trap->Error( ERR_DROP, "CG_ConfigString: bad index: %i", index );
	}
	return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}

//==================================================================

/*
======================
CG_StartMusic

======================
*/
void CG_StartMusic( qboolean bForceStart ) {
	char	*s;
	char	parm1[MAX_QPATH], parm2[MAX_QPATH];

	// start the background music
	s = (char *)CG_ConfigString( CS_MUSIC );
	Q_strncpyz( parm1, COM_Parse( (const char **)&s ), sizeof( parm1 ) );
	Q_strncpyz( parm2, COM_Parse( (const char **)&s ), sizeof( parm2 ) );

	cg_trap->S_StartBackgroundTrack( parm1, parm2, !bForceStart );
}

char *CG_GetMenuBuffer(const char *filename) {
	int	len;
	fileHandle_t	f;
	static char buf[MAX_MENUFILE];

	len = cg_trap->FS_Open( filename, &f, FS_READ );
	if ( !f ) {
		cg_trap->Print( S_COLOR_RED "menu file not found: %s, using default\n", filename );
		return NULL;
	}
	if ( len >= MAX_MENUFILE ) {
		cg_trap->Print( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i\n", filename, len, MAX_MENUFILE );
		cg_trap->FS_Close( f );
		return NULL;
	}

	cg_trap->FS_Read( buf, len, f );
	buf[len] = 0;
	cg_trap->FS_Close( f );

	return buf;
}

//
// ==============================
// new hud stuff ( mission pack )
// ==============================
//
qboolean CG_Asset_Parse(int handle) {
	pc_token_t token;

	if (!cg_trap->PC_ReadToken(handle, &token))
		return qfalse;
	if (Q_stricmp(token.string, "{") != 0) {
		return qfalse;
	}

	while ( 1 ) {
		if (!cg_trap->PC_ReadToken(handle, &token))
			return qfalse;

		if (Q_stricmp(token.string, "}") == 0) {
			return qtrue;
		}

		// font
		if (Q_stricmp(token.string, "font") == 0) {
			int pointSize;
			if (!cg_trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}

//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.textFont);
			cgDC.Assets.qhMediumFont = cgDC.RegisterFont(token.string);
			continue;
		}

		// smallFont
		if (Q_stricmp(token.string, "smallFont") == 0) {
			int pointSize;
			if (!cg_trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.smallFont);
			cgDC.Assets.qhSmallFont = cgDC.RegisterFont(token.string);
			continue;
		}

		// smallFont
		if (Q_stricmp(token.string, "small2Font") == 0) {
			int pointSize;
			if (!cg_trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.smallFont);
			cgDC.Assets.qhSmall2Font = cgDC.RegisterFont(token.string);
			continue;
		}

		// font
		if (Q_stricmp(token.string, "bigfont") == 0) {
			int pointSize;
			if (!cg_trap->PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
//			cgDC.registerFont(token.string, pointSize, &cgDC.Assets.bigFont);
			cgDC.Assets.qhBigFont = cgDC.RegisterFont(token.string);
			continue;
		}

		// gradientbar
		if (Q_stricmp(token.string, "gradientbar") == 0) {
			if (!cg_trap->PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			cgDC.Assets.gradientBar = cg_trap->R_RegisterShaderNoMip(token.string);
			continue;
		}

		// enterMenuSound
		if (Q_stricmp(token.string, "menuEnterSound") == 0) {
			if (!cg_trap->PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			cgDC.Assets.menuEnterSound = cg_trap->S_RegisterSound( token.string );
			continue;
		}

		// exitMenuSound
		if (Q_stricmp(token.string, "menuExitSound") == 0) {
			if (!cg_trap->PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			cgDC.Assets.menuExitSound = cg_trap->S_RegisterSound( token.string );
			continue;
		}

		// itemFocusSound
		if (Q_stricmp(token.string, "itemFocusSound") == 0) {
			if (!cg_trap->PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			cgDC.Assets.itemFocusSound = cg_trap->S_RegisterSound( token.string );
			continue;
		}

		// menuBuzzSound
		if (Q_stricmp(token.string, "menuBuzzSound") == 0) {
			if (!cg_trap->PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			cgDC.Assets.menuBuzzSound = cg_trap->S_RegisterSound( token.string );
			continue;
		}

		if (Q_stricmp(token.string, "cursor") == 0) {
			if (!PC_String_Parse(handle, &cgDC.Assets.cursorStr)) {
				return qfalse;
			}
			cgDC.Assets.cursor = cg_trap->R_RegisterShaderNoMip( cgDC.Assets.cursorStr);
			continue;
		}

		if (Q_stricmp(token.string, "fadeClamp") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.fadeClamp)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeCycle") == 0) {
			if (!PC_Int_Parse(handle, &cgDC.Assets.fadeCycle)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeAmount") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.fadeAmount)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowX") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.shadowX)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowY") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.shadowY)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowColor") == 0) {
			if (!PC_Color_Parse(handle, &cgDC.Assets.shadowColor)) {
				return qfalse;
			}
			cgDC.Assets.shadowFadeClamp = cgDC.Assets.shadowColor[3];
			continue;
		}
	}
	return qfalse; // bk001204 - why not?
}

void CG_ParseMenu(const char *menuFile) {
	pc_token_t token;
	int handle;

	handle = cg_trap->PC_LoadSource(menuFile);
	if (!handle)
		handle = cg_trap->PC_LoadSource("ui/testhud.menu");
	if (!handle)
		return;

	while ( 1 ) {
		if (!cg_trap->PC_ReadToken( handle, &token )) {
			break;
		}

		//if ( Q_stricmp( token, "{" ) ) {
		//	Com_Printf( "Missing { in menu file\n" );
		//	break;
		//}

		//if ( menuCount == MAX_MENUS ) {
		//	Com_Printf( "Too many menus!\n" );
		//	break;
		//}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_stricmp(token.string, "assetGlobalDef") == 0) {
			if (CG_Asset_Parse(handle)) {
				continue;
			} else {
				break;
			}
		}


		if (Q_stricmp(token.string, "menudef") == 0) {
			// start a new menu
			Menu_New< moduleType::cgame >(handle);
		}
	}
	cg_trap->PC_FreeSource(handle);
}


qboolean CG_Load_Menu(const char **p)
{

	char *token;

	token = COM_ParseExt((const char **)p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	while ( 1 ) {

		token = COM_ParseExt((const char **)p, qtrue);

		if (Q_stricmp(token, "}") == 0) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		CG_ParseMenu(token);
	}
	return qfalse;
}


static qboolean CG_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key) {
	return qfalse;
}


static int CG_FeederCount(float feederID) {
	int i, count;
	count = 0;
	if (feederID == FEEDER_REDTEAM_LIST) {
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == TEAM_RED) {
				count++;
			}
		}
	} else if (feederID == FEEDER_BLUETEAM_LIST) {
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == TEAM_BLUE) {
				count++;
			}
		}
	} else if (feederID == FEEDER_SCOREBOARD) {
		return cg.numScores;
	}
	return count;
}


void CG_SetScoreSelection(void *p) {
	menuDef_t *menu = (menuDef_t*)p;
	playerState_t *ps = &cg.snap->ps;
	int i, red, blue;
	red = blue = 0;
	for (i = 0; i < cg.numScores; i++) {
		if (cg.scores[i].team == TEAM_RED) {
			red++;
		} else if (cg.scores[i].team == TEAM_BLUE) {
			blue++;
		}
		if (ps->clientNum == cg.scores[i].client) {
			cg.selectedScore = i;
		}
	}

	if (menu == NULL) {
		// just interested in setting the selected score
		return;
	}

	if ( cgs.gametype >= GT_TEAM ) {
		int feeder = FEEDER_REDTEAM_LIST;
		i = red;
		if (cg.scores[cg.selectedScore].team == TEAM_BLUE) {
			feeder = FEEDER_BLUETEAM_LIST;
			i = blue;
		}
		Menu_SetFeederSelection(menu, feeder, i, NULL);
	} else {
		Menu_SetFeederSelection(menu, FEEDER_SCOREBOARD, cg.selectedScore, NULL);
	}
}

// FIXME: might need to cache this info
static clientInfo_t * CG_InfoFromScoreIndex(int index, int team, int *scoreIndex) {
	int i, count;
	if ( cgs.gametype >= GT_TEAM ) {
		count = 0;
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == team) {
				if (count == index) {
					*scoreIndex = i;
					return &cgs.clientinfo[cg.scores[i].client];
				}
				count++;
			}
		}
	}
	*scoreIndex = index;
	return &cgs.clientinfo[ cg.scores[index].client ];
}

static const char *CG_FeederItemText(float feederID, int index, int column,
									 qhandle_t *handle1, qhandle_t *handle2, qhandle_t *handle3) {
	gitem_t *item;
	int scoreIndex = 0;
	clientInfo_t *info = NULL;
	int team = -1;
	score_t *sp = NULL;

	*handle1 = *handle2 = *handle3 = -1;

	if (feederID == FEEDER_REDTEAM_LIST) {
		team = TEAM_RED;
	} else if (feederID == FEEDER_BLUETEAM_LIST) {
		team = TEAM_BLUE;
	}

	info = CG_InfoFromScoreIndex(index, team, &scoreIndex);
	sp = &cg.scores[scoreIndex];

	if (info && info->infoValid) {
		switch (column) {
			case 0:
				if ( info->powerups & ( 1 << PW_NEUTRALFLAG ) ) {
					item = BG_FindItemForPowerup( PW_NEUTRALFLAG );
					*handle1 = cg_items[ ITEM_INDEX(item) ].icon;
				} else if ( info->powerups & ( 1 << PW_REDFLAG ) ) {
					item = BG_FindItemForPowerup( PW_REDFLAG );
					*handle1 = cg_items[ ITEM_INDEX(item) ].icon;
				} else if ( info->powerups & ( 1 << PW_BLUEFLAG ) ) {
					item = BG_FindItemForPowerup( PW_BLUEFLAG );
					*handle1 = cg_items[ ITEM_INDEX(item) ].icon;
				} else {
					/*
					if ( info->botSkill > 0 && info->botSkill <= 5 ) {
						*handle1 = cgs.media.botSkillShaders[ info->botSkill - 1 ];
					} else if ( info->handicap < 100 ) {
					return va("%i", info->handicap );
					}
					*/
				}
			break;
			case 1:
				if (team == -1) {
					return "";
				} else {
					*handle1 = CG_StatusHandle(info->teamTask);
				}
		  break;
			case 2:
				if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << sp->client ) ) {
					return "Ready";
				}
				if (team == -1) {
					if (cgs.gametype == GT_DUEL || cgs.gametype == GT_POWERDUEL) {
						return va("%i/%i", info->wins, info->losses);
					} else if (info->infoValid && info->team == TEAM_SPECTATOR ) {
						return "Spectator";
					} else {
						return "";
					}
				} else {
					if (info->teamLeader) {
						return "Leader";
					}
				}
			break;
			case 3:
				return info->name;
			break;
			case 4:
				return va("%i", info->score);
			break;
			case 5:
				return va("%4i", sp->time);
			break;
			case 6:
				if ( sp->ping == -1 ) {
					return "connecting";
				}
				return va("%4i", sp->ping);
			break;
		}
	}

	return "";
}

static qhandle_t CG_FeederItemImage(float feederID, int index) {
	return 0;
}

static qboolean CG_FeederSelection(float feederID, int index, itemDef_t *item) {
	if ( cgs.gametype >= GT_TEAM ) {
		int i, count;
		int team = (feederID == FEEDER_REDTEAM_LIST) ? TEAM_RED : TEAM_BLUE;
		count = 0;
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == team) {
				if (index == count) {
					cg.selectedScore = i;
				}
				count++;
			}
		}
	} else {
		cg.selectedScore = index;
	}

	return qtrue;
}

static float CG_Cvar_Get(const char *cvar) {
	char buff[128];
	memset(buff, 0, sizeof(buff));
	cg_trap->Cvar_VariableStringBuffer(cvar, buff, sizeof(buff));
	return atof(buff);
}

void CG_Text_PaintWithCursor(float x, float y, float scale, vec4_t color, const char *text, int cursorPos, char cursor, int limit, int style, int iMenuFont) {
	CG_Text_Paint(x, y, scale, color, text, 0, limit, style, iMenuFont);
}

static int CG_OwnerDrawWidth(int ownerDraw, float scale) {
	switch (ownerDraw) {
	  case CG_GAME_TYPE:
			return CG_Text_Width(BG_GetGametypeString( cgs.gametype ), scale, FONT_MEDIUM);
	  case CG_GAME_STATUS:
			return CG_Text_Width(CG_GetGameStatusText(), scale, FONT_MEDIUM);
			break;
	  case CG_KILLER:
			return CG_Text_Width(CG_GetKillerText(), scale, FONT_MEDIUM);
			break;
	  case CG_RED_NAME:
			return CG_Text_Width(DEFAULT_REDTEAM_NAME/*cg_redTeamName.string*/, scale, FONT_MEDIUM);
			break;
	  case CG_BLUE_NAME:
			return CG_Text_Width(DEFAULT_BLUETEAM_NAME/*cg_blueTeamName.string*/, scale, FONT_MEDIUM);
			break;
	}
	return 0;
}

static int CG_PlayCinematic(const char *name, float x, float y, float w, float h) {
	return cg_trap->CIN_PlayCinematic(name, x, y, w, h, CIN_loop);
}

static void CG_StopCinematic(int handle) {
	cg_trap->CIN_StopCinematic(handle);
}

static void CG_DrawCinematic(int handle, float x, float y, float w, float h) {
	cg_trap->CIN_SetExtents(handle, x, y, w, h);
	cg_trap->CIN_DrawCinematic(handle);
}

static void CG_RunCinematicFrame(int handle) {
	cg_trap->CIN_RunCinematic(handle);
}

/*
=================
CG_LoadMenus();

=================
*/
void CG_LoadMenus(const char *menuFile)
{
	const char	*token;
	const char	*p;
	int	len;
	fileHandle_t	f;
	static char buf[MAX_MENUDEFFILE];

	len = cg_trap->FS_Open( menuFile, &f, FS_READ );

	if ( !f )
	{
		if( Q_isanumber( menuFile ) ) // cg_hudFiles 1
			cg_trap->Print( S_COLOR_GREEN "hud menu file skipped, using default\n" );
		else
			cg_trap->Print( S_COLOR_YELLOW "hud menu file not found: %s, using default\n", menuFile );

		len = cg_trap->FS_Open( "ui/jahud.txt", &f, FS_READ );
		if (!f)
		{
			cg_trap->Error( ERR_DROP, S_COLOR_RED "default hud menu file not found: ui/jahud.txt, unable to continue!" );
		}
	}

	if ( len >= MAX_MENUDEFFILE )
	{
		cg_trap->FS_Close( f );
		cg_trap->Error( ERR_DROP, S_COLOR_RED "menu file too large: %s is %i, max allowed is %i", menuFile, len, MAX_MENUDEFFILE );
	}

	cg_trap->FS_Read( buf, len, f );
	buf[len] = 0;
	cg_trap->FS_Close( f );

	p = buf;

	COM_BeginParseSession ("CG_LoadMenus");
	while ( 1 )
	{
		token = COM_ParseExt( &p, qtrue );
		if( !token || token[0] == 0 || token[0] == '}')
		{
			break;
		}

		if ( Q_stricmp( token, "}" ) == 0 )
		{
			break;
		}

		if (Q_stricmp(token, "loadmenu") == 0)
		{
			if (CG_Load_Menu(&p))
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	//Com_Printf("UI menu load time = %d milli seconds\n", cgi_Milliseconds() - start);
}

/*
=================
CG_LoadHudMenu();

=================
*/
void CG_LoadHudMenu()
{
	const char *hudSet;

	cgDC.registerShaderNoMip			= cg_trap->R_RegisterShaderNoMip;
	cgDC.setColor						= cg_trap->R_SetColor;
	cgDC.drawHandlePic					= &CG_DrawPic;
	cgDC.drawStretchPic					= cg_trap->R_DrawStretchPic;
	cgDC.drawText						= &CG_Text_Paint;
	cgDC.textWidth						= &CG_Text_Width;
	cgDC.textHeight						= &CG_Text_Height;
	cgDC.registerModel					= cg_trap->R_RegisterModel;
	cgDC.modelBounds					= cg_trap->R_ModelBounds;
	cgDC.fillRect						= &CG_FillRect;
	cgDC.drawRect						= &CG_DrawRect;
	cgDC.drawSides						= &CG_DrawSides;
	cgDC.drawTopBottom					= &CG_DrawTopBottom;
	cgDC.clearScene						= cg_trap->R_ClearScene;
	cgDC.addRefEntityToScene			= cg_trap->R_AddRefEntityToScene;
	cgDC.renderScene					= cg_trap->R_RenderScene;
	cgDC.RegisterFont					= cg_trap->R_RegisterFont;
	cgDC.Font_StrLenPixels				= cg_trap->R_Font_StrLenPixels;
	cgDC.Font_StrLenChars				= cg_trap->R_Font_StrLenChars;
	cgDC.Font_HeightPixels				= cg_trap->R_Font_HeightPixels;
	cgDC.Font_DrawString				= cg_trap->R_Font_DrawString;
	cgDC.Language_IsAsian				= cg_trap->R_Language_IsAsian;
	cgDC.Language_UsesSpaces			= cg_trap->R_Language_UsesSpaces;
	cgDC.AnyLanguage_ReadCharFromString	= cg_trap->R_AnyLanguage_ReadCharFromString;
	cgDC.ownerDrawItem					= &CG_OwnerDraw;
	cgDC.getValue						= &CG_GetValue;
	cgDC.ownerDrawVisible				= &CG_OwnerDrawVisible;
	cgDC.runScript						= &CG_RunMenuScript;
	cgDC.deferScript					= &CG_DeferMenuScript;
	cgDC.getTeamColor					= &CG_GetTeamColor;
	cgDC.setCVar						= cg_trap->Cvar_Set;
	cgDC.getCVarString					= cg_trap->Cvar_VariableStringBuffer;
	cgDC.getCVarValue					= CG_Cvar_Get;
	cgDC.drawTextWithCursor				= &CG_Text_PaintWithCursor;
	//cgDC.setOverstrikeMode			= &cg_trap->Key_SetOverstrikeMode;
	//cgDC.getOverstrikeMode			= &cg_trap->Key_GetOverstrikeMode;
	cgDC.startLocalSound				= cg_trap->S_StartLocalSound;
	cgDC.ownerDrawHandleKey				= &CG_OwnerDrawHandleKey;
	cgDC.feederCount					= &CG_FeederCount;
	cgDC.feederItemImage				= &CG_FeederItemImage;
	cgDC.feederItemText					= &CG_FeederItemText;
	cgDC.feederSelection				= &CG_FeederSelection;
	//cgDC.setBinding					= &cg_trap->Key_SetBinding;
	//cgDC.getBindingBuf				= &cg_trap->Key_GetBindingBuf;
	//cgDC.keynumToStringBuf			= &cg_trap->Key_KeynumToStringBuf;
	//cgDC.executeText					= &cg_trap->Cmd_ExecuteText;
	cgDC.Error							= Com_Error;
	cgDC.Print							= Com_Printf;
	cgDC.ownerDrawWidth					= &CG_OwnerDrawWidth;
	//cgDC.Pause						= &CG_Pause;
	cgDC.registerSound					= cg_trap->S_RegisterSound;
	cgDC.startBackgroundTrack			= cg_trap->S_StartBackgroundTrack;
	cgDC.stopBackgroundTrack			= cg_trap->S_StopBackgroundTrack;
	cgDC.playCinematic					= &CG_PlayCinematic;
	cgDC.stopCinematic					= &CG_StopCinematic;
	cgDC.drawCinematic					= &CG_DrawCinematic;
	cgDC.runCinematicFrame				= &CG_RunCinematicFrame;
	cgDC.ext.Font_StrLenPixels			= cg_trap->ext.R_Font_StrLenPixels;


	Init_Display(&cgDC);

	Menu_Reset();

	hudSet = cg_hudFiles.string;
	if (hudSet[0] == '\0')
	{
		hudSet = "ui/jahud.txt";
	}

	CG_LoadMenus(hudSet);

}

void CG_AssetCache() {
	//if (Assets.textFont == NULL) {
	//  cg_trap->R_RegisterFont("fonts/arial.ttf", 72, &Assets.textFont);
	//}
	//Com_Printf("Menu Size: %i bytes\n", sizeof(Menus));
	cgDC.Assets.gradientBar = cg_trap->R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
	cgDC.Assets.fxBasePic = cg_trap->R_RegisterShaderNoMip( ART_FX_BASE );
	cgDC.Assets.fxPic[0] = cg_trap->R_RegisterShaderNoMip( ART_FX_RED );
	cgDC.Assets.fxPic[1] = cg_trap->R_RegisterShaderNoMip( ART_FX_YELLOW );
	cgDC.Assets.fxPic[2] = cg_trap->R_RegisterShaderNoMip( ART_FX_GREEN );
	cgDC.Assets.fxPic[3] = cg_trap->R_RegisterShaderNoMip( ART_FX_TEAL );
	cgDC.Assets.fxPic[4] = cg_trap->R_RegisterShaderNoMip( ART_FX_BLUE );
	cgDC.Assets.fxPic[5] = cg_trap->R_RegisterShaderNoMip( ART_FX_CYAN );
	cgDC.Assets.fxPic[6] = cg_trap->R_RegisterShaderNoMip( ART_FX_WHITE );
	cgDC.Assets.scrollBar = cg_trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR );
	cgDC.Assets.scrollBarArrowDown = cg_trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
	cgDC.Assets.scrollBarArrowUp = cg_trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
	cgDC.Assets.scrollBarArrowLeft = cg_trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
	cgDC.Assets.scrollBarArrowRight = cg_trap->R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
	cgDC.Assets.scrollBarThumb = cg_trap->R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
	cgDC.Assets.sliderBar = cg_trap->R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
	cgDC.Assets.sliderThumb = cg_trap->R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );
}

/*


/*
Ghoul2 Insert Start
*/

// initialise the cg_entities structure - take into account the ghoul2 stl stuff in the active snap shots
void CG_Init_CG(void)
{
	memset( &cg, 0, sizeof(cg));
}

// initialise the cg_entities structure - take into account the ghoul2 stl stuff
void CG_Init_CGents(void)
{
	memset(&cg_entities, 0, sizeof(cg_entities));
}


void CG_InitItems(void)
{
	memset( cg_items, 0, sizeof( cg_items ) );
}

void CG_TransitionPermanent(void)
{
	centity_t	*cent = cg_entities;
	int			i;

	cg_numpermanents = 0;
	for(i=0;i<MAX_GENTITIES;i++,cent++)
	{
		if (cg_trap->GetDefaultState(i, &cent->currentState))
		{
			cent->nextState = cent->currentState;
			VectorCopy (cent->currentState.origin, cent->lerpOrigin);
			VectorCopy (cent->currentState.angles, cent->lerpAngles);
			cent->currentValid = qtrue;

			cg_permanents[cg_numpermanents++] = cent;
		}
	}
}

/*
Ghoul2 Insert End
*/

extern playerState_t *cgSendPS[MAX_GENTITIES]; //is not MAX_CLIENTS because NPCs exceed MAX_CLIENTS
void CG_PmoveClientPointerUpdate();

template< moduleType mod >
void WP_SaberLoadParms( void );
template< moduleType mod >
void BG_VehicleLoadParms( void );

/*
=================
CG_Init

Called after every level change or subsystem restart
Will perform callbacks to make the loading info screen update.
=================
*/
void CG_Init( int serverMessageNum, int serverCommandSequence, int clientNum )
{
	static gitem_t *item;
	char buf[64];
	const char	*s;
	int i = 0;

	BG_InitAnimsets(); //clear it out

	cg_trap->RegisterSharedMemory( cg.sharedBuffer.raw );

	//Load external vehicle data
	BG_VehicleLoadParms< moduleType::cgame >();

	// clear everything
/*
Ghoul2 Insert Start
*/

//	memset( cg_entities, 0, sizeof( cg_entities ) );
	CG_Init_CGents();
// this is a No-No now we have stl vector classes in here.
//	memset( &cg, 0, sizeof( cg ) );
	CG_Init_CG();
	CG_InitItems();

	//create the global jetpack instance
	CG_InitJetpackGhoul2();

	CG_PmoveClientPointerUpdate();

/*
Ghoul2 Insert End
*/

	//Load sabers.cfg data
	WP_SaberLoadParms< moduleType::cgame >();

	// this is kinda dumb as well, but I need to pre-load some fonts in order to have the text available
	//	to say I'm loading the assets.... which includes loading the fonts. So I'll set these up as reasonable
	//	defaults, then let the menu asset parser (which actually specifies the ingame fonts) load over them
	//	if desired during parse.  Dunno how legal it is to store in these cgDC things, but it causes no harm
	//	and even if/when they get overwritten they'll be legalised by the menu asset parser :-)
//	CG_LoadFonts();
	cgDC.Assets.qhSmallFont  = cg_trap->R_RegisterFont("ocr_a");
	cgDC.Assets.qhMediumFont = cg_trap->R_RegisterFont("ergoec");
	cgDC.Assets.qhBigFont = cgDC.Assets.qhMediumFont;

	memset( &cgs, 0, sizeof( cgs ) );
	memset( cg_weapons, 0, sizeof(cg_weapons) );

	cg.clientNum = clientNum;

	cgs.processedSnapshotNum = serverMessageNum;
	cgs.serverCommandSequence = serverCommandSequence;

	cg.loadLCARSStage		= 0;

	cg.itemSelect = -1;
	cg.forceSelect = -1;

	// load a few needed things before we do any screen updates
	cgs.media.charsetShader		= cg_trap->R_RegisterShaderNoMip( "gfx/2d/charsgrid_med" );
	cgs.media.whiteShader		= cg_trap->R_RegisterShader( "white" );

	cgs.media.loadBarLED		= cg_trap->R_RegisterShaderNoMip( "gfx/hud/load_tick" );
	cgs.media.loadBarLEDCap		= cg_trap->R_RegisterShaderNoMip( "gfx/hud/load_tick_cap" );
	cgs.media.loadBarLEDSurround= cg_trap->R_RegisterShaderNoMip( "gfx/hud/mp_levelload" );

	// Force HUD set up
	cg.forceHUDActive = qtrue;
	cg.forceHUDTotalFlashTime = 0;
	cg.forceHUDNextFlashTime = 0;

	i = WP_NONE+1;
	while (i <= LAST_USEABLE_WEAPON)
	{
		item = BG_FindItemForWeapon( static_cast< weapon_t >( i ) );

		if (item && item->icon && item->icon[0])
		{
			cgs.media.weaponIcons[i] = cg_trap->R_RegisterShaderNoMip(item->icon);
			cgs.media.weaponIcons_NA[i] = cg_trap->R_RegisterShaderNoMip(va("%s_na", item->icon));
		}
		else
		{ //make sure it is zero'd (default shader)
			cgs.media.weaponIcons[i] = 0;
			cgs.media.weaponIcons_NA[i] = 0;
		}
		i++;
	}
	cg_trap->Cvar_VariableStringBuffer("com_buildscript", buf, sizeof(buf));
	if (atoi(buf))
	{
		cg_trap->R_RegisterShaderNoMip("gfx/hud/w_icon_saberstaff");
		cg_trap->R_RegisterShaderNoMip("gfx/hud/w_icon_duallightsaber");
	}
	i = 0;

	// HUD artwork for cycling inventory,weapons and force powers
	cgs.media.weaponIconBackground		= cg_trap->R_RegisterShaderNoMip( "gfx/hud/background");
	cgs.media.forceIconBackground		= cg_trap->R_RegisterShaderNoMip( "gfx/hud/background_f");
	cgs.media.inventoryIconBackground	= cg_trap->R_RegisterShaderNoMip( "gfx/hud/background_i");

	//rww - precache holdable item icons here
	while (i < bg_numItems)
	{
		if (bg_itemlist[i].giType == IT_HOLDABLE)
		{
			if (bg_itemlist[i].icon)
			{
				cgs.media.invenIcons[bg_itemlist[i].giTag] = cg_trap->R_RegisterShaderNoMip(bg_itemlist[i].icon);
			}
			else
			{
				cgs.media.invenIcons[bg_itemlist[i].giTag] = 0;
			}
		}

		i++;
	}

	//rww - precache force power icons here
	i = 0;

	while (i < NUM_FORCE_POWERS)
	{
		cgs.media.forcePowerIcons[i] = cg_trap->R_RegisterShaderNoMip(HolocronIcons[i]);

		i++;
	}
	cgs.media.rageRecShader = cg_trap->R_RegisterShaderNoMip("gfx/mp/f_icon_ragerec");


	//body decal shaders -rww
	cgs.media.bdecal_bodyburn1 = cg_trap->R_RegisterShader("gfx/damage/bodyburnmark1");
	cgs.media.bdecal_saberglow = cg_trap->R_RegisterShader("gfx/damage/saberglowmark");
	cgs.media.bdecal_burn1 = cg_trap->R_RegisterShader("gfx/damage/bodybigburnmark1");
	cgs.media.mSaberDamageGlow = cg_trap->R_RegisterShader("gfx/effects/saberDamageGlow");

	CG_RegisterCvars();

	CG_InitConsoleCommands();

	cg.renderingThirdPerson = cg_thirdPerson.integer;

	cg.weaponSelect = WP_BRYAR_PISTOL;

	cgs.redflag = cgs.blueflag = -1; // For compatibily, default to unset for
	cgs.flagStatus = -1;
	// old servers

	// get the rendering configuration from the client system
	cg_trap->GetGlconfig( &cgs.glconfig );
	cgs.screenXScale = cgs.glconfig.vidWidth / 640.0;
	cgs.screenYScale = cgs.glconfig.vidHeight / 480.0;

	// get the gamestate from the client system
	cg_trap->GetGameState( &cgs.gameState );

	CG_TransitionPermanent(); //rwwRMG - added

	// check version
	s = CG_ConfigString( CS_GAME_VERSION );
	if ( strcmp( s, GAME_VERSION ) ) {
		cg_trap->Error( ERR_DROP, "Client/Server game mismatch: %s/%s", GAME_VERSION, s );
	}

	s = CG_ConfigString( CS_LEVEL_START_TIME );
	cgs.levelStartTime = atoi( s );

	CG_ParseServerinfo();

	// load the new map
//	CG_LoadingString( "collision map" );

	cg_trap->CM_LoadMap( cgs.mapname, qfalse );

	String_Init();

	cg.loading = qtrue;		// force players to load instead of defer

	//make sure saber data is loaded before this! (so we can precache the appropriate hilts)
	CG_InitSiegeMode();

	CG_RegisterSounds();

//	CG_LoadingString( "graphics" );

	CG_RegisterGraphics();

//	CG_LoadingString( "clients" );

	CG_RegisterClients();		// if low on memory, some clients will be deferred

	CG_AssetCache();
	CG_LoadHudMenu();      // load new hud stuff

	cg.loading = qfalse;	// future players will be deferred

	CG_InitLocalEntities();

	CG_InitMarkPolys();

	// remove the last loading update
	cg.infoScreenText[0] = 0;

	// Make sure we have update values (scores)
	CG_SetConfigValues();

	CG_StartMusic(qfalse);

//	CG_LoadingString( "Clearing light styles" );
	CG_ClearLightStyles();

//	CG_LoadingString( "Creating automap data" );
	//init automap
	cg_trap->R_InitializeWireframeAutomap();

	CG_LoadingString( "" );

	CG_ShaderStateChanged();

	cg_trap->S_ClearLoopingSounds();

	cg.distanceCull = cg_trap->R_GetDistanceCull();

	CG_ParseEntitiesFromString();

	cg_trap->Cvar_VariableStringBuffer( "rate", buf, sizeof( buf ) );
	if ( atoi( buf ) == 4000 )
		cg_trap->Print( "^3WARNING: Default /rate value detected. Suggest typing /rate 25000 for a smoother connection!\n" );
}

//makes sure returned string is in localized format
const char *CG_GetLocationString(const char *loc)
{
	static char text[1024]={0};

	if (!loc || loc[0] != '@')
	{ //just a raw string
		return loc;
	}

	cg_trap->SE_GetStringTextString(loc+1, text, sizeof(text));
	return text;
}

//clean up all the ghoul2 allocations, the nice and non-hackly way -rww
void CG_KillCEntityG2(int entNum);
void CG_DestroyAllGhoul2(void)
{
	int i = 0;
	int j;

//	Com_Printf("... CGameside GHOUL2 Cleanup\n");
	while (i < MAX_GENTITIES)
	{ //free all dynamically allocated npc client info structs and ghoul2 instances
		CG_KillCEntityG2(i);
		i++;
	}

	//Clean the weapon instances
	CG_ShutDownG2Weapons();

	i = 0;
	while (i < MAX_ITEMS)
	{ //and now for items
		j = 0;
		while (j < MAX_ITEM_MODELS)
		{
			if (cg_items[i].g2Models[j] && cg_trap->G2_HaveWeGhoul2Models(cg_items[i].g2Models[j]))
			{
				cg_trap->G2API_CleanGhoul2Models(&cg_items[i].g2Models[j]);
				cg_items[i].g2Models[j] = NULL;
			}
			j++;
		}
		i++;
	}

	//Clean the global jetpack instance
	CG_CleanJetpackGhoul2();
}

/*
=================
CG_Shutdown

Called before every level change or subsystem restart
=================
*/
void CG_Shutdown( void )
{
	BG_ClearAnimsets(); //free all dynamic allocations made through the engine

    CG_DestroyAllGhoul2();

//	Com_Printf("... FX System Cleanup\n");
	cg_trap->FX_FreeSystem();
	cg_trap->ROFF_Clean();

	//reset weather
	cg_trap->R_WorldEffectCommand("die");

	UI_CleanupGhoul2();
	//If there was any ghoul2 stuff in our side of the shared ui code, then remove it now.

	// some mods may need to do cleanup work here,
	// like closing files or archiving session data
}

/*
===============
CG_NextForcePower_f
===============
*/
void CG_NextForcePower_f( void )
{
	int current;
	usercmd_t cmd;
	if ( !cg.snap )
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	current = cg_trap->GetCurrentCmdNumber();
	cg_trap->GetUserCmd(current, &cmd);
	if ((cmd.buttons & BUTTON_USE) || CG_NoUseableForce())
	{
		CG_NextInventory_f();
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

//	BG_CycleForce(&cg.snap->ps, 1);
	if (cg.forceSelect != -1)
	{
		cg.snap->ps.fd.forcePowerSelected = cg.forceSelect;
	}

	BG_CycleForce(&cg.snap->ps, 1);

	if (cg.snap->ps.fd.forcePowersKnown & (1 << cg.snap->ps.fd.forcePowerSelected))
	{
		cg.forceSelect = cg.snap->ps.fd.forcePowerSelected;
		cg.forceSelectTime = cg.time;
	}
}

/*
===============
CG_PrevForcePower_f
===============
*/
void CG_PrevForcePower_f( void )
{
	int current;
	usercmd_t cmd;
	if ( !cg.snap )
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	current = cg_trap->GetCurrentCmdNumber();
	cg_trap->GetUserCmd(current, &cmd);
	if ((cmd.buttons & BUTTON_USE) || CG_NoUseableForce())
	{
		CG_PrevInventory_f();
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

//	BG_CycleForce(&cg.snap->ps, -1);
	if (cg.forceSelect != -1)
	{
		cg.snap->ps.fd.forcePowerSelected = cg.forceSelect;
	}

	BG_CycleForce(&cg.snap->ps, -1);

	if (cg.snap->ps.fd.forcePowersKnown & (1 << cg.snap->ps.fd.forcePowerSelected))
	{
		cg.forceSelect = cg.snap->ps.fd.forcePowerSelected;
		cg.forceSelectTime = cg.time;
	}
}

void CG_NextInventory_f(void)
{
	if ( !cg.snap )
	{
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	if (cg.itemSelect != -1)
	{
		cg.snap->ps.stats[STAT_HOLDABLE_ITEM] = BG_GetItemIndexByTag(cg.itemSelect, IT_HOLDABLE);
	}
	BG_CycleInven(&cg.snap->ps, 1);

	if (cg.snap->ps.stats[STAT_HOLDABLE_ITEM])
	{
		cg.itemSelect = bg_itemlist[cg.snap->ps.stats[STAT_HOLDABLE_ITEM]].giTag;
		cg.invenSelectTime = cg.time;
	}
}

void CG_PrevInventory_f(void)
{
	if ( !cg.snap )
	{
		return;
	}

	if (cg.snap->ps.pm_flags & PMF_FOLLOW)
	{
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_SPECTATOR)
	{
		return;
	}

	if (cg.itemSelect != -1)
	{
		cg.snap->ps.stats[STAT_HOLDABLE_ITEM] = BG_GetItemIndexByTag(cg.itemSelect, IT_HOLDABLE);
	}
	BG_CycleInven(&cg.snap->ps, -1);

	if (cg.snap->ps.stats[STAT_HOLDABLE_ITEM])
	{
		cg.itemSelect = bg_itemlist[cg.snap->ps.stats[STAT_HOLDABLE_ITEM]].giTag;
		cg.invenSelectTime = cg.time;
	}
}

static void _CG_MouseEvent( int x, int y ) {
	cgDC.cursorx = cgs.cursorX;
	cgDC.cursory = cgs.cursorY;
	CG_MouseEvent( x, y );
}

static qboolean CG_IncomingConsoleCommand( void ) {
	//rww - let mod authors filter client console messages so they can cut them off if they want.
	//return qtrue if the command is ok. Otherwise, you can set char 0 on the command str to 0 and return
	//qfalse to not execute anything, or you can fill conCommand in with something valid and return 0
	//in order to have that string executed in place. Some example code:
#if 0
	TCGIncomingConsoleCommand *icc = &cg.sharedBuffer.icc;
	if ( strstr( icc->conCommand, "wait" ) )
	{ //filter out commands contaning wait
		Com_Printf( "You can't use commands containing the string wait with MyMod v1.0\n" );
		icc->conCommand[0] = 0;
		return qfalse;
	}
	else if ( strstr( icc->conCommand, "blah" ) )
	{ //any command containing the string "blah" is redirected to "quit"
		strcpy( icc->conCommand, "quit" );
		return qfalse;
	}
#endif
	return qtrue;
}

static void CG_GetOrigin( int entID, vec3_t out ) {
	VectorCopy( cg_entities[entID].currentState.pos.trBase, out );
}

static void CG_GetAngles( int entID, vec3_t out ) {
	VectorCopy( cg_entities[entID].currentState.apos.trBase, out );
}

static trajectory_t *CG_GetOriginTrajectory( int entID ) {
	return &cg_entities[entID].nextState.pos;
}

static trajectory_t *CG_GetAngleTrajectory( int entID ) {
	return &cg_entities[entID].nextState.apos;
}

static void _CG_ROFF_NotetrackCallback( int entID, const char *notetrack ) {
	CG_ROFF_NotetrackCallback( &cg_entities[entID], notetrack );
}

static void CG_MapChange( void ) {
	// this may be called more than once for a given map change, as the server is going to attempt to send out
	// multiple broadcasts in hopes that the client will receive one of them
	cg.mMapChange = qtrue;
}

static void CG_AutomapInput( void ) {
	autoMapInput_t *autoInput = &cg.sharedBuffer.autoMapInput;

	memcpy( &cg_autoMapInput, autoInput, sizeof( autoMapInput_t ) );

#if 0
	if ( !arg0 ) //if this is non-0, it's actually a one-frame mouse event
		cg_autoMapInputTime = cg.time + 1000;
	else
#endif
	{
		if ( cg_autoMapInput.yaw )		cg_autoMapAngle[YAW] += cg_autoMapInput.yaw;
		if ( cg_autoMapInput.pitch )	cg_autoMapAngle[PITCH] += cg_autoMapInput.pitch;
		cg_autoMapInput.yaw = 0.0f;
		cg_autoMapInput.pitch = 0.0f;
	}
}

static void CG_FX_CameraShake( void ) {
	TCGCameraShake *data = &cg.sharedBuffer.cameraShake;
	CG_DoCameraShake( data->mOrigin, data->mIntensity, data->mRadius, data->mTime );
}

/*
============
GetCGameAPI
============
*/

cgameImport_t *cg_trap = NULL;

cgameExport_t* GetCGameAPI( int apiVersion, cgameImport_t *import )
{
	static cgameExport_t cge = {0};

	assert( import );
	cg_trap = import;

	memset( &cge, 0, sizeof( cge ) );

	if ( apiVersion != CGAME_API_VERSION ) {
		cg_trap->Print( "Mismatched CGAME_API_VERSION: expected %i, got %i\n", CGAME_API_VERSION, apiVersion );
		return NULL;
	}

	cge.Init					= CG_Init;
	cge.Shutdown				= CG_Shutdown;
	cge.ConsoleCommand			= CG_ConsoleCommand;
	cge.DrawActiveFrame			= CG_DrawActiveFrame;
	cge.CrosshairPlayer			= CG_CrosshairPlayer;
	cge.LastAttacker			= CG_LastAttacker;
	cge.KeyEvent				= CG_KeyEvent;
	cge.MouseEvent				= _CG_MouseEvent;
	cge.EventHandling			= CG_EventHandling;
	cge.PointContents			= C_PointContents;
	cge.GetLerpOrigin			= C_GetLerpOrigin;
	cge.GetLerpData				= C_GetLerpData;
	cge.Trace					= C_Trace;
	cge.G2Trace					= C_G2Trace;
	cge.G2Mark					= C_G2Mark;
	cge.RagCallback				= CG_RagCallback;
	cge.IncomingConsoleCommand	= CG_IncomingConsoleCommand;
	cge.NoUseableForce			= CG_NoUseableForce;
	cge.GetOrigin				= CG_GetOrigin;
	cge.GetAngles				= CG_GetAngles;
	cge.GetOriginTrajectory		= CG_GetOriginTrajectory;
	cge.GetAngleTrajectory		= CG_GetAngleTrajectory;
	cge.ROFF_NotetrackCallback	= _CG_ROFF_NotetrackCallback;
	cge.MapChange				= CG_MapChange;
	cge.AutomapInput			= CG_AutomapInput;
	cge.MiscEnt					= CG_MiscEnt;
	cge.CameraShake				= CG_FX_CameraShake;

	return &cge;
}
