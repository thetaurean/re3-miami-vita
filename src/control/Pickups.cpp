#include "common.h"

#include "main.h"

#include "Camera.h"
#include "Coronas.h"
#include "Darkel.h"
#include "Entity.h"
#include "Explosion.h"
#include "Font.h"
#include "Garages.h"
#include "General.h"
#include "ModelIndices.h"
#include "Object.h"
#include "Pad.h"
#include "Pickups.h"
#include "PlayerPed.h"
#include "Wanted.h"
#include "DMAudio.h"
#include "Fire.h"
#include "PointLights.h"
#include "Pools.h"
#ifdef FIX_BUGS
#include "Replay.h"
#endif
#include "Script.h"
#include "Shadows.h"
#include "SpecialFX.h"
#include "Sprite.h"
#include "Timer.h"
#include "WaterLevel.h"
#include "World.h"
#include "Hud.h"
#include "Messages.h"

CPickup CPickups::aPickUps[NUMPICKUPS];
int16 CPickups::NumMessages;
int32 CPickups::aPickUpsCollected[NUMCOLLECTEDPICKUPS];
int16 CPickups::CollectedPickUpIndex;

int32 CPickups::PlayerOnWeaponPickup;
int32 CPickups::CollectPickupBuffer;

// unused
bool CPickups::bPickUpcamActivated;
CVehicle *CPickups::pPlayerVehicle;
CVector CPickups::StaticCamCoors;
uint32 CPickups::StaticCamStartTime;

tPickupMessage CPickups::aMessages[NUMPICKUPMESSAGES];

// TODO(Miami)
uint16 AmmoForWeapon[20] = { 0, 1, 45, 125, 25, 150, 300, 25, 5, 250, 5, 5, 0, 500, 0, 100, 0, 0, 0, 0 };

// --MIAMI: Done
uint16 AmmoForWeapon_OnStreet[WEAPONTYPE_TOTALWEAPONS] = {
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 34,
	12, 16, 14, 10, 100, 60, 60, 60, 60, 60, 20, 14,
	4, 150, 100, 500, 1, 400, 36, 0,
};
uint16 CostOfWeapon[20] = { 0, 10, 250, 800, 1500, 3000, 5000, 10000, 25000, 25000, 2000, 2000, 0, 50000, 0, 3000, 0, 0, 0, 0 };

// TODO(Miami): Those are all placeholders!!
uint8 aWeaponReds[] = { 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 0, 128, 255, 255, 0, 255, 0, 128, 128, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 128, 0, 255, 0 };
uint8 aWeaponGreens[] = { 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
0, 255, 128, 255, 0, 255, 128, 255, 0, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
0, 255, 0, 255, 0 };
uint8 aWeaponBlues[] = { 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
0, 0, 255, 0, 255, 255, 0, 128, 255, 0, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
0, 128, 255, 0, 0 };

void
ModifyStringLabelForControlSetting(char *str)
{
	int len = (int)strlen(str);
	if (len <= 2)
		return;

	if (str[len - 2] != '_')
		return;

	switch (CPad::GetPad(0)->Mode) {
		case 0:
		case 1:
			str[len - 1] = 'L';
			break;
		case 2:
			str[len - 1] = 'T';
			break;
		case 3:
			str[len - 1] = 'C';
			break;
		default:
			return;
	}
}

void
CPickup::RemoveKeepType()
{
	CWorld::Remove(m_pObject);
	delete m_pObject;

	m_bRemoved = true;
	m_pObject = nil;
}

void
CPickup::Remove()
{
	RemoveKeepType();
	m_eType = PICKUP_NONE;
}

CObject *
CPickup::GiveUsAPickUpObject(int32 handle)
{
	CObject *object;

	if (handle <= 0) object = new CObject(m_eModelIndex, false);
	else {
		CPools::MakeSureSlotInObjectPoolIsEmpty(handle);
		object = new(handle) CObject(m_eModelIndex, false);
	}

	if (object == nil) return nil;
	object->ObjectCreatedBy = MISSION_OBJECT;
	object->SetPosition(m_vecPos);
	object->SetOrientation(0.0f, 0.0f, -HALFPI);
	object->GetMatrix().UpdateRW();
	object->UpdateRwFrame();

	object->bAffectedByGravity = false;
	object->bExplosionProof = true;
	object->bUsesCollision = false;
	object->bIsPickup = true;
	object->bHasPreRenderEffects = true;

	object->m_nBonusValue = m_eModelIndex == MI_PICKUP_BONUS ? m_nQuantity : 0;

	switch (m_eType)
	{
	case PICKUP_IN_SHOP:
		object->bPickupObjWithMessage = true;
		object->bOutOfStock = false;
		break;
	case PICKUP_ON_STREET:
	case PICKUP_ONCE:
	case PICKUP_ONCE_TIMEOUT:
	case PICKUP_COLLECTABLE1:
	case PICKUP_MONEY:
	case PICKUP_MINE_INACTIVE:
	case PICKUP_MINE_ARMED:
	case PICKUP_NAUTICAL_MINE_INACTIVE:
	case PICKUP_NAUTICAL_MINE_ARMED:
	case PICKUP_FLOATINGPACKAGE:
	case PICKUP_ON_STREET_SLOW:
		object->bPickupObjWithMessage = false;
		object->bOutOfStock = false;
		break;
	case PICKUP_IN_SHOP_OUT_OF_STOCK:
		object->bPickupObjWithMessage = false;
		object->bOutOfStock = true;
		object->bRenderScorched = true;
		break;
	case PICKUP_FLOATINGPACKAGE_FLOATING:
	default:
		break;
	}
	return object;
}

bool
CPickup::CanBePickedUp(CPlayerPed *player, int playerId)
{
	assert(m_pObject != nil);
	bool cannotBePickedUp =
		(m_pObject->GetModelIndex() == MI_PICKUP_BODYARMOUR && player->m_fArmour > CWorld::Players[playerId].m_nMaxArmour - 0.5f)
		|| (m_pObject->GetModelIndex() == MI_PICKUP_HEALTH && player->m_fHealth > CWorld::Players[playerId].m_nMaxHealth - 0.5f)
		|| (m_pObject->GetModelIndex() == MI_PICKUP_BRIBE && player->m_pWanted->m_nWantedLevel == 0)
		|| (m_pObject->GetModelIndex() == MI_PICKUP_KILLFRENZY && (CTheScripts::IsPlayerOnAMission() || CDarkel::FrenzyOnGoing() || !CGame::nastyGame));
	return !cannotBePickedUp;
}

