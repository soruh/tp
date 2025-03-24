/**
 * @file d_a_cow.cpp
 *
 */

#include "d/actor/d_a_cow.h"
#include "SSystem/SComponent/c_lib.h"
#include "d/actor/d_a_npc_aru.h"
#include "d/actor/d_a_player.h"
#include "d/d_cc_uty.h"
#include "d/d_com_inf_actor.h"
#include "d/d_com_inf_game.h"
#include "d/d_meter2_info.h"
#include "d/d_timer.h"
#include "dol2asm.h"
#include "dolphin/types.h"
#include "f_op/f_op_actor_mng.h"
#include "m_Do/m_Do_ext.h"
#include "m_Do/m_Do_lib.h"
#include "m_Do/m_Do_mtx.h"

UNK_REL_DATA;
UNK_REL_BSS;

static u8 cc_sph_src[sizeof(dCcD_SrcSph)] = {
    /* 0x00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x08 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x10 */ 0x00, 0xfb, 0xfd, 0xfb, 0x00, 0x00, 0x00, 0x11,
    /* 0x18 */ 0x00, 0x00, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00,
    /* 0x20 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x28 */ 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00,
    /* 0x30 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x38 */ 0x00, 0x00, 0x00, 0x00, 0x42, 0x70, 0x00, 0x00,
};  // todo: parse these bytes

static s16 pen_dir = -0x4000;
static s16 gate_dir = -0x8000;

/* 80662FBC-80662FC0 000064 0004+00 0/2 0/0 0/0 .data l_CowRoomPosY__21@unnamed@d_a_cow_cpp@ */
static f32 l_CowRoomPosY = 15000.0f;

/* 80662FC0-80663010 000068 0050+00 1/3 0/0 0/0 .data l_CowRoomPosX__21@unnamed@d_a_cow_cpp@ */
static f32 l_CowRoomPosX[20] = {-10600.0f, -10600.0f, -10800.0f, -10800.0f, -11000.0f,
                                -11000.0f, -11200.0f, -11200.0f, -11400.0f, -11400.0f,
                                -11600.0f, -11600.0f, -11800.0f, -11800.0f, -12000.0f,
                                -12000.0f, -12200.0f, -12200.0f, -12400.0f, -12400.0f};

/* 80663010-80663018 0000B8 0008+00 1/3 0/0 0/0 .data l_CowRoomPosZ__21@unnamed@d_a_cow_cpp@ */
static f32 l_CowRoomPosZ[2] = {-19646.0f, -20926.0f};

#define N_WOLF_BUSTERS 3
extern fpc_ProcID gWolfBustersID[N_WOLF_BUSTERS] = {-1, -1, -1};

/* 806585CC-80658730 0000EC 0164+00 6/6 0/0 0/0 .text            calcRunAnime__7daCow_cFi */
int daCow_c::calcRunAnime(int resetAnimation) {
    if (resetAnimation != 0) {
        mAnimationPhase = 0;
    }

    int animationPhase = mAnimationPhase;

    f32 newSpeed;
    switch (animationPhase) {
    case 0:
        if (speedF < 35.0f) {
            setBck(0x19, 2, 5.0f, 1.0f);
            mAnimationPhase = 1;
        } else {
            setBck(0x13, 2, 5.0f, 1.0f);
            mAnimationPhase = 2;
        }
        break;

    case 1:
        newSpeed = (speedF * 3.0f) / 25.0f;
        if (newSpeed > 5.0f) {
            newSpeed = 5.0f;
        }
        mpMorf->setPlaySpeed(newSpeed);
        if (speedF > 35.0f) {
            setBck(0x13, 2, 5.0f, 1.0f);
            mAnimationPhase = 2;
        }
        break;

    case 2:
        mpMorf->setPlaySpeed(1.3f);
        if (speedF < 35.0f) {
            setBck(0x19, 2, 5.0f, 1.0f);
            mAnimationPhase = 1;
        }
    }
    return 1;
}

/* 80658730-806587D4 000250 00A4+00 20/20 0/0 0/0 .text            setBck__7daCow_cFiUcff */
void daCow_c::setBck(int param_0, u8 param_1, f32 param_2, f32 param_3) {
    J3DAnmTransform* animation = (J3DAnmTransform*)dComIfG_getObjectRes("Cow", param_0);
    mpMorf->setAnm(animation, param_1, param_2, param_3, 0.0f, -1.0f);
}

/* 806587D4-80658830 0002F4 005C+00 1/1 0/0 0/0 .text            checkBck__7daCow_cFi */
int daCow_c::checkBck(int param_0) {
    J3DAnmTransform* animation = (J3DAnmTransform*)dComIfG_getObjectRes("Cow", param_0);
    return mpMorf->getAnm() == animation;
}

static cXyz pen_pos(-10200.0f, 15000.0f, -20246.0f);
static cXyz gate_pos(-9246.0f, 15000.0f, -22763.0f);

/* 806634D0-806634D4 000078 0004+00 3/3 0/0 0/0 .bss l_CowRoomNo__21@unnamed@d_a_cow_cpp@ */
static u32 l_CowRoomNo = 0;
static u32 l_CowType = 0;

/* 80658830-80658A68 000350 0238+00 1/1 0/0 0/0 .text            setEffect__7daCow_cFv */
void daCow_c::setEffect() {
    cXyz c;
    cXyz b;
    cXyz a;

    if (mShouldSetEffect) {
        if (mShouldSetEffect == 1) {
            if ((field_0xc68 & 1)) {
                cXyz offset(0.0f, 10.0f, -70.0f);

                offset.x = 25.0f;
                cLib_offsetPos(&a, &current.pos, field_0xc32.y, &offset);
                offset.x = 0.0f;
                cLib_offsetPos(&c, &current.pos, field_0xc32.y, &offset);
                offset.x = -25.0f;
                cLib_offsetPos(&c, &current.pos, field_0xc32.y, &offset);
            } else {
                mShouldSetEffect = 0;
            }

            int roomNumber = fopAcM_GetRoomNo(this);

            cXyz* v3 = mShouldSetEffect ? &b : NULL;
            cXyz* v2 = mShouldSetEffect ? &c : NULL;
            cXyz* v1 = mShouldSetEffect ? &a : NULL;

            mParticle.setEffectTwo(&tevStr, &current.pos, 0, 0, v1, v2, v3, &field_0xc32, NULL,
                                   roomNumber, 1.0f, speedF);

            static cXyz runScale(2.0f, 2.0f, 2.0f);

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    JPABaseEmitter* emitter = mParticle.getEmitterTwo(i, j, 0);

                    if (emitter) {
                        emitter->setGlobalScale(runScale);
                        emitter->setRate(1.3f);
                    }
                }
            }
        }
    } else {
        fopAcM_effSmokeSet1(&field_0xd38, &field_0xd3c, &current.pos, NULL, 2.0f, &tevStr, 1);
    }
    mShouldSetEffect = 0;
}

/* 80658AA4-80658B10 0005C4 006C+00 5/5 0/0 0/0 .text            isChaseCowGame__7daCow_cFv */
bool daCow_c::isChaseCowGame() {
    if (strcmp(dComIfGp_getStartStageName(), "F_SP00") == 0) {
        if (dComIfG_play_c::getLayerNo(0) == 4 || dComIfG_play_c::getLayerNo(0) == 5) {
            return true;
        }
    }
    return false;
}

/* 80658B10-80658C18 000630 0108+00 6/6 0/0 0/0 .text            setCarryStatus__7daCow_cFv */
void daCow_c::setCarryStatus() {
    f32 zMax = 270.0f;
    f32 xMax = 70.0f;
    if (speedF >= 10.0f) {
        zMax = 600.0f;
        xMax = 100.0f;
    }
    mDoMtx_stack_c::YrotS(-shape_angle.y);
    mDoMtx_stack_c::transM(-current.pos.x, -current.pos.y, -current.pos.z);

    Vec carryPosition;
    mDoMtx_stack_c::multVec(&daPy_getPlayerActorClass()->current.pos, &carryPosition);

    if (fabsf(carryPosition.x) < xMax && carryPosition.z > 0.0f && carryPosition.z < zMax) {
        // todo: what does this mean
        attention_info.flags |= 0x10;  // in debug this is 0x80
    }
    return;
}

/* 80658C18-80658C78 000738 0060+00 5/5 0/0 0/0 .text            setActetcStatus__7daCow_cFv */
void daCow_c::setActetcStatus() {
    if (!field_0xca8) {
        s32 playerAngle = fopAcM_seenPlayerAngleY(this);
        if (playerAngle < 0x6000 && playerAngle > 0x2000) {
            attention_info.flags |= 0x80;  // in debug this is 0x800
        }
    }
}

// NOTE: nade nade seems to be petting the Goat

/* 80658C78-80658CA4 000798 002C+00 3/3 0/0 0/0 .text            checkNadeNadeFinish__7daCow_cFv */
bool daCow_c::checkNadeNadeFinish() {
    if (mFlags & 0x100) {
        mFlags &= ~0x0100;  // todo: enum
        field_0xca8 = 0;
        return true;
    }
    return false;
}

/* 80658CA4-80658CD0 0007C4 002C+00 5/5 0/0 0/0 .text            checkNadeNade__7daCow_cFv */
bool daCow_c::checkNadeNade() {
    if ((mFlags & 0x80) != 0) {
        mFlags &= ~0x080;  // todo: enum
        field_0xca8 = 1;
        return 1;
    }
    return 0;
}

/* 80658CD0-80658D3C 0007F0 006C+00 3/3 0/0 0/0 .text            setSeSnort__7daCow_cFv */
void daCow_c::setSeSnort() {
    if (mpMorf->checkFrame(1.0f)) {
        mSound.startCreatureVoice(JAISoundID(0x50079), -1);
    }
}

/* 80658D3C-80658DB8 00085C 007C+00 2/2 0/0 0/0 .text            setRushVibration__7daCow_cFi */
void daCow_c::setRushVibration(int i_vibmode) {
    if (mpMorf->checkFrame(1.0f)) {
        dComIfGp_getVibration().StartShock(i_vibmode, 0x1F, cXyz(0.0f, 1.0f, 0.0f));
    }
}

/* 80658DB8-80658E98 0008D8 00E0+00 6/6 0/0 0/0 .text            checkThrow__7daCow_cFv */
bool daCow_c::checkThrow() {
    if (mFlags != 0) {
        if ((mFlags & 1) != 0) {
            setProcess(&daCow_c::action_thrown, 0);
            initCrazyBeforeCatch(0);
            mFlags &= ~0x0001;
            return true;
        }
        if ((mFlags & 2) != 0) {
            setProcess(&daCow_c::action_thrown, 0);
            initCrazyCatch(0);
            mFlags &= ~0x0002;
            return true;
        }
    }
    return false;
}

#define CLAMP(val, min, max)                                                                       \
    STATIC_ASSERT(min <= max);                                                                     \
    if ((val) > (max)) {                                                                           \
        (val) = (max);                                                                             \
    }                                                                                              \
    if ((val) < (min)) {                                                                           \
        (val) = (min);                                                                             \
    }

#define CLAMP_COW_BODY_ANGLE(angle) CLAMP((angle), -0x2000, 0x2000)

/* 80658E98-80658F94 0009B8 00FC+00 3/3 0/0 0/0 .text            setBodyAngle__7daCow_cFs */
void daCow_c::setBodyAngle(s16 angle) {
    s16 offsetAngle = field_0xc32.y - angle;

    CLAMP_COW_BODY_ANGLE(offsetAngle);

    // round small angles to 0
    if (abs(offsetAngle) < 0x100) {
        offsetAngle = 0;
    }
    cLib_chaseS(&field_0xc3e.y, offsetAngle * 0.7f, 0x100);
    cLib_chaseS(&field_0xc38.y, offsetAngle * 0.3f, 0x100);
}

/* 80658F94-806590E8 000AB4 0154+00 1/1 0/0 0/0 .text            setBodyAngle2__7daCow_cFs */
void daCow_c::setBodyAngle2(s16 angle) {
    s16 offsetAngle = field_0xc32.y - angle;

    CLAMP_COW_BODY_ANGLE(offsetAngle);

    if (abs(offsetAngle) < 0x100) {
        offsetAngle = 0;
    }

    cLib_chaseS(&field_0xc38.y, offsetAngle * 0.3f, 0x100);
    daPy_py_c* player = daPy_getPlayerActorClass();
    s16 targetAngle = cLib_targetAngleY(&current.pos, &player->current.pos);
    s16 bodyAngle = field_0xc32.y - targetAngle;

    if (abs(bodyAngle) < 0x5000) {
        CLAMP_COW_BODY_ANGLE(bodyAngle);
    } else {
        bodyAngle = offsetAngle * 0.7f;
    }

    cLib_chaseS(&field_0xc3e.y, bodyAngle, 0x100);
}

/* 806590E8-80659114 000C08 002C+00 5/5 0/0 0/0 .text checkProcess__7daCow_cFM7daCow_cFPCvPv_v */
BOOL daCow_c::checkProcess(void (daCow_c::*process)()) {
    return this->mProcess == process;
}

/* 80659114-806591BC 000C34 00A8+00 16/16 0/0 0/0 .text setProcess__7daCow_cFM7daCow_cFPCvPv_vi */
bool daCow_c::setProcess(void (daCow_c::*process)(), int param_1) {
    mMode = 3;
    if (mProcess) {
        (this->*mProcess)();
    }
    field_0xcaa = param_1;
    mMode = 0;
    mProcess = process;
    if (mProcess) {
        (this->*mProcess)();
    }

    return true;
}

#define COW_ATTACK_TYPES                                                                           \
    (AT_TYPE_NORMAL_SWORD | AT_TYPE_BOMB | AT_TYPE_ARROW | AT_TYPE_SPINNER | AT_TYPE_IRON_BALL)

STATIC_ASSERT(COW_ATTACK_TYPES == 0x482022);

/* 806591BC-8065945C 000CDC 02A0+00 1/1 0/0 0/0 .text            damage_check__7daCow_cFv */
void daCow_c::damage_check() {
    mCcStts.Move();

    if (field_0xca5) {
        return;
    }

    if (field_0xc80) {
        field_0xc80--;
        return;
    }

    cCcD_ObjHitInf* hitObject = NULL;
    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
        if (mSph[iSphere].ChkTgHit()) {
            hitObject = mSph[iSphere].GetTgHitObj();
            break;
        }
    }

    if (!hitObject) {
        return;
    }
    field_0xc80 = 10;

    if (checkProcess(&daCow_c::action_crazy)) {
        if (field_0xc9f == 8) {
            if (field_0xc61 == 0) {
                if (hitObject->ChkAtType(COW_ATTACK_TYPES)) {
                    field_0xc8c = 0x96;
                } else {
                    field_0xc8c += 0x3c;
                }
                if (field_0xc8c >= 0x96) {
                    field_0xc61 = 5;
                }
            }
        }
    } else if (checkProcess(&daCow_c::action_angry)) {
        field_0xc98 = 200;
    } else if (hitObject->ChkAtType(COW_ATTACK_TYPES)) {
        setProcess(&daCow_c::action_damage, 0);
    } else {
        field_0xc8c += 0x3c;
        if (field_0xc8c >= 0x96) {
            setProcess(&daCow_c::action_damage, 0);
        } else {
            field_0xc88 = 0x5a;

            if (!checkProcess(&daCow_c::action_wait)) {
                speedF = 0.0f;
                setProcess(&daCow_c::action_wait, 0);
            }
        }
    }

    mSph[0].ClrTgHit();
    mSph[1].ClrTgHit();
    mSph[2].ClrTgHit();
    STATIC_ASSERT(N_COW_COLLIDERS == 3);
}

/* 8065945C-80659540 000F7C 00E4+00 1/1 0/0 0/0 .text            setEnterCow20__7daCow_cFv */
void daCow_c::setEnterCow20() {
    for (int iCow = 0; iCow < 20; iCow++) {
        cXyz spawnPosition(l_CowRoomPosX[iCow], l_CowRoomPosY, l_CowRoomPosZ[iCow & 1]);

        l_CowRoomNo |= 1 << (iCow & ~!0xc0);  // todo: what is this flag?

        csXyz spawnAngle;
        if (iCow & 1) {
            spawnAngle.set(0, 0, 0);
        } else {
            spawnAngle.set(0, -0x8000, 0);
        }

        fopAcM_create(0x106, ~0xfb, &spawnPosition, fopAcM_GetRoomNo(this), &spawnAngle, 0, -1);
    }
}

