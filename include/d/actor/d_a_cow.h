#ifndef D_A_COW_H
#define D_A_COW_H

#include "d/d_bg_s_acch.h"
#include "d/d_cc_d.h"
#include "d/d_particle_copoly.h"
#include "d/d_path.h"
#include "f_op/f_op_actor_mng.h"

#define N_COW_COLLIDERS 3

#define FLAG(name, mask)                                                                           \
    void set##name() {                                                                             \
        mFlags |= (mask);                                                                          \
    }                                                                                              \
    bool get##name() {                                                                             \
        return mFlags & (mask);                                                                    \
    }                                                                                              \
    void clear##name() {                                                                           \
        mFlags &= ~(mask);                                                                         \
    }

#define TIMER(name, field)                                                                         \
    bool get##name() {                                                                             \
        return (field);                                                                            \
    }                                                                                              \
    void set##name(int duration) {                                                                 \
        (field) = duration;                                                                        \
    }                                                                                              \
    void tick##name() {                                                                            \
        if ((field)) {                                                                             \
            (field)--;                                                                             \
        }                                                                                          \
    }

/**
 * @ingroup actors-unsorted
 * @class daCow_c
 * @brief Ordon Goat
 *
 * @details
 *
 */
class daCow_c : public fopAc_ac_c {
public:
    /* 806585CC */ int calcRunAnime(int);
    /* 80658730 */ void setBck(int, u8, f32, f32);
    /* 806587D4 */ u8 checkBck(int);
    /* 80658830 */ void setEffect();
    /* 80658AA4 */ bool isChaseCowGame();
    /* 80658B10 */ void setCarryStatus();
    /* 80658C18 */ void setActetcStatus();
    /* 80658C78 */ bool checkNadeNadeFinish();
    /* 80658CA4 */ bool checkNadeNade();
    /* 80658CD0 */ void setSeSnort();
    /* 80658D3C */ void setRushVibration(int);
    /* 80658DB8 */ bool checkThrow();
    /* 80658E98 */ void setBodyAngle(s16);
    /* 80658F94 */ void setBodyAngle2(s16);
    /* 806590E8 */ BOOL checkProcess(void (daCow_c::*)());
    /* 80659114 */ bool setProcess(void (daCow_c::*)(), int);
    /* 806591BC */ void damage_check();
    /* 8065945C */ void setEnterCow20();
    /* 80659540 */ void setEnterCow10();
    /* 80659630 */ void setGroundAngle();
    /* 80659970 */ bool checkRun();
    /* 806599C0 */ bool checkNearCowRun();
    /* 80659ADC */ void action_wait();
    /* 8065A0E8 */ void action_eat();
    /* 8065A594 */ void action_moo();
    /* 8065A8A4 */ void action_shake();
    /* 8065ACC8 */ bool checkNearWolf();
    /* 8065AD2C */ bool checkPlayerWait();
    /* 8065ADB0 */ bool checkPlayerSurprise();
    /* 8065AE88 */ bool checkPlayerPos();
    /* 8065B034 */ void checkBeforeBg();
    /* 8065B760 */ int checkOutOfGate(cXyz);
    /* 8065B8A8 */ s16 getCowshedAngle();
    /* 8065B8D8 */ double getCowshedDist();
    /* 8065BA30 */ int checkCowIn(f32, f32);
    /* 8065BB34 */ bool checkCowInOwn(int);
    /* 8065BC68 */ void action_run();
    /* 8065C32C */ bool checkCurringPen();
    /* 8065C508 */ void setCowInCage();
    /* 8065C680 */ void setEnterCount();
    /* 8065C70C */ void action_enter();
    /* 8065CFBC */ bool isAngry();
    /* 8065D03C */ bool isGuardFad();
    /* 8065D0B8 */ void setAngryHit();
    /* 8065D17C */ bool checkBeforeBgAngry(s16);
    /* 8065D230 */ void setRedTev();
    /* 8065D29C */ void setAngryTurn();
    /* 8065D2F0 */ void action_angry();
    /* 8065DC08 */ void calcCatchPos(f32, int);
    /* 8065DE70 */ void executeCrazyWait();
    /* 8065DF40 */ void executeCrazyDash();
    /* 8065E6BC */ void initCrazyBeforeCatch(int);
    /* 8065E6E8 */ void executeCrazyBeforeCatch();
    /* 8065E7D0 */ void initCrazyCatch(int);
    /* 8065E888 */ void executeCrazyCatch();
    /* 8065EAF4 */ void initCrazyThrow(int);
    /* 8065EBF0 */ void executeCrazyThrow();
    /* 8065F088 */ void initCrazyAttack(int);
    /* 8065F144 */ void executeCrazyAttack();
    /* 8065F308 */ void initCrazyAway(int);
    /* 8065F37C */ void executeCrazyAway();
    /* 8065F6E0 */ void executeCrazyEnd();
    /* 8065F744 */ void initCrazyBack(int);
    /* 8065F7DC */ void executeCrazyBack();
    /* 8065FE50 */ void action_crazy();
    /* 8066010C */ void executeCrazyBack2();
    /* 80660544 */ void action_thrown();
    /* 806607B8 */ bool checkWolfBusters();
    /* 806608F0 */ void action_wolf();
    /* 806612DC */ void action_damage();
    /* 806613EC */ void action();
    /* 80661580 */ void setMtx();
    /* 806615EC */ void setAttnPos();
    /* 80661720 */ void setCollisions();
    /* 80661940 */ int Execute();
    /* 80661AF0 */ int CreateHeap();
    /* 80661D24 */ static int createHeapCallBack(fopAc_ac_c*);
    /* 80661D44 */ u8 initialize();
    /* 80662228 */ int create();
    /* 80662710 */ int ctrlJoint(J3DJoint*, J3DModel*);
    /* 80662920 */ static int ctrlJointCallBack(J3DJoint*, int);
    /* 8066296C */ int Draw();
    /* 80662BE4 */ int Delete();
    /* 80662D70 */ csXyz getShapeAngle();

