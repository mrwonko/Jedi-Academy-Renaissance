/*
===========================================================================
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
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

//
// cg_weaponinit.c -- events and effects dealing with weapons
#include "cg_local.h"
#include "fx_local.h"


/*
=================
CG_RegisterWeapon

The server says this item is used on this level
=================
*/
void CG_RegisterWeapon( int weaponNum) {
	weaponInfo_t	*weaponInfo;
	gitem_t			*item, *ammo;
	char			path[MAX_QPATH];
	vec3_t			mins, maxs;
	int				i;

	weaponInfo = &cg_weapons[weaponNum];

	if ( weaponNum == 0 ) {
		return;
	}

	if ( weaponInfo->registered ) {
		return;
	}

	//if ( cgs.wDisable & (1<<weaponNum) )
	//	return;

	memset( weaponInfo, 0, sizeof( *weaponInfo ) );
	weaponInfo->registered = qtrue;

	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( item->giType == IT_WEAPON && item->giTag == weaponNum ) {
			weaponInfo->item = item;
			break;
		}
	}
	if ( !item->classname ) {
		cg_trap->Error( ERR_DROP, "Couldn't find weapon %i", weaponNum );
	}
	CG_RegisterItemVisuals( item - bg_itemlist );

	// load cmodel before model so filecache works
	weaponInfo->weaponModel = cg_trap->R_RegisterModel( item->world_model[0] );
	// load in-view model also
	weaponInfo->viewModel = cg_trap->R_RegisterModel(item->view_model);

	// calc midpoint for rotation
	cg_trap->R_ModelBounds( weaponInfo->weaponModel, mins, maxs );
	for ( i = 0 ; i < 3 ; i++ ) {
		weaponInfo->weaponMidpoint[i] = mins[i] + 0.5 * ( maxs[i] - mins[i] );
	}

	weaponInfo->weaponIcon = cg_trap->R_RegisterShader( item->icon );
	weaponInfo->ammoIcon = cg_trap->R_RegisterShader( item->icon );

	for ( ammo = bg_itemlist + 1 ; ammo->classname ; ammo++ ) {
		if ( ammo->giType == IT_AMMO && ammo->giTag == weaponNum ) {
			break;
		}
	}
	if ( ammo->classname && ammo->world_model[0] ) {
		weaponInfo->ammoModel = cg_trap->R_RegisterModel( ammo->world_model[0] );
	}

//	strcpy( path, item->view_model );
//	COM_StripExtension( path, path );
//	strcat( path, "_flash.md3" );
	weaponInfo->flashModel = 0;//cg_trap->R_RegisterModel( path );

	if (weaponNum == WP_DISRUPTOR ||
		weaponNum == WP_FLECHETTE ||
		weaponNum == WP_REPEATER ||
		weaponNum == WP_ROCKET_LAUNCHER ||
		weaponNum == WP_CONCUSSION)
	{
		Q_strncpyz( path, item->view_model, sizeof(path) );
		COM_StripExtension( path, path, sizeof( path ) );
		Q_strcat( path, sizeof(path), "_barrel.md3" );
		weaponInfo->barrelModel = cg_trap->R_RegisterModel( path );
	}
	else if (weaponNum == WP_STUN_BATON)
	{ //only weapon with more than 1 barrel..
		cg_trap->R_RegisterModel("models/weapons2/stun_baton/baton_barrel.md3");
		cg_trap->R_RegisterModel("models/weapons2/stun_baton/baton_barrel2.md3");
		cg_trap->R_RegisterModel("models/weapons2/stun_baton/baton_barrel3.md3");
	}
	else
	{
		weaponInfo->barrelModel = 0;
	}

	if (weaponNum != WP_SABER)
	{
		Q_strncpyz( path, item->view_model, sizeof(path) );
		COM_StripExtension( path, path, sizeof( path ) );
		Q_strcat( path, sizeof(path), "_hand.md3" );
		weaponInfo->handsModel = cg_trap->R_RegisterModel( path );
	}
	else
	{
		weaponInfo->handsModel = 0;
	}