/* ############################################################################################## */
/* 80663084-806630AC 00012C 0028+00 0/1 0/0 0/0 .data            cow_number$4349 */
static u32 cow_number[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* 80659540-80659630 001060 00F0+00 1/1 0/0 0/0 .text            setEnterCow10__7daCow_cFv */
void daCow_c::setEnterCow10() {
    for (int iCow = 0; iCow < 10; iCow++) {
        int cowNumber = cow_number[iCow];

        cXyz spawnPosition(l_CowRoomPosX[cowNumber], l_CowRoomPosY, l_CowRoomPosZ[cowNumber & 1]);

        l_CowRoomNo |= 1 << cowNumber;  // todo: what is this flag?

        csXyz spawnAngle;
        if (cowNumber & 1) {
            spawnAngle.set(0, 0, 0);
        } else {
            spawnAngle.set(0, -0x8000, 0);
        }
        int roomNumber = fopAcM_GetRoomNo(this);
        fopAcM_create(0x106, ~0xfb, &spawnPosition, roomNumber, &spawnAngle, 0, -1);
    }
}

/* 80659630-806596E4 001150 00B4+00 1/1 0/0 0/0 .text            setGroundAngle__7daCow_cFv */
void daCow_c::setGroundAngle() {
    s16 alpha = 0;
    s16 beta = 0;

    if (mAcch.ChkGroundHit()) {
        cM3dGPla plane;

        dComIfG_Bgsp().GetTriPla(mAcch.m_gnd, &plane);
        cXyz* pfVar2 = plane.GetNP();
        alpha = (s16)cM_atan2s(pfVar2->z, pfVar2->y);
        beta = (s16)cM_atan2s(pfVar2->x, pfVar2->y);
    }

    cLib_chaseAngleS(&field_0xc2c.x, alpha, 128);
    cLib_chaseAngleS(&field_0xc2c.y, beta, 128);
}

/* ############################################################################################## */
/* 806634F4-806634F8 00009C 0004+00 0/3 0/0 0/0 .bss             m_near_dist */
static f32 m_near_dist;

/* 806634F8 0002+00 data_806634F8 m_view_angle_wide */
static s16 m_view_angle_wide;

/* 806634FA 0002+00 data_806634FA m_view_angle */
static s16 m_view_angle;

#define IS_COW(actor) (fopAcM_GetName((actor)) == PROC_COW)
#define IS_VALID_COW_INTERACTION(cow_1, actor)                                                     \
    (fopAcM_IsActor((cow_1)) && !fpcM_IsCreating(fopAcM_GetID((cow_1))) && IS_COW((cow_1)) &&      \
     (cow_1) != (actor))

/* 8065972C-80659814 00124C 00E8+00 2/2 0/0 0/0 .text            s_near_cow__FPvPv */
static void* s_near_cow(void* param_1, void* param_2) {
    if (IS_VALID_COW_INTERACTION(param_1, param_2)) {
        daCow_c* cow_1 = (daCow_c*)param_1;
        daCow_c* cow_2 = (daCow_c*)param_2;

        if (!cow_1->getCowIn()) {
            s16 actorAngleY = fopAcM_searchActorAngleY(cow_2, cow_1);

            actorAngleY = cLib_distanceAngleS(actorAngleY, cow_2->getShapeAngle().y);

            if (cLib_distanceAngleS(actorAngleY, m_view_angle) < m_view_angle_wide) {
                f32 dVar5 = fopAcM_searchActorDistance(cow_1, cow_2);

                if (dVar5 < (double)m_near_dist) {
                    m_near_dist = (float)dVar5;
                }
            }
        }
    }
    return NULL;
}

/* 806634FC-80663500 0000A4 0002+02 4/4 0/0 0/0 .bss             m_angry_cow */
static s16 m_angry_cow;

/* 80659814-806598D4 001334 00C0+00 1/1 0/0 0/0 .text            s_angry_cow__FPvPv */
static void* s_angry_cow(void* param_1, void* param_2) {
    fopAc_ac_c* other_actor = (fopAc_ac_c*)param_2;
    if (IS_VALID_COW_INTERACTION(param_1, other_actor)) {
        daCow_c* cow_1 = (daCow_c*)param_1;
        if ((cow_1->isAngry() || cow_1->isGuardFad()) &&
            fopAcM_searchActorDistance(cow_1, other_actor) < 500.0f)
        {
            m_angry_cow = true;
        }
    }
    return NULL;
}

/* 806598D4-80659970 0013F4 009C+00 1/1 0/0 0/0 .text            s_angry_cow2__FPvPv */
static void* s_angry_cow2(void* param_1, void* param_2) {
    fopAc_ac_c* other_actor = (fopAc_ac_c*)param_2;
    if (IS_VALID_COW_INTERACTION(param_1, other_actor)) {
        daCow_c* cow_1 = (daCow_c*)param_1;
        if (cow_1->isAngry()) {
            m_angry_cow = true;
            return cow_1;
        }
    }
    return NULL;
}

/* 80659970-806599C0 001490 0050+00 1/1 0/0 0/0 .text            checkRun__7daCow_cFv */
bool daCow_c::checkRun() {
    return checkProcess(&daCow_c::action_run);
}

/* 806599C0-80659ADC 0014E0 011C+00 4/4 0/0 0/0 .text            checkNearCowRun__7daCow_cFv */
bool daCow_c::checkNearCowRun() {
    if (field_0xca5) {
        return false;
    }

    m_angry_cow = false;
    fpcM_Search(&s_angry_cow, this);

    if (m_angry_cow) {
        return true;
    }

    if (!isChaseCowGame()) {
        return false;
    }

    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
        cCcD_Obj* obj = mSph[iSphere].GetCoHitObj();
        if (!obj) {
            continue;
        }

        daCow_c* cow = (daCow_c*)obj->GetAc();
        if (cow && IS_COW(cow) && !cow->getNoNearCheckTimer() && cow->checkRun()) {
            s16 angle = fopAcM_searchActorAngleY(this, cow);
            int angleDifference = cLib_distanceAngleS(angle, field_0xc32.y);
            if (angleDifference >= 0x1000 && angleDifference < 0x7000) {
                return true;
            }
        }
    }

    return false;
}

/* 80659ADC-8065A0E8 0015FC 060C+00 15/0 0/0 0/0 .text            action_wait__7daCow_cFv */
void daCow_c::action_wait() {
    f32 rand = cM_rnd();

    switch (mMode) {
    case 0:
        field_0xc58 = cM_rndF(100.0f) + 300.0f;
        mMode = 1;
        field_0xc90 = 0;
        if (!field_0xcaa) {
            setBck(0x1a, 2, 12.0f, 1.0f);
            mMode = 2;
        } else {
            setBck(6, 0, 12.0f, 1.0f);

            mpMorf->setFrame(mpMorf->getEndFrame());
            mpMorf->setPlaySpeed(-1.0f);

            mMode = 1;
        }
        return;

    case 1:
        if (mpMorf->isStop()) {
            setBck(0x1a, 2, 0.0f, 1.0f);
            mMode = 2;
        }
        break;

    case 2:
        break;

    case 3:
        field_0xc38.y = 0;
        field_0xc3e.y = 0;
        field_0xc88 = 0;
        field_0xca8 = 0;
    default:
        return;
    }

    s16 angle = 0;
    if (field_0xc88 > 0x1e) {
        angle = field_0xc32.y - fopAcM_searchPlayerAngleY(this);
        CLAMP(angle, -0x2800, 0x2800);
    }

    cLib_addCalcAngleS2(&field_0xc3e.y, angle * 0.9f, 0x10, 0x100);
    cLib_addCalcAngleS2(&field_0xc38.y, angle * 0.1f, 0x10, 0x100);

    if (!field_0xca5) {
        if (checkCowInOwn(0x8000)) {
            return;
        }
        if (field_0xca8) {
            field_0xc88 = 0;
            if (!checkNadeNadeFinish()) {
                return;
            }
            setProcess(&daCow_c::action_moo, 0);
            return;
        }
        if (checkPlayerWait() && (checkPlayerSurprise() || checkPlayerPos())) {
            setProcess(&daCow_c::action_run, 0);
            return;
        }
        if (checkWolfBusters()) {
            return;
        }
        setActetcStatus();
        if (checkNadeNade()) {
            return;
        }
        setCarryStatus();
        if (checkThrow()) {
            return;
        }
        if (checkNearCowRun()) {
            setProcess(&daCow_c::action_run, 0);
            return;
        }
    }
    if (!cLib_calcTimer((int*)&field_0xc58) && !field_0xc88) {
        if (checkNearWolf()) {
            setProcess(&daCow_c::action_moo, 0);
        } else {
            daPy_py_c* playerActor = daPy_getPlayerActorClass();
            if (current.pos.absXZ(playerActor->current.pos) > 500.0f && rand < 0.4f) {
                setProcess(&daCow_c::action_moo, 0);
            } else {
                if (rand < 0.5f) {
                    setProcess(&daCow_c::action_eat, 1);
                } else {
                    setProcess(&daCow_c::action_shake, 1);
                }
            }
        }
    }
}

/* 8065A0E8-8065A594 001C08 04AC+00 4/0 0/0 0/0 .text            action_eat__7daCow_cFv */
void daCow_c::action_eat() {
    int nextAction = mMode;
    if (nextAction != 2) {
        if (nextAction < 2) {
            if (nextAction != 0) {
                f32 rand = cM_rndF(100.0f);
                field_0xc58 = rand + 300.0f;
                field_0xc90 = 0;
                if (!field_0xcaa) {
                    setBck(9, 2, 12.0f, 1.0f);
                    mMode = 2;
                } else {
                    setBck(6, 0, 12.0f, 1.0f);
                    mMode = 1;
                }
                return;
            } else {
                if (mpMorf->isStop()) {
                    setBck(9, 2, 0.0f, 1.0f);
                    mMode = 2;
                }
            }
        } else {
            return;
        }
    }

    if (mpMorf->checkFrame(10.0f) || mpMorf->checkFrame(40.0f) || mpMorf->checkFrame(68.0f) ||
        mpMorf->checkFrame(98.0f))
    {
        mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_EAT), -1);
    }

    if (!field_0xca5) {
        if (checkNearCowRun() || checkPlayerWait()) {
            setProcess(&daCow_c::action_wait, 0);
            return;
        }
        setCarryStatus();
        if (checkThrow()) {
            return;
        }
        setActetcStatus();
        if (checkNadeNade()) {
            setProcess(&daCow_c::action_wait, 1);
            return;
        }
    }
    if (!cLib_calcTimer((int*)&field_0xc58) && mpMorf->isLoop()) {
        if (checkNearWolf()) {
            setProcess(&daCow_c::action_moo, 0);

        } else {
            f32 rand = cM_rnd();
            if (current.pos.absXZ(daPy_getPlayerActorClass()->current.pos) > 500.0f && rand < 0.4f)
            {
                setProcess(&daCow_c::action_moo, 0);
            } else {
                if (rand < 0.5f) {
                    setProcess(&daCow_c::action_shake, 0);
                } else {
                    setProcess(&daCow_c::action_wait, 1);
                }
            }
        }
    }
}

/* 8065A594-8065A8A4 0020B4 0310+00 9/0 0/0 0/0 .text            action_moo__7daCow_cFv */
void daCow_c::action_moo() {
    int nextAction = mMode;
    if (nextAction != 2) {
        if (nextAction < 2) {
            if (nextAction != 0) {
                if (!field_0xcaa) {
                    setBck(0xf, 0, 0.0f, 1.0f);
                    mMode = 2;
                } else {
                    setBck(0xf, 0, 12.0f, 1.0f);
                    mpMorf->setFrame(mpMorf->getEndFrame());
                    mpMorf->setPlaySpeed(-1.0f);
                    mMode = 1;
                }
                return;
            } else {
                if (mpMorf->isStop()) {
                    setBck(0xf, 0, 0.0f, 1.0f);
                    mMode = 2;
                }
            }
        } else {
            return;
        }
    }

    if (mpMorf->checkFrame(35.0f)) {
        mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_CRY), -1);
    }

    if (!field_0xca5) {
        if (checkNearCowRun() || checkPlayerWait()) {
            setProcess(&daCow_c::action_wait, 0);
            return;
        } else {
            setCarryStatus();
            if (checkThrow()) {
                return;
            }
        }
    }
    if (mpMorf->isStop()) {
        if (checkNearWolf()) {
            setProcess(&daCow_c::action_shake, 1);
        } else {
            f32 rand = cM_rnd();
            if (rand < 0.4f) {
                setProcess(&daCow_c::action_eat, 1);
            } else {
                if (rand < 0.7f) {
                    setProcess(&daCow_c::action_shake, 1);
                } else {
                    setProcess(&daCow_c::action_wait, 0);
                }
            }
        }
    }
}

/* 8065A8A4-8065ACC8 0023C4 0424+00 5/0 0/0 0/0 .text            action_shake__7daCow_cFv */
void daCow_c::action_shake() {
    int nextAction = mMode;
    if (nextAction != 2) {
        if (nextAction < 2) {
            if (nextAction != 0) {
                if (!field_0xcaa) {
                    setBck(6, 0, 12.0f, 1.0f);
                    mMode = 2;
                } else {
                    setBck(0x15, 2, 12.0f, 1.0f);
                    mMode = 1;
                }
                return;
            } else {
                if (mpMorf->isStop()) {
                    setBck(0x15, 2, 0.0f, 1.0f);
                    mMode = 2;
                }
            }
        } else {
            return;
        }
    }

    if (mpMorf->checkFrame(68.0f)) {
        mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_NOSE), -1);
    }

    if (!field_0xca5) {
        if (checkNearCowRun() || checkPlayerWait()) {
            setProcess(&daCow_c::action_wait, 0);
            return;
        }
        setCarryStatus();
        if (checkThrow()) {
            return;
        }
        setActetcStatus();
        if (checkNadeNade()) {
            setProcess(&daCow_c::action_wait, 1);
            return;
        }
    }
    if (mpMorf->isLoop()) {
        if (!checkNearWolf()) {
            setProcess(&daCow_c::action_moo, 1);
        } else {
            f32 rand = cM_rnd();
            if (current.pos.absXZ(daPy_getPlayerActorClass()->current.pos) > 500.0f && rand < 0.4f)
            {
                setProcess(&daCow_c::action_moo, 1);
            } else {
                if (rand >= 0.5f) {
                    setProcess(&daCow_c::action_eat, 0);
                } else {
                    setProcess(&daCow_c::action_wait, 1);
                }
            }
        }
    }
}

/* 8065ACC8-8065AD2C 0027E8 0064+00 4/4 0/0 0/0 .text            checkNearWolf__7daCow_cFv */
bool daCow_c::checkNearWolf() {
    if ((u32)daPy_getPlayerActorClass()->checkNowWolf() != 0 &&
        fopAcM_searchPlayerDistance(this) < 3000.f)
    {
        return true;
    } else {
        return false;
    }
}

/* 8065AD2C-8065ADB0 00284C 0084+00 5/5 0/0 0/0 .text            checkPlayerWait__7daCow_cFv */
bool daCow_c::checkPlayerWait() {
    if ((daPy_getPlayerActorClass()->checkHorseRide() ||
         (u32)daPy_getPlayerActorClass()->checkNowWolf() != 0) &&
        fopAcM_searchPlayerDistance(this) < field_0xc78)
    {
        return true;
    } else {
        return false;
    }
}

/* 8065ADB0-8065AE88 0028D0 00D8+00 2/2 0/0 0/0 .text            checkPlayerSurprise__7daCow_cFv
 */
bool daCow_c::checkPlayerSurprise() {
    if (field_0xca5 != 0) {
        return false;
    }

    daPy_py_c* player = daPy_getPlayerActorClass();

    if (!player->checkHorseRide()) {
        return false;
    }

    if (fopAcM_searchPlayerDistance(this) < 1500.0f && player->checkCowGameLash()) {
        mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_CRY), -1);
        field_0xca3 = 0x32;
        return true;
    } else {
        return false;
    }
}

