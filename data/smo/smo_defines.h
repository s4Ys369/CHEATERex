#ifndef SMO_DEFINES_H
#define SMO_DEFINES_H

/* Global */
#define SMO_FALL_DAMAGE_HEIGHT					1600.f
#define MARIO_BREATH_MAX_DURATION               1800        // 60 seconds, 20 seconds in cold water, 15 seconds in poison gas
#define MARIO_ABOUT_TO_DROWN                    1350        // 45 seconds, 15 seconds in cold water, ~11 seconds in poison gas
<<<<<<< HEAD

/* Models */
#define MODEL_SHOCK_WAVE                        0xFC        // invisible_bowser_accessory_geo
#define MODEL_DBG_CYLINDER_HITBOX               0xFD        // dbg_cylinder_hitbox_geo
#define MODEL_DBG_CYLINDER_HURTBOX              0xFE        // dbg_cylinder_hurtbox_geo
=======
#define HUD_DISPLAY_FLAG_TIME_TRIAL_TIMER       (1 << 7)    // 0x0080

/* Models */
#define MODEL_SHOCK_WAVE                        0xFC        // invisible_bowser_accessory_geo
#define MODEL_COSMIC_MARIO                      0xFD        // mario_ghost_geo
#define MODEL_DBG_CYLINDER_HITBOX               0xFE        // dbg_cylinder_hitbox_geo
#define MODEL_DBG_CYLINDER_HURTBOX              0xFF        // dbg_cylinder_hurtbox_geo
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

/* Dialogs */
enum DialogSmoId {
    DIALOG_SMO_START_INDEX =                    200,
    DIALOG_SMO_KING_BOBOMB =                    200,
    DIALOG_SMO_KING_WHOMP =                     201,
};

/* Stationary actions */
#define ACT_SMO_SHOCKED_FROM_HIGH_FALL          (0x037 | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)

/* Moving actions */
#define ACT_SMO_ROLLING							(0x07B | ACT_FLAG_MOVING)
#define ACT_SMO_CAPPY_THROW_GROUND				(0x07C | ACT_FLAG_MOVING)

/* Airborne actions */
#define ACT_SMO_CAPPY_JUMP						(0x081 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
#define ACT_SMO_GROUND_POUND_JUMP				(0x084 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
#define ACT_SMO_LEAVE_OBJECT_JUMP				(0x0B9 | ACT_FLAG_AIR | ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)
#define ACT_SMO_WALL_SLIDE						(0x0BA | ACT_FLAG_AIR)
#define ACT_SMO_CAPPY_THROW_AIRBORNE			(0x0BB | ACT_FLAG_AIR)

/* Water actions */
#define ACT_SMO_WATER_DESCENT					(0x0E4 | ACT_FLAG_SWIMMING)
#define ACT_SMO_WATER_DASH						(0x0E5 | ACT_FLAG_SWIMMING)
#define ACT_SMO_CAPPY_THROW_WATER				(0x0E6 | ACT_FLAG_SWIMMING)
#define ACT_SMO_LEAVE_OBJECT_WATER				(0x0E7 | ACT_FLAG_STATIONARY | ACT_FLAG_SWIMMING | ACT_FLAG_SWIMMING_OR_FLYING | ACT_FLAG_WATER_OR_TEXT)
#define ACT_SMO_METAL_WATER_PUNCH				(0x0FC | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)
#define ACT_SMO_METAL_WATER_KICK				(0x0FD | ACT_FLAG_MOVING | ACT_FLAG_METAL_WATER)
#define ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR	(0x0FE | ACT_FLAG_METAL_WATER)
#define ACT_SMO_CAPPY_THROW_METAL_WATER_AIR		(0x0FF | ACT_FLAG_METAL_WATER)

/* Cutscene actions */
#define ACT_SMO_POSSESSION                      (0x13F | ACT_FLAG_STATIONARY | ACT_FLAG_INVULNERABLE)

/* Sounds */
#define SMO_SOUND_BANK                          0x0A
#define SMO_SOUND_COUNT                         0x05
#define SOUND_ACTION_CAPPY_1					SOUND_ARG_LOAD(SMO_SOUND_BANK, 4, 0x00, 0xF0, 8)
#define SOUND_ACTION_CAPPY_2					SOUND_ARG_LOAD(SMO_SOUND_BANK, 4, 0x01, 0xF0, 8)
#define SOUND_ACTION_SMO_LIFE_UP				SOUND_ARG_LOAD(SMO_SOUND_BANK, 4, 0x02, 0xFE, 8)
#define SOUND_ACTION_SMO_LIFE_UP_END			SOUND_ARG_LOAD(SMO_SOUND_BANK, 4, 0x04, 0xFF, 8)
#define SOUND_ACTION_SMO_DAMAGE					SOUND_ARG_LOAD(SMO_SOUND_BANK, 4, 0x03, 0xF8, 8)
#define SOUND_ACTION_SMO_HEAL					SOUND_ARG_LOAD(SMO_SOUND_BANK, 4, 0x04, 0xF8, 8)