bool
CPickup::Update(CPlayerPed *player, CVehicle *vehicle, int playerId)
{
	float waterLevel;

	if (m_pObject) {
		m_pObject->SetPosition(m_vecPos);
		// TODO(Miami): Extra object
	}
	if (m_eType == PICKUP_ASSET_REVENUE) {
		uint32 oldTimer = m_nTimer;
		m_nTimer = CTimer::GetTimeInMilliseconds();
		float calculatedRevenue;
		if ((FindPlayerCoors() - m_vecPos).Magnitude() > 10.0) {
			uint32 timePassed = CTimer::GetTimeInMilliseconds() - oldTimer;
			calculatedRevenue = m_nRevenue + (timePassed * m_nMoneySpeed) * sq(1.f / 1200.f);
		} else {
			calculatedRevenue = m_nRevenue;
		}
		m_nRevenue = Min(calculatedRevenue, m_nQuantity);
		// TODO(Miami): For pickup glow effect?
		/*
		if (calculatedRevenue < 10.0) {
			m_pObject->m_nCostValue = 0;
		} else {
			m_pObject->m_nCostValue = calculatedRevenue;
		}
		*/
	}

	if (m_bRemoved) {
		if (CTimer::GetTimeInMilliseconds() > m_nTimer) {
			// respawn pickup if we're far enough
			float dist = (FindPlayerCoors().x - m_vecPos.x) * (FindPlayerCoors().x - m_vecPos.x) + (FindPlayerCoors().y - m_vecPos.y) * (FindPlayerCoors().y - m_vecPos.y);
			if (dist > 100.0f || m_eType == PICKUP_IN_SHOP && dist > 2.4f) {
				m_pObject = GiveUsAPickUpObject(-1);
				if (m_pObject) {
					CWorld::Add(m_pObject);
					m_bRemoved = false;
				}
			}
		}
		return false;
	}

	if (!m_pObject) return false;

	if (!IsMine()) {
		// let's check if we touched the pickup
		bool isPickupTouched = false;
		if (m_pObject->GetModelIndex() == MI_PICKUP_BRIBE) {
			if (vehicle != nil) {
				if (vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z, 2.0f))
					isPickupTouched = true;
			}
			else {
				if (Abs(player->GetPosition().z - m_pObject->GetPosition().z) < 2.0f) {
					if ((player->GetPosition().x - m_pObject->GetPosition().x) * (player->GetPosition().x - m_pObject->GetPosition().x) +
						(player->GetPosition().y - m_pObject->GetPosition().y) * (player->GetPosition().y - m_pObject->GetPosition().y) < 1.8f)
						isPickupTouched = true;
				}
			}
		} else if (m_pObject->GetModelIndex() == MI_PICKUP_CAMERA) {
			if (vehicle != nil && vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z, 2.0f)) {
				isPickupTouched = true;
			}
		} else if (vehicle == nil) {
			if (Abs(player->GetPosition().z - m_pObject->GetPosition().z) < 2.0f) {
				if ((player->GetPosition().x - m_pObject->GetPosition().x) * (player->GetPosition().x - m_pObject->GetPosition().x) +
					(player->GetPosition().y - m_pObject->GetPosition().y) * (player->GetPosition().y - m_pObject->GetPosition().y) < 1.8f)
					isPickupTouched = true;
			}
		}

		// if we didn't then we've got nothing to do
		if (isPickupTouched && CanBePickedUp(player, playerId)) {
			CPad::GetPad(0)->StartShake(120, 100);
			switch (m_eType)
			{
			case PICKUP_IN_SHOP:
				if (CWorld::Players[playerId].m_nMoney < CostOfWeapon[CPickups::WeaponForModel(m_pObject->GetModelIndex())]) {
					CGarages::TriggerMessage("PU_MONY", -1, 6000, -1);
				} else {
					CWorld::Players[playerId].m_nMoney -= CostOfWeapon[CPickups::WeaponForModel(m_pObject->GetModelIndex())];
					if (!CPickups::GivePlayerGoodiesWithPickUpMI(m_pObject->GetModelIndex(), playerId)) {
						player->GiveWeapon(CPickups::WeaponForModel(m_pObject->GetModelIndex()), AmmoForWeapon[CPickups::WeaponForModel(m_pObject->GetModelIndex())]);
						player->m_nSelectedWepSlot = player->GetWeaponSlot(CPickups::WeaponForModel(m_pObject->GetModelIndex()));
						DMAudio.PlayFrontEndSound(SOUND_PICKUP_WEAPON_BOUGHT, m_pObject->GetModelIndex() - MI_GRENADE);
					}
					RemoveKeepType();
					m_nTimer = CTimer::GetTimeInMilliseconds() + 5000;
					return true;
				}
				break;
			case PICKUP_ON_STREET:
			case PICKUP_ON_STREET_SLOW:
				if (!CPickups::GivePlayerGoodiesWithPickUpMI(m_pObject->GetModelIndex(), playerId)) {
					if (CPickups::WeaponForModel(m_pObject->GetModelIndex())) {
						player->GiveWeapon(CPickups::WeaponForModel(m_pObject->GetModelIndex()), m_nQuantity != 0 ? m_nQuantity : AmmoForWeapon_OnStreet[CPickups::WeaponForModel(m_pObject->GetModelIndex())]);
						if (player->m_nSelectedWepSlot == player->GetWeaponSlot(WEAPONTYPE_UNARMED)) {
							player->m_nSelectedWepSlot = player->GetWeaponSlot(CPickups::WeaponForModel(m_pObject->GetModelIndex()));
						}
						DMAudio.PlayFrontEndSound(SOUND_PICKUP_WEAPON, m_pObject->GetModelIndex() - MI_GRENADE);
					} else if (m_pObject->GetModelIndex() == MI_PICKUP_CAMERA && vehicle != nil) {
						DMAudio.PlayFrontEndSound(SOUND_PICKUP_BONUS, 0);
						CPickups::bPickUpcamActivated = true;
						CPickups::pPlayerVehicle = FindPlayerVehicle();
						CPickups::StaticCamCoors = m_pObject->GetPosition();
						CPickups::StaticCamStartTime = CTimer::GetTimeInMilliseconds();
					}
				}
				if (m_eType == PICKUP_ON_STREET) {
					m_nTimer = CTimer::GetTimeInMilliseconds() + 30000;
				} else if (m_eType == PICKUP_ON_STREET_SLOW) {
					if (MI_PICKUP_BRIBE == m_pObject->GetModelIndex())
						m_nTimer = CTimer::GetTimeInMilliseconds() + 300000;
					else
						m_nTimer = CTimer::GetTimeInMilliseconds() + 720000;
				}

				RemoveKeepType();
				return true;
			case PICKUP_ONCE:
			case PICKUP_ONCE_TIMEOUT:
				if (!CPickups::GivePlayerGoodiesWithPickUpMI(m_pObject->GetModelIndex(), playerId)) {
					if (CPickups::WeaponForModel(m_pObject->GetModelIndex())) {
						player->GiveWeapon(CPickups::WeaponForModel(m_pObject->GetModelIndex()), m_nQuantity != 0 ? m_nQuantity : AmmoForWeapon[CPickups::WeaponForModel(m_pObject->GetModelIndex())]);
						if (player->m_nSelectedWepSlot == player->GetWeaponSlot(WEAPONTYPE_UNARMED))
							player->m_nSelectedWepSlot = player->GetWeaponSlot(CPickups::WeaponForModel(m_pObject->GetModelIndex()));
					}
					DMAudio.PlayFrontEndSound(SOUND_PICKUP_WEAPON, m_pObject->GetModelIndex() - MI_GRENADE);
				}
				Remove();
				return true;
			case PICKUP_COLLECTABLE1:
				CWorld::Players[playerId].m_nCollectedPackages++;
				CWorld::Players[playerId].m_nMoney += 100;

				if (CWorld::Players[playerId].m_nCollectedPackages == CWorld::Players[playerId].m_nTotalPackages) {
					printf("All collectables have been picked up\n");
					CGarages::TriggerMessage("CO_ALL", -1, 5000, -1);
					CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 100000;
				} else
					CGarages::TriggerMessage("CO_ONE", CWorld::Players[CWorld::PlayerInFocus].m_nCollectedPackages, 5000, CWorld::Players[CWorld::PlayerInFocus].m_nTotalPackages);

				Remove();
				DMAudio.PlayFrontEndSound(SOUND_PICKUP_HIDDEN_PACKAGE, 0);
				return true;
			case PICKUP_MONEY:
				CWorld::Players[playerId].m_nMoney += m_nQuantity;
				sprintf(gString, "$%d", m_nQuantity);
#ifdef MONEY_MESSAGES
				CMoneyMessages::RegisterOne(m_vecPos + CVector(0.0f, 0.0f, 1.0f), gString, 0, 255, 0, 0.5f, 0.5f);
#endif
				Remove();
				DMAudio.PlayFrontEndSound(SOUND_PICKUP_MONEY, 0);
				return true;
			case PICKUP_ASSET_REVENUE:
				CWorld::Players[CWorld::PlayerInFocus].m_nMoney += m_nRevenue;
				m_nRevenue = 0;
				DMAudio.PlayFrontEndSound(SOUND_PICKUP_MONEY, 0);
				return false;
			// TODO(Miami): Control flow
			case PICKUP_PROPERTY_FORSALE:
				ModifyStringLabelForControlSetting(m_sTextKey);
				CMessages::InsertNumberInString(TheText.Get(m_sTextKey), m_nQuantity,
					0, 0, 0, 0, 0, gUString);
				if (!CHud::IsHelpMessageBeingDisplayed())
					CHud::SetHelpMessage(gUString, false); // 0, 0, 0);
				if (CPickups::CollectPickupBuffer == 0)
					return false;
				if (CTheScripts::IsPlayerOnAMission()) {
					CHud::SetHelpMessage(TheText.Get("PROP_2"), true); // , false);
				} else {
					if (CWorld::Players[CWorld::PlayerInFocus].m_nMoney >= m_nQuantity) {
						CWorld::Players[CWorld::PlayerInFocus].m_nMoney -= m_nQuantity;
						CHud::SetHelpMessage(nil, true); //, false);
						Remove();
						return true;
					}
					CHud::SetHelpMessage(TheText.Get("PROP_1"), true); //, false);
				}
				return false;
			default:
				break;
			}
		}
	} else {
		switch (m_eType)
		{
		case PICKUP_MINE_INACTIVE:
			if (vehicle != nil && !vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z, 2.0f)) {
				m_eType = PICKUP_MINE_ARMED;
				m_nTimer = CTimer::GetTimeInMilliseconds() + 10000;
			}
			break;
		case PICKUP_NAUTICAL_MINE_INACTIVE:
		{
			if (CWaterLevel::GetWaterLevel(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z + 5.0f, &waterLevel, false))
				m_pObject->GetMatrix().GetPosition().z = waterLevel + 0.6f;

			m_pObject->GetMatrix().UpdateRW();
			m_pObject->UpdateRwFrame();

			bool touched = false;
			for (int32 i = CPools::GetVehiclePool()->GetSize()-1; i >= 0; i--) {
				CVehicle *vehicle = CPools::GetVehiclePool()->GetSlot(i);
				if (vehicle != nil && vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z, 1.5f)) {
					touched = true;
					break; // added break here
				}
			}

			if (!touched) {
				m_eType = PICKUP_NAUTICAL_MINE_ARMED;
				m_nTimer = CTimer::GetTimeInMilliseconds() + 10000;
			}
			break;
		}
		case PICKUP_NAUTICAL_MINE_ARMED:
			if (CWaterLevel::GetWaterLevel(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z + 5.0f, &waterLevel, false))
				m_pObject->GetMatrix().GetPosition().z = waterLevel + 0.6f;

			m_pObject->GetMatrix().UpdateRW();
			m_pObject->UpdateRwFrame();
			// no break here
		case PICKUP_MINE_ARMED:
		{
			bool explode = false;
			if (CTimer::GetTimeInMilliseconds() > m_nTimer)
				explode = true;
			else {// added else here since vehicle lookup is useless
				for (int32 i = CPools::GetVehiclePool()->GetSize()-1; i >= 0; i--) {
					CVehicle *vehicle = CPools::GetVehiclePool()->GetSlot(i);
					if (vehicle != nil && vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z, 1.5f)) {
						explode = true;
						break; // added break here
					}
				}
			}
			if (explode) {
				CExplosion::AddExplosion(nil, nil, EXPLOSION_MINE, m_pObject->GetPosition(), 0);
				Remove();
			}
			break;
		}
		case PICKUP_FLOATINGPACKAGE:
			m_pObject->m_vecMoveSpeed.z -= 0.01f * CTimer::GetTimeStep();
			m_pObject->GetMatrix().GetPosition() += m_pObject->GetMoveSpeed() * CTimer::GetTimeStep();

			m_pObject->GetMatrix().UpdateRW();
			m_pObject->UpdateRwFrame();
			if (CWaterLevel::GetWaterLevel(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z + 5.0f, &waterLevel, 0) && waterLevel >= m_pObject->GetPosition().z)
				m_eType = PICKUP_FLOATINGPACKAGE_FLOATING;
			break;
		case PICKUP_FLOATINGPACKAGE_FLOATING:
			if (CWaterLevel::GetWaterLevel(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z + 5.0f, &waterLevel, 0))
				m_pObject->GetMatrix().GetPosition().z = waterLevel;

			m_pObject->GetMatrix().UpdateRW();
			m_pObject->UpdateRwFrame();
			if (vehicle != nil && vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition().x, m_pObject->GetPosition().y, m_pObject->GetPosition().z, 2.0f)) {
				Remove();
				DMAudio.PlayFrontEndSound(SOUND_PICKUP_FLOAT_PACKAGE, 0);
				return true;
			}
			break;
		default: break;
		}
	}
	if (!m_bRemoved && (m_eType == PICKUP_ONCE_TIMEOUT || m_eType == PICKUP_MONEY) && CTimer::GetTimeInMilliseconds() > m_nTimer)
		Remove();
	return false;
}

