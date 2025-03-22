#ifndef D_A_COW_H
#define D_A_COW_H

#include "d/d_bg_s_acch.h"
#include "d/d_cc_d.h"
#include "d/d_particle_copoly.h"
#include "d/d_path.h"
#include "f_op/f_op_actor_mng.h"

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
    /* 806587D4 */ int checkBck(int);
    /* 80658830 */ void setEffect();
    /* 80658AA4 */ bool isChaseCowGame();
    /* 80658B10 */ void setCarryStatus();
    /* 80658C18 */ void setActetcStatus();
    /* 80658C78 */ bool checkNadeNadeFinish();
    /* 80658CA4 */ bool checkNadeNade();
    /* 80658CD0 */ void setSeSnort();
    /* 80658D3C */ void setRushVibration(int);
    /* 80658DB8 */ void checkThrow();
    /* 80658E98 */ void setBodyAngle(s16);
    /* 80658F94 */ void setBodyAngle2(s16);
    /* 806590E8 */ void checkProcess(void (daCow_c::*)());
    /* 80659114 */ void setProcess(void (daCow_c::*)(), int);
    /* 806591BC */ void damage_check();
    /* 8065945C */ void setEnterCow20();
    /* 80659540 */ void setEnterCow10();
    /* 80659630 */ void setGroundAngle();
    /* 80659970 */ void checkRun();
    /* 806599C0 */ void checkNearCowRun();
    /* 80659ADC */ void action_wait();
    /* 8065A0E8 */ void action_eat();
    /* 8065A594 */ void action_moo();
    /* 8065A8A4 */ void action_shake();
    /* 8065ACC8 */ void checkNearWolf();
    /* 8065AD2C */ void checkPlayerWait();
    /* 8065ADB0 */ void checkPlayerSurprise();
    /* 8065AE88 */ void checkPlayerPos();
    /* 8065B034 */ void checkBeforeBg();
    /* 8065B760 */ void checkOutOfGate(cXyz);
    /* 8065B8A8 */ void getCowshedAngle();
    /* 8065B8D8 */ void getCowshedDist();
    /* 8065BA30 */ void checkCowIn(f32, f32);
    /* 8065BB34 */ void checkCowInOwn(int);
    /* 8065BC68 */ void action_run();
    /* 8065C32C */ void checkCurringPen();
    /* 8065C508 */ void setCowInCage();
    /* 8065C680 */ void setEnterCount();
    /* 8065C70C */ void action_enter();
    /* 8065CFBC */ bool isAngry();
    /* 8065D03C */ void isGuardFad();
    /* 8065D0B8 */ void setAngryHit();
    /* 8065D17C */ void checkBeforeBgAngry(s16);
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
    /* 806607B8 */ void checkWolfBusters();
    /* 806608F0 */ void action_wolf();
    /* 806612DC */ void action_damage();
    /* 806613EC */ void action();
    /* 80661580 */ void setMtx();
    /* 806615EC */ void setAttnPos();
    /* 80661720 */ void setCollisions();
    /* 80661940 */ void Execute();
    /* 80661AF0 */ void CreateHeap();
    /* 80661D24 */ void createHeapCallBack(fopAc_ac_c*);
    /* 80661D44 */ void initialize();
    /* 80662228 */ void create();
    /* 80662710 */ void ctrlJoint(J3DJoint*, J3DModel*);
    /* 80662920 */ void ctrlJointCallBack(J3DJoint*, int);
    /* 8066296C */ void Draw();
    /* 80662BE4 */ void Delete();
    /* 80662D70 */ void getShapeAngle();

    void setNaderu() { mFlags |= 0x80; }
    void setNaderuFinish() { mFlags |= 0x100; }
    void setCrazyDash() { mFlags |= 4; }
    void setCrazyBeforeCatch() { mFlags |= 1; }
    void setCrazyCatch() { mFlags |= 2; }
    void setCrazyThrowLeft() { mFlags |= 8; }
    void setCrazyThrowRight() { mFlags |= 0x10; }