/* Renamed Object fields */
#define oWall                                   OBJECT_FIELD_SURFACE(0x19)
#define oScaleX                                 header.gfx.scale[0]
#define oScaleY                                 header.gfx.scale[1]
#define oScaleZ                                 header.gfx.scale[2]

/* SMO fields */
#define SMO_FIELD_AS_S8(index, subindex)        smoData->fields.asS8[index][subindex]
#define SMO_FIELD_AS_S16(index, subindex)       smoData->fields.asS16[index][subindex]
#define SMO_FIELD_AS_S32(index)                 smoData->fields.asS32[index]
#define SMO_FIELD_AS_F32(index)                 smoData->fields.asF32[index]
#define SMO_REF_AS_OBJ(index)                   smoData->refs.asObject[index]
#define SMO_REF_AS_PTR(index)                   smoData->refs.asVoidPtr[index]
#define oMario                                  smoData->mario
#define oMarioObj                               smoData->mario->marioObj

/* Mario fields */
<<<<<<< HEAD
#define oWallSlide								marioObj->SMO_FIELD_AS_S8(0x00, 0)
#define oCappyJumped							marioObj->SMO_FIELD_AS_S8(0x00, 1)
#define oPossessionLock                         marioObj->SMO_FIELD_AS_S8(0x00, 2)
#define oCoinCounter							marioObj->SMO_FIELD_AS_S16(0x01, 0)
#define oHpCounter							    marioObj->SMO_FIELD_AS_S16(0x01, 1)
#define oO2										marioObj->SMO_FIELD_AS_S16(0x02, 0)
#define oPossessionTimer                        marioObj->SMO_FIELD_AS_S16(0x02, 1)
#define oPeakHeight								marioObj->SMO_FIELD_AS_F32(0x03)
#define oCappyBehavior                          marioObj->SMO_FIELD_AS_S8(0x04, 0)
#define oCappyInteractMario                     marioObj->SMO_FIELD_AS_S8(0x04, 1)
#define oCappyActionFlag                        marioObj->SMO_FIELD_AS_S8(0x04, 2)
#define oCappyInitialized                       marioObj->SMO_FIELD_AS_S8(0x04, 3)
#define oInputStickX                            marioObj->SMO_FIELD_AS_F32(0x05) // [-1, +1], positive is right
#define oInputStickY                            marioObj->SMO_FIELD_AS_F32(0x06) // [-1, +1], positive is up
#define oInputStickMag                          marioObj->SMO_FIELD_AS_F32(0x07) // [0, 1]
#define oInputStickYaw                          marioObj->SMO_FIELD_AS_S32(0x08) // intended yaw
#define oInputButtonPressed                     marioObj->SMO_FIELD_AS_S16(0x09, 0)
#define oInputButtonDown                        marioObj->SMO_FIELD_AS_S16(0x09, 1)
#define oPossessAnimPos(i, j)					marioObj->SMO_FIELD_AS_F32(0x0A + (3 * i) + j)

#define oCappyObject                            marioObj->SMO_REF_AS_OBJ(0x00)
#define oCappyWallObject                        marioObj->SMO_REF_AS_OBJ(0x01)
#define oCappyFloorObject                       marioObj->SMO_REF_AS_OBJ(0x02)
#define oCappyCeilObject                        marioObj->SMO_REF_AS_OBJ(0x03)
#define oPossessedObject                        marioObj->SMO_REF_AS_OBJ(0x04)
#define oObjectCap                              marioObj->SMO_REF_AS_OBJ(0x05)
=======
#define oWallSlide								SMO_FIELD_AS_S8(0x00, 0)
#define oCappyJumped							SMO_FIELD_AS_S8(0x00, 1)
#define oPossessionLock                         SMO_FIELD_AS_S8(0x00, 2)
#define oCoinCounter							SMO_FIELD_AS_S16(0x01, 0)
#define oHpCounter							    SMO_FIELD_AS_S16(0x01, 1)
#define oO2										SMO_FIELD_AS_S16(0x02, 0)
#define oPossessionTimer                        SMO_FIELD_AS_S16(0x02, 1)
#define oPeakHeight								SMO_FIELD_AS_F32(0x03)
#define oInputStickX                            SMO_FIELD_AS_F32(0x04) // [-1, +1], positive is right
#define oInputStickY                            SMO_FIELD_AS_F32(0x05) // [-1, +1], positive is up
#define oInputStickMag                          SMO_FIELD_AS_F32(0x06) // [0, 1]
#define oInputStickYaw                          SMO_FIELD_AS_S32(0x07) // intended yaw
#define oInputButtonPressed                     SMO_FIELD_AS_S16(0x08, 0)
#define oInputButtonDown                        SMO_FIELD_AS_S16(0x08, 1)
#define oPossessAnimPos(i, j)					SMO_FIELD_AS_F32(0x09 + (3 * i) + j)