void
CPickups::Init(void)
{
	NumMessages = 0;
	for (int i = 0; i < NUMPICKUPS; i++) {
		aPickUps[i].m_eType = PICKUP_NONE;
		aPickUps[i].m_nIndex = 1;
		aPickUps[i].m_pObject = nil;
	}

	for (int i = 0; i < NUMCOLLECTEDPICKUPS; i++)
		aPickUpsCollected[i] = 0;

	CollectedPickUpIndex = 0;
}

// --MIAMI: Done
bool
CPickups::TestForPickupsInBubble(CVector pos, float range)
{
	for (int i = 0; i < NUMPICKUPS; i++) {
		if ((aPickUps[i].m_vecPos - pos).Magnitude() < range)
			return true;
	}
	return false;
}

// --MIAMI: Done
bool
CPickups::TryToMerge_WeaponType(CVector pos, eWeaponType weapon, uint8 type, uint32 quantity, bool unused) {
	for (int i = 0; i < NUMPICKUPS; i++) {
		if (aPickUps[i].m_eType == type && aPickUps[i].m_eModelIndex == ModelForWeapon(weapon))
			if ((aPickUps[i].m_vecPos - pos).Magnitude() < 7.5f) {
				aPickUps[i].m_nQuantity += quantity;
				return true;
			}
	}
	return false;
}

bool
CPickups::IsPickUpPickedUp(int32 pickupId)
{
	for (int i = 0; i < NUMCOLLECTEDPICKUPS; i++) {
		if (pickupId == aPickUpsCollected[i]) {
			aPickUpsCollected[i] = 0;
			return true;
		}
	}
	return false;
}

void
CPickups::PassTime(uint32 time)
{
	for (int i = 0; i < NUMPICKUPS; i++) {
		if (aPickUps[i].m_eType != PICKUP_NONE) {
			if (aPickUps[i].m_nTimer <= time)
				aPickUps[i].m_nTimer = 0;
			else
				aPickUps[i].m_nTimer -= time;
		}
	}
}

int32
CPickups::GetActualPickupIndex(int32 index)
{
	if (index == -1) return -1;

	// doesn't look nice
	if ((uint16)((index & 0xFFFF0000) >> 16) != aPickUps[(uint16)index].m_nIndex) return -1;
	return (uint16)index;
}

bool
CPickups::GivePlayerGoodiesWithPickUpMI(int16 modelIndex, int playerIndex)
{
	CPlayerPed *player;

	if (playerIndex <= 0) player = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	else player = CWorld::Players[playerIndex].m_pPed;

	if (modelIndex == MI_PICKUP_ADRENALINE) {
		player->m_bAdrenalineActive = true;
		player->m_nAdrenalineTime = CTimer::GetTimeInMilliseconds() + 20000;
		player->m_fCurrentStamina = player->m_fMaxStamina;
		DMAudio.PlayFrontEndSound(SOUND_PICKUP_ADRENALINE, 0);
		return true;
	} else if (modelIndex == MI_PICKUP_BODYARMOUR) {
		player->m_fArmour = CWorld::Players[playerIndex].m_nMaxArmour;
		DMAudio.PlayFrontEndSound(SOUND_PICKUP_ARMOUR, 0);
		return true;
	} else if (modelIndex == MI_PICKUP_INFO) {
		DMAudio.PlayFrontEndSound(SOUND_PICKUP_BONUS, 0);
		return true;
	} else if (modelIndex == MI_PICKUP_HEALTH) {
		player->m_fHealth = CWorld::Players[playerIndex].m_nMaxHealth;
		DMAudio.PlayFrontEndSound(SOUND_PICKUP_HEALTH, 0);
		return true;
	} else if (modelIndex == MI_PICKUP_BONUS) {
		DMAudio.PlayFrontEndSound(SOUND_PICKUP_BONUS, 0);
		return true;
	} else if (modelIndex == MI_PICKUP_BRIBE) {
		int32 level = FindPlayerPed()->m_pWanted->m_nWantedLevel - 1;
		if (level < 0) level = 0;
		player->SetWantedLevel(level);
		DMAudio.PlayFrontEndSound(SOUND_PICKUP_BONUS, 0);
		return true;
	} else if (modelIndex == MI_PICKUP_KILLFRENZY) {
		DMAudio.PlayFrontEndSound(SOUND_PICKUP_BONUS, 0);
		return true;
	}
	return false;
}

void
CPickups::RemoveAllFloatingPickups()
{
	for (int i = 0; i < NUMPICKUPS; i++) {
		if (aPickUps[i].m_eType == PICKUP_FLOATINGPACKAGE || aPickUps[i].m_eType == PICKUP_FLOATINGPACKAGE_FLOATING) {
			if (aPickUps[i].m_pObject) {
				CWorld::Remove(aPickUps[i].m_pObject);
				delete aPickUps[i].m_pObject;
				aPickUps[i].m_pObject = nil;
			}
		}
	}
}