    FLAG(CrazyBeforeCatch, 1 << 0);
    FLAG(CrazyCatch, 1 << 1);
    FLAG(CrazyDash, 1 << 2);
    FLAG(CrazyThrowLeft, 1 << 3);
    FLAG(CrazyThrowRight, 1 << 4);
    FLAG(UnkFlag1, 1 << 5);
    FLAG(UnkFlag2, 1 << 6);
    FLAG(Naderu, 1 << 7);
    FLAG(NaderuFinish, 1 << 8);
    FLAG(UnkFlag3, 1 << 9);

    bool anyFlagsSet() { return mFlags; };
    void clearAllFlags() { mFlags = 0; };

    void setCowIn() { mCowIn = 1; }
    bool getCowIn() { return mCowIn; }

    TIMER(NoNearCheckTimer, mNoNearCheckTimer);
    TIMER(Timer1, mTimer1);
    TIMER(Timer2, mTimer2);
    TIMER(Timer3, mTimer3);
    TIMER(DamageTimer, mDamageTimer);
    TIMER(Timer5, mTimer5);
    TIMER(Timer6, mTimer6);
    TIMER(Timer7, mTimer7);
    TIMER(Timer8, mTimer8);
    TIMER(Timer9, mTimer9);
    TIMER(Timer10, mTimer10);

    daCow_c* getCowP() { return mCowP; }