#define oPossessedObject                        SMO_REF_AS_OBJ(0x00)
#define oObjectCap                              SMO_REF_AS_OBJ(0x01)

/* Cappy fields */
#define oCappyVelX								SMO_FIELD_AS_F32(0x00)
#define oCappyVelY								SMO_FIELD_AS_F32(0x01)
#define oCappyVelZ								SMO_FIELD_AS_F32(0x02)
#define oCappyRadius							SMO_FIELD_AS_F32(0x00)
#define oCappyRadiusGrowth						SMO_FIELD_AS_F32(0x01)
#define oCappyAngleVel							SMO_FIELD_AS_S16(0x02, 0)
#define oCappyYaw0								SMO_FIELD_AS_S16(0x02, 1)
#define oCappyActionFlag						SMO_FIELD_AS_S16(0x03, 0)
#define oCappyYaw								SMO_FIELD_AS_S16(0x03, 1)
#define oCappyTimer                             SMO_FIELD_AS_S16(0x04, 0)
#define oCappyInvincible                        SMO_FIELD_AS_S8(0x04, 2)
#define oCappyInteractMario                     SMO_FIELD_AS_S8(0x04, 3)

#define oCappyWallObj                           SMO_REF_AS_OBJ(0x00)
#define oCappyFloorObj                          SMO_REF_AS_OBJ(0x01)
#define oCappyCeilObj                           SMO_REF_AS_OBJ(0x02)
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

/* Capture data fields (scaled) */
#define cdWalkSpeed                             smoData->captureData->walkSpeed * pobj->oScaleX
#define cdRunSpeed                              smoData->captureData->runSpeed * pobj->oScaleX
#define cdDashSpeed                             smoData->captureData->dashSpeed * pobj->oScaleX
#define cdJumpVelocity                          smoData->captureData->jumpVelocity * pobj->oScaleY
#define cdTerminalVelocity                      smoData->captureData->terminalVelocity * pobj->oScaleY
#define cdGravity                               smoData->captureData->gravity * pobj->oScaleY
#define cdHitboxRadius                          smoData->captureData->hitboxRadius * pobj->oScaleX
#define cdHitboxHeight                          smoData->captureData->hitboxHeight * pobj->oScaleY
#define cdHitboxDownOffset                      smoData->captureData->hitboxDownOffset * pobj->oScaleY
#define cdWallHitboxRadius                      smoData->captureData->wallHitboxRadius * pobj->oScaleX

/* Possessed object properties */
#define POBJ_PROP_ABOVE_WATER					(1 << 0)
#define POBJ_PROP_UNDERWATER					(1 << 1)
#define POBJ_PROP_INVULNERABLE					(1 << 2)
#define POBJ_PROP_UNPUSHABLE					(1 << 3)
#define POBJ_PROP_IMMUNE_TO_FIRE				(1 << 4)
#define POBJ_PROP_IMMUNE_TO_LAVA				(1 << 5)
#define POBJ_PROP_IMMUNE_TO_QUICKSANDS			(1 << 6)
#define POBJ_PROP_RESIST_STRONG_WINDS			(1 << 7)
#define POBJ_PROP_MOVE_THROUGH_VC_WALLS			(1 << 8)
#define POBJ_PROP_WALK_ON_WATER			        (1 << 9)
#define POBJ_PROP_ATTACK						(1 << 10)
#define POBJ_PROP_STRONG_ATTACK					(1 << 11)