void
CPickups::RemovePickUp(int32 pickupIndex)
{
	int32 index = CPickups::GetActualPickupIndex(pickupIndex);
	if (index == -1) return;

	if (aPickUps[index].m_pObject) {
		CWorld::Remove(aPickUps[index].m_pObject);
		delete aPickUps[index].m_pObject;
		aPickUps[index].m_pObject = nil;
	}
	aPickUps[index].m_eType = PICKUP_NONE;
	aPickUps[index].m_bRemoved = true;
}

int32
CPickups::GenerateNewOne(CVector pos, uint32 modelIndex, uint8 type, uint32 quantity, uint32 rate, bool highPriority, char* pText)
{
	bool bFreeFound = false;
	int32 slot = 0;

	if (type == PICKUP_FLOATINGPACKAGE || type == PICKUP_NAUTICAL_MINE_INACTIVE || highPriority) {
		for (slot = NUMPICKUPS-1; slot >= 0; slot--) {
			if (aPickUps[slot].m_eType == PICKUP_NONE) {
				bFreeFound = true;
				break;
			}
		}
	} else {
		for (slot = 0; slot < NUMGENERALPICKUPS; slot++) {
			if (aPickUps[slot].m_eType == PICKUP_NONE) {
				bFreeFound = true;
				break;
			}
		}
	}

	if (!bFreeFound) {
		for (slot = 0; slot < NUMGENERALPICKUPS; slot++) {
			if (aPickUps[slot].m_eType == PICKUP_MONEY) break;
		}

		if (slot >= NUMGENERALPICKUPS) {
			for (slot = 0; slot < NUMGENERALPICKUPS; slot++) {
				if (aPickUps[slot].m_eType == PICKUP_ONCE_TIMEOUT || aPickUps[slot].m_eType == PICKUP_ONCE_TIMEOUT_SLOW) break;
			}

			if (slot >= NUMGENERALPICKUPS) return -1;
		}
	}

	if (slot >= NUMPICKUPS) return -1;

	aPickUps[slot].m_eType = (ePickupType)type;
	aPickUps[slot].m_bRemoved = false;
	aPickUps[slot].m_nQuantity = quantity;
	aPickUps[slot].m_nMoneySpeed = rate;
	aPickUps[slot].m_nRevenue = 0;
	aPickUps[slot].m_nTimer = CTimer::GetTimeInMilliseconds();
	if (type == PICKUP_ONCE_TIMEOUT)
		aPickUps[slot].m_nTimer = CTimer::GetTimeInMilliseconds() + 20000;
	else if (type == PICKUP_ONCE_TIMEOUT_SLOW)
		aPickUps[slot].m_nTimer = CTimer::GetTimeInMilliseconds() + 120000;
	else if (type == PICKUP_MONEY)
		aPickUps[slot].m_nTimer = CTimer::GetTimeInMilliseconds() + 30000;
	else if (type == PICKUP_MINE_INACTIVE || type == PICKUP_MINE_ARMED) {
		aPickUps[slot].m_eType = PICKUP_MINE_INACTIVE;
		aPickUps[slot].m_nTimer = CTimer::GetTimeInMilliseconds() + 1500;
	} else if (type == PICKUP_NAUTICAL_MINE_INACTIVE || type == PICKUP_NAUTICAL_MINE_ARMED) {
		aPickUps[slot].m_eType = PICKUP_NAUTICAL_MINE_INACTIVE;
		aPickUps[slot].m_nTimer = CTimer::GetTimeInMilliseconds() + 1500;
	}
	aPickUps[slot].m_eModelIndex = modelIndex;
	if (pText)
		strncpy(aPickUps[slot].m_sTextKey, pText, 8);
	else
		aPickUps[slot].m_sTextKey[0] = '\0';

	aPickUps[slot].m_vecPos = pos;
	aPickUps[slot].m_pObject = aPickUps[slot].GiveUsAPickUpObject(-1);
	if (aPickUps[slot].m_pObject)
		CWorld::Add(aPickUps[slot].m_pObject);
	return GetNewUniquePickupIndex(slot);
}

int32
CPickups::GenerateNewOne_WeaponType(CVector pos, eWeaponType weaponType, uint8 type, uint32 quantity)
{
	return GenerateNewOne(pos, ModelForWeapon(weaponType), type, quantity);
}

int32
CPickups::GetNewUniquePickupIndex(int32 slot)
{
	if (aPickUps[slot].m_nIndex >= 0xFFFE)
		aPickUps[slot].m_nIndex = 1;
	else
		aPickUps[slot].m_nIndex++;
	return slot | (aPickUps[slot].m_nIndex << 16);
}

// --MIAMI: Done
int32
CPickups::ModelForWeapon(eWeaponType weaponType)
{
	return CWeaponInfo::GetWeaponInfo(weaponType)->m_nModelId;
}

// --MIAMI: Done
eWeaponType
CPickups::WeaponForModel(int32 model)
{
	if (model == MI_PICKUP_BODYARMOUR) return WEAPONTYPE_ARMOUR;
	if (model == MI_PICKUP_HEALTH) return WEAPONTYPE_HEALTH;
	if (model == MI_PICKUP_ADRENALINE) return WEAPONTYPE_ARMOUR;
	if (model == -1) return WEAPONTYPE_UNARMED;
	return (eWeaponType)((CWeaponModelInfo*)CModelInfo::GetModelInfo(model))->GetWeaponInfo();
}

void
CPickups::AddToCollectedPickupsArray(int32 index)
{
	aPickUpsCollected[CollectedPickUpIndex++] = index | (aPickUps[index].m_nIndex << 16);
	if (CollectedPickUpIndex >= NUMCOLLECTEDPICKUPS)
		CollectedPickUpIndex = 0;
}