//	if ( !weaponInfo->handsModel ) {
//		weaponInfo->handsModel = cg_trap->R_RegisterModel( "models/weapons2/shotgun/shotgun_hand.md3" );
//	}

	switch ( weaponNum ) {
	case WP_STUN_BATON:
	case WP_MELEE:
/*		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->firingSound = cg_trap->S_RegisterSound( "sound/weapons/saber/saberhum.wav" );
//		weaponInfo->flashSound[0] = cg_trap->S_RegisterSound( "sound/weapons/melee/fstatck.wav" );
*/
		//cg_trap->R_RegisterShader( "gfx/effects/stunPass" );
		cg_trap->FX_RegisterEffect( "stunBaton/flesh_impact" );

		if (weaponNum == WP_STUN_BATON)
		{
			cg_trap->S_RegisterSound( "sound/weapons/baton/idle.wav" );
			weaponInfo->flashSound[0] = cg_trap->S_RegisterSound( "sound/weapons/baton/fire.mp3" );
			weaponInfo->altFlashSound[0] = cg_trap->S_RegisterSound( "sound/weapons/baton/fire.mp3" );
		}
		else
		{
			/*
			int j = 0;

			while (j < 4)
			{
				weaponInfo->flashSound[j] = cg_trap->S_RegisterSound( va("sound/weapons/melee/swing%i", j+1) );
				weaponInfo->altFlashSound[j] = weaponInfo->flashSound[j];
				j++;
			}
			*/
			//No longer needed, animsound config plays them for us
		}
		break;
	case WP_SABER:
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->firingSound = cg_trap->S_RegisterSound( "sound/weapons/saber/saberhum1.wav" );
		weaponInfo->missileModel		= cg_trap->R_RegisterModel( "models/weapons2/saber/saber_w.glm" );
		break;

	case WP_CONCUSSION:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/concussion/select.wav");

		weaponInfo->flashSound[0]		= NULL_SOUND;
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "concussion/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		//weaponInfo->missileDlightColor= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_ConcussionProjectileThink;

		weaponInfo->altFlashSound[0]	= NULL_SOUND;
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= cg_trap->S_RegisterSound( "sound/weapons/bryar/altcharge.wav");
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "concussion/altmuzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		//weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_ConcussionProjectileThink;

		cgs.effects.disruptorAltMissEffect		= cg_trap->FX_RegisterEffect( "disruptor/alt_miss" );

		cgs.effects.concussionShotEffect		= cg_trap->FX_RegisterEffect( "concussion/shot" );
		cgs.effects.concussionImpactEffect		= cg_trap->FX_RegisterEffect( "concussion/explosion" );
		cg_trap->R_RegisterShader("gfx/effects/blueLine");
		cg_trap->R_RegisterShader("gfx/misc/whiteline2");
		break;

	case WP_BRYAR_PISTOL:
	case WP_BRYAR_OLD:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/bryar/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/bryar/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "bryar/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		//weaponInfo->missileDlightColor= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_BryarProjectileThink;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/bryar/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= cg_trap->S_RegisterSound( "sound/weapons/bryar/altcharge.wav");
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "bryar/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
		//weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_BryarAltProjectileThink;

		cgs.effects.bryarShotEffect			= cg_trap->FX_RegisterEffect( "bryar/shot" );
		cgs.effects.bryarPowerupShotEffect	= cg_trap->FX_RegisterEffect( "bryar/crackleShot" );
		cgs.effects.bryarWallImpactEffect	= cg_trap->FX_RegisterEffect( "bryar/wall_impact" );
		cgs.effects.bryarWallImpactEffect2	= cg_trap->FX_RegisterEffect( "bryar/wall_impact2" );
		cgs.effects.bryarWallImpactEffect3	= cg_trap->FX_RegisterEffect( "bryar/wall_impact3" );
		cgs.effects.bryarFleshImpactEffect	= cg_trap->FX_RegisterEffect( "bryar/flesh_impact" );
		cgs.effects.bryarDroidImpactEffect	= cg_trap->FX_RegisterEffect( "bryar/droid_impact" );

		cgs.media.bryarFrontFlash = cg_trap->R_RegisterShader( "gfx/effects/bryarFrontFlash" );

		// Note these are temp shared effects
		cg_trap->FX_RegisterEffect("blaster/wall_impact.efx");
		cg_trap->FX_RegisterEffect("blaster/flesh_impact.efx");

		break;

	case WP_BLASTER:
	case WP_EMPLACED_GUN: //rww - just use the same as this for now..
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/blaster/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/blaster/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "blaster/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_BlasterProjectileThink;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/blaster/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "blaster/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_BlasterProjectileThink;

		cg_trap->FX_RegisterEffect( "blaster/deflect" );
		cgs.effects.blasterShotEffect			= cg_trap->FX_RegisterEffect( "blaster/shot" );
		cgs.effects.blasterWallImpactEffect		= cg_trap->FX_RegisterEffect( "blaster/wall_impact" );
		cgs.effects.blasterFleshImpactEffect	= cg_trap->FX_RegisterEffect( "blaster/flesh_impact" );
		cgs.effects.blasterDroidImpactEffect	= cg_trap->FX_RegisterEffect( "blaster/droid_impact" );
		break;

	case WP_DISRUPTOR:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/disruptor/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/disruptor/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "disruptor/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/disruptor/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= cg_trap->S_RegisterSound("sound/weapons/disruptor/altCharge.wav");
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "disruptor/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;

		cgs.effects.disruptorRingsEffect		= cg_trap->FX_RegisterEffect( "disruptor/rings" );
		cgs.effects.disruptorProjectileEffect	= cg_trap->FX_RegisterEffect( "disruptor/projectile" );
		cgs.effects.disruptorWallImpactEffect	= cg_trap->FX_RegisterEffect( "disruptor/wall_impact" );
		cgs.effects.disruptorFleshImpactEffect	= cg_trap->FX_RegisterEffect( "disruptor/flesh_impact" );
		cgs.effects.disruptorAltMissEffect		= cg_trap->FX_RegisterEffect( "disruptor/alt_miss" );
		cgs.effects.disruptorAltHitEffect		= cg_trap->FX_RegisterEffect( "disruptor/alt_hit" );

		cg_trap->R_RegisterShader( "gfx/effects/redLine" );
		cg_trap->R_RegisterShader( "gfx/misc/whiteline2" );
		cg_trap->R_RegisterShader( "gfx/effects/smokeTrail" );

		cg_trap->S_RegisterSound("sound/weapons/disruptor/zoomstart.wav");
		cg_trap->S_RegisterSound("sound/weapons/disruptor/zoomend.wav");

		// Disruptor gun zoom interface
		cgs.media.disruptorMask			= cg_trap->R_RegisterShader( "gfx/2d/cropCircle2");
		cgs.media.disruptorInsert		= cg_trap->R_RegisterShader( "gfx/2d/cropCircle");
		cgs.media.disruptorLight		= cg_trap->R_RegisterShader( "gfx/2d/cropCircleGlow" );
		cgs.media.disruptorInsertTick	= cg_trap->R_RegisterShader( "gfx/2d/insertTick" );
		cgs.media.disruptorChargeShader	= cg_trap->R_RegisterShaderNoMip("gfx/2d/crop_charge");

		cgs.media.disruptorZoomLoop		= cg_trap->S_RegisterSound( "sound/weapons/disruptor/zoomloop.wav" );
		break;

	case WP_BOWCASTER:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/bowcaster/select.wav");

		weaponInfo->altFlashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/bowcaster/fire.wav");
		weaponInfo->altFiringSound			= NULL_SOUND;
		weaponInfo->altChargeSound			= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "bowcaster/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight		= 0;
//		weaponInfo->altMissileDlightColor	= {0,0,0};
		weaponInfo->altMissileHitSound		= NULL_SOUND;
		weaponInfo->altMissileTrailFunc	= FX_BowcasterProjectileThink;

		weaponInfo->flashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/bowcaster/fire.wav");
		weaponInfo->firingSound		= NULL_SOUND;
		weaponInfo->chargeSound		= cg_trap->S_RegisterSound( "sound/weapons/bowcaster/altcharge.wav");
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "bowcaster/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight	= 0;
//		weaponInfo->missileDlightColor= {0,0,0};
		weaponInfo->missileHitSound	= NULL_SOUND;
		weaponInfo->missileTrailFunc = FX_BowcasterAltProjectileThink;

		cgs.effects.bowcasterShotEffect		= cg_trap->FX_RegisterEffect( "bowcaster/shot" );
		cgs.effects.bowcasterImpactEffect	= cg_trap->FX_RegisterEffect( "bowcaster/explosion" );

		cg_trap->FX_RegisterEffect( "bowcaster/deflect" );

		cgs.media.greenFrontFlash = cg_trap->R_RegisterShader( "gfx/effects/greenFrontFlash" );
		break;

	case WP_REPEATER:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/repeater/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/repeater/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "repeater/muzzle_flash" );
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_RepeaterProjectileThink;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/repeater/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "repeater/muzzle_flash" );
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_RepeaterAltProjectileThink;

		cgs.effects.repeaterProjectileEffect	= cg_trap->FX_RegisterEffect( "repeater/projectile" );
		cgs.effects.repeaterAltProjectileEffect	= cg_trap->FX_RegisterEffect( "repeater/alt_projectile" );
		cgs.effects.repeaterWallImpactEffect	= cg_trap->FX_RegisterEffect( "repeater/wall_impact" );
		cgs.effects.repeaterFleshImpactEffect	= cg_trap->FX_RegisterEffect( "repeater/flesh_impact" );
		//cgs.effects.repeaterAltWallImpactEffect	= cg_trap->FX_RegisterEffect( "repeater/alt_wall_impact" );
		cgs.effects.repeaterAltWallImpactEffect	= cg_trap->FX_RegisterEffect( "repeater/concussion" );
		break;

	case WP_DEMP2:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/demp2/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound("sound/weapons/demp2/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect("demp2/muzzle_flash");
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_DEMP2_ProjectileThink;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound("sound/weapons/demp2/altfire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= cg_trap->S_RegisterSound("sound/weapons/demp2/altCharge.wav");
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect("demp2/muzzle_flash");
		weaponInfo->altMissileModel		= NULL_HANDLE;
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;

		cgs.effects.demp2ProjectileEffect		= cg_trap->FX_RegisterEffect( "demp2/projectile" );
		cgs.effects.demp2WallImpactEffect		= cg_trap->FX_RegisterEffect( "demp2/wall_impact" );
		cgs.effects.demp2FleshImpactEffect		= cg_trap->FX_RegisterEffect( "demp2/flesh_impact" );

		cgs.media.demp2Shell = cg_trap->R_RegisterModel( "models/items/sphere.md3" );
		cgs.media.demp2ShellShader = cg_trap->R_RegisterShader( "gfx/effects/demp2shell" );

		cgs.media.lightningFlash = cg_trap->R_RegisterShader("gfx/misc/lightningFlash");
		break;

	case WP_FLECHETTE:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/flechette/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/flechette/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "flechette/muzzle_flash" );
		weaponInfo->missileModel		= cg_trap->R_RegisterModel("models/weapons2/golan_arms/projectileMain.md3");
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_FlechetteProjectileThink;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/flechette/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "flechette/muzzle_flash" );
		weaponInfo->altMissileModel		= cg_trap->R_RegisterModel( "models/weapons2/golan_arms/projectile.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_FlechetteAltProjectileThink;

		cgs.effects.flechetteShotEffect			= cg_trap->FX_RegisterEffect( "flechette/shot" );
		cgs.effects.flechetteAltShotEffect		= cg_trap->FX_RegisterEffect( "flechette/alt_shot" );
		cgs.effects.flechetteWallImpactEffect	= cg_trap->FX_RegisterEffect( "flechette/wall_impact" );
		cgs.effects.flechetteFleshImpactEffect	= cg_trap->FX_RegisterEffect( "flechette/flesh_impact" );
		break;

	case WP_ROCKET_LAUNCHER:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/rocket/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/rocket/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= cg_trap->FX_RegisterEffect( "rocket/muzzle_flash" ); //cg_trap->FX_RegisterEffect( "rocket/muzzle_flash2" );
		//flash2 still looks crappy with the fx bolt stuff. Because the fx bolt stuff doesn't work entirely right.
		weaponInfo->missileModel		= cg_trap->R_RegisterModel( "models/weapons2/merr_sonn/projectile.md3" );
		weaponInfo->missileSound		= cg_trap->S_RegisterSound( "sound/weapons/rocket/missleloop.wav");
		weaponInfo->missileDlight		= 125;
		VectorSet(weaponInfo->missileDlightColor, 1.0, 1.0, 0.5);
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_RocketProjectileThink;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/rocket/alt_fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= cg_trap->FX_RegisterEffect( "rocket/altmuzzle_flash" );
		weaponInfo->altMissileModel		= cg_trap->R_RegisterModel( "models/weapons2/merr_sonn/projectile.md3" );
		weaponInfo->altMissileSound		= cg_trap->S_RegisterSound( "sound/weapons/rocket/missleloop.wav");
		weaponInfo->altMissileDlight	= 125;
		VectorSet(weaponInfo->altMissileDlightColor, 1.0, 1.0, 0.5);
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = FX_RocketAltProjectileThink;

		cgs.effects.rocketShotEffect			= cg_trap->FX_RegisterEffect( "rocket/shot" );
		cgs.effects.rocketExplosionEffect		= cg_trap->FX_RegisterEffect( "rocket/explosion" );

		cg_trap->R_RegisterShaderNoMip( "gfx/2d/wedge" );
		cg_trap->R_RegisterShaderNoMip( "gfx/2d/lock" );

		cg_trap->S_RegisterSound( "sound/weapons/rocket/lock.wav" );
		cg_trap->S_RegisterSound( "sound/weapons/rocket/tick.wav" );
		break;

	case WP_THERMAL:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/thermal/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/thermal/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= cg_trap->S_RegisterSound( "sound/weapons/thermal/charge.wav");
		weaponInfo->muzzleEffect		= NULL_FX;
		weaponInfo->missileModel		= cg_trap->R_RegisterModel( "models/weapons2/thermal/thermal_proj.md3" );
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/thermal/fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= cg_trap->S_RegisterSound( "sound/weapons/thermal/charge.wav");
		weaponInfo->altMuzzleEffect		= NULL_FX;
		weaponInfo->altMissileModel		= cg_trap->R_RegisterModel( "models/weapons2/thermal/thermal_proj.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;

		cgs.effects.thermalExplosionEffect		= cg_trap->FX_RegisterEffect( "thermal/explosion" );
		cgs.effects.thermalShockwaveEffect		= cg_trap->FX_RegisterEffect( "thermal/shockwave" );

		cgs.media.grenadeBounce1		= cg_trap->S_RegisterSound( "sound/weapons/thermal/bounce1.wav" );
		cgs.media.grenadeBounce2		= cg_trap->S_RegisterSound( "sound/weapons/thermal/bounce2.wav" );

		cg_trap->S_RegisterSound( "sound/weapons/thermal/thermloop.wav" );
		cg_trap->S_RegisterSound( "sound/weapons/thermal/warning.wav" );

		break;

	case WP_TRIP_MINE:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/detpack/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/laser_trap/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= NULL_FX;
		weaponInfo->missileModel		= 0;//cg_trap->R_RegisterModel( "models/weapons2/laser_trap/laser_trap_w.md3" );
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/laser_trap/fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= NULL_FX;
		weaponInfo->altMissileModel		= 0;//cg_trap->R_RegisterModel( "models/weapons2/laser_trap/laser_trap_w.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;

		cgs.effects.tripmineLaserFX = cg_trap->FX_RegisterEffect("tripMine/laserMP.efx");
		cgs.effects.tripmineGlowFX = cg_trap->FX_RegisterEffect("tripMine/glowbit.efx");

		cg_trap->FX_RegisterEffect( "tripMine/explosion" );
		// NOTENOTE temp stuff
		cg_trap->S_RegisterSound( "sound/weapons/laser_trap/stick.wav" );
		cg_trap->S_RegisterSound( "sound/weapons/laser_trap/warning.wav" );
		break;

	case WP_DET_PACK:
		weaponInfo->selectSound			= cg_trap->S_RegisterSound("sound/weapons/detpack/select.wav");

		weaponInfo->flashSound[0]		= cg_trap->S_RegisterSound( "sound/weapons/detpack/fire.wav");
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= NULL_FX;
		weaponInfo->missileModel		= cg_trap->R_RegisterModel( "models/weapons2/detpack/det_pack.md3" );
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
//		weaponInfo->missileDlightColor	= {0,0,0};
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= 0;

		weaponInfo->altFlashSound[0]	= cg_trap->S_RegisterSound( "sound/weapons/detpack/fire.wav");
		weaponInfo->altFiringSound		= NULL_SOUND;
		weaponInfo->altChargeSound		= NULL_SOUND;
		weaponInfo->altMuzzleEffect		= NULL_FX;
		weaponInfo->altMissileModel		= cg_trap->R_RegisterModel( "models/weapons2/detpack/det_pack.md3" );
		weaponInfo->altMissileSound		= NULL_SOUND;
		weaponInfo->altMissileDlight	= 0;
//		weaponInfo->altMissileDlightColor= {0,0,0};
		weaponInfo->altMissileHitSound	= NULL_SOUND;
		weaponInfo->altMissileTrailFunc = 0;

		cg_trap->R_RegisterModel( "models/weapons2/detpack/det_pack.md3" );
		cg_trap->S_RegisterSound( "sound/weapons/detpack/stick.wav" );
		cg_trap->S_RegisterSound( "sound/weapons/detpack/warning.wav" );
		cg_trap->S_RegisterSound( "sound/weapons/explosions/explode5.wav" );
		break;
	case WP_TURRET:
		weaponInfo->flashSound[0]		= NULL_SOUND;
		weaponInfo->firingSound			= NULL_SOUND;
		weaponInfo->chargeSound			= NULL_SOUND;
		weaponInfo->muzzleEffect		= NULL_HANDLE;
		weaponInfo->missileModel		= NULL_HANDLE;
		weaponInfo->missileSound		= NULL_SOUND;
		weaponInfo->missileDlight		= 0;
		weaponInfo->missileHitSound		= NULL_SOUND;
		weaponInfo->missileTrailFunc	= FX_TurretProjectileThink;

		cg_trap->FX_RegisterEffect("effects/blaster/wall_impact.efx");
		cg_trap->FX_RegisterEffect("effects/blaster/flesh_impact.efx");
		break;

	 default:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 1 );
		weaponInfo->flashSound[0] = cg_trap->S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav" );
		break;
	}
}