/* 8065AE88-8065B034 0029A8 01AC+00 2/2 0/0 0/0 .text            checkPlayerPos__7daCow_cFv */
bool daCow_c::checkPlayerPos() {
    if (field_0xca5) {
        return false;
    }

    field_0xc60 = 6;

    f32 playerDistance = fopAcM_searchPlayerDistance(this);
    float cutoffDistance = field_0xc78;
    if (field_0xca3) {
        cutoffDistance = 1500.0f;
    }
    if (playerDistance > cutoffDistance) {
        return false;
    }

    s16 angleDifference = fopAcM_searchPlayerAngleY(this) - field_0xc32.y;
    s16 absAngleDifference = abs(angleDifference);
    if (absAngleDifference >= 0x2000) {
        if (angleDifference >= 1) {
            return false;
        } else {
            return true;
        }
    }

    if (absAngleDifference >= 0x6001) {
        if (angleDifference < 1) {
            field_0xc60 = 5;
        } else {
            field_0xc60 = 4;
        }
    } else {
        if (fabsf(cutoffDistance * cM_scos(0x2000)) <
            fabsf(playerDistance * cM_scos(angleDifference)))
        {
            return false;
        }

        if (fabsf(cutoffDistance * cM_ssin(0x2000)) <
            fabsf(playerDistance * cM_ssin(angleDifference)))
        {
            return false;
        }

        if (angleDifference >= 1) {
            field_0xc60 = 2;
        } else {
            field_0xc60 = 3;
        }
    }

    return true;
}

/* 8065B034-8065B760 002B54 072C+00 2/2 0/0 0/0 .text            checkBeforeBg__7daCow_cFv */
void daCow_c::checkBeforeBg() {
    s16 x[3] = {0, -0x2000, 0x2000};
    f32 y[3];
    s16 z[3];

    cM3dGPla planes[3];

    cXyz a;
    cXyz b = current.pos;
    b.y += 110.0f;
    cXyz c;

    bool planeTri[3] = {false, false, false};

    f32 f1 = 1000.0f;
    f32 f2 = 700.0f;

    if (checkCowIn(1000.0f, 0.0f)) {
        f1 = 800.0f;
        f2 = 560.0f;
    }

    if (speedF != 0.0f) {
        for (int iPlane = 0; iPlane < 3; iPlane++) {
            c = b;
            if (iPlane == 0) {
                c.x += f1 * cM_ssin(field_0xc32.y + x[0]);
                c.z += f1 * cM_scos(field_0xc32.y + x[0]);
            } else {
                c.x += f2 * cM_ssin(field_0xc32.y + x[iPlane]);
                c.z += f2 * cM_scos(field_0xc32.y + x[iPlane]);
            }

            dBgS_LinChk linChk;
            linChk.Set(&b, &c, this);
            if (dComIfG_Bgsp().LineCross(&linChk)) {
                planeTri[iPlane] = dComIfG_Bgsp().GetTriPla(linChk, &planes[iPlane]);
                if (fabs(planes[iPlane].mNormal.y) >= cM_ssin(0x6000)) {
                    a = current.pos - linChk.GetCross();
                    y[iPlane] = a.absXZ();
                    z[iPlane] = cM_atan2s(planes[iPlane].mNormal.x, planes[iPlane].mNormal.z);
                } else {
                    planeTri[iPlane] = false;
                }
            }
        }
    }

    field_0xc6c = 1000.0f;
    if (planeTri[0] && y[0] < field_0xc6c) {
        field_0xc6c = y[0];
        field_0xc70 = z[0];
    }

    field_0xca2 = 0;
    if (planeTri[0]) {
        field_0xca2 |= 1;
    }
    if (planeTri[1]) {
        field_0xca2 |= 2;
    }
    if (planeTri[2]) {
        field_0xca2 |= 4;
    }
    if (cLib_calcTimer(&field_0xc54)) {
        return;
    }

    if (planeTri[1] && planeTri[2]) {
        if (planeTri[0]) {
            if (planeTri[1]) {
                field_0xc61 = field_0xc60 <= 3 ? 4 : 2;
            } else if (planeTri[2]) {
                field_0xc61 = field_0xc60 <= 3 ? 3 : 1;
            } else {
                s16 difference = z[0] - field_0xc32.y;
                if (abs(difference) < 0x7801) {
                    field_0xc61 = difference <= 0 ? 3 : 4;
                } else {
                    field_0xc61 = (field_0xc60 & 1) ? 4 : 3;
                }
            }
        } else if (planeTri[1]) {
            if (field_0xc60 != 2) {
                if (field_0xc60 < 2) {
                    field_0xc61 = 4;
                    field_0xc54 = 10;
                } else {
                    if (field_0xc60 < 4) {
                        field_0xc61 = 2;
                    }
                }
            } else {
                if (y[1] < 600.0f) {
                    field_0xc61 = 2;
                } else {
                    field_0xc61 = 0;
                }
            }
        } else if (!planeTri[2]) {
            if (field_0xc60 == 2) {
                field_0xc61 = 1;
            } else {
                if (field_0xc60 < 2) {
                    field_0xc61 = field_0xc60 == 0 ? 3 : 4;
                } else {
                    field_0xc61 = field_0xc60 <= 3 ? 2 : 0;
                }
            }
        } else {
            if (field_0xc60 == 2) {
                field_0xc61 = 1;
            } else {
                if (field_0xc60 > 1) {
                    field_0xc61 = y[2] < 600.0f ? 1 : 0;
                } else {
                    field_0xc61 = 3;
                    field_0xc54 = 10;
                }
            }
        }
    } else {
        if (field_0xc61 == 3) {
            field_0xc61 = 3;
        } else {
            if (field_0xc61 >= 3) {
                if (field_0xc61 < 5) {
                    field_0xc61 = 4;
                } else {
                    field_0xc61 = (field_0xc60 & 1) ? 4 : 3;
                }
            } else {
                if (field_0xc61 == 1) {
                    field_0xc61 = 3;
                }
                if (field_0xc61 != 0) {
                    field_0xc61 = 4;
                }
            }
        }
        field_0xc54 = 10;
    }
}

/* 8065B760-8065B8A8 003280 0148+00 6/6 0/0 0/0 .text            checkOutOfGate__7daCow_cF4cXyz
 */
int daCow_c::checkOutOfGate(cXyz pos) {
    if (!isChaseCowGame()) {
        return 0;
    }

    cXyz x = pos - gate_pos;
    mDoMtx_stack_c::YrotS(gate_dir);
    mDoMtx_stack_c::multVecSR(&x, &x);

    if (x.z > 0.0f && fabsf(x.x) < 1100.0f) {
        return 1;
    }

    x = pos - pen_pos;
    mDoMtx_stack_c::YrotS(-pen_dir);
    mDoMtx_stack_c::multVecSR(&x, &x);

    return x.z > 0.0f ? 2 : 0;
}

/* 8065B8A8-8065B8D8 0033C8 0030+00 3/3 0/0 0/0 .text            getCowshedAngle__7daCow_cFv */
int daCow_c::getCowshedAngle() {
    return cLib_targetAngleY(&current.pos, &pen_pos);
}

/* 8065B8D8-8065BA30 0033F8 0158+00 1/1 0/0 0/0 .text            getCowshedDist__7daCow_cFv */
double daCow_c::getCowshedDist() {
    cXyz diff = pen_pos - current.pos;
    return diff.absXZ();
}

/* 8065BA30-8065BB34 003550 0104+00 2/2 0/0 0/0 .text            checkCowIn__7daCow_cFff */
int daCow_c::checkCowIn(f32 param_1, f32 param_2) {
    if (!isChaseCowGame()) {
        return 0;
    }

    double dist = getCowshedDist();
    if (dist < param_1) {
        int angle = getCowshedAngle();
        if (dist < param_2) {
            if ((s16)angle < pen_dir + 0x2000 && (s16)angle >= pen_dir + -0x2000 &&
                cLib_distanceAngleS(angle, field_0xc32.y) < 0x1800)
            {
                return 1;
            } else {
                return 2;
            }
        } else {
            if (((s16)angle < pen_dir + 0x2aaa) && (s16)angle >= pen_dir + -0x2aaa &&
                cLib_distanceAngleS(angle, field_0xc32.y) < 0x3000)
            {
                return 1;
            }
        }
    }
    return 0;
}

/* 8065BB34-8065BC68 003654 0134+00 5/5 0/0 0/0 .text            checkCowInOwn__7daCow_cFi */
bool daCow_c::checkCowInOwn(int param_1) {
    if (!isChaseCowGame()) {
        return false;
    }

    cXyz diff = current.pos - pen_pos;
    mDoMtx_stack_c::YrotS(-pen_dir);
    mDoMtx_stack_c::multVecSR(&diff, &diff);
    if (diff.z > 250.0f && fabsf(diff.x) < 220.0f &&
        cLib_distanceAngleS(pen_dir, field_0xc32.y) < param_1)
    {
        setProcess(&daCow_c::action_enter, 0);
        setEnterCount();
        return true;
    }

    return false;
}

/* 8065BC68-8065C32C 003788 06C4+00 9/0 0/0 0/0 .text            action_run__7daCow_cFv */
void daCow_c::action_run() {
    int nextAction = mMode;
    if (nextAction != 2) {
        if (nextAction < 2) {
            if (nextAction == 0) {
                calcRunAnime(1);
                mMode = 1;
                field_0xc90 = 0x1e;
                field_0xc94 = 0x32;
                field_0xca1 = 0;
                field_0xc9e = 0;
                field_0xc9d = 0;
            } else {
                calcRunAnime(0);

                if (field_0xc90 != 0) {
                    field_0xc90--;
                }
                if (field_0xc94 != 0) {
                    field_0xc94--;
                }
                if (field_0xca3 != 0) {
                    field_0xca3--;
                }

                f32 fVar11 = 1.0f;
                if (field_0xc94 == 0) {
                    field_0xc08 = 0;
                }
                if (!isChaseCowGame()) {
                    if (!field_0xc9d) {
                        setCarryStatus();
                    }

                    u8 bVar1 = field_0xc9e;
                    if (checkThrow()) {
                        dComIfGp_getVibration().StartShock(2, 0x1f, cXyz(0.0f, 1.0f, 0.0f));
                        field_0xc9e = bVar1;
                        return;
                    }
                } else {
                    if (!checkCowInOwn(0x8000)) {
                        checkPlayerSurprise();
                    }
                    return;
                }

                bool havePlayerPos = checkPlayerPos();

                if (field_0xc60 == 0 || field_0xc60 == 1 || field_0xc60 == 6) {
                    field_0xca1 = 0;
                }

                checkBeforeBg();

                f32 fVar12;

                if (!havePlayerPos || !checkPlayerWait()) {
                    fVar12 = field_0xc90 ? (field_0xc7c - 10.0f) * (field_0xc6c / 1000.0f) : 0.0f;
                } else {
                    f32 rand = cM_rndFX(200.0f);
                    field_0xc74 = rand * 20.0f;
                    field_0xc08 = 0;
                    fVar11 = 2.0f;

                    daPy_py_c* player = daPy_getPlayerActorClass();

                    if (player->checkHorseRide() || (u32)player->checkNowWolf() != 0) {
                        f32 rand = cM_rndF(100.0f);
                        field_0xc90 = (int)(rand + 30.0f) & 0xff;
                    }
                    fVar12 = field_0xc7c * (field_0xc6c / 1000.0f);
                }

                if (fVar12 == 0.0f) {
                    m_near_dist = 200.0f;
                    m_view_angle = -0x8000;
                    m_view_angle_wide = 0x2000;
                    fpcEx_Search(s_near_cow, this);
                    if (m_near_dist < 200.0f) {
                        fVar12 = field_0xc7c * (field_0xc6c / 1000.0f);
                    }
                } else {
                    m_near_dist = 200.0f;
                    m_view_angle = 0;
                    m_view_angle_wide = 0x2000;
                    fpcEx_Search(s_near_cow, this);
                    if (m_near_dist < 200.0f) {
                        fVar12 = field_0xc7c * (field_0xc6c / 1000.0f) * 0.5f;
                    }
                }
                if ((field_0xc54 == 0) || (field_0xc54 == 10)) {
                    s16 sVar3 = current.angle.y;
                    if ((field_0xca2 == 0) && (field_0xc08 != 0)) {
                        sVar3 = *(short*)(field_0xc08 + 0xc34);
                    }
                    s8 bVar1 = field_0xc61;
                    if (bVar1 == 2) {
                        sVar3 = sVar3 + 0x1000;
                    } else if (bVar1 < 2) {
                        if (bVar1 == 0) {
                            int sVar7 = getCowshedAngle();
                            s16 playerAngle = fopAcM_searchPlayerAngleY(this);
                            sVar3 = playerAngle - 0x8000;
                            if (isChaseCowGame() &&
                                cLib_distanceAngleS(sVar7, (field_0xc32).y) < 0x3000 &&
                                cLib_distanceAngleS(sVar7, playerAngle) > 0x5800)
                            {
                                sVar3 = sVar7;
                            }
                        } else {
                            sVar3 = sVar3 + -0x1000;
                        }
                    } else if (bVar1 == 4) {
                        sVar3 = sVar3 + 0x4000;
                    } else if (bVar1 < 4) {
                        sVar3 = sVar3 + -0x4000;
                    }
                    if (havePlayerPos == 0) {
                        sVar3 = sVar3 + field_0xc74;
                    }
                    field_0xc72 = sVar3;
                }

                int outOfGate = checkOutOfGate(current.pos);
                if (outOfGate) {
                    field_0xc90 = 0x96;
                    field_0xc9d = outOfGate;
                }
                if (field_0xc9d == 1) {
                    field_0xc72 = gate_dir - 0x8000;
                } else if (field_0xc9d == 2) {
                    field_0xc72 = pen_dir - 0x8000;
                }
                int cowIn = checkCowIn(800.0f, 300.0f);
                if (cowIn != 1) {
                    setProcess(&daCow_c::action_enter, 0);
                } else {
                    if (cowIn == 2) {
                        fVar12 = field_0xc7c * (field_0xc6c / 1000.0f);
                    }
                    if (field_0xca3 == 0) {
                        field_0xca1 = 0;
                    } else {
                        this->mShouldSetEffect = 1;
                        fVar11 = 4.0f;
                        fVar12 = (field_0xc6c / 1000.0f) * 45.0f;
                        field_0xca1 = field_0xca1 + 1;
                        f32 rand = cM_rndF(50.0f);

                        if ((((int)(rand + 100.0f) & 0xffU) <= field_0xca1) &&
                                daPy_getPlayerActorClass()->checkHorseRide() ||
                            daPy_getPlayerActorClass()->checkNowWolf())
                        {
                            field_0xca0 = 0;
                            field_0xca1 = 0;

                            setProcess(&daCow_c::action_angry, 0);
                            return;
                        }
                    }
                    if (fVar12 < 0.0f) {
                        fVar12 = 0.0f;
                    }
                    cLib_chaseF(&speedF, fVar12, fVar11);
                    cLib_addCalcAngleS2(&current.angle.y, field_0xc72, 8, 0x400);
                    cLib_addCalcAngleS2(&shape_angle.y, current.angle.y, 8, 0x400);
                    (field_0xc32).y = shape_angle.y;
                    if (speedF == 0.0f) {
                        setProcess(&daCow_c::action_wait, 0);
                    }
                }
            }
        } else {
            if (nextAction < 4) {
                mNoNearCheckTimer = 0x1e;
                field_0xca3 = 0;
            }
        }
    }
}

/* 8065C32C-8065C508 003E4C 01DC+00 1/1 0/0 0/0 .text            checkCurringPen__7daCow_cFv */
bool daCow_c::checkCurringPen() {
    daPy_py_c* player = daPy_getPlayerActorClass();
    if (player->current.pos.abs(pen_pos) < 2500.0f) {
        return false;
    }
    cXyz positions;

    camera_class* camera = dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0));
    int windowId = dComIfGp_getCameraWinID(fopCamM_GetParam(camera));
    scissor_class* scissor = dComIfGp_getWindow(windowId)->getScissor();
    mDoLib_project(&current.pos, &positions);

    if (positions.x > 0.0f && positions.x < scissor->width && positions.y > 0.0f &&
        positions.y < scissor->height)
    {
        return false;
    } else {
        return true;
    }
}