private:
    /* 0x568 */ u16 mFlags;
    /* 0x56a */ u8 field_0x56a;  // undefined
    /* 0x56b */ u8 field_0x56b;  // undefined
    /* 0x56c */ request_of_phase_process_class mPhase;
    /* 0x574 */ mDoExt_McaMorfSO* mpMorf;
    /* 0x578 */ mDoExt_btpAnm* mpBtp;
    /* 0x57c */ Z2Creature mSound;
    /* 0x60c */ dBgS_ObjAcch mAcch;
    /* 0x7e4 */ dCcD_Stts mCcStts;
    /* 0x820 */ dBgS_AcchCir mAcchCir;
    /* 0x860 */ dCcD_Sph mSph[3];
    /* 0xc08 */ u32 field_0xc08;
    /* 0xc0c */ dPath* mPath;
    /* 0xc10 */ u8 field_0xc10;
    /* 0xc11 */ u8 field_0xc11;  // undefined
    /* 0xc12 */ u8 field_0xc12;  // undefined
    /* 0xc13 */ u8 field_0xc13;  // undefined
    /* 0xc14 */ cXyz field_0xc14;
    /* 0xc20 */ cXyz field_0xc20;
    /* 0xc2c */ csXyz field_0xc2c;
    /* 0xc32 */ csXyz field_0xc32;
    /* 0xc38 */ csXyz field_0xc38;
    /* 0xc3e */ csXyz field_0xc3e;
    /* 0xc44 */ f32 field_0xc44;
    /* 0xc48 */ u8 mProcess[0xc];  // todo: _ptmf (function pointer?)
    /* 0xc54 */ int field_0xc54;
    /* 0xc58 */ u8 field_0xc58;  // undefined
    /* 0xc59 */ u8 field_0xc59;  // undefined
    /* 0xc5a */ u8 field_0xc5a;  // undefined
    /* 0xc5b */ u8 field_0xc5b;  // undefined
    /* 0xc5c */ u16 field_0xc5c;
    /* 0xc5e */ s8 mAnimationPhase;
    /* 0xc5f */ u8 mPrm0;
    /* 0xc60 */ u8 field_0xc60;
    /* 0xc61 */ u8 field_0xc61;
    /* 0xc62 */ u8 field_0xc62;
    /* 0xc63 */ u8 field_0xc63;
    /* 0xc64 */ u32 field_0xc64;
    /* 0xc68 */ u8 mMoreFlags;  // todo: what is this
    /* 0xc69 */ u8 field_0xc69;
    /* 0xc6a */ u8 field_0xc6a;  // undefined
    /* 0xc6b */ u8 field_0xc6b;  // undefined
    /* 0xc6c */ f32 field_0xc6c;
    /* 0xc70 */ u8 field_0xc70;  // undefined
    /* 0xc71 */ u8 field_0xc71;  // undefined
    /* 0xc72 */ s16 field_0xc72;
    /* 0xc74 */ s16 field_0xc74;
    /* 0xc76 */ u16 field_0xc76;
    /* 0xc78 */ f32 field_0xc78;
    /* 0xc7c */ f32 field_0xc7c;
    /* 0xc80 */ int field_0xc80;
    /* 0xc84 */ u32 field_0xc84;
    /* 0xc88 */ int field_0xc88;
    /* 0xc8c */ int field_0xc8c;
    /* 0xc90 */ u32 field_0xc90;
    /* 0xc94 */ u32 field_0xc94;
    /* 0xc98 */ u32 field_0xc98;
    /* 0xc9c */ u8 field_0xc9c;
    /* 0xc9d */ u8 field_0xc9d;
    /* 0xc9e */ u8 field_0xc9e;
    /* 0xc9f */ u8 field_0xc9f;
    /* 0xca0 */ u8 field_0xca0;
    /* 0xca1 */ u8 field_0xca1;
    /* 0xca2 */ u8 field_0xca2;  // undefined
    /* 0xca3 */ u8 field_0xca3;
    /* 0xca4 */ u8 mNoNearCheckTimer;
    /* 0xca5 */ u8 field_0xca5;
    /* 0xca6 */ u8 field_0xca6;
    /* 0xca7 */ u8 field_0xca7;  // undefined
    /* 0xca8 */ u8 field_0xca8;
    /* 0xca9 */ u8 field_0xca9;
    /* 0xcaa */ u8 field_0xcaa;
    /* 0xcab */ u8 field_0xcab;  // undefined
    /* 0xcac */ f32 field_0xcac;
    /* 0xcb0 */ f32 field_0xcb0;
    /* 0xcb4 */ u8 field_0xcb4;
    /* 0xcb5 */ bool mShouldSetEffect;
    /* 0xcb6 */ u8 field_0xcb6;
    /* 0xcb7 */ u8 field_0xcb7;
    /* 0xcb8 */ dPaPoT_c mParticle;
    /* 0xd38 */ u32 field_0xd38;
    /* 0xd3C */ u32 field_0xd3c;
};

STATIC_ASSERT(sizeof(daCow_c) == 0xd40);

#endif /* D_A_COW_H */