void
CPickups::Update()
{
#ifdef FIX_BUGS // RIP speedrunning (solution from SA)
	if (CReplay::IsPlayingBack())
		return;
#endif
#ifdef CAMERA_PICKUP
	if ( bPickUpcamActivated ) // taken from PS2
	{
		float dist = (FindPlayerCoors() - StaticCamCoors).Magnitude2D();
		float mult;
		if ( dist < 10.0f )
			mult = 1.0f - (dist / 10.0f );
		else
			mult = 0.0f;

		CVector pos = StaticCamCoors;
		pos.z += (pPlayerVehicle->GetColModel()->boundingBox.GetSize().z + 2.0f) * mult;

		if ( (CTimer::GetTimeInMilliseconds() - StaticCamStartTime) > 750 )
		{
			TheCamera.SetCamPositionForFixedMode(pos, CVector(0.0f, 0.0f, 0.0f));
			TheCamera.TakeControl(FindPlayerVehicle(), CCam::MODE_FIXED, JUMP_CUT, CAMCONTROL_SCRIPT);
		}

		if ( FindPlayerVehicle() != pPlayerVehicle
			|| (FindPlayerCoors() - StaticCamCoors).Magnitude() > 40.0f
			|| ((CTimer::GetTimeInMilliseconds() - StaticCamStartTime) > 60000) )
		{
			TheCamera.RestoreWithJumpCut();
			bPickUpcamActivated = false;
		}
	}
#endif
	if (CPad::GetPad(0)->CollectPickupJustDown()) {
		CollectPickupBuffer = 6;
	} else {
		CollectPickupBuffer = Max(0, CollectPickupBuffer - 1);
	}

	if (PlayerOnWeaponPickup) {
		PlayerOnWeaponPickup = Max(0, PlayerOnWeaponPickup - 1);
	}

#define PICKUPS_FRAME_SPAN (6)
#ifdef FIX_BUGS
	for (uint32 i = NUMGENERALPICKUPS * (CTimer::GetFrameCounter() % PICKUPS_FRAME_SPAN) / PICKUPS_FRAME_SPAN; i < NUMGENERALPICKUPS * (CTimer::GetFrameCounter() % PICKUPS_FRAME_SPAN + 1) / PICKUPS_FRAME_SPAN; i++) {
#else // BUG: this code can only reach 318 out of 320 pickups
	for (uint32 i = NUMGENERALPICKUPS / PICKUPS_FRAME_SPAN * (CTimer::GetFrameCounter() % PICKUPS_FRAME_SPAN); i < NUMGENERALPICKUPS / PICKUPS_FRAME_SPAN * (CTimer::GetFrameCounter() % PICKUPS_FRAME_SPAN + 1); i++) {
#endif
		if (aPickUps[i].m_eType != PICKUP_NONE && aPickUps[i].Update(FindPlayerPed(), FindPlayerVehicle(), CWorld::PlayerInFocus)) {
			AddToCollectedPickupsArray(i);
		}
	}
#undef PICKUPS_FRAME_SPAN
	for (uint32 i = NUMGENERALPICKUPS; i < NUMPICKUPS; i++) {
		if (aPickUps[i].m_eType != PICKUP_NONE && aPickUps[i].Update(FindPlayerPed(), FindPlayerVehicle(), CWorld::PlayerInFocus)) {
			AddToCollectedPickupsArray(i);
		}
	}
}

void
CPickups::DoPickUpEffects(CEntity *entity)
{
	if (entity->GetModelIndex() == MI_PICKUP_KILLFRENZY)
		entity->bDoNotRender = CTheScripts::IsPlayerOnAMission() || CDarkel::FrenzyOnGoing() || !CGame::nastyGame;

	if (!entity->bDoNotRender) {
		float s = Sin((float)((CTimer::GetTimeInMilliseconds() + (uintptr)entity) & 0x7FF) * DEGTORAD(360.0f / 0x800));
		float modifiedSin = 0.3f * (s + 1.0f);


		int16 colorId;
		bool doInnerGlow = false;
		bool doOuterGlow = true;

		if (entity->GetModelIndex() == MI_PICKUP_ADRENALINE || entity->GetModelIndex() == MI_PICKUP_CAMERA) {
			colorId = WEAPONTYPE_TOTALWEAPONS;
			doInnerGlow = true;
			doOuterGlow = false;
		} else if (entity->GetModelIndex() == MI_PICKUP_BODYARMOUR) {
			colorId = WEAPONTYPE_TOTALWEAPONS + 1;
		} else if (entity->GetModelIndex() == MI_PICKUP_BRIBE) {
			doInnerGlow = true;
			doOuterGlow = false;
		} else if (entity->GetModelIndex() == MI_PICKUP_INFO || entity->GetModelIndex() == MI_PICKUP_KILLFRENZY) {
			colorId = WEAPONTYPE_TOTALWEAPONS + 2;
			doInnerGlow = true;
			doOuterGlow = false;
		} else if (entity->GetModelIndex() == MI_PICKUP_HEALTH || entity->GetModelIndex() == MI_PICKUP_BONUS) {
			colorId = WEAPONTYPE_TOTALWEAPONS;
			doInnerGlow = true;
			doOuterGlow = false;
		} else
			colorId = WeaponForModel(entity->GetModelIndex());

		const CVector& pos = entity->GetPosition();
		if (doOuterGlow) {
			float colorModifier = ((CGeneral::GetRandomNumber() & 0x1F) * 0.015f + 1.0f) * modifiedSin * 0.15f;
			CShadows::StoreStaticShadow((uintptr)entity, SHADOWTYPE_ADDITIVE, gpShadowExplosionTex, &pos, 2.0f, 0.0f, 0.0f, -2.0f, 0,
		                            aWeaponReds[colorId] * colorModifier, aWeaponGreens[colorId] * colorModifier, aWeaponBlues[colorId] * colorModifier, 4.0f,
		                            1.0f, 40.0f, false, 0.0f);

			float radius = (CGeneral::GetRandomNumber() & 0xF) * 0.1f + 3.0f;
			CPointLights::AddLight(CPointLights::LIGHT_POINT, pos, CVector(0.0f, 0.0f, 0.0f), radius, aWeaponReds[colorId] * modifiedSin / 256.0f, aWeaponGreens[colorId] * modifiedSin / 256.0f, aWeaponBlues[colorId] * modifiedSin / 256.0f, CPointLights::FOG_NONE, true);
			float size = (CGeneral::GetRandomNumber() & 0xF) * 0.0005f + 0.6f;
			CCoronas::RegisterCorona((uintptr)entity,
				aWeaponReds[colorId] * modifiedSin / 2.0f, aWeaponGreens[colorId] * modifiedSin / 2.0f, aWeaponBlues[colorId] * modifiedSin / 2.0f,
				255,
				pos,
				size, 65.0f, CCoronas::TYPE_RING, CCoronas::FLARE_NONE, CCoronas::REFLECTION_OFF, CCoronas::LOSCHECK_OFF, CCoronas::STREAK_OFF, 0.0f);
		}

		CObject *object = (CObject*)entity;
		if (object->bPickupObjWithMessage || object->bOutOfStock || object->m_nBonusValue) {
			float dist = (TheCamera.GetPosition() - pos).Magnitude();
			const float MAXDIST = 12.0f;

			if (dist < MAXDIST && NumMessages < NUMPICKUPMESSAGES) {
				RwV3d vecOut;
				float fDistX, fDistY;
				if (CSprite::CalcScreenCoors(entity->GetPosition() + CVector(0.0f, 0.0f, 0.7f), &vecOut, &fDistX, &fDistY, true)) {
					aMessages[NumMessages].m_pos.x = vecOut.x;
					aMessages[NumMessages].m_pos.y = vecOut.y;
					aMessages[NumMessages].m_dist.x = fDistX;
					aMessages[NumMessages].m_dist.y = fDistY;
					aMessages[NumMessages].m_weaponType = WeaponForModel(entity->GetModelIndex());
					aMessages[NumMessages].m_color.red = aWeaponReds[colorId];
					aMessages[NumMessages].m_color.green = aWeaponGreens[colorId];
					aMessages[NumMessages].m_color.blue = aWeaponBlues[colorId];
					aMessages[NumMessages].m_color.alpha = (1.0f - dist / MAXDIST) * 128.0f;
					aMessages[NumMessages].m_bOutOfStock = object->bOutOfStock;
					aMessages[NumMessages].m_quantity = object->m_nBonusValue;
					NumMessages++;
				}
			}
		}

		uint32 model = entity->GetModelIndex();
		CColModel* colModel = entity->GetColModel();
		CVector colLength = colModel->boundingBox.max - colModel->boundingBox.min;

		float scale = (Max(1.f, 1.2f / Max(colLength.x, Max(colLength.y, colLength.z))) - 1.0f) * 0.6f + 1.0f;
		if (model == MI_MINIGUN || model == MI_MINIGUN2)
			scale = 1.2f;

		entity->GetMatrix().SetRotateZOnlyScaled((float)(CTimer::GetTimeInMilliseconds() & 0x7FF) * DEGTORAD(360.0f / 0x800), scale);

		if (doInnerGlow)
			CCoronas::RegisterCorona((uintptr)entity + 1, 126, 69, 121, 255, entity->GetPosition(), 1.2f, 50.0f,
				CCoronas::TYPE_STAR, CCoronas::FLARE_NONE, CCoronas::REFLECTION_ON, CCoronas::LOSCHECK_OFF, CCoronas::STREAK_ON, 0.f, false);
	}
}

void
CPickups::DoMineEffects(CEntity *entity)
{
	const CVector &pos = entity->GetPosition();
	float dist = (TheCamera.GetPosition() - pos).Magnitude();
	const float MAXDIST = 20.0f;

	if (dist < MAXDIST) {
		float s = Sin((float)((CTimer::GetTimeInMilliseconds() + (uintptr)entity) & 0x1FF) * DEGTORAD(360.0f / 0x200));

		int32 red = (MAXDIST - dist) * (0.5f * s + 0.5f) / MAXDIST * 64.0f;
		CShadows::StoreStaticShadow((uintptr)entity, SHADOWTYPE_ADDITIVE, gpShadowExplosionTex, &pos, 2.0f, 0.0f, 0.0f, -2.0f, 0, red, 0, 0, 4.0f, 1.0f, 40.0f,
		                            false, 0.0f);
		CCoronas::RegisterCorona((uintptr)entity, red, 0, 0, 255, pos, 0.6f, 60.0f, CCoronas::TYPE_RING, CCoronas::FLARE_NONE, CCoronas::REFLECTION_OFF, CCoronas::LOSCHECK_OFF, CCoronas::STREAK_OFF, 0.0f);
	}

	entity->GetMatrix().SetRotateZOnly((float)(CTimer::GetTimeInMilliseconds() & 0x3FF) * DEGTORAD(360.0f / 0x400));
}

void
CPickups::DoMoneyEffects(CEntity *entity)
{
	const CVector &pos = entity->GetPosition();
	float dist = (TheCamera.GetPosition() - pos).Magnitude();
	const float MAXDIST = 20.0f;

	if (dist < MAXDIST) {
		float s = Sin((float)((CTimer::GetTimeInMilliseconds() + (uintptr)entity) & 0x3FF) * DEGTORAD(360.0f / 0x400));

		int32 green = (MAXDIST - dist) * (0.2f * s + 0.3f) / MAXDIST * 64.0f;
		CShadows::StoreStaticShadow((uintptr)entity, SHADOWTYPE_ADDITIVE, gpShadowExplosionTex, &pos, 2.0f, 0.0f, 0.0f, -2.0f, 0, 0, green, 0, 4.0f, 1.0f,
		                            40.0f, false, 0.0f);
		CCoronas::RegisterCorona((uintptr)entity, 0, green, 0, 255, pos, 0.4f, 40.0f, CCoronas::TYPE_RING, CCoronas::FLARE_NONE, CCoronas::REFLECTION_OFF, CCoronas::LOSCHECK_OFF, CCoronas::STREAK_OFF, 0.0f);
	}

	entity->GetMatrix().SetRotateZOnly((float)(CTimer::GetTimeInMilliseconds() & 0x7FF) * DEGTORAD(360.0f / 0x800));
}

void
CPickups::DoCollectableEffects(CEntity *entity)
{
	const CVector &pos = entity->GetPosition();
	float dist = (TheCamera.GetPosition() - pos).Magnitude();
	const float MAXDIST = 14.0f;

	if (dist < MAXDIST) {
		float s = Sin((float)((CTimer::GetTimeInMilliseconds() + (uintptr)entity) & 0x7FF) * DEGTORAD(360.0f / 0x800));

		int32 color = (MAXDIST - dist) * (0.5f * s + 0.5f) / MAXDIST * 255.0f;
		CShadows::StoreStaticShadow((uintptr)entity, SHADOWTYPE_ADDITIVE, gpShadowExplosionTex, &pos, 2.0f, 0.0f, 0.0f, -2.0f, 0, color, color, color, 4.0f,
		                            1.0f, 40.0f, false, 0.0f);
		CCoronas::RegisterCorona((uintptr)entity, color, color, color, 255, pos, 0.6f, 40.0f, CCoronas::TYPE_RING, CCoronas::FLARE_NONE, CCoronas::REFLECTION_OFF, CCoronas::LOSCHECK_OFF, CCoronas::STREAK_OFF, 0.0f);
	}

	entity->GetMatrix().SetRotateZOnly((float)(CTimer::GetTimeInMilliseconds() & 0xFFF) * DEGTORAD(360.0f / 0x1000));
}

void
CPickups::RenderPickUpText()
{
	wchar *strToPrint;
	for (int32 i = 0; i < NumMessages; i++) {
		if (aMessages[i].m_quantity <= 39) {
			switch (aMessages[i].m_quantity) // could use some enum maybe
			{
			case 0:
				if (aMessages[i].m_weaponType == WEAPONTYPE_TOTALWEAPONS) { // unreachable code?
					// what is this??
					sprintf(gString, "%d/%d", CWorld::Players[CWorld::PlayerInFocus].m_nCollectedPackages, 2903);
					strToPrint = nil;
				} else {
					if (aMessages[i].m_bOutOfStock)
						strToPrint = TheText.Get("STOCK");
					else {
						sprintf(gString, "$%d", CostOfWeapon[aMessages[i].m_weaponType]);
						AsciiToUnicode(gString, gUString);
						strToPrint = gUString;
					}
				}
				break;
			case 1:
				strToPrint = TheText.Get("SECURI");
				break;
			case 2:
				strToPrint = TheText.Get("MOONBM");
				break;
			case 3:
				strToPrint = TheText.Get("COACH");
				break;
			case 4:
				strToPrint = TheText.Get("FLATBED");
				break;
			case 5:
				strToPrint = TheText.Get("LINERUN");
				break;
			case 6:
				strToPrint = TheText.Get("TRASHM");
				break;
			case 7:
				strToPrint = TheText.Get("PATRIOT");
				break;
			case 8:
				strToPrint = TheText.Get("WHOOPEE");
				break;
			case 9:
				strToPrint = TheText.Get("BLISTA");
				break;
			case 10:
				strToPrint = TheText.Get("MULE");
				break;
			case 11:
				strToPrint = TheText.Get("YANKEE");
				break;
			case 12:
				strToPrint = TheText.Get("BOBCAT");
				break;
			case 13:
				strToPrint = TheText.Get("DODO");
				break;
			case 14:
				strToPrint = TheText.Get("BUS");
				break;
			case 15:
				strToPrint = TheText.Get("RUMPO");
				break;
			case 16:
				strToPrint = TheText.Get("PONY");
				break;
			case 17:
				strToPrint = TheText.Get("SENTINL");
				break;
			case 18:
				strToPrint = TheText.Get("CHEETAH");
				break;
			case 19:
				strToPrint = TheText.Get("BANSHEE");
				break;
			case 20:
				strToPrint = TheText.Get("IDAHO");
				break;
			case 21:
				strToPrint = TheText.Get("INFERNS");
				break;
			case 22:
				strToPrint = TheText.Get("TAXI");
				break;
			case 23:
				strToPrint = TheText.Get("KURUMA");
				break;
			case 24:
				strToPrint = TheText.Get("STRETCH");
				break;
			case 25:
				strToPrint = TheText.Get("PEREN");
				break;
			case 26:
				strToPrint = TheText.Get("STINGER");
				break;
			case 27:
				strToPrint = TheText.Get("MANANA");
				break;
			case 28:
				strToPrint = TheText.Get("LANDSTK");
				break;
			case 29:
				strToPrint = TheText.Get("STALION");
				break;
			case 30:
				strToPrint = TheText.Get("BFINJC");
				break;
			case 31:
				strToPrint = TheText.Get("CABBIE");
				break;
			case 32:
				strToPrint = TheText.Get("ESPERAN");
				break;
			case 33:
				strToPrint = TheText.Get("FIRETRK");
				break;
			case 34:
				strToPrint = TheText.Get("AMBULAN");
				break;
			case 35:
				strToPrint = TheText.Get("ENFORCR");
				break;
			case 36:
				strToPrint = TheText.Get("FBICAR");
				break;
			case 37:
				strToPrint = TheText.Get("RHINO");
				break;
			case 38:
				strToPrint = TheText.Get("BARRCKS");
				break;
			case 39:
				strToPrint = TheText.Get("POLICAR");
				break;
			default:
				break;
			}
		}
		if (strToPrint == nil)
			continue;
		CFont::SetPropOn();
		CFont::SetBackgroundOff();

		const float MAX_SCALE = 1.0f;

		float fScaleY = aMessages[i].m_dist.y / 100.0f;
		if (fScaleY > MAX_SCALE) fScaleY = MAX_SCALE;

		float fScaleX = aMessages[i].m_dist.x / 100.0f;
		if (fScaleX > MAX_SCALE) fScaleX = MAX_SCALE;

		CFont::SetScale(fScaleX, fScaleY);
		CFont::SetCentreOn();
		CFont::SetCentreSize(SCREEN_WIDTH);
		CFont::SetJustifyOff();

		CFont::SetColor(CRGBA(aMessages[i].m_color.red, aMessages[i].m_color.green, aMessages[i].m_color.blue, aMessages[i].m_color.alpha));
		CFont::SetBackGroundOnlyTextOff();
		CFont::SetFontStyle(FONT_STANDARD);
		CFont::PrintString(aMessages[i].m_pos.x, aMessages[i].m_pos.y, strToPrint);
	}
	NumMessages = 0;
}

void
CPickups::CreateSomeMoney(CVector pos, int money)
{
	bool found;

	int pickupCount = Min(money / 20 + 1, 7);
	int moneyPerPickup = money / pickupCount;

	for (int i = 0; i < pickupCount; i++) {
		// (CGeneral::GetRandomNumber() % 256) * PI / 128 gives a float up to something TWOPI-ish.
		pos.x += 1.5f * Sin((CGeneral::GetRandomNumber() % 256) * PI / 128);
		pos.y += 1.5f * Cos((CGeneral::GetRandomNumber() % 256) * PI / 128);
		pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z, &found) + 0.5f;
		if (found) {
			CPickups::GenerateNewOne(CVector(pos.x, pos.y, pos.z), MI_MONEY, PICKUP_MONEY, moneyPerPickup + (CGeneral::GetRandomNumber() & 3));
		}
	}
}