/* 8065C508-8065C680 004028 0178+00 2/2 0/0 0/0 .text            setCowInCage__7daCow_cFv */
void daCow_c::setCowInCage() {
    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
        mSph[iSphere].OffCoSetBit();
        mSph[iSphere].OnCoNoCrrBit();
        mCcStts.ClrCcMove();
    }
    mAcchCir.SetWall(0.0f, 0.0f);

    u8 cowIndex = cM_rndF(20.0f);
    if (l_CowRoomNo & 1 << cowIndex) {
        for (int i = 0; i < 0x14; i++) {
            if (!(l_CowRoomNo & 1 << i)) {
                cowIndex = i;
                break;
            }
        }
    }

    if (cowIndex >= 0x14) {
        cowIndex = 0x13;
    }

    current.pos.x = l_CowRoomPosX[cowIndex];
    current.pos.z = l_CowRoomPosZ[cowIndex & 1];

    old.pos = current.pos;

    l_CowRoomNo |= 1 << cowIndex;
    if ((cowIndex & 1) == 0) {
        field_0xc32.y = -0x8000;
        shape_angle.y = 0x8000;
        current.angle.y = 0x8000;
    } else {
        field_0xc32.y = 0;
        shape_angle.y = 0;
        current.angle.y = 0;
    }
}

/* 8065C680-8065C70C 0041A0 008C+00 2/2 0/0 0/0 .text            setEnterCount__7daCow_cFv */
void daCow_c::setEnterCount() {
    dTimer_createGetIn2D(2, current.pos);
    dMeter2Info_setNowCount(dMeter2Info_getNowCount() + 1);

    field_0xc90 = 0x32;
    field_0xc9f = 1;
    field_0xca9 = false;

    if ((u32)dMeter2Info_getNowCount() == (u32)dMeter2Info_getMaxCount()) {
        field_0xca9 = true;
    }
}

/* 8065C70C-8065CFBC 00422C 08B0+00 2/0 0/0 0/0 .text            action_enter__7daCow_cFv */
void daCow_c::action_enter() {
    cXyz penDistance = current.pos - pen_pos;
    f32 penDistanceAbs = penDistance.absXZ();
    cXyz penDistanceNow = penDistance;
    mDoMtx_stack_c::YrotS(-pen_dir);
    mDoMtx_stack_c::multVecSR(&penDistanceNow, &penDistanceNow);

    int nextAction = mMode;
    if (nextAction != 2) {
        if (nextAction < 2) {
            if (nextAction != 0) {
                if (field_0xc9f == 2) {
                    s16 targetAngle = cLib_targetAngleY(&current.pos, &field_0xc20);
                    cLib_addCalcAngleS2(&current.angle.y, targetAngle, 4, 0x100);
                    cLib_addCalcAngleS2(&shape_angle.y, current.angle.y, 8, 0x800);

                    field_0xc32.y = shape_angle.y;

                    if (current.pos.abs(field_0xc20) < 50.0f) {
                        setBck(0xf, 0, 12.0f, 1.0f);
                        this->speedF = 0;
                        field_0xc9f = 3;
                    }
                    if (checkCurringPen()) {
                        field_0xc9f = 4;
                        setCowInCage();
                    }

                } else {
                    u8 bVar1 = field_0xc9f;
                    if (bVar1 < 2) {
                        if (bVar1 == 0) {
                            calcRunAnime(0);

                            if (penDistanceAbs >= 80.0f) {
                                field_0xc72 = getCowshedAngle();
                                if (penDistanceAbs >= 200.0f) {
                                    cLib_addCalcAngleS2(&current.angle.y, field_0xc72, 8, 0x800);
                                    cLib_addCalcAngleS2(&shape_angle.y, current.angle.y, 8, 0x800);
                                } else {
                                    cLib_addCalcAngleS2(&current.angle.y, field_0xc72, 4, 0x1000);
                                    cLib_addCalcAngleS2(&shape_angle.y, current.angle.y, 4, 0x1000);
                                }
                                field_0xc32.y = shape_angle.y;
                            } else {
                                setEnterCount();
                            }
                        } else {
                            calcRunAnime(0);
                            field_0xc72 = 0xc000;
                            cLib_addCalcAngleS2(&current.angle.y, field_0xc72, 4, 0x800);
                            cLib_addCalcAngleS2(&shape_angle.y, current.angle.y, 8, 0x800);
                            field_0xc32.y = shape_angle.y;
                            if (penDistanceNow.z > 500.0f) {
                                for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
                                    mSph[iSphere].OnCoSetBit();
                                }
                                mSph[0].SetCoSPrm(0x19);
                                if (!cLib_calcTimer((int*)&field_0xc90)) {
                                    penDistanceNow.set(cM_rndFX(4.0f) * 40.0f, 0.0f,
                                                       cM_rndFX(5.0f) * 40.0f + 2000.0f);
                                    mDoMtx_stack_c::YrotS(pen_dir);
                                    mDoMtx_stack_c::multVecSR(&penDistanceNow, &penDistanceNow);

                                    field_0xc20 = pen_pos + penDistance;

                                    setBck(0x1b, 2, 12.0f, 1.0f);
                                    this->speedF = 0x40400000;
                                    field_0xc9f = 2;

                                    if ((u32)dMeter2Info_getNowCount() ==
                                            (u32)dMeter2Info_getMaxCount() &&
                                        field_0xca9)
                                    {
                                        daNpc_Aru_c* aru;
                                        if (fopAcM_SearchByName(PROC_NPC_ARU, (fopAc_ac_c**)&aru)) {
                                            aru->setLastIn();
                                        }
                                    }
                                }
                            }
                        }
                    } else if (bVar1 == 4) {
                        this->speedF = 0;
                        field_0xca5 = 1;

                        setProcess(&daCow_c::action_wait, 0);
                        mAcchCir.SetWall(100.0f, 110.0f);
                    } else if (bVar1 < 4) {
                        if (mpMorf->isStop()) {
                            if (current.pos.abs(field_0xc20) <= 100.0f) {
                                setBck(0xf, 0, 12.0f, 1.0f);
                            } else {
                                field_0xc9f = 2;
                                this->speedF = 0x40400000;
                                setBck(0x1b, 2, 12.0f, 1.0f);
                            }
                        }
                        if (checkCurringPen()) {
                            field_0xc9f = 4;
                            setCowInCage();
                        }
                    }
                    if (field_0xc9f < 2) {
                        m_near_dist = 200.0f;
                        m_view_angle = 0;
                        m_view_angle_wide = 0x2000;
                        fpcM_Search(s_near_cow, this);

                        f32 fVar2 = 30.0f;
                        if (m_near_dist < 200.0f) {
                            fVar2 = 2.0f;
                        }
                        if (fVar2 <= 30.0f) {
                            if (fVar2 < 0.0f) {
                                fVar2 = 2.0f;
                            }
                        } else {
                            fVar2 = fVar2 - 1.0f;
                        }
                        cLib_chaseF(&this->speedF, fVar2, 1.5f);
                    }
                }
            } else {
                calcRunAnime(1);
                field_0xc6c = 1;
                field_0xc9f = 0;
                for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
                    mSph[iSphere].OffCoSetBit();
                    mCcStts.ClrCcMove();
                }
            }
        }
    }

    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
        this->mSph[iSphere].OffTgSetBit();
    }
}

/* 8065CFBC-8065D03C 004ADC 0080+00 4/4 0/0 1/1 .text            isAngry__7daCow_cFv */
bool daCow_c::isAngry() {
    if (checkProcess(&daCow_c::action_angry) &&
        (field_0xc9f == 0 || field_0xc9f == 1 || field_0xc9f == 5))
    {
        return true;
    } else {
        return false;
    }
}

/* 8065D03C-8065D0B8 004B5C 007C+00 1/1 0/0 1/1 .text            isGuardFad__7daCow_cFv */
bool daCow_c::isGuardFad() {
    if (checkProcess(&daCow_c::action_wolf)) {
        return true;
    } else {
        if ((u32)daPy_getPlayerActorClass()->checkNowWolf() != (u32)0) {
            return isAngry();
        } else {
            return false;
        }
    }
}

/* 8065D0B8-8065D17C 004BD8 00C4+00 0/0 0/0 1/1 .text            setAngryHit__7daCow_cFv */
void daCow_c::setAngryHit() {
    if (isAngry()) {
        field_0xc72 = field_0xc32.y - 0x8000;
        speedF = 0.0f;
        current.angle.y = field_0xc32.y;
        calcRunAnime(1);

        if (!daPy_getPlayerActorClass()->checkHorseRide() &&
            (u32)daPy_getPlayerActorClass()->checkNowWolf() == (u32)0)
        {
            field_0xc9f = 3;
        } else {
            field_0xc9f = 4;
            field_0xc90 = 0x1e;
            field_0xcb0 = 0.0f;
            field_0xcb4 = 2;
        }
    }
}

/* 8065D17C-8065D230 004C9C 00B4+00 1/1 0/0 0/0 .text            checkBeforeBgAngry__7daCow_cFs
 */
bool daCow_c::checkBeforeBgAngry(s16 angle) {
    checkBeforeBg();

    if (field_0xc6c < 1000.0f) {
        s16 angleDistance = cLib_distanceAngleS(field_0xc70, field_0xc32.y);
        if (field_0xc6c < (fabsf(cM_ssin(angleDistance) * 250.0f) + 200.0f) &&
            angleDistance >= angle)
        {
            speedF = 0.0f;
            return true;
        }
    }
    return false;
}

/* 8065D230-8065D29C 004D50 006C+00 2/2 0/0 0/0 .text            setRedTev__7daCow_cFv */
void daCow_c::setRedTev() {
    if (field_0xcb4 < 2) {
        if (field_0xcb4 == 0) {
            if (field_0xcac == 1.0f) {
                field_0xcb0 = 0.0f;
                field_0xcb4 = 1;
            }
        } else {
            if (field_0xcac == 0.0f) {
                field_0xcb0 = 1.0f;
                field_0xcb4 = 0;
            }
        }
    }
}

/* 8065D29C-8065D2F0 004DBC 0054+00 1/1 0/0 0/0 .text            setAngryTurn__7daCow_cFv */
void daCow_c::setAngryTurn() {
    setBck(0xe, 0, 0.0f, 1.0f);
    speedF = field_0xc7c;
    field_0xc9f = 5;
}

/* 8065D2F0-8065DC08 004E10 0918+00 7/0 0/0 0/0 .text            action_angry__7daCow_cFv */
void daCow_c::action_angry() {
    daPy_py_c* player = daPy_getPlayerActorClass();
    f32 playerDistance = fopAcM_searchPlayerDistance(this);
    s16 playerAngle = fopAcM_searchPlayerAngleY(this);

    s16 targetZ = 0;
    int uVar3 = mMode;
    if (uVar3 == 2) {
        return;
    }
    if (uVar3 > 1) {
        if (uVar3 > 3) {
            return;
        }
        field_0xc94 = 0;
        field_0xc90 = 0;
        field_0xcb0 = 0.0f;
        field_0xc3e.z = 0;
        field_0xc38.y = 0;
        field_0xc3e.y = 0;

        attention_info.flags &= ~1;
        return;
    }
    if (uVar3 == 0) {
        calcRunAnime(1);
        mMode = 1;
        if (field_0xca0 == 0) {
            field_0xc9f = 0;
            field_0xc90 = 0x14;
            field_0xca0 = 1;
        } else {
            field_0xc9f = 1;
        }
        field_0xc98 = 200;
        field_0xcb0 = 1.0f;
        field_0xcb4 = 0;
        if (field_0xca1 == 0) {
            speedF = 75.0f;
        } else {
            speedF = 60.0f;
        }

        mSound.startCreatureVoice(Z2SE_GOAT_V_ANGRY, -1);
        field_0xc84 = 0;
        return;
    }
    setSeSnort();

    if (player->checkHorseRide()) {
        attention_info.flags &= ~1;
    } else {
        attention_info.flags |= 1;
    }
    if (field_0xc9f == 1) {
        setCarryStatus();
    }
    if (checkThrow()) {
        dComIfGp_getVibration().StartShock(4, 0x1f, cXyz(0.0f, 1.0f, 0.0f));
        field_0xc9e = 1;
        return;
    }
    if (field_0xc84 == 0) {
        if (!player->checkHorseRide() && mSph[0].ChkCoHit()) {
            if (fopAcM_GetName(mSph[0].GetCoHitObj()->GetAc()) == PROC_ALINK) {
                cXyz pos = daPy_getPlayerActorClass()->current.pos;
                pos.y += 100.0f;
                cXyz pos2;
                mDoMtx_stack_c::transS(pos);
                mDoMtx_stack_c::YrotM(shape_angle.y);
                mDoMtx_stack_c::transM(0.0f, 0.0f, 200.0f);
                mDoMtx_stack_c::multVecZero(&pos2);

                dBgS_LinChk linkChck;
                linkChck.Set(&pos, &pos2, this);

                s16 angle;
                if (dComIfG_Bgsp().LineCross(&linkChck)) {
                    angle = shape_angle.y + -0x8000;
                } else {
                    angle = shape_angle.y;
                }

                field_0xc84 = 0x1e;
                daPy_getPlayerActorClass()->setThrowDamage(angle, 35.0f, 40.0f, 0, 0, 0);
            }
        }
    }
    if (field_0xc98 == 0) {
        if (field_0xc9f != 5) {
            setProcess(&daCow_c::action_run, 0);
            field_0xc9e = 1;
            return;
        }
    }
    if (field_0xc84) {
        field_0xc84--;
    }
    if (field_0xc98) {
        field_0xc98--;
    }
    if (field_0xc94) {
        field_0xc94--;
    }
    if (field_0xc90) {
        field_0xc90--;
    }
    setRedTev();
    if (checkCowInOwn(0x4000)) {
        return;
    }
    if (field_0xca1 != 0) {
        if (checkOutOfGate(daPy_getPlayerActorClass()->current.pos)) {
            setProcess(&daCow_c::action_run, 0);
            field_0xc9e = 1;
            return;
        } else {
            if (checkOutOfGate(current.pos)) {
                setProcess(&daCow_c::action_run, 0);
                field_0xc9e = 1;
                return;
            }
        }
    }
    int bVar1 = field_0xc9f;
    if (bVar1 != 3) {
        if (bVar1 < 3) {
            if (bVar1 == 1) {
                calcRunAnime(0);
                mShouldSetEffect = 1;
                targetZ = 0x2000;
                f32 targetSpeed = 50.0f;
                if (player->getSpeedF() >= 15.0f) {
                    targetSpeed = player->getSpeedF() + 35.0f;

                    if (field_0xca1) {
                        if (targetSpeed > 60.0f) {
                            targetSpeed = 60.0f;
                        }
                    } else {
                        if (targetSpeed > 75.0f) {
                            targetSpeed = 75.0f;
                        }
                    }
                } else {
                    targetSpeed = 50.0f;
                }
                cLib_chaseF(&speedF, targetSpeed, 4.0f);

                if (checkBeforeBgAngry(0x6000)) {
                    current.angle.y = field_0xc32.y;
                    field_0xc9f = 2;
                    return;
                }
                if (field_0xc94 == 0) {
                    field_0xc72 = playerAngle;
                    s32 angleToPlayer = cLib_distanceAngleS(playerAngle, field_0xc32.y);

                    if (player->getSpeedF() <= 5.0f) {
                        if (playerDistance >= 500.0f) {
                            if ((playerDistance >= 1500.0f) && angleToPlayer > 0x57ff) {
                                current.angle.y = field_0xc32.y;
                                field_0xc9f = 2;
                                return;
                            }
                        } else {
                            field_0xc94 = 0x23;
                        }
                    } else {
                        if (playerDistance >= 350.0f) {
                            if ((playerDistance < 1200.0f) && angleToPlayer > 0x3fff) {
                                setAngryTurn();
                                return;
                            }
                        } else {
                            field_0xc94 = 10;
                        }
                    }
                } else {
                    field_0xc72 = current.angle.y;
                }

                int lockedOn;
                if (field_0xca1 && field_0xc94 && dComIfGp_getAttention().LockonTruth() &&
                    dComIfGp_getAttention().LockonTarget(0) &&
                    cLib_distanceAngleS(playerAngle, field_0xc32.y) < 0x800)
                {
                    field_0xc72 = playerAngle;
                    lockedOn = true;
                } else {
                    lockedOn = false;
                }
                if (lockedOn) {
                    cLib_chaseAngleS(&current.angle.y, field_0xc72, 0x800);
                } else {
                    cLib_addCalcAngleS2(&current.angle.y, field_0xc72, 0x10, 0x400);
                }
                cLib_addCalcAngleS2(&shape_angle.y, current.angle.y, 4, 0x800);
                field_0xc32.y = shape_angle.y;
            } else if (bVar1 == 0) {
                calcRunAnime(0);
                mShouldSetEffect = 1;
                targetZ = 0x2000;
                if (checkBeforeBgAngry(0)) {
                    current.angle.y = (field_0xc32).y;
                    field_0xc9f = 2;
                } else {
                    if (field_0xc90 == 0) {
                        field_0xc9f = 1;
                    }
                }
                cLib_addCalcAngleS2(&shape_angle.y, current.angle.y, 8, 0x800);
                field_0xc32.y = shape_angle.y;
            } else {
                calcRunAnime(0);
                if (checkBeforeBgAngry(0)) {
                    speedF = 0.0f;
                }
                if (cLib_chaseF(&speedF, 0.0f, 4.0f)) {
                    field_0xc9f = 3;
                    field_0xc72 = playerAngle;
                    current.angle.y = field_0xc32.y;
                }
            }

            cLib_chaseS(&field_0xc3e.z, targetZ, 0x400);
            return;
        }
        if (bVar1 == 5) {
            mpMorf->setPlaySpeed(1.0f);

            playerAngle = fopAcM_searchPlayerAngleY(this);
            cLib_chaseAngleS(&field_0xc32.y, playerAngle, 0x800);

            targetZ = field_0xc32.y;
            shape_angle.y = targetZ;
            current.angle.y = targetZ;

            if (this->mpMorf->isStop()) {
                field_0xc9f = 1;
                calcRunAnime(1);
            }
            return;
        }
        if (bVar1 > 4) {
            cLib_chaseS(&field_0xc3e.z, targetZ, 0x400);
            return;
        } else {
            calcRunAnime(0);
            if (field_0xc90) {
                cLib_chaseS(&field_0xc3e.z, targetZ, 0x400);
                return;
            }
        }
    }

    calcRunAnime(0);
    speedF = 15.0f;
    cLib_addCalcAngleS2(&current.angle.y, field_0xc72, 8, 0x400);
    targetZ = current.angle.y;
    shape_angle.y = targetZ;
    (field_0xc32).y = targetZ;
    setBodyAngle(field_0xc72);
    s16 angleDist = cLib_distanceAngleS(field_0xc72, field_0xc32.y);
    if (angleDist < 0x200 && field_0xc3e.y < 0x200) {
        if (field_0xc9f == 4) {
            setProcess(&daCow_c::action_run, 0);
        } else {
            field_0xc9f = 1;
            field_0xc38.y = 0;
            field_0xc3e.y = 0;
        }
    }
}