    // todo
    u8 getParam1() { return fopAcM_GetParam(this) >> 8; }

private:
    /* 0x568 */ u16 mFlags;
    /* 0x56c */ request_of_phase_process_class mPhase;
    /* 0x574 */ mDoExt_McaMorfSO* mpMorf;
    /* 0x578 */ mDoExt_btpAnm* mpBtp;
    /* 0x57c */ Z2Creature mSound;
    /* 0x60c */ dBgS_ObjAcch mAcch;
    /* 0x7e4 */ dCcD_Stts mCcStts;
    /* 0x820 */ dBgS_AcchCir mAcchCir;
    /* 0x860 */ dCcD_Sph mSph[N_COW_COLLIDERS];
    /* 0xc08 */ daCow_c* mCowP;
    /* 0xc0c */ dPath* mPath;
    /* 0xc10 */ s8 mPointIndex;
    /* 0xc14 */ cXyz mJointPos;
    /* 0xc20 */ cXyz mTarget;
    /* 0xc2c */ csXyz mGroundTransform;
    /* 0xc32 */ csXyz mSavedAngle;
    /* 0xc38 */ csXyz mJoint1Offset;
    /* 0xc3e */ csXyz mJoint8Offset;
    /* 0xc44 */ f32 mGroundHeight;
    /* 0xc48 */ void (daCow_c::*mProcess)();
    /* 0xc54 */ int mTimer6;
    /* 0xc58 */ int mTimer7;
    /* 0xc5c */ u16 mMode;
    /* 0xc5e */ s8 mAnimationPhase;
    /* 0xc5f */ u8 mPrm0;
    /* 0xc60 */ u8 field_0xc60;
    /* 0xc61 */ u8 mAction;
    /* 0xc62 */ u8 mJointIndex;
    /* 0xc63 */ bool mIsCrazy;
    /* 0xc64 */ u32 mShadowKey;
    /* 0xc68 */ u8 mCounter1;
    /* 0xc69 */ u8 mISpeed;
    /* 0xc6c */ f32 mBoostSpeed;
    /* 0xc70 */ s16 mBoostAngle;
    /* 0xc72 */ s16 mTargetAngle;
    /* 0xc74 */ s16 mTurningSpeed;
    /* 0xc76 */ s16 mThrowIntensity;
    /* 0xc78 */ f32 mAttentionDistance;
    /* 0xc7c */ f32 mSpeed;
    /* 0xc80 */ int mTimer8;
    /* 0xc84 */ int mTimer9;
    /* 0xc88 */ int mTimer5;
    /* 0xc8c */ int mDamageTimer;
    /* 0xc90 */ int mTimer1;
    /* 0xc94 */ int mTimer2;
    /* 0xc98 */ int mTimer10;
    /* 0xc9c */ u8 mAnimationInterval;
    /* 0xc9d */ u8 mOutOfGate;
    /* 0xc9e */ u8 mWillGetAngry;
    /* 0xc9f */ u8 mCrazy;
    /* 0xca0 */ bool mReadyToDash;
    /* 0xca1 */ u8 mRunDuration;
    /* 0xca2 */ u8 mIntersectedPlanes;
    /* 0xca3 */ u8 mTimer3;
    /* 0xca4 */ u8 mNoNearCheckTimer;
    /* 0xca5 */ u8 mCowIn;
    /* 0xca6 */ bool mDisabled;
    /* 0xca7 */ u8 _unused;
    /* 0xca8 */ bool mNadeNade;
    /* 0xca9 */ bool mEnterTimerDone;
    /* 0xcaa */ u8 mWaitForMorf;
    /* 0xcac */ f32 mRedTev;
    /* 0xcb0 */ f32 mTargetRedTev;
    /* 0xcb4 */ u8 mChangeRedTev;
    /* 0xcb5 */ u8 mShouldSetEffect;
    /* 0xcb8 */ dPaPoT_c mParticle;
    /* 0xd38 */ u32 mSmokeKey;
    /* 0xd3c */ u32 mSmokeKey2;
};

STATIC_ASSERT(sizeof(daCow_c) == 0xd40);

enum daCow_Crazy {
    daCow_Crazy_Wait_e = 0,
    daCow_Crazy_Dash_e = 1,
    daCow_Crazy_BeforeCatch_e = 2,
    daCow_Crazy_Catch_e = 3,
    daCow_Crazy_Throw_e = 4,
    daCow_Crazy_Attack_e = 5,
    daCow_Crazy_Away_e = 6,
    daCow_Crazy_End_e = 7,
    daCow_Crazy_Back_e = 8,
};

// todo: these names are only accurate for some cases...
enum daCow_Mode {
    daCow_Mode_Starting_e = 0,
    daCow_Mode_WaitingForMorf_e = 1,
    daCow_Mode_Active_e = 2,
    daCow_Mode_Done_e = 3,
};

enum daCow_Action {
    daCow_Action_Wait_e = 0,
    daCow_Action_NadeNade_e = 1,
    daCow_Action_Cry_e = 2,
    daCow_Action_Die_e = 3,
    daCow_Action_Delete_e = 4,
    daCow_Action_Damaged_e = 5,
    daCow_Action_AfterDamage_e = 6,
    daCow_Action_Running_e = 7,
};

#endif /* D_A_COW_H */