void
CPickups::Load(uint8 *buf, uint32 size)
{
INITSAVEBUF

	for (int32 i = 0; i < NUMPICKUPS; i++) {
		aPickUps[i] = ReadSaveBuf<CPickup>(buf);

		if (aPickUps[i].m_eType != PICKUP_NONE && aPickUps[i].m_pObject != nil)
			aPickUps[i].m_pObject = CPools::GetObjectPool()->GetSlot((uintptr)aPickUps[i].m_pObject - 1);
	}

	CollectedPickUpIndex = ReadSaveBuf<uint16>(buf);
	ReadSaveBuf<uint16>(buf);
	NumMessages = 0;

	for (uint16 i = 0; i < NUMCOLLECTEDPICKUPS; i++)
		aPickUpsCollected[i] = ReadSaveBuf<int32>(buf);

VALIDATESAVEBUF(size)
}

void
CPickups::Save(uint8 *buf, uint32 *size)
{
	*size = sizeof(aPickUps) + sizeof(uint16) + sizeof(uint16) + sizeof(aPickUpsCollected);

INITSAVEBUF

	for (int32 i = 0; i < NUMPICKUPS; i++) {
		CPickup *buf_pickup = WriteSaveBuf(buf, aPickUps[i]);
		if (buf_pickup->m_eType != PICKUP_NONE && buf_pickup->m_pObject != nil)
			buf_pickup->m_pObject = (CObject*)(CPools::GetObjectPool()->GetJustIndex(buf_pickup->m_pObject) + 1);
	}

	WriteSaveBuf(buf, CollectedPickUpIndex);
	WriteSaveBuf(buf, (uint16)0); // possibly was NumMessages

	for (uint16 i = 0; i < NUMCOLLECTEDPICKUPS; i++)
		WriteSaveBuf(buf, aPickUpsCollected[i]);

VALIDATESAVEBUF(*size)
}