/* 8065DC08-8065DE70 005728 0268+00 4/4 0/0 0/0 .text            calcCatchPos__7daCow_cFfi */
void daCow_c::calcCatchPos(f32 distance, int someBool) {
    daPy_py_c* player = daPy_getPlayerActorClass();
    s16 offsetAngle = player->shape_angle.y + -0x8000;
    s16 angle =
        cM_atan2s(current.pos.absXZ(player->current.pos), player->current.pos.y - field_0xc44);

    cXyz catchPos(0.0f, distance * cM_scos(angle), distance * cM_ssin(angle));
    shape_angle.x = angle - 0x4000;
    field_0xc32.x = angle - 0x4000;

    if (someBool != 0) {
        cLib_addCalcAngleS(&shape_angle.y, offsetAngle, 8, 0x400, 0x100);
        angle = shape_angle.y;
        current.angle.y = angle;
        field_0xc32.y = angle;
        cXyz target;
        player = daPy_getPlayerActorClass();
        cLib_offsetPos(&target, &player->current.pos, offsetAngle, &catchPos);
        cLib_chasePos(&current.pos, target, 30.0f);
    } else {
        player = daPy_getPlayerActorClass();
        cLib_offsetPos(&current.pos, &player->current.pos, offsetAngle, &catchPos);
        shape_angle.y = offsetAngle;
        current.angle.y = offsetAngle;
        field_0xc32.y = offsetAngle;
    }
}

/* 8065DE70-8065DF40 005990 00D0+00 1/1 0/0 0/0 .text            executeCrazyWait__7daCow_cFv */
void daCow_c::executeCrazyWait() {
    if (mFlags & 0x20) {
        mFlags &= ~0x0200;
    }
    if (mFlags & 0x200) {
        field_0xca6 = 0;
        mFlags &= ~0x0200;
    }
    if (mFlags & 0x40) {
        field_0xc9f = 1;
        speedF = 30.0f;
        setBck(0x14, 2, 12.0f, 1.0f);
        field_0xca6 = 0;
        mFlags = 0;

        mAcchCir.SetWall(100.0f, 110.0f);
        field_0xc90 = 0x1e;
        fopAcM_OffStatus(this, 0x100);
    }
}

/* 8065DF40-8065E6BC 005A60 077C+00 1/1 0/0 0/0 .text            executeCrazyDash__7daCow_cFv */
void daCow_c::executeCrazyDash() {
    this->mShouldSetEffect = 1;

    cXyz cStack_20 = dPath_GetPnt(mPath, field_0xc10)->m_position;
    cXyz cStack_38;
    setSeSnort();
    setRushVibration(2);

    if (field_0xc90 == 1) {
        mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_ANGRY), -1);
    }
    if (field_0xc10 == 4 || field_0xc10 == 5) {
        cLib_chaseS(&field_0xc3e.z, 0x1000, 0x400);

        if (mFlags == 0) {
            if (field_0xc10 == 4) {
                cXyz cStack_2c = dPath_GetPnt(mPath, 3)->m_position;
                s16 sVar4 = cLib_targetAngleY(cStack_2c, cStack_20);

                if (current.pos.abs(cStack_20) <= 600.0f) {
                    cLib_addCalcAngleS(&current.angle.y, sVar4, 0x10, 0x800, 0x100);
                    if (current.pos.abs(cStack_20) >= 250.0f) {
                        return;
                    }
                    field_0xc10++;
                    return;
                }
                field_0xcb0 = 0;
                s16 sVar5 = cLib_targetAngleY(&cStack_2c, &daPy_getPlayerActorClass()->current.pos);
                if (sVar5 - sVar4 < 0) {
                    cStack_38.set(200.0f, 0.0f, 0.0f);
                } else {
                    cStack_38.set(-200.0f, 0.0f, 0.0f);
                }
                cLib_offsetPos(&cStack_20, &cStack_20, sVar4, &cStack_38);
                field_0xc20 = cStack_20;
            } else {
                field_0xc20 = cStack_20;
            }
            s16 sVar4 = cLib_targetAngleY(&current.pos, &field_0xc20);
            cLib_addCalcAngleS(&current.angle.y, sVar4, 0x10, 0x800, 0x100);
            if (current.pos.abs(cStack_20) < 250.0f) {
                field_0xc10++;
            }
        } else if ((mFlags & 1) == 0) {
            if ((mFlags & 2) == 0) {
                if ((mFlags & 4) != 0) {
                    initCrazyAttack(0);
                    field_0xc10 = 6;
                    mFlags = mFlags & 0xfffb;
                }
            } else {
                initCrazyCatch(0);
                field_0xc10 = 6;
                mFlags &= ~0x0004;
                dComIfGp_getVibration().StartShock(8, 0x1f, cXyz(0.0f, 1.0f, 0.0f));
            }
        } else {
            initCrazyBeforeCatch(0);
            field_0xc10 = 6;
            mFlags &= ~1;
        }
    } else {
        s16 sVar4 = cLib_targetAngleY(&current.pos, &cStack_20);
        cLib_addCalcAngleS(&current.angle.y, sVar4, 0x10, 0x800, 0x100);
        if (current.pos.abs(cStack_20) < 200.0f) {
            field_0xc10++;
            if (mPath->m_num <= field_0xc10) {
                if (mPath->m_nextID == -1) {
                    field_0xc9f = 7;
                    speedF = 0.0f;
                } else {
                    field_0xc10 = 0;
                    mPath = dPath_GetRoomPath(mPath->m_nextID, fopAcM_GetRoomNo(this));
                    field_0xc9f = 6;
                }
            }
        }
    }
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x400, 0x100);
    field_0xc32.y = shape_angle.y;
}

/* 8065E6BC-8065E6E8 0061DC 002C+00 2/2 0/0 0/0 .text initCrazyBeforeCatch__7daCow_cFi */
void daCow_c::initCrazyBeforeCatch(int param_0) {
    field_0xc9f = 2;
    speedF = 0.0f;
    field_0xc3e.z = 0;
    field_0xc63 = 1;
    gravity = 0.0f;
}

/* 8065E6E8-8065E7D0 006208 00E8+00 2/2 0/0 0/0 .text executeCrazyBeforeCatch__7daCow_cFv
 */
void daCow_c::executeCrazyBeforeCatch() {
    calcCatchPos(-220.0f, 1);
    if (this->mFlags & 2) {
        initCrazyCatch(0);
        mFlags &= ~2;
        dComIfGp_getVibration().StartShock(8, 0x1f, cXyz(0.0f, 1.0f, 0.0f));
    } else {
        if (this->mFlags & 4) {
            if (daPy_getPlayerActorClass()->speedF == 0.0f) {
                initCrazyAttack(0);
            } else {
                initCrazyAttack(1);
            }
            mFlags &= ~4;
        }
    }
}

/* 8065E7D0-8065E888 0062F0 00B8+00 3/3 0/0 0/0 .text            initCrazyCatch__7daCow_cFi */
void daCow_c::initCrazyCatch(int param_0) {
    setBck(0x17, 0, 0.0f, 1.0f);
    field_0xc9f = 3;
    speedF = 0.0f;
    field_0xc3e.z = 0;
    field_0xc60 = 0;
    calcCatchPos(-220.0f, 1);

    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
        mSph[iSphere].OffCoSetBit();
        mCcStts.ClrCcMove();
    }

    field_0xc63 = 1;
    gravity = 0.0f;
}

/* 8065E888-8065EAF4 0063A8 026C+00 2/2 0/0 0/0 .text            executeCrazyCatch__7daCow_cFv
 */
void daCow_c::executeCrazyCatch() {
    f32 fVar2 = -220.0f;
    int bVar1 = field_0xc60;

    if (bVar1 != 2) {
        if (bVar1 < 2) {
            mShouldSetEffect = 1;
            if (!field_0xc60 && mpMorf->isStop()) {
                setBck(5, 2, 0.0f, 1.0f);
                field_0xc60 = 1;
            }

            if (daPy_getPlayerActorClass()->speedF == 0.0f) {
                setBck(0x10, 0, 0.0f, 1.0f);
                field_0xc60 = 2;
            }
        } else if (bVar1 < 5) {
            fVar2 = -260.0f;

            if (!field_0xc90) {
                if (field_0xc60 == 3) {
                    setBck(0x12, 2, 0.0f, 1.0f);
                    field_0xc90 = 0x3c;
                    field_0xc60 = 4;
                } else {
                    setBck(0x11, 2, 0.0f, 1.0f);
                    field_0xc90 = 0x3c;
                    field_0xc60 = 3;
                }
            }
        }

    } else {
        fVar2 = -260.0f;
        if (mpMorf->isStop()) {
            setBck(0x12, 2, 0.0f, 1.0f);
            field_0xc60 = 4;
        }
    }

    if (mFlags != 0) {
        if (mFlags & 8) {
            initCrazyThrow(0);
        } else {
            if (mFlags & 0x10) {
                initCrazyThrow(1);
            } else {
                if (mFlags & 4) {
                    if (daPy_getPlayerActorClass()->speedF == 0.0f) {
                        initCrazyAttack(0);
                    } else {
                        initCrazyAttack(1);
                    }
                }
            }
        }
        mFlags = 0;
    } else {
        calcCatchPos(fVar2, 1);
    }
}

/* 8065EAF4-8065EBF0 006614 00FC+00 1/1 0/0 0/0 .text            initCrazyThrow__7daCow_cFi */
void daCow_c::initCrazyThrow(int param_1) {
    if (param_1) {
        setBck(0xb, 0, 5.0f, 1.0f);
    } else {
        setBck(10, 0, 5.0f, 1.0f);
    }
    field_0xc61 = param_1;

    field_0xc60 = 0;
    field_0xc62 = 1;
    field_0xc9f = 4;
    gravity = 0.0f;
    speedF = 0.0f;
    speed.y = 0.0f;

    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
        mSph[iSphere].OffCoSetBit();
        mCcStts.ClrCcMove();
    }
    calcCatchPos(-260.0f, 0);

    field_0xc20 = current.pos;
    field_0xc63 = 1;
    gravity = 0.0f;
}

/* 8065EBF0-8065F088 006710 0498+00 2/2 0/0 0/0 .text            executeCrazyThrow__7daCow_cFv
 */
void daCow_c::executeCrazyThrow() {
    int bVar1 = field_0xc60;
    if (bVar1 != 3) {
        if (bVar1 < 3) {
            if (bVar1 != 1) {
                if (bVar1 == 0) {
                    current.pos = field_0xc20;

                    if (mpMorf->checkFrame(10.0f)) {
                        mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_THROWN), -1);
                    }
                    if (mpMorf->checkFrame(34.0f)) {
                        field_0xc60 = 1;
                        field_0xc62 = 2;
                        field_0xc63 = 0;
                        gravity = -4.0f;

                        for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
                            mSph[iSphere].OnCoSetBit();
                        }

                        if (!field_0xc61) {
                            current.angle.y = daPy_getPlayerActorClass()->shape_angle.y + -0x6800;
                        } else {
                            current.angle.y = daPy_getPlayerActorClass()->shape_angle.y + 0x7000;
                        }
                        speedF = 10.0f;
                        speed.y = 10.0f;
                    }
                    return;
                }
            } else {
                field_0xc20.y += 100.0f;

                dBgS_LinChk linChk;
                linChk.Set(&field_0xc20, &current.pos, NULL);
                if (dComIfG_Bgsp().LineCross(&linChk)) {
                    current.pos = linChk.GetCross();

                    cM3dGPla plane;
                    dComIfG_Bgsp().GetTriPla(linChk, &plane);

                    cXyz* normal = plane.GetNP();
                    current.pos.x += normal->x * 50.0f;
                    current.pos.z += normal->z * 50.0f;
                    speedF = 0.0f;
                    old.pos = current.pos;
                }
                field_0xc60 = 2;
            }

            cLib_chaseAngleS(&field_0xc32.x, 0, 0x800);
            field_0xc62 = 3;

            if (mAcch.ChkGroundHit()) {
                shape_angle.x = field_0xc32.x;

                mSound.startCreatureSound(JAISoundID(Z2SE_GOAT_V_ANGRY), 0, -1);
                dComIfGp_getVibration().StartShock(5, 0x1f, cXyz(0.0f, 1.0f, 0.0f));

                speed.y = 0.0f;
                field_0xc90 = 0x5a;
                field_0xc60 = 3;
                field_0xc62 = 0;
                mShouldSetEffect = 2;

                if (field_0xc61) {
                    setBck(8, 2, 0.0f, 1.0f);
                    field_0xc32.y -= 0x7000;
                    field_0xc76 = 0xfc18;

                } else {
                    setBck(7, 2, 0.0f, 1.0f);
                    field_0xc32.y += 0x7000;
                    field_0xc76 = 1000;
                }
            }
        } else {
            if (bVar1 == 5) {
                if (!field_0xc90) {
                    initCrazyBack(0);
                }
            } else if (bVar1 < 5) {
                if (mpMorf->isStop()) {
                    if (this->mPrm0 == 3) {
                        setBck(0x1a, 2, 10.0f, 1.0f);
                        field_0xc60 = 5;
                        field_0xc90 = 10;
                    } else {
                        initCrazyBack(0);
                    }
                }
            }
        }
    } else {
        field_0xc32.y += field_0xc76;
        cLib_chaseAngleS(&field_0xc76, 0, 0x1e);
        if (cLib_chaseF(&speedF, 0.0f, 0.5f) && !field_0xc90) {
            field_0xc60 = 4;
            if (field_0xc61 == 0) {
                setBck(0xc, 0, 5.0f, 1.0f);
            } else {
                setBck(0xd, 0, 5.0f, 1.0f);
            }
            shape_angle.y = field_0xc32.y;
        }
    }
}