/* Possessed object common fields */
#define oProperties								SMO_FIELD_AS_S32(0x00)
#define oUnresponsiveTimer                      SMO_FIELD_AS_S16(0x01, 0)
#define oSquishedTimer                          SMO_FIELD_AS_S8(0x01, 2)
#ifdef BETTERCAMERA
#define oCameraBehindMario                      SMO_FIELD_AS_S8(0x01, 3)
#endif
#define oWalkDistance                           SMO_FIELD_AS_F32(0x02)
#define oCappyForwardDist                       SMO_FIELD_AS_F32(0x03)
#define oCappyLateralDist                       SMO_FIELD_AS_F32(0x04)
#define oCappyVerticalDist                      SMO_FIELD_AS_F32(0x05)
#define oCappyInitialPitch                      SMO_FIELD_AS_S16(0x06, 0)
#define oCappyInitialYaw                        SMO_FIELD_AS_S16(0x06, 1)
#define oCappyInitialRoll                       SMO_FIELD_AS_S16(0x07, 0)
#define oCappyInitialScale                      SMO_FIELD_AS_S16(0x07, 1)
#define COMMON_FIELD_END						0x08

#define COMMON_REF_END                          0x00

/* Inputs for possessed object */
<<<<<<< HEAD
#define oStickX                                 oMario->oInputStickX // [-1, +1], positive is right
#define oStickY                                 oMario->oInputStickY // [-1, +1], positive is up
#define oStickMag                               oMario->oInputStickMag // [0, 1]
#define oStickYaw                               oMario->oInputStickYaw // intended yaw
#define oButtonPressed                          oMario->oInputButtonPressed
#define oButtonDown                             oMario->oInputButtonDown
=======
#define oStickX                                 oMarioObj->oInputStickX // [-1, +1], positive is right
#define oStickY                                 oMarioObj->oInputStickY // [-1, +1], positive is up
#define oStickMag                               oMarioObj->oInputStickMag // [0, 1]
#define oStickYaw                               oMarioObj->oInputStickYaw // intended yaw
#define oButtonPressed                          oMarioObj->oInputButtonPressed
#define oButtonDown                             oMarioObj->oInputButtonDown
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

/* Goomba */
#define oCappyGoombaStackCount					SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x00)
#define oCappyGoombaStackCurOffset(i)			SMO_FIELD_AS_S16(COMMON_FIELD_END + 0x01 + i, 0)	// Offset from address of object's behavior (bhvGoomba)
#define oCappyGoombaStackStkIndex(i)			SMO_FIELD_AS_S16(COMMON_FIELD_END + 0x01 + i, 1)	// Stack index before captured

#define oCappyGoombaStackObject(i)				SMO_REF_AS_OBJ(COMMON_REF_END + 0x00 + i)

/* Koopa */
#define oCappyKoopaState						SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x00)

/* Bob-omb */
#define oCappyBobombExplosionTimer				SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x00)
#define oCappyBobombExplosionCount				SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x01)

/* Bob-omb buddy */
#define oCappyBobombBuddyState					SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x00)
#define oCappyBobombBuddyTimer					SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x01)

/* Bob-omb king */
#define oCappyBobombKingState					SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x00)
#define oCappyBobombKingTimer					SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x01)

/* Chain chomp */
#define oCappyChainChompIsFreed					SMO_FIELD_AS_S8(COMMON_FIELD_END + 0x00, 0)
#define oCappyChainChompIsBiting				SMO_FIELD_AS_S8(COMMON_FIELD_END + 0x00, 1)
#define oCappyChainChompPivotX					SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x01)
#define oCappyChainChompPivotZ					SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x02)

/* Bullet bill */
#define oCappyBulletBillInitialHomeX            SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x00)
#define oCappyBulletBillInitialHomeY            SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x01)
#define oCappyBulletBillInitialHomeZ            SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x02)
#define oCappyBulletBillDestroyed               SMO_FIELD_AS_S8(COMMON_FIELD_END + 0x03, 0)

/* Hoot */
#define oCappyHootInitialHomeX                  SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x00)
#define oCappyHootInitialHomeY                  SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x01)
#define oCappyHootInitialHomeZ                  SMO_FIELD_AS_F32(COMMON_FIELD_END + 0x02)

/* Whomp */
#define oCappyWhompActionState                  SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x00)
#define oCappyWhompActionTimer                  SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x01)

/* Whomp king */
#define oCappyWhompKingState                    SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x00)
#define oCappyWhompKingTimer                    SMO_FIELD_AS_S32(COMMON_FIELD_END + 0x01)

/* Freed Chain chomp (regular object fields) */
#define oChainChompFreedState					OBJECT_FIELD_S32(0x1F)
#define oChainChompFreedTimer					OBJECT_FIELD_S32(0x20)
#define oChainChompFreedAngle					OBJECT_FIELD_S32(0x21)

#endif // SMO_DEFINES_H