void
CPacManPickup::Update()
{
	if (FindPlayerVehicle() == nil) return;

	CVehicle *veh = FindPlayerVehicle();

	if (DistanceSqr2D(FindPlayerVehicle()->GetPosition(), m_vecPosn.x, m_vecPosn.y) < 100.0f && veh->IsSphereTouchingVehicle(m_vecPosn.x, m_vecPosn.y, m_vecPosn.z, 1.5f)) {
		switch (m_eType)
		{
		case PACMAN_SCRAMBLE:
		{
			veh->m_nPacManPickupsCarried++;
			veh->m_vecMoveSpeed *= 0.65f;
			float massMult = (veh->m_fMass + 250.0f) / veh->m_fMass;
			veh->m_fMass *= massMult;
			veh->m_fTurnMass *= massMult;
			veh->m_fForceMultiplier *= massMult;
			FindPlayerPed()->m_pWanted->m_nChaos += 10;
			FindPlayerPed()->m_pWanted->UpdateWantedLevel();
			DMAudio.PlayFrontEndSound(SOUND_PICKUP_PACMAN_PACKAGE, 0);
			break;
		}
		case PACMAN_RACE:
			CPacManPickups::PillsEatenInRace++;
			DMAudio.PlayFrontEndSound(SOUND_PICKUP_PACMAN_PILL, 0);
			break;
		default:
			break;
		}
		m_eType = PACMAN_NONE;
		if (m_pObject != nil) {
			CWorld::Remove(m_pObject);
			delete m_pObject;
			m_pObject = nil;
		}
	}
}

int32 CollectGameState;
int16 ThingsToCollect;

CPacManPickup CPacManPickups::aPMPickUps[NUMPACMANPICKUPS];
CVector CPacManPickups::LastPickUpCoors;
int32 CPacManPickups::PillsEatenInRace;
bool CPacManPickups::bPMActive;

void
CPacManPickups::Init()
{
	for (int i = 0; i < NUMPACMANPICKUPS; i++)
		aPMPickUps[i].m_eType = PACMAN_NONE;
	bPMActive = false;
}

void
CPacManPickups::Update()
{
	if (FindPlayerVehicle()) {
		float dist = Distance(FindPlayerCoors(), CVector(1072.0f, -948.0f, 14.5f));
		switch (CollectGameState) {
		case 1:
			if (dist < 10.0f) {
				ThingsToCollect -= FindPlayerVehicle()->m_nPacManPickupsCarried;
				FindPlayerVehicle()->m_nPacManPickupsCarried = 0;
				FindPlayerVehicle()->m_fMass /= FindPlayerVehicle()->m_fForceMultiplier;
				FindPlayerVehicle()->m_fTurnMass /= FindPlayerVehicle()->m_fForceMultiplier;
				FindPlayerVehicle()->m_fForceMultiplier = 1.0f;
			}
			if (ThingsToCollect <= 0) {
				CollectGameState = 2;
				ClearPMPickUps();
			}
			break;
		case 2:
			if (dist > 11.0f)
				CollectGameState = 0;
			break;
		case 20:
			if (Distance(FindPlayerCoors(), LastPickUpCoors) > 30.0f) {
				LastPickUpCoors = FindPlayerCoors();
				printf("%f, %f, %f,\n", LastPickUpCoors.x, LastPickUpCoors.y, LastPickUpCoors.z);
			}
			break;
		default:
			break;
		}
	}
	if (bPMActive) {
#define PACMANPICKUPS_FRAME_SPAN (4)
		for (uint32 i = (CTimer::GetFrameCounter() % PACMANPICKUPS_FRAME_SPAN) * (NUMPACMANPICKUPS / PACMANPICKUPS_FRAME_SPAN); i < ((CTimer::GetFrameCounter() % PACMANPICKUPS_FRAME_SPAN) + 1) * (NUMPACMANPICKUPS / PACMANPICKUPS_FRAME_SPAN); i++) {
			if (aPMPickUps[i].m_eType != PACMAN_NONE)
				aPMPickUps[i].Update();
		}
#undef PACMANPICKUPS_FRAME_SPAN
	}
}

void
CPacManPickups::GeneratePMPickUps(CVector pos, float scrambleMult, int16 count, uint8 type)
{
}