/* 8065F088-8065F144 006BA8 00BC+00 3/3 0/0 0/0 .text            initCrazyAttack__7daCow_cFi */
void daCow_c::initCrazyAttack(int param_1) {
    field_0xc3e.z = 0;
    field_0xc9f = 5;
    if (param_1) {
        field_0xc61 = 1;
        speedF = 0.0f;
        setBck(0x18, 0, 3.0f, 1.0f);
    } else {
        field_0xc61 = 0;
        setBck(0x16, 0, 3.0f, 1.0f);
    }
    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
        mSph[iSphere].OnCoSetBit();
    }
    gravity = -4.0f;
}

/* 8065F144-8065F308 006C64 01C4+00 2/2 0/0 0/0 .text            executeCrazyAttack__7daCow_cFv */
void daCow_c::executeCrazyAttack() {
    int bVar1 = field_0xc61;

    if (bVar1 != 1) {
        if (bVar1 == 0) {
            cLib_chaseF(&speedF, 10.0f, 1.0f);

            if (mpMorf->checkFrame(2.0f)) {
                mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_BREATH_SHAKE), -1);
            }
            if (mpMorf->isStop()) {
                initCrazyAway(0);
            }
        } else if (bVar1 < 3 && mpMorf->isStop()) {
            initCrazyAway(0);
        }
    } else {
        if (mpMorf->checkFrame(2.0f)) {
            mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_BREATH_SHAKE), -1);
        }

        if (mpMorf->checkFrame(10.0f)) {
            if (mPrm0 == 3) {
                initCrazyAway(0);
            } else {
                setBck(0x15, 0, 7.0f, 1.0f);
                field_0xc61 = 2;
            }
        }
    }
}

/* 8065F308-8065F37C 006E28 0074+00 1/1 0/0 0/0 .text            initCrazyAway__7daCow_cFi */
void daCow_c::initCrazyAway(int param_0) {
    field_0xc9f = 6;
    if (mPrm0 == 3) {
        setBck(0x14, 2, 0.0f, 1.0f);
    }
    s16 sVar1 = shape_angle.y;
    current.angle.y = sVar1;
    field_0xc32.y = sVar1;
    gravity = -4.0f;
}

/* 8065F37C-8065F6E0 006E9C 0364+00 1/1 0/0 0/0 .text            executeCrazyAway__7daCow_cFv */
void daCow_c::executeCrazyAway() {
    setSeSnort();

    if (current.pos.abs(daPy_getPlayerActorClass()->current.pos) < 2500.0f) {
        setRushVibration(2);
    }

    this->mShouldSetEffect = 1;
    cLib_chaseF(&speedF, 30.0f, 2.0f);
    cLib_chaseS(&field_0xc3e.z, 0x1000, 0x400);

    cXyz pointPos = dPath_GetPnt(mPath, field_0xc10)->m_position;
    cLib_addCalcAngleS(&current.angle.y, cLib_targetAngleY(current.pos, pointPos), 0x10, 0x800,
                       0x100);

    if (current.pos.abs(pointPos) < 200.0f) {
        field_0xc10++;

        if (mPath->m_num <= field_0xc10) {
            if (mPath->m_nextID == -1) {
                field_0xc9f = 7;
                speedF = 0.0f;
            } else {
                field_0xc10 = 0;
                mPath = dPath_GetRoomPath(mPath->m_nextID, fopAcM_GetRoomNo(this));
            }
        }
    }
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x400, 0x100);
    field_0xc32.y = shape_angle.y;
    if (mFlags & 0x20) {
        field_0xc9f = 7;
        speedF = 0.0f;
    }
}

/* 8065F6E0-8065F744 007200 0064+00 1/1 0/0 0/0 .text            executeCrazyEnd__7daCow_cFv */
void daCow_c::executeCrazyEnd() {
    mAcchCir.SetWall(0.0f, 0.0f);
    field_0xca6 = 1;
    mMode = 0;

    mPath = dPath_GetRoomPath((fopAcM_GetParam(this) & 0xff00) >> 8, fopAcM_GetRoomNo(this));
}

/* 8065F744-8065F7DC 007264 0098+00 1/1 0/0 0/0 .text            initCrazyBack__7daCow_cFi */
void daCow_c::initCrazyBack(int param_0) {
    if (mPrm0 == 3) {
        setBck(0x1c, 2, 10.0f, 1.0f);
        u8 pathIndex = (fopAcM_GetParam(this) & 0xff00) >> 8;
        if (pathIndex == 0xff) {
            return;
        }

        mPath = dPath_GetRoomPath(pathIndex, fopAcM_GetRoomNo(this));
        field_0xc10 = 3;
    } else {
        field_0xc90 = 0;
        field_0xc61 = 0;
        s16 angle = field_0xc32.y;
        shape_angle.y = angle;
        current.angle.y = angle;
        field_0xc9f = 8;
    }
}

/* 8065F7DC-8065FE50 0072FC 0674+00 2/1 0/0 0/0 .text            executeCrazyBack__7daCow_cFv */
void daCow_c::executeCrazyBack() {
    cXyz pointPos;
    s16 angle;

    switch (field_0xc61) {
    case 0:
        setActetcStatus();

        pointPos = dPath_GetPnt(mPath, field_0xc10)->m_position;
        angle = cLib_targetAngleY(current.pos, pointPos);
        cLib_addCalcAngleS(&current.angle.y, angle, 0x10, 0x100, 0x80);
        if (speedF <= 3.0f) {
            cLib_chaseF(&speedF, 2.0f, 1.0f);
        } else {
            cLib_chaseF(&speedF, 2.0f, 3.0f);
        }
        cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x100, 0x80);
        field_0xc32.y = shape_angle.y;
        setBodyAngle(angle);

        if (current.pos.abs(pointPos) < 300.0f && --field_0xc10 < 0) {
            speedF = 0.0f;
            field_0xc61 = 3;
            field_0xc72 -= 0x2000;
        }
        if (checkNadeNade()) {
            setBck(0x1a, 2, 10.0f, 1.0f);
            field_0xc61 = 1;
            speedF = 0.0f;
        }
        break;
    case 1:
        if (checkNadeNadeFinish()) {
            setBck(0xf, 0, 10.0f, 1.0f);
            field_0xc61 = 2;
            speedF = 0.0f;
        }
        break;
    case 2:
        if (mpMorf->checkFrame(35.0f)) {
            mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_BREATH_SHAKE), -1);
        }
        if (mpMorf->isStop()) {
            setBck(0x1c, 2, 10.0f, 1.0f);
            field_0xc61 = 0;
        }
        break;
    case 3:
        if (mpMorf->checkFrame(1.0f)) {
            setBck(0x1a, 2, 10.0f, 1.0f);
            field_0xc61 = 4;
        }
        break;
    case 4:
        fopAcM_OnStatus(this, 0x100);
        if (fopAcM_CheckCondition(this, 4)) {
            fopAcM_delete(this);
        }
        break;
    case 5:
        setBck(0x18, 0, 3.0f, 1.0f);
        field_0xc61 = 6;
        break;
    case 6:
        if (mpMorf->isStop()) {
            if (field_0xc10 < 0) {
                setBck(0x1c, 2, 10.0f, 1.0f);
                field_0xc61 = 3;
            } else if (field_0xc10 < 2) {
                setBck(0x1c, 2, 10.0f, 1.0f);
                field_0xc61 = 0;
            } else {
                calcRunAnime(1);
                field_0xc61 = 7;
                field_0xc90 = 0x1e;
                speedF = 30.0f;
            }
        }
        break;
    case 7:
        pointPos = dPath_GetPnt(mPath, field_0xc10)->m_position;
        angle = cLib_targetAngleY(current.pos, pointPos);
        cLib_addCalcAngleS(&current.angle.y, angle, 0x10, 0x100, 0x80);

        if (field_0xc10 < 2) {
            cLib_chaseF(&speedF, 10.0f, 1.0f);
        } else {
            cLib_chaseF(&speedF, 45.0f, 1.0f);
        }
        cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x100, 0x80);
        field_0xc32.y = shape_angle.y;
        setBodyAngle(angle);
        calcRunAnime(0);

        if (current.pos.abs(pointPos) < 300.0f && --field_0xc10 < 1 && !field_0xc90) {
            setBck(0x1c, 2, 10.0f, 1.0f);
            field_0xc61 = 0;
        }
    }
}

/* 8065FE50-8066010C 007970 02BC+00 4/0 0/0 0/0 .text            action_crazy__7daCow_cFv */
void daCow_c::action_crazy() {
    int uVar1 = mMode;
    if (uVar1 != 2) {
        if (uVar1 < 2) {
            if (!uVar1) {
                field_0xcb4 = 0;
                field_0xc10 = 0;

                current.pos = dPath_GetPnt(mPath, field_0xc10)->m_position;
                field_0xc10++;

                cXyz acStack_28 = dPath_GetPnt(mPath, field_0xc10)->m_position;

                s16 angle = cLib_targetAngleY(&current.pos, &acStack_28);
                current.angle.y = angle;
                shape_angle.y = angle;
                field_0xc32.y = angle;

                field_0xc9f = 0;
                speedF = 0.0f;
                field_0xca6 = 1;

                dComIfGoat_SetThrow(this);
                field_0xcac = 1;
                fopAcM_OnStatus(this, 0x100);
            } else {
                if (field_0xc94) {
                    field_0xc94--;
                }
                if (field_0xc90) {
                    field_0xc90--;
                }

                fopAcM_OnStatus(this, 0x4000);

                if (dComIfGp_event_runCheck() &&
                    strcmp(dComIfGp_getEventManager().getRunEventName(), "WILDGOAT") &&
                    strcmp(dComIfGp_getEventManager().getRunEventName(), "WILDGOAT_SUCCESS") &&
                    strcmp(dComIfGp_getEventManager().getRunEventName(), "WILDGOAT_FAILURE"))
                {
                    fopAcM_OffStatus(this, 0x4000);
                }
            }
            switch (field_0xc9f) {
            case 0:
                executeCrazyWait();
                break;
            case 1:
                executeCrazyDash();
                break;
            case 2:
                executeCrazyBeforeCatch();
                break;
            case 3:
                executeCrazyCatch();
                break;
            case 4:
                executeCrazyThrow();
                break;
            case 5:
                executeCrazyAttack();
                break;
            case 6:
                executeCrazyAway();
                break;
            case 7:
                executeCrazyEnd();
                break;
            case 8:
                setGroundAngle();
                executeCrazyBack();
            default:
                for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
                    mSph[iSphere].OffTgSetBit();
                }
            }
        }
    } else if (uVar1 < 4) {
        dComIfGoat_SetThrow(0);
        field_0xca6 = 0;
        field_0xcb0 = 0;
        field_0xc3e.z = 0;
        field_0xc38.y = 0;
        field_0xc3e.y = 0;
    }
}

/* 8066010C-80660544 007C2C 0438+00 1/1 0/0 0/0 .text            executeCrazyBack2__7daCow_cFv
 */
void daCow_c::executeCrazyBack2() {
    if (checkOutOfGate(daPy_getPlayerActorClass()->current.pos) || checkOutOfGate(current.pos)) {
        setProcess(&daCow_c::action_run, 0);
        field_0xc9e = 1;
        return;
    }

    if (checkCowInOwn(0x8000)) {
        return;
    }

    if (!field_0xc9e) {
        field_0xca0 = 0;
        field_0xca1 = 1;
        setProcess(&daCow_c::action_angry, 0);
        return;
    }

    int bVar1 = field_0xc61;
    if (bVar1 == 2) {
        if (bVar1 < 2) {
            if (bVar1 == 0) {
                setBck(0x1c, 2, 10.0f, 1.0f);
                field_0xc61 = 1;
                field_0xc90 = 600;
            }

            setActetcStatus();
            s16 targetAngle = cLib_targetAngleY(&current.pos, &old.pos);
            cLib_addCalcAngleS(&current.angle.y, targetAngle, 0x10, 0x100, 0x80);
            cLib_chaseF(&speedF, 2.0f, 1.0f);
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 8, 0x100, 0x800);
            field_0xc32.y = shape_angle.y;
            setBodyAngle(targetAngle);

            if (current.pos.abs(old.pos) < 200.0f || field_0xc90) {
                speedF = 0.0f;
                setProcess(&daCow_c::action_moo, 0);
            } else {
                if (checkNadeNade()) {
                    setBck(0x1a, 2, 10.0f, 1.0f);
                    field_0xc61 = 2;
                    speedF = 0.0f;
                }
            }
        } else if (bVar1 < 4) {
            if (mpMorf->checkFrame(35.0f)) {
                mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_CRY), -1);
            }
            if (mpMorf->isStop()) {
                setBck(0x1c, 2, 10.0f, 1.0f);
                field_0xc61 = 1;
            }
        }
    } else {
        if (checkNadeNadeFinish()) {
            setBck(0xf, 0, 10.0f, 1.0f);
            field_0xc61 = 3;
            speedF = 0.0f;
        }
    }
}

/* 80660544-806607B8 008064 0274+00 4/0 0/0 0/0 .text            action_thrown__7daCow_cFv */
void daCow_c::action_thrown() {
    int uVar1 = mMode;
    if (uVar1 != 2) {
        if (uVar1 < 2) {
            if (uVar1 == 0) {
                field_0xc9f = 2;
                mMode = 1;
                dComIfGoat_SetThrow(this);
                field_0xc9e = 0;
            } else {
                if (field_0xc94) {
                    field_0xc94--;
                }
                if (field_0xc90) {
                    field_0xc90--;
                }

                daPy_py_c* player;

                switch (field_0xc9f) {
                case 2:
                    executeCrazyBeforeCatch();
                    break;
                case 3:
                    executeCrazyCatch();
                    break;
                case 4:
                    executeCrazyThrow();
                    break;
                case 5:
                    executeCrazyAttack();
                    break;
                case 6:
                    player = daPy_getPlayerActorClass();
                    if (checkOutOfGate(player->current.pos) || checkOutOfGate(current.pos)) {
                        setProcess(&daCow_c::action_run, 0);
                        field_0xc9e = 1;
                    }
                    if (checkCowInOwn(0x8000)) {
                        return;
                    }
                    if (checkBck(0x15)) {
                        setProcess(&daCow_c::action_wait, 1);
                    } else {
                        setProcess(&daCow_c::action_wait, 0);
                    }
                    break;
                case 8:
                    executeCrazyBack2();
                    break;
                default:
                    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
                        mSph[iSphere].OffTgSetBit();
                    }
                }
            }
        } else {
            if (uVar1 < 4) {
                field_0xc3e.set(0, 0, 0);
                dComIfGoat_SetThrow(0);
            }
        }
    }
}

/* 806607B8-806608F0 0082D8 0138+00 1/1 0/0 0/0 .text            checkWolfBusters__7daCow_cFv */
bool daCow_c::checkWolfBusters() {
    daNpc_Aru_c* aru;

    if (daPy_getPlayerActorClass()->checkNowWolf() &&
        fopAcM_SearchByName(PROC_NPC_ARU, (fopAc_ac_c**)&aru))
    {
        if (!checkOutOfGate(current.pos) && cM_rnd() >= 0.9f) {
            for (int iWolfBuster = 0; iWolfBuster < 3; iWolfBuster = iWolfBuster + 1) {
                if (gWolfBustersID[iWolfBuster] == -1) {
                    gWolfBustersID[iWolfBuster] = fopAcM_GetID(this);
                    setProcess(&daCow_c::action_wolf, 0);
                    return true;
                }
            }
        }
    }
    return false;
}

/* 806608F0-806612DC 008410 09EC+00 2/0 0/0 0/0 .text            action_wolf__7daCow_cFv */
void daCow_c::action_wolf() {
    daPy_py_c* player = daPy_getPlayerActorClass();
    daNpc_Aru_c* aru;

    if (!fopAcM_SearchByName(PROC_NPC_ARU, (fopAc_ac_c**)&aru)) {
        return;
    }

    cXyz aruPos = aru->current.pos;

    s16 aruAngle = cLib_targetAngleY(&current.pos, &aru->current.pos);
    int uVar3 = mMode;
    if (uVar3 == 2) {
        return;
    }
    if (uVar3 > 1) {
        if (uVar3 > 3) {
            return;
        }
        field_0xc98 = 0;
        field_0xc94 = 0;
        field_0xc90 = 0;
        field_0xcb0 = 0.0f;
        field_0xc3e.z = 0;
        field_0xc38.y = 0;
        field_0xc3e.y = 0;

        attention_info.flags &= ~0x1;

        // todo: clean up
        int iWolfBuster = 0;
        for (int i = N_WOLF_BUSTERS; i != 0; i -= 1) {
            if (gWolfBustersID[iWolfBuster] == fopAcM_GetID(this)) {
                break;
            }
            iWolfBuster += 1;
        }
        gWolfBustersID[iWolfBuster] = -1;
        return;
    }
    if (uVar3 == 0) {
        mMode = 1;
        field_0xc9f = 0;
        calcRunAnime(1);
        attention_info.flags |= 1;
        mSound.startCreatureVoice(JAISoundID(Z2SE_GOAT_V_ANGRY), -1);
        field_0xc98 = cM_rndF(90.0f) + 90.0f;
        return;
    }
    if (field_0xc90) {
        field_0xc90--;
    }
    if (field_0xc98) {
        field_0xc98--;
    }

    calcRunAnime(0);

    if (daPy_getPlayerActorClass()->checkNowWolf()) {
        setProcess(&daCow_c::action_run, 0);
        field_0xc9e = 1;
        return;
    }

    if (checkOutOfGate(current.pos)) {
        setProcess(&daCow_c::action_run, 0);
        field_0xc9e = 1;
        return;
    }

    int bVar1 = field_0xc9f;
    if (bVar1 == 1) {
        if (cM_rnd() >= 0.5f) {
            aruAngle -= 0x3000;
        } else {
            aruAngle += 0x3000;
        }

        field_0xc20 = aruPos;

        field_0xc20.x += cM_ssin(aruAngle) * 500.0f;
        field_0xc20.z += cM_scos(aruAngle) * 500.0f;
        field_0xc72 = cLib_targetAngleY(&current.pos, &field_0xc20);
        field_0xc9f = 2;
        field_0xc90 = 0x96;
    } else {
        if (bVar1 == 0) {
            cLib_chaseF(&speedF, 36.0f, 1.0f);
            cLib_addCalcAngleS2(&current.angle.y, aruAngle, 8, 0x400);
            shape_angle.y = current.angle.y;
            field_0xc32.y = current.angle.y;
            setBodyAngle2(aruAngle);

            if (aruPos.absXZ(current.pos) < 500.0f) {
                field_0xc9f = 1;
            }
            // goto LAB_80661240;
            cLib_chaseS(&field_0xc3e.z, 0, 0x400);
            return;
        }
        if (bVar1 > 2) {
            // goto LAB_80661240;
            cLib_chaseS(&field_0xc3e.z, 0, 0x400);
            return;
        }

        // LAB_80661240;
        // cLib_chaseS(&field_0xc3e.z, 0, 0x400);
        // return;
    }
    field_0xc72 = cLib_targetAngleY(&current.pos, &field_0xc20);

    f32 fVar13 = current.pos.absXZ(aru->current.pos) / 100.0f;

    if (fVar13 < 7.0f) {
        fVar13 = 7.0f;
    }

    cLib_chaseF(&speedF, fVar13, 1.0f);
    cLib_addCalcAngleS2(&current.angle.y, field_0xc72, 8, 0x200);

    shape_angle.y = current.angle.y;
    field_0xc32.y = current.angle.y;

    setBodyAngle2(field_0xc72);

    if (!field_0xc90) {
        field_0xc9f = 1;
    } else {
        if (current.pos.absXZ(field_0xc20) < 100.0f || mAcch.ChkWallHit()) {
            field_0xc9f = 1;
        }
    }

    if (current.pos.absXZ(aru->current.pos) < 700.0f) {
        if (!checkOutOfGate(current.pos)) {
            if (abs(fopAcM_searchPlayerAngleY(this) - field_0xc32.y) < 0x2000) {
                field_0xca0 = 0;
                field_0xca1 = 1;
                setProcess(&daCow_c::action_angry, 0);
                return;
            }
        }
    }
    if (!field_0xc98) {
        field_0xc98 = (int)(cM_rndF(90.0f) + 150.0f);
        if (!checkOutOfGate(current.pos)) {
            m_angry_cow = 0;
            if (!fpcEx_Search(s_angry_cow2, this)) {
                if (abs(fopAcM_searchPlayerAngleY(this) - field_0xc32.y) < 0x2000) {
                    setProcess(&daCow_c::action_angry, 0);
                    return;
                }
            }
        }
    }
}

/* 806612DC-806613EC 008DFC 0110+00 2/0 0/0 0/0 .text            action_damage__7daCow_cFv */
void daCow_c::action_damage() {
    int uVar1 = mMode;
    if (uVar1 == 2) {
        return;
    }
    if (uVar1 < 2) {
        if (uVar1 != 0) {
            setBck(0x18, 0, 3.0f, 1.0f);
            mMode = 1;
            field_0xc98 = 200;
            field_0xcb0 = 1.0f;
            field_0xcb4 = 0;
            speedF = 0.0f;
        } else {
            setRedTev();
            if (mpMorf->isStop()) {
                field_0xca0 = 0;
                field_0xca1 = 1;

                setProcess(&daCow_c::action_angry, 0);
            }
        }
    }
}

/* 806613EC-80661580 008F0C 0194+00 1/1 0/0 0/0 .text            action__7daCow_cFv */
void daCow_c::action() {
    if (fopAcM_checkCarryNow(this)) {
        fopAcM_cancelCarryNow(this);

        speedF = 0.0f;
        speed.y = 0.0f;
    }
    if (mNoNearCheckTimer) {
        mNoNearCheckTimer--;
    }
    if (field_0xc8c) {
        field_0xc8c--;
    }
    if (field_0xc88) {
        field_0xc88--;
    }

    cLib_chaseF(&field_0xcac, field_0xcb0, 0.1f);
    damage_check();

    s16 sVar2 = field_0xc38.y;
    if (!field_0xca5) {
        for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere++) {
            mSph[iSphere].OnTgSetBit();
        }
    }
    if (this->mProcess) {
        (this->*mProcess)();
    }

    sVar2 = (sVar2 - 0.2f * field_0xc32.y) * current.angle.y;

    CLAMP(sVar2, -0x1000, 0x1000);

    cLib_addCalcAngleS2(&field_0xc32.z, sVar2, 8, 0x800);
    dComIfGp_att_LookRequest(this, 1500.0f, 300.0f, -300.0f, 0x6000, 1);
}

/* 80661580-806615EC 0090A0 006C+00 1/1 0/0 0/0 .text            setMtx__7daCow_cFv */
void daCow_c::setMtx() {
    if (mpMorf) {
        mDoMtx_stack_c::transS(current.pos);
        mDoMtx_stack_c::ZXYrotM(field_0xc2c);
        mDoMtx_stack_c::ZXYrotM(field_0xc32);
        mpMorf->getModel()->setBaseTRMtx(mDoMtx_stack_c::get());
        mpMorf->modelCalc();
    }
}

/* 806615EC-80661720 00910C 0134+00 1/1 0/0 0/0 .text            setAttnPos__7daCow_cFv */
void daCow_c::setAttnPos() {
    if (mpMorf) {
        if (attention_info.flags & 0x80) {
            cXyz arg1(0.0f, 0.0f, 0.0f);
            mDoMtx_stack_c::copy(mpMorf->getModel()->getAnmMtx(9));
            mDoMtx_stack_c::multVec(&arg1, &eyePos);
            cXyz pos = eyePos;
            attention_info.position.set(pos.x, pos.y + 30.0f, pos.z);
        } else {
            cXyz arg1(18.0f, 30.0f, 0.0f);
            mDoMtx_stack_c::copy(mpMorf->getModel()->getAnmMtx(0xb));
            mDoMtx_stack_c::multVec(&arg1, &eyePos);
            cXyz pos = eyePos;
            attention_info.position.set(pos.x, pos.y + 80.0f, pos.z);
        }
    }
}

/* 80663500-80663504 0000A8 0004+00 0/0 0/0 0/0 .bss             m_search_range */
static f32 m_search_range;

/* 80663514-80663520 0000BC 000C+00 0/1 0/0 0/0 .bss             headOfst$7630 */
static cXyz headOfst(20.0f, 10.0f, 0.0f);

/* 80663530-8066353C 0000D8 000C+00 0/1 0/0 0/0 .bss             backBornOfst$7634 */
static cXyz backBornOfst(60.0f, 20.0f, 0.0f);

/* 8066354C-80663558 0000F4 000C+00 0/1 0/0 0/0 .bss             waistOfst$7638 */
static cXyz waistOfst(-30.0f, 30.0f, 0.0f);

/* 80661720-80661940 009240 0220+00 1/1 0/0 0/0 .text            setCollisions__7daCow_cFv */
void daCow_c::setCollisions() {
    J3DModel* pJVar1;
    Mtx* pMVar2;
    cCcS* pcVar3;
    cXyz acStack_28[2];

    if (!field_0xca6) {
        // todo: is this an unrolled loop / macro?
        cXyz ofstNow;

        mDoMtx_stack_c::copy(mpMorf->getModel()->getAnmMtx(9));
        mDoMtx_stack_c::multVec(&headOfst, &ofstNow);
        mSph[0].SetR(40.0f);
        mSph[0].SetC(ofstNow);
        dComIfG_Ccsp()->Set(&mSph[0]);

        mDoMtx_stack_c::copy(mpMorf->getModel()->getAnmMtx(1));
        mDoMtx_stack_c::multVec(&backBornOfst, &ofstNow);
        mSph[1].SetR(45.0f);
        mSph[1].SetC(ofstNow);
        dComIfG_Ccsp()->Set(&mSph[1]);

        mDoMtx_stack_c::copy(mpMorf->getModel()->getAnmMtx(0xb));
        mDoMtx_stack_c::multVec(&waistOfst, &ofstNow);
        mSph[2].SetR(45.0f);
        mSph[2].SetC(ofstNow);
        dComIfG_Ccsp()->Set(&mSph[2]);
    }
}

/* 80661940-80661AD0 009460 0190+00 2/2 0/0 0/0 .text            Execute__7daCow_cFv */
int daCow_c::Execute() {
    field_0xc68++;
    field_0xc62 = 0;

    attention_info.flags &= ~0x10;
    attention_info.flags &= ~0x80;
    action();

    if (!field_0xca6) {
        fopAcM_posMoveF(this, mCcStts.GetCCMoveP());
        mAcch.CrrPos(dComIfG_Bgsp());
        field_0xc44 = mAcch.GetGroundH();
        setEffect();
        mpMorf->play(0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));

        if (!field_0xc9c) {
            mpBtp->setPlaySpeed(1.0f);
            field_0xc9c = cM_rndF(120.0f) + 60.0f;
            if (field_0xc9c < 0x50) {
                field_0xc9c = 10;
            }
        } else {
            field_0xc9c--;
            if (mpBtp->isStop()) {
                mpBtp->setPlaySpeed(0.0f);
                mpBtp->setFrame(0.0f);
            }
        }

        mpBtp->play();
        setMtx();
        setAttnPos();
        setCollisions();
    } else {
        setMtx();
        setAttnPos();
    }
    return 1;
}

/* 80661AD0-80661AF0 0095F0 0020+00 1/0 0/0 0/0 .text            daCow_Execute__FPv */
static int daCow_Execute(void* param_0) {
    return static_cast<daCow_c*>(param_0)->Execute();
}

/* 80661AF0-80661CDC 009610 01EC+00 1/1 0/0 0/0 .text            CreateHeap__7daCow_cFv */
int daCow_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Cow", 0x1f);

#ifdef DEBUG
    if (modelData == NULL) {
        JUTAssertion::showAssert(JUTAssertion::getSDevice(), "d_a_cow.cpp", 0xef2,
                                 "0 != modelData");
        OSPanic("d_a_cow.cpp", 0xef2, "Halt");
    }
#endif

    mpMorf = new mDoExt_McaMorfSO(modelData, NULL, NULL, NULL, -1, 1.0f, 0, -1, &this->mSound,
                                  0x80000, 0x11020084);

    if (!mpMorf || !mpMorf->getModel()) {
        return cPhs_INIT_e;
    }

    mpMorf->getModel()->setUserArea((u32)this);

    for (u16 iJoint = 0; iJoint < modelData->getJointNum(); iJoint++) {
        if (iJoint == 1 || iJoint == 8 || iJoint == 0) {
            modelData->getJointNodePointer(iJoint)->setCallBack(daCow_c::ctrlJointCallBack);
        }
    }
    setBck(0x1a, 2, 0.0f, 1.0f);

    mpBtp = new mDoExt_btpAnm();

    if (mpBtp == NULL) {
        return cPhs_ERROR_e;
    }

    J3DAnmTexPattern* pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes("Cow", 0x22);
    modelData = mpMorf->getModel()->getModelData();
    if (mpBtp->init(modelData, pattern, 1, 0, 1.0f, 0, -1)) {
        return cPhs_LOADING_e;
    } else {
        return cPhs_ERROR_e;
    }
}

/* 80661D24-80661D44 009844 0020+00 1/1 0/0 0/0 .text createHeapCallBack__7daCow_cFP10fopAc_ac_c
 */
int daCow_c::createHeapCallBack(fopAc_ac_c* actor) {
    return static_cast<daCow_c*>(actor)->CreateHeap();
}

/* 80661D44-80662228 009864 04E4+00 1/1 0/0 0/0 .text            initialize__7daCow_cFv */
int daCow_c::initialize() {
    fopAcM_SetMtx(this, mpMorf->getModel()->getBaseTRMtx());
    mSound.init(&current.pos, &eyePos, 3, 1);

    eventInfo.setArchiveName("Cow");
    mAcchCir.SetWall(100.0f, 110.0f);

    mAcch.Set(fopAcM_GetPosition_p(this), fopAcM_GetOldPosition_p(this), this, 1, &mAcchCir, &speed,
              fopAcM_GetAngle_p(this), fopAcM_GetShapeAngle_p(this));

    mCcStts.Init(0xdc, 0, this);

    for (int iSphere = 0; iSphere < N_COW_COLLIDERS; iSphere = iSphere + 1) {
        mSph[iSphere].Set(*(dCcD_SrcSph*)&cc_sph_src);  // todo
        mSph[iSphere].SetStts(&mCcStts);
    }

    fopAcM_setCullSizeBox(this, -100.0f, -100.0f, -200.0f, 100.0f, 250.0f, 200.0f);
    fopAcM_OnStatus(this, 0x8000000);
    mAcch.CrrPos(dComIfG_Bgsp());

    field_0xc44 = mAcch.GetGroundH();

    attention_info.distances[4] = 0x28;
    attention_info.distances[0] = 0x16;
    attention_info.distances[7] = 0x30;
    attention_info.flags = 0;
    mParticle.init(&mAcch, 60.0f, 200.0f);
    current.angle.set(0, home.angle.y, 0);
    field_0xc32 = current.angle;
    shape_angle = field_0xc32;
    speedF = 0.0f;
    speed.set(0.0f, 0.0f, 0.0f);

    if (!mPrm0 && dComIfGs_isTmpBit(0xa08)) {
        mPrm0 = 4;
        setCowInCage();
    }

    if (mPrm0 == 4) {
        field_0xca5 = 1;
    } else {
        if (mPrm0 < 4 && mPrm0 > 2) {
            int param = fopAcM_GetParam(this);
            if ((param >> 8) != 0xff) {
                mPath = dPath_GetRoomPath(param, fopAcM_GetRoomNo(this));
                field_0xc10 = 0;

                dStage_dPnt_c* point = dPath_GetPnt(mPath, field_0xc10);
                current.pos = point->m_position;

                setProcess(&daCow_c::action_crazy, 0);
            }
            goto SKIP_ACTION_CHANGE;  // todo
        }
    }

    // todo: what is this?
    s32 iVar12 = cM_rndF(4.0f) + fopAcM_GetID(this);
    s32 iVar1 = iVar12 >> 0x1f;
    iVar1 = (iVar1 * 4 | (iVar12 * 0x40000000 + iVar1) >> 0x1e) - iVar1;

    switch (iVar1) {
    case 1:
        setProcess(&daCow_c::action_shake, 0);

        break;
    case 2:
        setProcess(&daCow_c::action_moo, 0);

        break;
    case 3:
        setProcess(&daCow_c::action_eat, 0);

        break;
    default:
        setProcess(&daCow_c::action_wait, 0);
    }

SKIP_ACTION_CHANGE:

    mAcchCir.SetWallR(100.f);
    mAcchCir.SetWallH(110.f);
    gravity = -4.0f;

    f32 rand = cM_rnd();
    int bVar11 = 0;
    if (rand >= 0.1f) {
        if (rand >= 0.9f && !(l_CowType & 2)) {
            l_CowType |= 2;
            bVar11 = 2;
        }
    } else {
        if (!(l_CowType & 1)) {
            l_CowType |= 1;
            bVar11 = 1;
        }
    }

    if (bVar11 == 2) {
        field_0xc78 = 700.0f;
        field_0xc7c = 15.0f;
    } else if (bVar11 < 2 && bVar11) {
        field_0xc78 = 1300.0f;
        field_0xc7c = 35.0f;
    } else {
        field_0xc78 = cM_rndFX(100.0f) + 1000.0f;
        field_0xc7c = cM_rndFX(5.0f) + 25.0f;
    }

    field_0xc69 = bVar11;
    Execute();

    if (!mPrm0) {
        dMeter2Info_setNowCount(0);
        dMeter2Info_setMaxCount(dMeter2Info_getMaxCount() + 1);
    }
    return 1;
}