// diablo porn mission pickups
static const CVector aRacePoints1[] = {
	CVector(913.62219f, -155.13692f, 4.9699469f),
	CVector(913.92401f, -124.12943f, 4.9692569f),
	CVector(913.27899f, -93.524231f, 7.4325991f),
	CVector(912.60852f, -63.15905f, 7.4533591f),
	CVector(934.22144f, -42.049122f, 7.4511471f),
	CVector(958.88092f, -23.863735f, 7.4652338f),
	CVector(978.50812f, -0.78458798f, 5.13515f),
	CVector(1009.4175f, -2.1041219f, 2.4461579f),
	CVector(1040.6313f, -2.0793829f, 2.293175f),
	CVector(1070.7863f, -2.084095f, 2.2789791f),
	CVector(1100.5773f, -8.468729f, 5.3248072f),
	CVector(1119.9341f, -31.738031f, 7.1913071f),
	CVector(1122.1664f, -62.762737f, 7.4703908f),
	CVector(1122.814f, -93.650566f, 8.5577497f),
	CVector(1125.8253f, -124.26616f, 9.9803305f),
	CVector(1153.8727f, -135.47169f, 14.150617f),
	CVector(1184.0831f, -135.82845f, 14.973998f),
	CVector(1192.0432f, -164.57816f, 19.18627f),
	CVector(1192.7761f, -194.28871f, 24.799675f),
	CVector(1215.1527f, -215.0714f, 25.74975f),
	CVector(1245.79f, -215.39304f, 28.70726f),
	CVector(1276.2477f, -216.39485f, 33.71236f),
	CVector(1306.5535f, -216.71007f, 39.711472f),
	CVector(1335.0244f, -224.59329f, 46.474979f),
	CVector(1355.4879f, -246.27664f, 49.934841f),
	CVector(1362.6003f, -276.47064f, 49.96265f),
	CVector(1363.027f, -307.30847f, 49.969173f),
	CVector(1365.343f, -338.08609f, 49.967789f),
	CVector(1367.5957f, -368.01105f, 50.092304f),
	CVector(1368.2749f, -398.38049f, 50.061268f),
	CVector(1366.9034f, -429.98483f, 50.057545f),
	CVector(1356.8534f, -459.09259f, 50.035545f),
	CVector(1335.5819f, -481.13544f, 47.217903f),
	CVector(1306.7552f, -491.07443f, 40.202629f),
	CVector(1275.5978f, -491.33194f, 33.969223f),
	CVector(1244.702f, -491.46451f, 29.111021f),
	CVector(1213.2222f, -491.8754f, 25.771168f),
	CVector(1182.7729f, -492.19995f, 24.749964f),
	CVector(1152.6874f, -491.42221f, 21.70038f),
	CVector(1121.5352f, -491.94604f, 20.075182f),
	CVector(1090.7056f, -492.63751f, 17.585758f),
	CVector(1059.6008f, -491.65762f, 14.848632f),
	CVector(1029.113f, -489.66031f, 14.918498f),
	CVector(998.20679f, -486.78107f, 14.945688f),
	CVector(968.00555f, -484.91266f, 15.001229f),
	CVector(937.74939f, -492.09015f, 14.958629f),
	CVector(927.17352f, -520.97736f, 14.972308f),
	CVector(929.29749f, -552.08643f, 14.978855f),
	CVector(950.69525f, -574.47778f, 14.972788f),
	CVector(974.02826f, -593.56024f, 14.966445f),
	CVector(989.04779f, -620.12854f, 14.951016f),
	CVector(1014.1639f, -637.3905f, 14.966736f),
	CVector(1017.5961f, -667.3736f, 14.956415f),
	CVector(1041.9735f, -685.94391f, 15.003841f),
	CVector(1043.3064f, -716.11298f, 14.974236f),
	CVector(1043.5337f, -746.63855f, 14.96919f),
	CVector(1044.142f, -776.93823f, 14.965424f),
	CVector(1044.2657f, -807.29395f, 14.97171f),
	CVector(1017.0797f, -820.1076f, 14.975431f),
	CVector(986.23865f, -820.37103f, 14.972883f),
	CVector(956.10065f, -820.23291f, 14.981133f),
	CVector(925.86914f, -820.19049f, 14.976553f),
	CVector(897.69702f, -831.08734f, 14.962709f),
	CVector(868.06586f, -835.99237f, 14.970685f),
	CVector(836.93054f, -836.84387f, 14.965049f),
	CVector(811.63586f, -853.7915f, 15.067576f),
	CVector(811.46344f, -884.27368f, 12.247812f),
	CVector(811.60651f, -914.70959f, 9.2393751f),
	CVector(811.10425f, -945.16272f, 5.817255f),
	CVector(816.54584f, -975.64587f, 4.998558f),
	CVector(828.2951f, -1003.3685f, 5.0471172f),
	CVector(852.28839f, -1021.5963f, 4.9371028f),
	CVector(882.50067f, -1025.4459f, 5.14077f),
	CVector(912.84821f, -1026.7874f, 8.3415451f),
	CVector(943.68274f, -1026.6914f, 11.341879f),
	CVector(974.4129f, -1027.3682f, 14.410345f),
	CVector(1004.1079f, -1036.0778f, 14.92961f),
	CVector(1030.1144f, -1051.1224f, 14.850387f),
	CVector(1058.7585f, -1060.342f, 14.821624f),
	CVector(1087.7797f, -1068.3263f, 14.800561f),
	CVector(1099.8807f, -1095.656f, 11.877907f),
	CVector(1130.0005f, -1101.994f, 11.853914f),
	CVector(1160.3809f, -1101.6355f, 11.854824f),
	CVector(1191.8524f, -1102.1577f, 11.853843f),
	CVector(1223.3307f, -1102.7448f, 11.852233f),
	CVector(1253.564f, -1098.1045f, 11.853944f),
	CVector(1262.0203f, -1069.1785f, 14.8147f),
	CVector(1290.9998f, -1059.1882f, 14.816016f),
	CVector(1316.246f, -1041.0635f, 14.81109f),
	CVector(1331.7539f, -1013.835f, 14.81207f),
	CVector(1334.0579f, -983.55402f, 14.827253f),
	CVector(1323.2429f, -954.23083f, 14.954678f),
	CVector(1302.7495f, -932.21216f, 14.962917f),
	CVector(1317.418f, -905.89325f, 14.967506f),
	CVector(1337.9503f, -883.5025f, 14.969675f),
	CVector(1352.6929f, -855.96954f, 14.967854f),
	CVector(1357.2388f, -826.26971f, 14.97295f),
	CVector(1384.8668f, -812.47693f, 12.907736f),
	CVector(1410.8983f, -795.39056f, 12.052228f),
	CVector(1433.901f, -775.55811f, 11.96265f),
	CVector(1443.8615f, -746.92511f, 11.976114f),
	CVector(1457.7015f, -720.00903f, 11.971177f),
	CVector(1481.5685f, -701.30237f, 11.977908f),
	CVector(1511.4004f, -696.83295f, 11.972709f),
	CVector(1542.1796f, -695.61676f, 11.970441f),
	CVector(1570.3301f, -684.6239f, 11.969202f),
	CVector(0.0f, 0.0f, 0.0f),
};

void
CPacManPickups::GeneratePMPickUpsForRace(int32 race)
{
}

void
CPacManPickups::GenerateOnePMPickUp(CVector pos)
{
	bPMActive = true;
	aPMPickUps[0].m_eType = PACMAN_RACE;
	aPMPickUps[0].m_vecPosn = pos;
}

void
CPacManPickups::Render()
{
	if (!bPMActive) return;

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(gpCoronaTexture[6]));

	RwV3d pos;
	float w, h;

	for (int i = 0; i < NUMPACMANPICKUPS; i++) {
		switch (aPMPickUps[i].m_eType)
		{
		case PACMAN_SCRAMBLE:
		case PACMAN_RACE:
			if (CSprite::CalcScreenCoors(aPMPickUps[i].m_vecPosn, &pos, &w, &h, true) && pos.z < 100.0f) {
				if (aPMPickUps[i].m_pObject != nil) {
					aPMPickUps[i].m_pObject->GetMatrix().SetRotateZOnly((CTimer::GetTimeInMilliseconds() % 1024) * TWOPI / 1024.0f);
					aPMPickUps[i].m_pObject->GetMatrix().UpdateRW();
					aPMPickUps[i].m_pObject->UpdateRwFrame();
				}
				float fsin = Sin((CTimer::GetTimeInMilliseconds() % 1024) * 6.28f / 1024.0f); // yes, it is 6.28f when it was TWOPI just now...
				CSprite::RenderOneXLUSprite(pos.x, pos.y, pos.z, 0.8f * w * fsin, 0.8f * h, 100, 50, 5, 255, 1.0f / pos.z, 255);
			}
			break;
		default:
			break;
		}
	}

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, FALSE);
}

void
CPacManPickups::ClearPMPickUps()
{
	bPMActive = false;

	for (int i = 0; i < NUMPACMANPICKUPS; i++) {
		if (aPMPickUps[i].m_pObject != nil) {
			CWorld::Remove(aPMPickUps[i].m_pObject);
			delete aPMPickUps[i].m_pObject;
			aPMPickUps[i].m_pObject = nil;
		}
		aPMPickUps[i].m_eType = PACMAN_NONE;
	}
}

void
CPacManPickups::StartPacManRace(int32 race)
{
	GeneratePMPickUpsForRace(race);
	PillsEatenInRace = 0;
}

void
CPacManPickups::StartPacManRecord()
{
	CollectGameState = 20;
	LastPickUpCoors = FindPlayerCoors();
}

uint32
CPacManPickups::QueryPowerPillsEatenInRace()
{
	return PillsEatenInRace;
}

void
CPacManPickups::ResetPowerPillsEatenInRace()
{
	PillsEatenInRace = 0;
}

void
CPacManPickups::CleanUpPacManStuff()
{
	ClearPMPickUps();
}

void
CPacManPickups::StartPacManScramble(CVector pos, float scrambleMult, int16 count)
{
	GeneratePMPickUps(pos, scrambleMult, count, PACMAN_SCRAMBLE);
}

uint32
CPacManPickups::QueryPowerPillsCarriedByPlayer()
{
	if (FindPlayerVehicle())
		return FindPlayerVehicle()->m_nPacManPickupsCarried;
	return 0;
}

void
CPacManPickups::ResetPowerPillsCarriedByPlayer()
{
	if (FindPlayerVehicle() != nil) {
		FindPlayerVehicle()->m_nPacManPickupsCarried = 0;
		FindPlayerVehicle()->m_fMass /= FindPlayerVehicle()->m_fForceMultiplier;
		FindPlayerVehicle()->m_fTurnMass /= FindPlayerVehicle()->m_fForceMultiplier;
		FindPlayerVehicle()->m_fForceMultiplier = 1.0f;
	}
}