/* 80662228-806623D4 009D48 01AC+00 1/1 0/0 0/0 .text            create__7daCow_cFv */
int daCow_c::create() {
    daCow_c* _this;
    if (fopAcM_CheckCondition(this, 8)) {
        _this = new daCow_c();
        fopAcM_OnCondition(this, 8);
    }

    mPrm0 = fopAcM_GetParam(this);

    if (this->mPrm0 == -1 || this->mPrm0 > 4) {
        this->mPrm0 = 0;
    }

    if (mPrm0 == 2) {
        setEnterCow20();
        field_0xca6 = 1;
        return cPhs_ERROR_e;
    } else if (mPrm0 < 2 && mPrm0 != 0) {
        setEnterCow10();
        field_0xca6 = 1;
        return cPhs_ERROR_e;
    } else {
        int res = dComIfG_resLoad(&mPhase, "Cow");
        if (res != cPhs_COMPLEATE_e) {
            return res;
        }
        if (!fopAcM_entrySolidHeap(_this, daCow_c::createHeapCallBack, 0x1df0)) {
            return cPhs_ERROR_e;
        }
        if (!initialize()) {
            return cPhs_ERROR_e;
        }
        return cPhs_COMPLEATE_e;
    }
}

/* 806626F0-80662710 00A210 0020+00 1/0 0/0 0/0 .text            daCow_Create__FPv */
static int daCow_Create(void* param_0) {
    return static_cast<daCow_c*>(param_0)->create();
}

/* 80662710-80662920 00A230 0210+00 1/1 0/0 0/0 .text ctrlJoint__7daCow_cFP8J3DJointP8J3DModel
 */
int daCow_c::ctrlJoint(J3DJoint* joint, J3DModel* model) {
    int jointNo = joint->getJntNo();

    mDoMtx_stack_c::copy(model->getAnmMtx(jointNo));
    if (jointNo != 8) {
        if (jointNo < 8 && jointNo == 1) {
            mDoMtx_stack_c::YrotM(field_0xc38.y);
        }
    } else {
        mDoMtx_stack_c::ZrotM(field_0xc3e.y);
        mDoMtx_stack_c::YrotM(field_0xc3e.y);
    }

    model->setAnmMtx(jointNo, mDoMtx_stack_c::get());
    cMtx_copy(mDoMtx_stack_c::get(), &J3DSys::mCurrentMtx[0]);

    if (jointNo == 0) {
        int bVar1 = field_0xc62;
        if (bVar1 != 2) {
            if (bVar1 <= 1) {
                if (bVar1 != 0) {
                    // todo
                    field_0xc14.set(J3DSys::mCurrentMtx[0][3], J3DSys::mCurrentMtx[1][3],
                                    J3DSys::mCurrentMtx[2][3]);
                }
            } else {
                if (bVar1 < 4) {
                    // todo
                    cXyz v = current.pos + field_0xc14;
                    J3DSys::mCurrentMtx[0][3] = v.x;
                    J3DSys::mCurrentMtx[1][3] = v.y;
                    J3DSys::mCurrentMtx[2][3] = v.z;
                }
            }
        } else {
            // todo
            cXyz currentMtx(J3DSys::mCurrentMtx[0][3], J3DSys::mCurrentMtx[1][3],
                            J3DSys::mCurrentMtx[2][3]);

            cXyz cStack_30 = currentMtx - (field_0xc14 - current.pos);
            currentMtx = field_0xc14 - currentMtx;
            current.pos -= currentMtx;
            field_0xc14 -= 50.0f;
        }
    }
    return 1;
}

/* 80662920-8066296C 00A440 004C+00 1/1 0/0 0/0 .text ctrlJointCallBack__7daCow_cFP8J3DJointi */
int daCow_c::ctrlJointCallBack(J3DJoint* joint, int param_1) {
    if (!param_1) {
        J3DModel* model = j3dSys.getModel();
        daCow_c* cow = (daCow_c*)model->getUserArea();
        if (cow) {
            cow->ctrlJoint(joint, model);
        }
    }
    return 1;
}

/* 8066296C-80662BC4 00A48C 0258+00 1/1 0/0 0/0 .text            Draw__7daCow_cFv */
int daCow_c::Draw() {
    if (field_0xca6) {
        return 1;
    }

    J3DModel* model = mpMorf->getModel();

    g_env_light.settingTevStruct(0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType_MAJI(model, &tevStr);

    mpBtp->entry(model->getModelData());
    mpMorf->entryDL();

    if (strcmp(dComIfGp_getEventManager().getRunEventName(), "MAKI_OP") == 0) {
        cXyz shadowPos;
        cXyz arg1(0.0f, 0.0f, -20.0f);
        cLib_offsetPos(&shadowPos, &current.pos, current.angle.y, &arg1);

        dComIfGd_setSimpleShadow(&shadowPos, field_0xc44, 90.0f, mAcch.m_gnd, 0, 1.0f,
                                 dDlst_shadowControl_c::getSimpleTex());

        cXyz arg2(0.0f, 0.0f, 120.0f);
        cLib_offsetPos(&shadowPos, &current.pos, current.angle.y, &arg2);

        dComIfGd_setSimpleShadow(&shadowPos, field_0xc44, 50.0f, mAcch.m_gnd, 0, 1.0f,
                                 dDlst_shadowControl_c::getSimpleTex());

    } else {
        f32 fVar1 = 800.0f;

        if ((checkProcess(&daCow_c::action_crazy) || checkProcess(&daCow_c::action_thrown)) &&
            field_0xc9f == 4)
        {
            fVar1 = 1500.0f;
        }

        field_0xc64 = dComIfGd_setShadow(field_0xc64, 1, model, &current.pos, fVar1, 0.0f,
                                         current.pos.y, field_0xc44, mAcch.m_gnd, &tevStr, 0, 1.0f,
                                         dDlst_shadowControl_c::getSimpleTex());
    }
    tevStr.FogCol.r = field_0xcac * 50.0f;
    return 1;
}

/* 80662BC4-80662BE4 00A6E4 0020+00 1/0 0/0 0/0 .text            daCow_Draw__FPv */
static int daCow_Draw(void* param_0) {
    return static_cast<daCow_c*>(param_0)->Draw();
}

/* 80662BE4-80662C40 00A704 005C+00 1/1 0/0 0/0 .text            Delete__7daCow_cFv */
int daCow_c::Delete() {
    fopAcM_GetID(this);
    dComIfG_resDelete(&this->mPhase, "Cow");

    if (heap != NULL) {
        mSound.deleteObject();
    }
    return true;
}

/* 80662C40-80662C60 00A760 0020+00 1/0 0/0 0/0 .text            daCow_Delete__FPv */
static int daCow_Delete(void* param_0) {
    return static_cast<daCow_c*>(param_0)->Delete();
}

/* 80662C60-80662C68 00A780 0008+00 1/0 0/0 0/0 .text            daCow_IsDelete__FPv */
static int daCow_IsDelete(void* param_0) {
    return true;
}

/* 80662D60-80662D68 00A880 0008+00 1/0 0/0 0/0 .text            @36@__dt__12dBgS_ObjAcchFv */
static void func_80662D60() {
    // NONMATCHING
}

/* 80662D68-80662D70 00A888 0008+00 1/0 0/0 0/0 .text            @20@__dt__12dBgS_ObjAcchFv */
static void func_80662D68() {
    // NONMATCHING
}

// todo
/* 80662D70-80662D84 00A890 0014+00 1/1 0/0 0/0 .text            getShapeAngle__7daCow_cFv */
csXyz daCow_c::getShapeAngle() {
    return field_0xc32;
}

/* 80663390-806633B0 -00001 0020+00 1/0 0/0 0/0 .data            daCow_MethodTable */
static actor_method_class daCow_MethodTable = {
    daCow_Create, daCow_Delete, daCow_Execute, daCow_IsDelete, daCow_Draw,
};

/* 806633B0-806633E0 -00001 0030+00 0/0 0/0 1/0 .data            g_profile_COW */
extern actor_process_profile_definition g_profile_COW = {
    fpcLy_CURRENT_e,         // mLayerID
    7,                       // mListID
    fpcPi_CURRENT_e,         // mListPrio
    PROC_COW,                // mProcName
    &g_fpcLf_Method.base,    // sub_method
    sizeof(daCow_c),         // mSize
    0,                       // mSizeOther
    0,                       // mParameters
    &g_fopAc_Method.base,    // sub_method
    692,                     // mPriority
    &daCow_MethodTable,      // sub_method
    0x00040100,              // mStatus
    fopAc_NPC_e,             // mActorType
    fopAc_CULLBOX_CUSTOM_e,  // cullType
};

// TODO
/* ############################################################################################## */
/* 80663558-8066355C 000100 0004+00 0/0 0/0 0/0 .bss
 * sInstance__40JASGlobalInstance<19JASDefaultBankTable>        */
#pragma push
#pragma force_active on
static u8 data_80663558[4];
#pragma pop

/* 8066355C-80663560 000104 0004+00 0/0 0/0 0/0 .bss
 * sInstance__35JASGlobalInstance<14JASAudioThread>             */
#pragma push
#pragma force_active on
static u8 data_8066355C[4];
#pragma pop

/* 80663560-80663564 000108 0004+00 0/0 0/0 0/0 .bss sInstance__27JASGlobalInstance<7Z2SeMgr> */
#pragma push
#pragma force_active on
static u8 data_80663560[4];
#pragma pop

/* 80663564-80663568 00010C 0004+00 0/0 0/0 0/0 .bss sInstance__28JASGlobalInstance<8Z2SeqMgr> */
#pragma push
#pragma force_active on
static u8 data_80663564[4];
#pragma pop

/* 80663568-8066356C 000110 0004+00 0/0 0/0 0/0 .bss sInstance__31JASGlobalInstance<10Z2SceneMgr>
 */
#pragma push
#pragma force_active on
static u8 data_80663568[4];
#pragma pop

/* 8066356C-80663570 000114 0004+00 0/0 0/0 0/0 .bss sInstance__32JASGlobalInstance<11Z2StatusMgr>
 */
#pragma push
#pragma force_active on
static u8 data_8066356C[4];
#pragma pop

/* 80663570-80663574 000118 0004+00 0/0 0/0 0/0 .bss sInstance__31JASGlobalInstance<10Z2DebugSys>
 */
#pragma push
#pragma force_active on
static u8 data_80663570[4];
#pragma pop

/* 80663574-80663578 00011C 0004+00 0/0 0/0 0/0 .bss
 * sInstance__36JASGlobalInstance<15JAISoundStarter>            */
#pragma push
#pragma force_active on
static u8 data_80663574[4];
#pragma pop

/* 80663578-8066357C 000120 0004+00 0/0 0/0 0/0 .bss
 * sInstance__35JASGlobalInstance<14Z2SoundStarter>             */
#pragma push
#pragma force_active on
static u8 data_80663578[4];
#pragma pop

/* 8066357C-80663580 000124 0004+00 0/0 0/0 0/0 .bss
 * sInstance__33JASGlobalInstance<12Z2SpeechMgr2>               */
#pragma push
#pragma force_active on
static u8 data_8066357C[4];
#pragma pop

/* 80663580-80663584 000128 0004+00 0/0 0/0 0/0 .bss sInstance__28JASGlobalInstance<8JAISeMgr> */
#pragma push
#pragma force_active on
static u8 data_80663580[4];
#pragma pop

/* 80663584-80663588 00012C 0004+00 0/0 0/0 0/0 .bss sInstance__29JASGlobalInstance<9JAISeqMgr> */
#pragma push
#pragma force_active on
static u8 data_80663584[4];
#pragma pop

/* 80663588-8066358C 000130 0004+00 0/0 0/0 0/0 .bss
 * sInstance__33JASGlobalInstance<12JAIStreamMgr>               */
#pragma push
#pragma force_active on
static u8 data_80663588[4];
#pragma pop

/* 8066358C-80663590 000134 0004+00 0/0 0/0 0/0 .bss sInstance__31JASGlobalInstance<10Z2SoundMgr>
 */
#pragma push
#pragma force_active on
static u8 data_8066358C[4];
#pragma pop

/* 80663590-80663594 000138 0004+00 0/0 0/0 0/0 .bss
 * sInstance__33JASGlobalInstance<12JAISoundInfo>               */
#pragma push
#pragma force_active on
static u8 data_80663590[4];
#pragma pop

/* 80663594-80663598 00013C 0004+00 0/0 0/0 0/0 .bss
 * sInstance__34JASGlobalInstance<13JAUSoundTable>              */
#pragma push
#pragma force_active on
static u8 data_80663594[4];
#pragma pop

/* 80663598-8066359C 000140 0004+00 0/0 0/0 0/0 .bss
 * sInstance__38JASGlobalInstance<17JAUSoundNameTable>          */
#pragma push
#pragma force_active on
static u8 data_80663598[4];
#pragma pop

/* 8066359C-806635A0 000144 0004+00 0/0 0/0 0/0 .bss
 * sInstance__33JASGlobalInstance<12JAUSoundInfo>               */
#pragma push
#pragma force_active on
static u8 data_8066359C[4];
#pragma pop

/* 806635A0-806635A4 000148 0004+00 0/0 0/0 0/0 .bss sInstance__32JASGlobalInstance<11Z2SoundInfo>
 */
#pragma push
#pragma force_active on
static u8 data_806635A0[4];
#pragma pop

/* 806635A4-806635A8 00014C 0004+00 0/0 0/0 0/0 .bss
 * sInstance__34JASGlobalInstance<13Z2SoundObjMgr>              */
#pragma push
#pragma force_active on
static u8 data_806635A4[4];
#pragma pop

/* 806635A8-806635AC 000150 0004+00 0/0 0/0 0/0 .bss sInstance__31JASGlobalInstance<10Z2Audience>
 */
#pragma push
#pragma force_active on
static u8 data_806635A8[4];
#pragma pop

/* 806635AC-806635B0 000154 0004+00 0/0 0/0 0/0 .bss sInstance__32JASGlobalInstance<11Z2FxLineMgr>
 */
#pragma push
#pragma force_active on
static u8 data_806635AC[4];
#pragma pop

/* 806635B0-806635B4 000158 0004+00 0/0 0/0 0/0 .bss sInstance__31JASGlobalInstance<10Z2EnvSeMgr>
 */
#pragma push
#pragma force_active on
static u8 data_806635B0[4];
#pragma pop

/* 806635B4-806635B8 00015C 0004+00 0/0 0/0 0/0 .bss sInstance__32JASGlobalInstance<11Z2SpeechMgr>
 */
#pragma push
#pragma force_active on
static u8 data_806635B4[4];
#pragma pop

/* 806635B8-806635BC 000160 0004+00 0/0 0/0 0/0 .bss
 * sInstance__34JASGlobalInstance<13Z2WolfHowlMgr>              */
#pragma push
#pragma force_active on
static u8 data_806635B8[4];
#pragma pop

/* 80662F18-80662F18 000168 0000+00 0/0 0/0 0/0 .rodata          @stringBase0 */
