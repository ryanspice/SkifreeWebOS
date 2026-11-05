// Adapted from Eric Fry's SkiFree 1.04 reconstruction and Jeff's SDL port.
// See PROVENANCE.md. The original/reconstructed code is not licensed by our MIT notice.
#include "skifree/classic.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
namespace ski::classic {
namespace {
#include "types.hpp"
#include "consts.hpp"
#include "data.hpp"
#include "resource.hpp"
uint32_t randomState=1,nextSeed=1;
double accumulator=0,activeClockFraction=0;
uint32_t activeTickCount=0;
std::vector<Draw> drawCommands;
Result lastResult;
bool initialized=false;
constexpr int spriteDimensions[90][2]={{0,0},{16,32},{16,32},{24,28},{24,28},{16,32},{24,28},{24,28},{24,28},{24,28},{24,28},{24,28},{32,32},{32,24},{32,32},{28,31},{28,31},{28,34},{32,26},{32,32},{31,24},{25,31},{25,31},{12,24},{12,24},{12,24},{12,24},{64,32},{24,30},{22,29},{21,29},{24,24},{24,24},{21,15},{21,15},{19,19},{19,19},{26,30},{20,30},{25,31},{30,29},{32,32},{32,32},{25,29},{29,25},{23,11},{16,11},{16,4},{24,8},{28,32},{22,27},{32,64},{32,8},{93,57},{52,10},{92,30},{63,32},{42,27},{42,27},{50,29},{50,29},{40,36},{44,36},{40,35},{24,64},{26,32},{26,32},{26,32},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{32,48},{16,8},{22,27},{22,27},{22,27},{8,11},{28,32},{28,32},{28,32}};
#define ski_assert(exp,line) (void)((exp)||(assertFailed(sourceFilename,line),0))
int max_(int a, int b);
int min_(int a, int b);
void timerUpdateFunc();
void assertFailed(const char*, int lineNumber);
BOOL doRectsOverlap(RECT* rect1, RECT* rect2);
BOOL areRectanglesEqual(RECT* rect1, RECT* rect2);
const char* getCachedString(uint32_t stringIdx);
int formatElapsedTime(int totalMillis, char* outputString);
BOOL allocateMemory();
Actor* updateActorType1_Beginner(Actor* actor);
Actor* updateActorType2_dog(Actor* actor);
Actor* updateActorType9_treeOnFire(Actor* actor);
Actor* getLinkedActorIfExists(Actor* actor);
int showErrorMessage(LPCSTR message);
Actor* addActorOfTypeWithSpriteIdx(int actorType, uint16_t spriteIdx);
void setupGameTitleActors();
uint16_t getSpriteIdxForActorType(int actorType);
void playSound(Sound*);
Actor* updateActorPositionWithVelocityMaybe(Actor* actor);
void startGameTimer();
void togglePausedState();
void pauseGame();
void enlargeRect(RECT* rect1, RECT* rect2);
short ski_random(short maxValue);
Actor* addActor(Actor* actor, BOOL insertBack);
void addStylePoints(int points);
Actor* getFreeActor();
Actor* addActorOfType(int actorType, uint32_t frameNo);
void handleGameReset();
void handleCharMessage(uint32_t charCode);
RECT* updateActorSpriteRect(Actor* actor);
void mainWindowPaint(HWND);
void paintActors(HDC hdc, RECT* paintRect);
void setupActorList();
void resetPermObjectCount();
BOOL setupGame();
void setPointerToNull(PermObjectList* param_1);
void actorSetFlag8IfFlag1IsUnset(Actor* actor);
void removeFlag8ActorsFromList();
BOOL changeScratchBitmapSize(short, short);
void actorClearFlag10(Actor* actor1, Actor* linkedActor);
Actor* setActorFrameNo(Actor* actor, uint32_t ActorframeNo);
BOOL isSlowTile(short spriteIdx);
Actor* actorSetSpriteIdx(Actor* actor, uint16_t spriteIdx);
Actor* duplicateAndLinkActor(Actor* actor);
Actor* updateActorWithOffscreenStartingPosition(Actor* actor, int borderType);
Actor* updateActorPositionMaybe(Actor* actor, short newX, short newY, short inAir);
void updateActorRectsAfterPlayerMove(short newPlayerX, short newPlayerY);
void getRandomOffscreenStartingPosition(int borderType, short* xPos, short* yPos);
Actor* addRandomActor(int borderType);
int randomActorType1(void);
int areaBasedActorType();
int randomActorType3();
int randomActorType2();
Actor* updateActor(Actor* actor);
Actor* updatePlayerActor(Actor* actor);
void updateSsGameMode(Actor* actor, short xPos, short yPos);
int FUN_00402e30(int param_1, int param_2, int param_3, int param_4, int param_5);
void resetPlayerFrameNo();
void updateEntPackIniKeyValue(LPCSTR key, int value, int isTime);
void permObjectSetSpriteIdx(PermObject* permObject, uint16_t spriteIdx);
void updateFsGameMode(Actor* actor, short xPos, short yPos);
void updateGsGameMode(Actor* actor, short xPos, short yPos);
Actor* updateActorVelMaybe(Actor* actor, const ActorVelStruct* param_2);
Actor* updateActorTypeA_walkingTree(Actor* actor);
Actor* updateActorType3_snowboarder(Actor* actor);
Actor* handleActorCollision(Actor* actor1, Actor* actor2);
void updateAllPermObjectsInList(PermObjectList* param_1);
Actor* addActorForPermObject(PermObject* permObject);
void updatePermObject(PermObject* permObject);
void updatePermObjectActorType4(PermObject* permObject);
void updateYeti(PermObject* permObject);
void FUN_004046e0(PermObjectList* permObjList);
BOOL resetGame(void);
int getSkierGroundSpriteFromMousePosition(short param_1, short param_2);
int getSkierInAirSpriteFromMousePosition(short param_1, short param_2);
void handleMouseClick();
void handleMouseMoveMessage(short xPos, short yPos);
void updateWindowSize(HWND);
void updateActorsAfterWindowResize(short centreX, short centreY);
void updateRectForSpriteAtLocation(RECT* rect, Sprite* sprite, short newX, short newY, short param_5);
void formatAndPrintStatusStrings(HDC);
PermObject* addPermObject(PermObjectList* objList, PermObject* permObject);
void setupPermObjects();
void handleKeydownMessage(int key);
void updateGameState();
BOOL createBitmapSheets(HDC);
void drawWindow(HDC hdc, RECT* windowRect);
void drawActor(HDC hdc, Actor* actor);

int max_(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int min_(int a, int b) {
    if (b > a) {
        return a;
    }
    return b;
}

void timerUpdateFunc() {
    // Original 0x401000 samples GetTickCount once, then advances the game once.
    // The adapter clock excludes pauses; overdue callbacks never invent moves.
    timerFrameDurationInMillis=activeTickCount-currentTickCount;
    prevTickCount=currentTickCount;currentTickCount=activeTickCount;
    updateGameState();drawWindow(nullptr,&windowClientRect);redrawRequired=TRUE;
}

void assertFailed(const char*, int lineNumber) { throw std::runtime_error("Classic core assertion " + std::to_string(lineNumber)); }

BOOL doRectsOverlap(RECT* rect1, RECT* rect2) {
    ski_assert(rect1 != NULL, 352);
    ski_assert(rect2 != NULL, 353);

    if ((((rect2->left < rect1->right) && (rect1->left < rect2->right)) && (rect2->top < rect1->bottom)) && (rect1->top < rect2->bottom)) {
        return TRUE;
    }
    return FALSE;
}

BOOL areRectanglesEqual(RECT* rect1, RECT* rect2) {
    ski_assert(rect1 != NULL, 381);
    ski_assert(rect2 != NULL, 382);

    if ((((rect1->top == rect2->top) && (rect1->left == rect2->left)) && (rect1->right == rect2->right)) && (rect1->bottom == rect2->bottom)) {
        return TRUE;
    }
    return FALSE;
}

const char* getCachedString(uint32_t stringIdx) { return STRINGTABLE[stringIdx-1]; }

int formatElapsedTime(int totalMillis, char* outputString) {
    int iVar1;
    const char* pcVar2;
    uint32_t uVar3;
    uint32_t uVar4;
    uint32_t uVar5;
    uint32_t uVar6;

    uVar6 = (totalMillis % 1000 & 0xffffU) / 10;
    uVar5 = (totalMillis / 1000) % 60 & 0xffff;
    iVar1 = (totalMillis / 1000) / 60;
    uVar3 = iVar1 % 60 & 0xffff;
    uVar4 = iVar1 / 60 & 0xffff;
    pcVar2 = getCachedString(IDS_TIME_FORMAT);
    sprintf(outputString, pcVar2, uVar4, uVar3, uVar5, uVar6);
    return strlen(outputString);
}

BOOL allocateMemory() { static Sprite s[NUM_SPRITES]{}; static Actor a[NUM_ACTORS]{}; static PermObject p[NUM_PERM_OBJECTS]{}; sprites=s; actors=a; permObjects=p; return TRUE; }

Actor* updateActorType1_Beginner(Actor* actor) {
    int random_result;
    Actor* pAVar2;
    uint32_t actorFrameNo;

    actorFrameNo = actor->frameNo;
    if (actor == NULL) {
        assertFailed(sourceFilename, 2130);
    }
    if (actor->typeMaybe != 1) {
        assertFailed(sourceFilename, 2131);
    }
    if (actorFrameNo < 25) {
        pAVar2 = updateActorPositionWithVelocityMaybe(actor);
        if (actorFrameNo - 22 >= 5) {
            assertFailed(sourceFilename, 2135);
        }
        pAVar2 = updateActorVelMaybe(pAVar2, &beginnerActorMovementTbl[actorFrameNo - 22]);
        if (ski_random(0xc) == 0) {
            random_result = ski_random(3);
            if (random_result == 0) {
                actorFrameNo = 0x16;
            } else if (random_result == 1) {
                return setActorFrameNo(pAVar2, 0x17);
            } else if (random_result == 2) {
                return setActorFrameNo(pAVar2, 0x18);
            }
        }
        return setActorFrameNo(pAVar2, actorFrameNo);
    }
    return actor;
}

Actor* updateActorType2_dog(Actor* actor) {
    short sVar1;
    short rand_result;
    Actor* new_actor;
    short newY;
    uint32_t actorFrameNo;
    short inAir;

    actorFrameNo = actor->frameNo;
    if (actor->typeMaybe != 2) {
        assertFailed(sourceFilename, 2162);
    }
    switch (actorFrameNo) {
    case 0x1b:
        rand_result = ski_random(3);
        actor->verticalVelocityMaybe = rand_result - 1;
        new_actor = updateActorPositionWithVelocityMaybe(actor);
        return setActorFrameNo(new_actor, 0x1c);
    case 0x1c:
        actor->HorizontalVelMaybe = 4;
        new_actor = updateActorPositionWithVelocityMaybe(actor);
        return setActorFrameNo(new_actor, 0x1b);
    case 0x1d:
        actor->verticalVelocityMaybe = 0;
        actor->HorizontalVelMaybe = 0;
        rand_result = ski_random(0x20);
        new_actor = updateActorPositionWithVelocityMaybe(actor);
        return setActorFrameNo(new_actor, rand_result != 0 ? 0x1b + 3 : 0x1b);
    case 0x1e:
        rand_result = ski_random(100);
        if (rand_result != 0) {
            new_actor = updateActorPositionWithVelocityMaybe(actor);
            return setActorFrameNo(new_actor, 0x1d);
        }
        inAir = actor->isInAir;
        sVar1 = actor->xPosMaybe;
        newY = actor->yPosMaybe + -2;
        /* dog wee */
        new_actor = addActorOfTypeWithSpriteIdx(ACTOR_TYPE_17_SIGN, 0x52);
        updateActorPositionMaybe(new_actor, (short)(sVar1 - 4), newY, inAir);
        actorFrameNo = 0x1b;
        playSound(&sound_8);
    }
    new_actor = updateActorPositionWithVelocityMaybe(actor);
    return setActorFrameNo(new_actor, actorFrameNo);
}

Actor* updateActorType9_treeOnFire(Actor* actor) {
    int frameNo = actor->frameNo;
    ski_assert(actor->typeMaybe == 9, 2204);
    ski_assert(frameNo >= 0x38, 2205);
    ski_assert(frameNo < 0x3c, 2206);

    frameNo++;
    if (frameNo >= 0x3c) {
        frameNo = 0x38;
    }
    return setActorFrameNo(actor, frameNo);
}

Actor* getLinkedActorIfExists(Actor* actor) {
    ski_assert(actor, 965);
    if (actor->linkedActor == NULL) {
        return actor;
    }
    return actor->linkedActor;
}

int showErrorMessage(LPCSTR message) { throw std::runtime_error(message); }

Actor* addActorOfTypeWithSpriteIdx(int actorType, uint16_t spriteIdx) {
    Actor* actor;

    actor = getFreeActor();
    if (actor != NULL) {
        ski_assert(actorType >= 0, 1403);
        ski_assert(actorType < 0x12, 1404);

        actor->typeMaybe = actorType;
        actor = actorSetSpriteIdx(actor, spriteIdx);
        return actor;
    }
    return actor;
}

void setupGameTitleActors() {
    Actor* actor;
    short x;
    short y;

    y = playerY;
    x = -(sprites[0x35].width / 2) - 40;

    actor = addActorOfTypeWithSpriteIdx(ACTOR_TYPE_17_SIGN, 0x35);
    updateActorPositionMaybe(actor, x, y, 0);

    y = y + sprites[0x36].height + 4;
    actor = addActorOfTypeWithSpriteIdx(ACTOR_TYPE_17_SIGN, 0x36);
    updateActorPositionMaybe(actor, x, y, 0);
    x = sprites[0x37].width;
    if (sprites[0x37].width <= sprites[0x38].width) {
        x = sprites[0x38].width;
    }
    y = sprites[0x37].height;
    actor = addActorOfTypeWithSpriteIdx(ACTOR_TYPE_17_SIGN, 0x37);
    updateActorPositionMaybe(actor, x, y, 0);

    y = y + sprites[0x38].height + 4;
    actor = addActorOfTypeWithSpriteIdx(ACTOR_TYPE_17_SIGN, 0x38);
    updateActorPositionMaybe(actor, x, y, 0);
}

uint16_t getSpriteIdxForActorType(int actorType) {
    int rand_result;

    switch (actorType) {
    case ACTOR_TYPE_11_MOGULS:
        return 0x1b;

    case ACTOR_TYPE_13_TREE:
        rand_result = ski_random(8);
        // TODO bytes here don't match exactly
        if (rand_result == 0) {
            return 0x32;
        }
        if (rand_result == 1) {
            return 0x33;
        }
        return 0x31;

    case ACTOR_TYPE_14_ROCK_STUMP:
        return ski_random(4) != 0 ? 0x2d : 0x2e;

    case ACTOR_TYPE_15_BUMP:
        return ski_random(3) != 0 ? 0x2f : 0x30;

    case ACTOR_TYPE_16_JUMP:
        return 0x34;

    default:
        assertFailed(sourceFilename, 1571);
        return 0;
    }
}

void playSound(Sound*) {} // Original 1.04 executable contains no sound resources.

Actor* updateActorPositionWithVelocityMaybe(Actor* actor) {
    short newX;
    short newY;
    short inAir;

    newX = actor->xPosMaybe + actor->HorizontalVelMaybe;
    newY = actor->yPosMaybe + actor->verticalVelocityMaybe;
    inAir = actor->isInAir + actor->inAirCounter;

    ski_assert(actor, 1061);

    if (isTurboMode != 0) {
        newX = newX + actor->HorizontalVelMaybe;
        newY = newY + actor->verticalVelocityMaybe;
        inAir = inAir + actor->inAirCounter;
    }
    if (inAir > 0) {
        actor->inAirCounter--;
        return updateActorPositionMaybe(actor, newX, newY, inAir);
    }
    actor->inAirCounter = 0;
    return updateActorPositionMaybe(actor, newX, newY, 0);
}

void startGameTimer() {
    if(!isGameTimerRunning&&!isPaused) {
        isGameTimerRunning=TRUE;currentTickCount=activeTickCount;accumulator=0;
        if(isSsGameMode||isGsGameMode)timedGameRelated+=currentTickCount-pauseStartTickCount;
    }
}

void togglePausedState() { isPaused=isGameTimerRunning; if(isGameTimerRunning) pauseGame(); else startGameTimer(); }

void pauseGame() { if(isGameTimerRunning) {isGameTimerRunning=FALSE;pauseStartTickCount=currentTickCount;accumulator=0;} }

void enlargeRect(RECT* rect1, RECT* rect2) {
    ski_assert(rect2, 365);
    ski_assert(rect1, 366);

    if (rect2->left < rect1->left) {
        rect1->left = rect2->left;
    }
    if (rect2->right > rect1->right) {
        rect1->right = rect2->right;
    }
    if (rect2->top < rect1->top) {
        rect1->top = rect2->top;
    }
    if (rect2->bottom > rect1->bottom) {
        rect1->bottom = rect2->bottom;
    }
}

short ski_random(short maxValue) { randomState=randomState*214013u+2531011u; return static_cast<short>(((randomState>>16)&32767u)%static_cast<unsigned>(maxValue)); }

Actor* addActor(Actor* actor, BOOL insertBack) {
    Actor* targetActor;

    targetActor = currentFreeActor;
    ski_assert(actor, 840);

    if (targetActor) {
        currentFreeActor = targetActor->next;

        memcpy(targetActor, actor, sizeof(Actor));

        targetActor->permObject = NULL;
        if (insertBack) {
            targetActor->next = actor->next;
            actor->next = targetActor;
        } else {
            targetActor->next = actorListPtr;
            actorListPtr = targetActor;
        }
        return targetActor;
    } else {
        assertFailed(sourceFilename, 857);
    }

    return targetActor; // TODO fixme the original does `MOV EAX, EBX` but we seem to be doing `XOR EAX, EAX`
}

void addStylePoints(int points) {
    if (isFsGameMode != 0) {
        stylePoints = stylePoints + points;
    }
}

Actor* getFreeActor() {
    Actor* actor;

    blankTemplateActor.spritePtr = sprites;
    actor = addActor(&blankTemplateActor, 0);
    return actor;
}

Actor* addActorOfType(int actorType, uint32_t frameNo) {
    Actor* actor;

    actor = getFreeActor();
    if (actor != NULL) {
        ski_assert(actorType >= 0, 1388);
        ski_assert(actorType < 0x12, 1389);

        actor->typeMaybe = actorType;
        actor = setActorFrameNo(actor, frameNo);
    }
    return actor;
}

void handleGameReset() {
    if (resetGame()) {
        if (isPaused != 0) {
            togglePausedState();
        }
        // InvalidateRect(hSkiMainWnd, NULL, TRUE);
        if (setupGame()) {
            // UpdateWindow(hSkiMainWnd);
            return;
        }
    }
    throw std::runtime_error("Classic reset failed");
}

void handleCharMessage(uint32_t charCode) {
    switch (charCode) {
    case 'X':
        /* 'X' */
        if (playerActor) {
            updateActorPositionMaybe(playerActor, (short)(playerActor->xPosMaybe - 2), playerActor->yPosMaybe, playerActor->isInAir);
            return;
        }
        break;
    case 'Y':
        /* 'Y' */
        if (playerActor) {
            updateActorPositionMaybe(playerActor, playerActor->xPosMaybe, (short)(playerActor->yPosMaybe + -2), playerActor->isInAir);
        }
        break;
    case 'f':
        isTurboMode = (isTurboMode == 0);
        /* 'f' key */
        return;
    case 'r':
        /* 'r' */
        drawWindow(mainWindowDC, &windowClientRect);
        return;
    case 't':
        /* 't' */
        timerUpdateFunc();
        return;
    case 'x':
        /* 'x' */
        if (playerActor) {
            updateActorPositionMaybe(playerActor, (short)(playerActor->xPosMaybe + 2), playerActor->yPosMaybe, playerActor->isInAir);
            return;
        }
        break;
    case 'y':
        /* 'y' */
        if (playerActor) {
            updateActorPositionMaybe(playerActor, playerActor->xPosMaybe, (short)(playerActor->yPosMaybe + 2), playerActor->isInAir);
            return;
        }
    }
    return;
}

RECT* updateActorSpriteRect(Actor* actor) {
    ski_assert(actor, 931);
    ski_assert((actor->flags & FLAG_4) == 0, 932);
    ski_assert(actor->spriteIdx2 != 0, 933);

    if (&sprites[actor->spriteIdx2] != actor->spritePtr) {
        assertFailed(sourceFilename, 934);
    }
    updateRectForSpriteAtLocation(&actor->someRect, actor->spritePtr, actor->xPosMaybe, actor->yPosMaybe, actor->isInAir);
    actor->flags |= FLAG_4;
    return &actor->someRect;
}

void mainWindowPaint(HWND) { drawCommands.clear(); paintActors(nullptr,&windowClientRect); }

void paintActors(HDC hdc, RECT* paintRect) {
    Actor* actor;
    RECT* rect;

    // ski_assert(hdc != NULL, 1347);
    ski_assert(paintRect != NULL, 1348);

    for (actor = actorListPtr; actor != NULL; actor = actor->next) {
        if ((actor->flags & 4) == 0) {
            rect = updateActorSpriteRect(actor);
        } else {
            rect = &actor->someRect;
        }
        if (doRectsOverlap(rect, paintRect)) {
            actor->flags = actor->flags & 0xfffffffe;
        }
    }
    drawWindow(hdc, paintRect);
}

void setupActorList() {
    uint32_t uVar1;
    uint32_t uVar2;
    uint32_t uVar3;

    uVar3 = 0;
    actorListPtr = NULL;
    currentFreeActor = actors;
    uVar2 = 1;
    uVar1 = 0;
    do {
        uVar3 = uVar3 + 1;
        actors[uVar1].next = actors + uVar2;
        uVar1 = uVar3 & 0xffff;
        uVar2 = uVar1 + 1;
    } while (uVar2 < NUM_ACTORS);
    actors[uVar3].next = NULL;
}

void resetPermObjectCount() {
    permObjectCount = 0;
}

BOOL setupGame() {
    Actor* actor;
    short newY;
    short inAir;

    inAir = 0;
    newY = 0;
    actor = addActorOfType(ACTOR_TYPE_0_PLAYER, 3);
    playerActorPtrMaybe_1 = updateActorPositionMaybe(actor, 0, newY, inAir);
    playerActor = playerActorPtrMaybe_1;
    if (!playerActorPtrMaybe_1) {
        return FALSE;
    }
    setupGameTitleActors();
    setupPermObjects();
    isPaused = FALSE;
    startGameTimer();
    return TRUE;
}

void setPointerToNull(PermObjectList* param_1) {
    ski_assert(param_1, 2578);
    param_1->startingObject = NULL;
}

void actorSetFlag8IfFlag1IsUnset(Actor* actor) {
    ski_assert(actor, 865);

    if ((actor->flags & FLAG_1) == 0) {
        if (actor->linkedActor) {
            actor->linkedActor->linkedActor = NULL;
        }
        actor->flags |= FLAG_8;
    }
}

void removeFlag8ActorsFromList() {
    Actor* currentActor;
    Actor* prevActor;

    currentActor = actorListPtr;
    prevActor = (Actor*)&actorListPtr;
    if (actorListPtr) {
        do {
            if ((currentActor->flags & FLAG_8) != 0) {
                if (currentActor->permObject) {
                    ski_assert(currentActor->permObject->actor == currentActor, 886);
                    currentActor->permObject->actor = NULL;
                }
                if (currentActor == playerActor) {
                    playerActor = NULL;
                }
                if (currentActor == playerActorPtrMaybe_1) {
                    playerActorPtrMaybe_1 = NULL;
                }
                prevActor->next = currentActor->next;
                currentActor->next = currentFreeActor;
                currentFreeActor = currentActor;
            } else {
                prevActor = currentActor;
            }
            currentActor = prevActor->next;
        } while (currentActor != NULL);
    }
}

BOOL changeScratchBitmapSize(short, short) { return TRUE; }

void actorClearFlag10(Actor* actor1, Actor* linkedActor) {
    Actor* pAVar1;
    Actor* pAVar2 = actor1;
    Actor** ppAVar3;

    ski_assert(actor1, 1252);
    ski_assert(linkedActor, 1253);
    ski_assert((actor1->flags & FLAG_10), 1254); // <---
    ski_assert((linkedActor->flags & FLAG_10), 1255);
    ski_assert(actor1 != linkedActor, 1256);

    ppAVar3 = &actor1->actorPtr;
    pAVar1 = actor1->actorPtr;

    while (pAVar1 != NULL) {
        pAVar2 = *ppAVar3;
        if ((pAVar2->flags & FLAG_10) != 0) {
            assertFailed(sourceFilename, 1260);
        }
        ppAVar3 = &pAVar2->actorPtr;
        pAVar1 = pAVar2->actorPtr;
    }
    pAVar2->actorPtr = linkedActor;

    enlargeRect(&actor1->rect, &linkedActor->rect);
    /* clear FLAG_10 */
    linkedActor->flags &= 0xffffffef;
}

Actor* setActorFrameNo(Actor* actor, uint32_t ActorframeNo) {
    Actor* pAVar1;

    ski_assert(actor, 1084);
    ski_assert((int)ActorframeNo < 64, 1085);

    if (actor->frameNo != ActorframeNo) {
        ski_assert(ActorframeNo < 64, 1088);

        pAVar1 = actorSetSpriteIdx(actor, actorFrameToSpriteTbl[ActorframeNo]);
        pAVar1->frameNo = ActorframeNo;
        return pAVar1;
    }
    return actor;
}

BOOL isSlowTile(short spriteIdx) {
    if ((spriteIdx != 27) && (spriteIdx != 82)) {
        return FALSE;
    }
    return TRUE;
}

Actor* actorSetSpriteIdx(Actor* actor, uint16_t spriteIdx) {
    ski_assert(actor, 979);
    if (spriteIdx != actor->spriteIdx2) {
        totalAreaOfActorSprites = totalAreaOfActorSprites - actor->spritePtr->totalPixels;
        if ((actor->flags & FLAG_1) != 0) {
            actor = duplicateAndLinkActor(actor);
        }
        actor->spriteIdx2 = spriteIdx;
        actor->spritePtr = &sprites[spriteIdx];
        totalAreaOfActorSprites = totalAreaOfActorSprites + actor->spritePtr->totalPixels;
        actor->flags = actor->flags & (int)0xfffffffb;
        actor->flags |= FLAG_20;

        actor->flags = ((isSlowTile(spriteIdx) & 1) << 6) | (actor->flags & 0xffffffbf);
    }
    return actor;
}

Actor* duplicateAndLinkActor(Actor* actor) {
    Actor* pAVar1;

    ski_assert(actor, 947);
    ski_assert((actor->flags & FLAG_1), 949);

    pAVar1 = addActor(actor, 1);
    actor->linkedActor = pAVar1;
    if (pAVar1 != NULL) {
        pAVar1->linkedActor = actor;
        pAVar1->flags |= FLAG_2;
        actor->flags &= 0xfffffffe; // Clear FLAG_1
    }
    return actor;
}

Actor* updateActorWithOffscreenStartingPosition(Actor* actor, int borderType) {
    short y;
    short x;

    if (actor) {
        getRandomOffscreenStartingPosition(borderType, &x, &y);
        actor = updateActorPositionMaybe(actor, x, y, 0);
    }

    return actor;
}

Actor* updateActorPositionMaybe(Actor* actor, short newX, short newY, short inAir) {
    BOOL hasMoved;
    BOOL bVar3;
    BOOL bVar4;
    BOOL isPlayer;
    uint32_t flags;

    if ((actor->xPosMaybe == newX) && (actor->yPosMaybe == newY)) {
        hasMoved = FALSE;
    } else {
        hasMoved = TRUE;
    }
    bVar4 = actor->isInAir != inAir;
    isPlayer = actor == playerActorPtrMaybe_1;
    ski_assert(actor, 1037);

    if (isPlayer && hasMoved) {
        updateActorRectsAfterPlayerMove(newX, newY);
    }

    if (hasMoved || bVar4) {
        flags = actor->flags;
        if ((flags & FLAG_1) != 0) {
            actor = duplicateAndLinkActor(actor);
        }
        if ((flags & FLAG_4) == 0 || !isPlayer || bVar4) {
            bVar3 = 0;
        } else {
            bVar3 = 1;
        }
        actor->yPosMaybe = newY;
        actor->xPosMaybe = newX;
        actor->isInAir = inAir;
        actor->flags = (uint32_t)(bVar3 | 8) << 2 | (actor->flags & 0xfffffffb);
    }
    return actor;
}

void updateActorRectsAfterPlayerMove(short newPlayerX, short newPlayerY) {
    short dx = newPlayerX - playerX;
    short dy = newPlayerY - playerY;
    Actor* actor = actorListPtr;

    for (; actor != NULL; actor = actor->next) {
        if (actor != playerActorPtrMaybe_1 && (actor->flags & FLAG_4) != 0 && (actor->flags & FLAG_2) == 0) {
            if ((actor->flags & FLAG_1) != 0) {
                duplicateAndLinkActor(actor);
            }

            actor->someRect.left -= dx;
            actor->someRect.right -= dx;
            actor->someRect.top -= dy;
            actor->someRect.bottom -= dy;
        }
    }

    playerX = newPlayerX;
    playerY = newPlayerY;
}

void getRandomOffscreenStartingPosition(int borderType, short* xPos, short* yPos) {
    short sVar1;

    *xPos = (short)playerX - (short)skierScreenXOffset;
    *yPos = playerY - (short)skierScreenYOffset;
    switch (borderType) {
    case BORDER_LEFT:
    case BORDER_RIGHT:
        if (borderType == BORDER_LEFT) {
            sVar1 = (short)windowClientRect.left + -0x3c;
        } else {
            sVar1 = (short)windowClientRect.right + 0x3c;
        }
        *xPos = *xPos + sVar1;
        *yPos = *yPos + (short)windowClientRect.top + ski_random(windowHeight);
        return;
    case BORDER_TOP:
    case BORDER_BOTTOM:
        break;
    default:
        assertFailed(sourceFilename, 1454);
        return;
    }
    *xPos = *xPos + ski_random(windowWidth) + (short)windowClientRect.left;
    if (borderType == BORDER_TOP) {
        *yPos = *yPos + (short)windowClientRect.top + -0x3c;
        return;
    }
    *yPos = *yPos + (short)windowClientRect.bottom + 0x3c;
    return;
}

Actor* addRandomActor(int borderType) {
    uint16_t spriteIdx;
    int actorType;
    Actor* actor = NULL;
    short y;
    short x;

    getRandomOffscreenStartingPosition(borderType, &x, &y);
    if ((((x < -576) || (-320 < x)) || (y < 640)) || (8640 < y)) {
        if (((x < 320) || (512 < x)) || ((y < 640 || (16640 < y)))) {
            if (((x < -160) || (160 < x)) || ((y < 640 || (16640 < y)))) {
                actorType = randomActorType1();
            } else {
                actorType = randomActorType2();
            }
        } else {
            actorType = randomActorType3();
        }
    } else {
        actorType = areaBasedActorType();
    }
    // actorType = 11;
    if (actorType != 0x12) {
        if (actorType < 0xb) {
            actor = addActorOfType(actorType, uint32_t_ARRAY_0040a22c[actorType]);
        } else {
            spriteIdx = getSpriteIdxForActorType(actorType);
            actor = addActorOfTypeWithSpriteIdx(actorType, spriteIdx);
        }
        if (actor != NULL) {
            actor = updateActorPositionMaybe(actor, x, y, 0);
            return actor;
        }
    }

    return actor;
}

int randomActorType1(void) {
    uint16_t uVar1;

    if (totalAreaOfActorSprites > windowWithMarginTotalArea / 32) {
        return 0x12;
    }

    uVar1 = ski_random(1000);
    if (uVar1 < 0x32) {
        return 10;
    }
    if (uVar1 < 500) {
        return ACTOR_TYPE_13_TREE;
    }
    if (uVar1 < 700) {
        return ACTOR_TYPE_15_BUMP;
    }
    if (uVar1 < 0x2ee) {
        return ACTOR_TYPE_11_MOGULS;
    }
    if (uVar1 < 0x3b6) {
        return ACTOR_TYPE_14_ROCK_STUMP;
    }
    if (uVar1 < 0x3ca) {
        return ACTOR_TYPE_16_JUMP;
    }
    return (uVar1 < 0x3de) ? 1 : 2;
}

int areaBasedActorType() {
    return (totalAreaOfActorSprites > windowWithMarginTotalArea / 64) ? ACTOR_TYPE_18_NOTHING : ACTOR_TYPE_11_MOGULS;
}

int randomActorType3() {
    if (totalAreaOfActorSprites > windowWithMarginTotalArea / 16) {
        return ACTOR_TYPE_18_NOTHING;
    }

    return ski_random(0x40) != 0 ? ACTOR_TYPE_13_TREE : ACTOR_TYPE_2_DOG;
}

int randomActorType2() {
    uint16_t uVar1;

    if (totalAreaOfActorSprites > windowWithMarginTotalArea / 32) {
        return 0x12;
    }

    uVar1 = ski_random(100);
    if (uVar1 < 2) {
        return 0xa;
    }
    if (uVar1 < 0x14) {
        return ACTOR_TYPE_13_TREE;
    }
    if (uVar1 < 0x32) {
        return ACTOR_TYPE_15_BUMP;
    }
    if (uVar1 < 0x3c) {
        return ACTOR_TYPE_11_MOGULS;
    }
    return uVar1 < 0x50 ? ACTOR_TYPE_14_ROCK_STUMP : ACTOR_TYPE_16_JUMP;
}

Actor* updateActor(Actor* actor) {
    ski_assert(actor, 2311);
    ski_assert(actor->typeMaybe < 11 && !actor->permObject, 2312);

    switch (actor->typeMaybe) {
    case ACTOR_TYPE_0_PLAYER:
        return updatePlayerActor(actor);
    case ACTOR_TYPE_3_SNOWBOARDER:
        return updateActorType3_snowboarder(actor);
    case ACTOR_TYPE_2_DOG:
        return updateActorType2_dog(actor);
    case ACTOR_TYPE_1_BEGINNER:
        return updateActorType1_Beginner(actor);
    default:
        assertFailed(sourceFilename, 2335);
        return actor;
    case ACTOR_TYPE_9_TREE_ON_FIRE:
        return updateActorType9_treeOnFire(actor);
    case ACTOR_TYPE_10_WALKING_TREE:
        return updateActorTypeA_walkingTree(actor);
    }
}

Actor* updatePlayerActor(Actor* actor) {
    short sVar1;
    short uVar5;
    Actor* pAVar2;
    Sound* sound;
    int points;
    uint32_t ActorframeNo;
    short xPos;
    short yPos;

    xPos = actor->xPosMaybe;
    yPos = actor->yPosMaybe;
    ActorframeNo = actor->frameNo;
    ski_assert(actor, 2022);
    ski_assert(actor->typeMaybe == 0, 2023);

    if (ActorframeNo == 0xb) {
        ski_assert(actor->isInAir == 0, 2027);
        ski_assert(actor->inAirCounter == 0, 2028);

        sVar1 = actor->HorizontalVelMaybe;
        if ((sVar1 == 0) && (actor->verticalVelocityMaybe == 0)) {
            ActorframeNo = 0xc;
        }
        if (sVar1 < 0) {
            uVar5 = -1;
        } else {
            uVar5 = (short)(sVar1 > 0);
        }
        actor->HorizontalVelMaybe = sVar1 - uVar5;
        sVar1 = actor->verticalVelocityMaybe;
        if (sVar1 < 0) {
            actor->verticalVelocityMaybe = sVar1 + 1;
        } else {
            actor->verticalVelocityMaybe = (sVar1 > 0) ? sVar1 - 1 : sVar1;
        }
    } else {
        pAVar2 = updateActorPositionWithVelocityMaybe(actor);
        ski_assert(ActorframeNo < 22, 2040);

        actor = updateActorVelMaybe(pAVar2, &ActorVelStruct_ARRAY_0040a308[ActorframeNo]);
        switch (ActorframeNo) {
        case 8:
        case 10:
            ActorframeNo = 6;
            break;
        case 7:
        case 9:
            ActorframeNo = 3;
            break;
        case 0xd:
        case 0xe:
        case 0xf:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
            if (actor->isInAir == 0) {
                ski_assert(ActorframeNo - 0xd < 9, 2066);

                ActorframeNo = uint32_t_ARRAY_0040a434[ActorframeNo];
                if (ActorframeNo == 0x11) {
                    addStylePoints(-0x40);
                    sound = &sound_1;
                } else {
                    sound = &sound_4;
                }
                playSound(sound);
            }
        }
    }
    pAVar2 = setActorFrameNo(actor, ActorframeNo);
    switch (ActorframeNo) {
    case 7:
    case 8:
    case 9:
    case 10:
        points = -1;
        break;
    default:
        goto switchD_00402b0b_caseD_b;
    case 0x14:
    case 0x15:
        points = 8;
        break;
    case 0x12:
    case 0x13:
        points = 4;
        break;
    case 0x10:
        points = 2;
        break;
    }
    addStylePoints(points);
switchD_00402b0b_caseD_b:
    updateSsGameMode(pAVar2, xPos, yPos);
    updateFsGameMode(pAVar2, xPos, yPos);
    updateGsGameMode(pAVar2, xPos, yPos);
    return pAVar2;
}

void updateSsGameMode(Actor* actor, short xPos, short yPos) {
    int iVar1;
    uint16_t spriteIdx;
    short x;
    short y;

    if (actor == playerActor) {
        x = actor->xPosMaybe;
        y = actor->yPosMaybe;
        ski_assert(actor->typeMaybe == 0, 1788);
        if (isSsGameMode != 0) {
            elapsedTime = currentTickCount - timedGameRelated;
            if (y > 8640) {
                iVar1 = FUN_00402e30(currentTickCount, prevTickCount, (int)y, (int)yPos, 0x21c0);
                isSsGameMode = 0;
                elapsedTime = iVar1 - timedGameRelated;
                INT_0040c964 = 1;
                resetPlayerFrameNo();
                updateEntPackIniKeyValue(iniSsConfigKey, elapsedTime, 1); // TODO this is currently a jmp when it should be a call + ret
                return;
            }
            if (y <= 0x280) {
                isSsGameMode = 0;
                return;
            }
            if (y > currentSlalomFlag->maybeY) {
                spriteIdx = 0x19;
                iVar1 = FUN_00402e30((int)x, (int)xPos, (int)y, (int)yPos, (int)currentSlalomFlag->maybeY);
                if (((currentSlalomFlag->spriteIdx == 0x17) && ((short)iVar1 > currentSlalomFlag->maybeX)) || ((currentSlalomFlag->spriteIdx == 0x18 && ((short)iVar1 < currentSlalomFlag->maybeX)))) {
                    spriteIdx = 0x1a;
                    timedGameRelated = timedGameRelated - 5000;
                }
                permObjectSetSpriteIdx(currentSlalomFlag, spriteIdx);
                currentSlalomFlag = currentSlalomFlag + 1;
                return;
            }
        } else {
            if ((yPos <= 0x280) && (y > 0x280)) {
                iVar1 = FUN_00402e30((int)x, (int)xPos, (int)y, (int)yPos, 0x280);
                if (((short)iVar1 >= -576) && ((short)iVar1 <= -320)) {
                    isSsGameMode = 1;
                    timedGameRelated = FUN_00402e30(currentTickCount, prevTickCount, (int)y, (int)yPos, 0x280);
                    elapsedTime = timedGameRelated - currentTickCount;
                    currentSlalomFlag = firstSlalomFlagLeft;
                }
            }
        }
    }
}

int FUN_00402e30(int param_1, int param_2, int param_3, int param_4, int param_5) {
    ski_assert(param_3 != param_4, 1612);
    return param_1 - ((param_1 - param_2) * (param_3 - param_5)) / (param_3 - param_4);
}

void resetPlayerFrameNo() {
    uint32_t ActorframeNo;

    if (playerActor) {
        ActorframeNo = playerActor->frameNo;
        if ((ActorframeNo != 0xb) && (ActorframeNo != 0x11)) {
            ActorframeNo = (playerActor->isInAir > 0) ? 0xe : 3;
        }
        setActorFrameNo(playerActor, ActorframeNo);
        formatAndPrintStatusStrings(statusWindowDC);
    }
}

void updateEntPackIniKeyValue(LPCSTR key, int value, int isTime) { lastResult={key,value,isTime!=0,lastResult.serial+1}; }

void permObjectSetSpriteIdx(PermObject* permObject, uint16_t spriteIdx) {
    ski_assert(permObject, 1773);

    permObject->spriteIdx = spriteIdx;
    permObject->spritePtr = &sprites[spriteIdx];
    if (permObject->actor) {
        actorSetSpriteIdx(permObject->actor, spriteIdx);
    }
}

void updateFsGameMode(Actor* actor, short xPos, short yPos) {
    int iVar1;
    short x;
    short y;

    if (actor == playerActor) {
        x = actor->xPosMaybe;
        y = actor->yPosMaybe;
        ski_assert(actor->typeMaybe == 0, 1839);

        if (isFsGameMode != 0) {
            if (y > 0x4100) {
                isFsGameMode = 0;
                INT_0040c968 = 1;
                resetPlayerFrameNo();
                updateEntPackIniKeyValue(iniFsConfigKey, stylePoints, 0);
                return;
            }
            if (y <= 0x280) {
                isFsGameMode = 0;
                return;
            }
        } else {
            if ((yPos <= 0x280) && (0x280 < y)) {
                iVar1 = FUN_00402e30((int)x, (int)xPos, (int)y, (int)yPos, 0x280);
                if (((short)iVar1 >= -160) && ((short)iVar1 <= 160)) {
                    isFsGameMode = 1;
                }
            }
        }
    }
}

void updateGsGameMode(Actor* actor, short xPos, short yPos) {
    int iVar1;
    uint16_t spriteIdx;
    short x;
    short y;

    if (actor == playerActor) {
        x = actor->xPosMaybe;
        y = actor->yPosMaybe;
        ski_assert(actor->typeMaybe == 0, 1870);
        if (isGsGameMode != 0) {
            elapsedTime = currentTickCount - timedGameRelated;
            if (0x4100 < y) {
                iVar1 = FUN_00402e30(currentTickCount, prevTickCount, (int)y, (int)yPos, 0x4100);
                isGsGameMode = 0;
                elapsedTime = iVar1 - timedGameRelated;
                INT_0040c960 = 1;
                resetPlayerFrameNo();
                updateEntPackIniKeyValue(iniGsConfigKey, elapsedTime, 1);
                return;
            }
            if (y <= 640) {
                isGsGameMode = 0;
                return;
            }
            /* FIXME this decomp isn't right */
            if (y > currentSlalomFlag->maybeY) {
                spriteIdx = 0x19;
                iVar1 = FUN_00402e30((int)x, (int)xPos, (int)y, (int)yPos, (int)currentSlalomFlag->maybeY);
                if (((currentSlalomFlag->spriteIdx == 0x17) && ((short)iVar1 > currentSlalomFlag->maybeX)) || ((currentSlalomFlag->spriteIdx == 0x18 && ((short)iVar1 < currentSlalomFlag->maybeX)))) {
                    spriteIdx = 0x1a;
                    timedGameRelated = timedGameRelated - 5000;
                }
                permObjectSetSpriteIdx(currentSlalomFlag, spriteIdx);
                currentSlalomFlag = currentSlalomFlag + 1;
                return;
            }
        } else {
            if ((yPos <= 0x280) && (0x280 < y)) {
                iVar1 = FUN_00402e30((int)x, (int)xPos, (int)y, (int)yPos, 0x280);
                if (((short)iVar1 >= 320) && ((short)iVar1 <= 0x200)) {
                    isGsGameMode = 1;
                    timedGameRelated = FUN_00402e30(currentTickCount, prevTickCount, (int)y, (int)yPos, 0x280);
                    elapsedTime = timedGameRelated - currentTickCount;
                    currentSlalomFlag = FirstSlalomFlagRight;
                }
            }
        }
    }
}

Actor* updateActorVelMaybe(Actor* actor, const ActorVelStruct* param_2) {
    short xRelated;
    short sVar1;
    int iVar2;
    short existingHorizontalVel;
    short existingVerticalVel;

    existingHorizontalVel = actor->HorizontalVelMaybe;
    existingVerticalVel = actor->verticalVelocityMaybe;
    ski_assert(actor, 1951);

    if (param_2 == (ActorVelStruct*)0x0) {
        assertFailed(sourceFilename, 1952);
    }
    if (actor->frameNo != param_2->frameNo) {
        assertFailed(sourceFilename, 1953);
    }
    xRelated = param_2->xRelated;
    if (xRelated == 0) {
        if (existingHorizontalVel < 0) {
            xRelated = -1;
        } else {
            xRelated = (short)(0 < existingHorizontalVel);
        }
    }
    existingHorizontalVel = xRelated * existingHorizontalVel;
    if (existingVerticalVel > 0) {
        iVar2 = (int)existingVerticalVel;
    } else {
        iVar2 = 0;
    }
    sVar1 = (short)((param_2->unk_6 * iVar2) / 2);
    if (existingHorizontalVel > sVar1) {
        iVar2 = existingHorizontalVel - 2;
        if ((int)sVar1 <= iVar2) {
            //            LAB_004034f1:
            sVar1 = (short)iVar2;
        }
    } else {
        iVar2 = (int)existingHorizontalVel + (int)param_2->unk_4;
        if (iVar2 <= sVar1) { // goto LAB_004034f1;
            sVar1 = (short)iVar2;
        }
    }
    existingHorizontalVel = param_2->unk_2;
    if (existingHorizontalVel < existingVerticalVel) {
        iVar2 = existingVerticalVel + -2;
        if (existingHorizontalVel <= iVar2) { // goto LAB_0040351c;
            existingHorizontalVel = (short)iVar2;
        }
    } else {
        iVar2 = (int)param_2->unk_0 + (int)existingVerticalVel;
        if (iVar2 <= existingHorizontalVel) { // goto LAB_0040351c;
            existingHorizontalVel = (short)iVar2;
        }
    }
    //    existingHorizontalVel = (short)iVar2;
    //    LAB_0040351c:
    actor->verticalVelocityMaybe = existingHorizontalVel;
    actor->HorizontalVelMaybe = xRelated * sVar1;
    return actor;
}

Actor* updateActorTypeA_walkingTree(Actor* actor) {
    uint16_t uVar2;
    Actor* pAVar3;
    int ActorframeNo;

    ActorframeNo = actor->frameNo;
    ski_assert(actor->typeMaybe == 10, 2217);
    ski_assert(ActorframeNo >= 0x3c, 2218);
    ski_assert(ActorframeNo < 0x40, 2219);

    switch (ActorframeNo) {
    case 0x3c:
        if (actor->HorizontalVelMaybe != 0) {
            assertFailed(sourceFilename, 2223);
        }
        uVar2 = ski_random(100);
        if (uVar2 == 0) {
            uVar2 = ski_random(2);
            actor->HorizontalVelMaybe = uVar2 * 2 + -1;
            pAVar3 = updateActorPositionWithVelocityMaybe(actor);
            return setActorFrameNo(pAVar3, 0x3d);
        }
        break;
    case 0x3d:
        ski_assert(actor->HorizontalVelMaybe != 0, 2232);
        uVar2 = ski_random(10);
        if (uVar2 != 0) {
            pAVar3 = updateActorPositionWithVelocityMaybe(actor);
            pAVar3 = setActorFrameNo(pAVar3, (actor->HorizontalVelMaybe >= 0) ? 0x3f : 0x3e);
            return pAVar3;
        } else {
            actor->HorizontalVelMaybe = 0;
            pAVar3 = updateActorPositionWithVelocityMaybe(actor);
            pAVar3 = setActorFrameNo(pAVar3, 0x3c);
            return pAVar3;
        }
    case 0x3e:
        ski_assert(actor->HorizontalVelMaybe < 0, 2243);
        ActorframeNo = 0x3d;
        break;
    case 0x3f:
        ski_assert(actor->HorizontalVelMaybe > 0, 2248);
        ActorframeNo = 0x3d;
        break;
    default:
        break;
    }

    pAVar3 = updateActorPositionWithVelocityMaybe(actor);
    pAVar3 = setActorFrameNo(pAVar3, ActorframeNo);
    return pAVar3;
}

Actor* updateActorType3_snowboarder(Actor* actor) {
    uint32_t ActorframeNo;

    ActorframeNo = actor->frameNo;
    ski_assert(actor->typeMaybe == 3, 2274);

    actor = updateActorPositionWithVelocityMaybe(actor);
    ski_assert(ActorframeNo - 0x1f < 8, 2277);

    actor = updateActorVelMaybe(actor, &snowboarderActorMovementTbl[ActorframeNo - 0x1f]);
    if (ActorframeNo == 0x1f) {
        if (ski_random(10) == 0) {
            ActorframeNo = 0x20;
        }
    } else if (ActorframeNo == 0x20) {
        if (ski_random(10) == 0) {
            return setActorFrameNo(actor, 0x1f);
        }
    } else if (ActorframeNo == 0x21) {
        if (actor->isInAir == 0) {
            return setActorFrameNo(actor, 0x20);
        }
    } else {
        ski_assert(((int)ActorframeNo >= 0x22) && ((int)ActorframeNo < 0x27), 2298);

        ActorframeNo++;
        if (ActorframeNo == 0x27) {
            return setActorFrameNo(actor, 0x20);
        }
    }
    return setActorFrameNo(actor, ActorframeNo);
}

Actor* handleActorCollision(Actor* actor1, Actor* actor2) {
    short sVar1;
    short sVar2;
    short sVar3;
    int iVar4;
    BOOL bVar5;
    Actor* pAVar6;
    Sound* sound;
    short sVar9;
    short actor1y;
    short actor2y;
    uint32_t local_c;
    short maxSpriteWidth;

    ski_assert(actor1, 2350);
    ski_assert(actor2, 2351);

    if (actor1->typeMaybe >= 11) {
        return actor1;
    }
    actor1y = actor1->yPosMaybe;
    actor2y = actor2->yPosMaybe;
    pAVar6 = getLinkedActorIfExists(actor1);
    sVar2 = pAVar6->yPosMaybe;
    pAVar6 = getLinkedActorIfExists(actor2);
    sVar3 = pAVar6->yPosMaybe;
    if ((((actor1y < actor2y) || (sVar2 > sVar3)) && ((actor1y > actor2y || (sVar2 < sVar3)))) || ((actor1y == actor2y && (sVar2 == sVar3)))) {
        bVar5 = FALSE;
    } else {
        bVar5 = TRUE;
    }
    iVar4 = actor2->typeMaybe;
    local_c = actor1->frameNo;
    sVar1 = actor1->isInAir;
    sVar9 = actor2->spritePtr->height + actor2->isInAir;
    switch (actor1->typeMaybe) {
    case ACTOR_TYPE_10_WALKING_TREE:
        actor1->HorizontalVelMaybe = 0;
        return setActorFrameNo(actor1, 0x3c);
    case ACTOR_TYPE_5_YETI_TOP:
    case ACTOR_TYPE_6_YETI_BOTTOM:
    case ACTOR_TYPE_7_YETI_LEFT:
    case ACTOR_TYPE_8_YETI_RIGHT:
        if (actor2 == playerActor) {
            ski_assert(iVar4 == 0, 2393);
            playSound(&sound_7);
            if ((actor2->flags & FLAG_1) != 0) {
                actor2 = duplicateAndLinkActor(actor2);
            }
            actorSetFlag8IfFlag1IsUnset(actor2);
            ski_assert(actor1->permObject, 2396);

            actor1->permObject->actorFrameNo = 0x32;
            actor1->HorizontalVelMaybe = 0;
            actor1->permObject->xVelocity = 0;
            actor1->verticalVelocityMaybe = 0;
            actor1->permObject->yVelocity = 0;
            actor1->permObject->unk_0x20 = currentTickCount;
            return setActorFrameNo(actor1, 0x32);
        }
        break;
    case ACTOR_TYPE_0_PLAYER:
        if (local_c == 0x11)
            break;
        switch (iVar4) {
        case ACTOR_TYPE_15_BUMP:
            if (sVar1 < 1) {
                actor1->inAirCounter = 4;
                //                        LAB_00403cb4:
                addStylePoints(1);
                playSound(&sound_2);
                return setActorFrameNo(actor1, 0xd);
            }
            if (sVar9 <= sVar1)
                break;
            //                LAB_00403bcc:
            actor1->inAirCounter = actor1->verticalVelocityMaybe / 2;
            addStylePoints(1);
            sound = &sound_2;
            //                LAB_00403be8:
            playSound(sound);
            return setActorFrameNo(actor1, local_c);
        case ACTOR_TYPE_2_DOG:
        case ACTOR_TYPE_12_SLALOM_FLAG:
        case ACTOR_TYPE_17_SIGN:
            if (bVar5) {
                actor1->verticalVelocityMaybe = actor1->verticalVelocityMaybe / 2;
            }
            if (actor2->spriteIdx2 == 0x52) {
                addStylePoints(-16);
                return setActorFrameNo(actor1, local_c);
            }
            break;

        case ACTOR_TYPE_11_MOGULS:
            if (local_c == 0) {
                local_c = 0xd;
                actor1->inAirCounter = 1;
                if (actor1->verticalVelocityMaybe > 4) {
                    actor1->verticalVelocityMaybe = actor1->verticalVelocityMaybe / 2;
                    return setActorFrameNo(actor1, 0xd);
                }
            }
            break;

            // here

        case ACTOR_TYPE_14_ROCK_STUMP:
            if (0 < sVar1) {
                if (sVar9 < sVar1) {
                    if (actor2->spriteIdx2 == 0x56) {
                        if ((actor2->flags & FLAG_1) != 0) {
                            actor2 = duplicateAndLinkActor(actor2);
                        }
                        actorSetFlag8IfFlag1IsUnset(actor2);
                        addStylePoints(100);
                        return setActorFrameNo(actor1, local_c);
                    }
                    break;
                }
                if (!bVar5)
                    break;
                // goto LAB_00403bcc;
                actor1->inAirCounter = actor1->verticalVelocityMaybe / 2;
                addStylePoints(1);
                sound = &sound_2;
                //                LAB_00403be8:
                playSound(sound);
                return setActorFrameNo(actor1, local_c);
            }
        case ACTOR_TYPE_1_BEGINNER:
        case ACTOR_TYPE_3_SNOWBOARDER:
        case 4:
        case 9:
        case 10:
        case ACTOR_TYPE_13_TREE:
            if ((sVar9 < sVar1) || ((short)(actor1->spritePtr->height + sVar1) < actor2->isInAir)) {
                if (iVar4 == 9) {
                    addStylePoints(1000);
                    actor2->typeMaybe = ACTOR_TYPE_13_TREE;
                    actorSetSpriteIdx(actor2, 0x32);
                    return setActorFrameNo(actor1, local_c);
                } else {
                    addStylePoints(6);
                    return setActorFrameNo(actor1, local_c);
                }
            }
            if (bVar5) {
                if (iVar4 == ACTOR_TYPE_13_TREE) {
                    maxSpriteWidth = max_(actor1->spritePtr->width, actor2->spritePtr->width);
                    if (abs((int)actor1->xPosMaybe - (int)actor2->xPosMaybe) > (int)maxSpriteWidth / 2) {
                        actor1->verticalVelocityMaybe = actor1->verticalVelocityMaybe / 2;
                        return setActorFrameNo(actor1, local_c);
                    }
                }
                if ((sVar1 == 0) && (actor1->inAirCounter == 0)) {
                    local_c = 0xb;
                } else {
                    local_c = 0x11;
                    if (actor2->spriteIdx2 == 0x32) {
                        actor2->typeMaybe = 9;
                        setActorFrameNo(actor2, 0x38);
                        addStylePoints(0x10);
                        return setActorFrameNo(actor1, 0x11);
                    }
                }
                if ((actor1->verticalVelocityMaybe < 0) && (actor2->spriteIdx2 == 0x2e)) {
                    actorSetSpriteIdx(actor2, 0x56);
                    return setActorFrameNo(actor1, local_c);
                }
                addStylePoints(-0x20);
                playSound(&sound_1);
                return setActorFrameNo(actor1, local_c);
            }
            break;

        case ACTOR_TYPE_16_JUMP:
            if (((bVar5) && ((int)sVar1 < (int)sVar9 / 2)) && (0 < actor1->verticalVelocityMaybe)) {
                actor1->inAirCounter = actor1->verticalVelocityMaybe;
                //                        goto LAB_00403cb4;
                addStylePoints(1);
                playSound(&sound_2);
                return setActorFrameNo(actor1, 0xd);
            }
        }
        break;
    case ACTOR_TYPE_1_BEGINNER:
        if (0x18 < (int)local_c)
            break;
        if (iVar4 == 0) {
            addStylePoints(0x14);
        }
        sound = &sound_6;
        local_c = (0 < actor2->isInAir) + 0x19;
        //            goto LAB_00403be8;
        playSound(sound);
        return setActorFrameNo(actor1, local_c);
    case ACTOR_TYPE_2_DOG:
        if (((int)local_c < 0x1d) && ((actor2->HorizontalVelMaybe != 0 || (actor2->verticalVelocityMaybe != 0)))) {
            if (iVar4 == 0) {
                addStylePoints(3);
            }
            local_c = 0x1d;
            sound = &sound_3;
            //                goto LAB_00403be8;
            playSound(sound);
            return setActorFrameNo(actor1, local_c);
        }
        break;
    case ACTOR_TYPE_3_SNOWBOARDER:
        switch (iVar4) {
        case ACTOR_TYPE_0_PLAYER:
            addStylePoints(0x14);
        case ACTOR_TYPE_1_BEGINNER:
        case ACTOR_TYPE_3_SNOWBOARDER:
        case ACTOR_TYPE_13_TREE:
        case ACTOR_TYPE_14_ROCK_STUMP:
            if ((sVar1 < sVar9) && (local_c != 0x22)) {
                return setActorFrameNo(actor1, 0x22);
            }
            break;
        case ACTOR_TYPE_15_BUMP:
        case ACTOR_TYPE_16_JUMP:
            if (sVar1 < sVar9) {
                actor1->inAirCounter = actor1->verticalVelocityMaybe / 2;
                playSound(&sound_5);
                return setActorFrameNo(actor1, 0x21);
            }
        }
        break;
    case ACTOR_TYPE_4_CHAIRLIFT:
    case ACTOR_TYPE_9_TREE_ON_FIRE:
        break;
    default:
        assertFailed(sourceFilename, 2376);
    }
    return setActorFrameNo(actor1, local_c);
}

void updateAllPermObjectsInList(PermObjectList* param_1) {
    short top = (short)((windowClientRectWith120Margin.top - skierScreenYOffset) - 0x3c);
    short bottom = (short)((windowClientRectWith120Margin.bottom - skierScreenYOffset) + 0x3c);
    short sVar4;
    PermObject* permObject;

    permObject = param_1->startingObject;
    ski_assert(param_1, 2829);
    ski_assert(permObject <= param_1->nextObject, 2830);

    if (permObject < param_1->nextObject) {
        do {
            updatePermObject(permObject);
            sVar4 = permObject->maybeY - playerY;
            if ((sVar4 >= top) && (sVar4 < bottom)) {
                addActorForPermObject(permObject);
            }
            permObject++;
        } while (permObject < param_1->nextObject);
    }
}

Actor* addActorForPermObject(PermObject* permObject) {
    uint16_t newX;
    uint16_t newY;
    short inAir;
    Actor* actor;
    RECT spriteRect;

    ski_assert(permObject, 2604);
    if (!permObject->actor) {
        newX = permObject->maybeX;
        newY = permObject->maybeY;
        inAir = permObject->unk_0x18;
        updateRectForSpriteAtLocation(&spriteRect, permObject->spritePtr, newX, newY, inAir);
        if (doRectsOverlap(&spriteRect, &windowClientRectWith120Margin)) {
            if (permObject->spriteIdx == 0) {
                actor = addActorOfType(permObject->actorTypeMaybe, permObject->actorFrameNo);
            } else {
                actor = addActorOfTypeWithSpriteIdx(permObject->actorTypeMaybe, permObject->spriteIdx);
            }
            if (actor) {
                actor = updateActorPositionMaybe(actor, newX, newY, inAir);
                permObject->actor = actor;
                actor->permObject = permObject;
            }
        }
    }
    return permObject->actor;
}

void updatePermObject(PermObject* permObject) {
    Actor* pAVar1;
    int actorType;

    if (permObject == (PermObject*)0x0) {
        assertFailed(sourceFilename, 2791);
    }
    permObject->maybeX = permObject->maybeX + permObject->xVelocity;
    actorType = permObject->actorTypeMaybe;
    permObject->maybeY = permObject->maybeY + permObject->yVelocity;
    permObject->unk_0x18 = permObject->unk_0x18 + permObject->unk_0x1e;
    if (actorType != ACTOR_TYPE_4_CHAIRLIFT) {
        // TODO this is byte accurate but probably not how it was originally written in C
        if ((actorType <= 4) || (8 < actorType)) {
            assertFailed(sourceFilename, 2809);
        } else {
            updateYeti(permObject);
        }
    } else {
        updatePermObjectActorType4(permObject);
    }

    pAVar1 = permObject->actor;
    if (pAVar1) {
        ski_assert(pAVar1, 2814); // TODO problems with deadcode removal here.
        ski_assert(pAVar1->permObject, 2815);
        ski_assert(pAVar1->permObject == permObject, 2816);

        pAVar1 = updateActorPositionMaybe(pAVar1, permObject->maybeX, permObject->maybeY, permObject->unk_0x18);
        setActorFrameNo(pAVar1, permObject->actorFrameNo);
    }
}

void updatePermObjectActorType4(PermObject* permObject) {
    ski_assert(permObject, 2633);
    ski_assert(permObject->actorTypeMaybe == ACTOR_TYPE_4_CHAIRLIFT, 2634);

    if (permObject->maybeY <= -1024) {
        permObject->actorFrameNo = 0x29;
        permObject->yVelocity = 2;
        permObject->maybeX = -144;
        return;
    }
    if (23552 <= permObject->maybeY) {
        permObject->actorFrameNo = 0x27;
        permObject->yVelocity = -2;
        permObject->maybeX = -112;
        return;
    }
    /* snowboarder jump out of chairlift */
    if (permObject->actor && (permObject->actorFrameNo == 0x27)) {
        if (ski_random(1000) == 0) {
            updateActorPositionMaybe(addActorOfType(ACTOR_TYPE_3_SNOWBOARDER, 0x21), permObject->maybeX, permObject->maybeY, permObject->unk_0x18);
            permObject->actorFrameNo = 0x28;
        }
    }
}

void updateYeti(PermObject* permObject) {
    short permObjX;
    short permObjY;
    int iVar6;
    short sVar9;
    int actorFrameNo;
    short local_c;
    short pX;
    short pY;
    int tickRelated;
    int dX;
    int dY;

    sVar9 = 0;
    actorFrameNo = permObject->actorFrameNo;
    iVar6 = permObject->actorTypeMaybe;
    ski_assert(permObject, 2664);
    if (permObject->unk_0x18 > 0) {
        permObject->unk_0x1e--;
    } else {
        permObject->unk_0x1e = 0;
        permObject->unk_0x18 = 0;
    }
    if (permObject->unk_0x18 == 0) { // goto LAB_004046b8;

        if (actorFrameNo >= 0x32 && actorFrameNo < 0x38) {
            tickRelated = currentTickCount - permObject->unk_0x20;
            switch (actorFrameNo) {
            case 0x32:
                permObject->actorFrameNo = 0x33;
                return;
            case 0x33:
                permObject->actorFrameNo = (tickRelated < 500) ? 0x32 : 0x34; // ((499 < iVar6) - 1 & 0xfffffffe) + 0x34;
                return;
            case 0x34:
                if (tickRelated > 700) {
                    permObject->actorFrameNo = 0x35;
                    return;
                }
                break;
            case 0x35:
                if (tickRelated > 1000) {
                    permObject->actorFrameNo = 0x36;
                    return;
                }
                break;
            case 0x36:
                permObject->actorFrameNo = 0x37;
                return;
            case 0x37:
                permObject->actorFrameNo = tickRelated < 3000 ? 0x36 : 0x2a; // ((2999 < iVar6) - 1 & 0xc) + 0x2a;
                return;
            default:
                assertFailed(sourceFilename, 2678);
                permObject->actorFrameNo = actorFrameNo;
                return;
            }
            //        goto LAB_004046b8;
            permObject->actorFrameNo = actorFrameNo;
            return;
        }
        permObjX = permObject->maybeX;
        permObjY = permObject->maybeY;
        local_c = 0;
        if (iVar6 == ACTOR_TYPE_5_YETI_TOP && permObjY > -2000) {
            sVar9 = -10;
        } else if (iVar6 == ACTOR_TYPE_6_YETI_BOTTOM && permObjY < 32000) {
            sVar9 = 0x1a;
        } else if (iVar6 == ACTOR_TYPE_7_YETI_LEFT && permObjX > -16000) {
            local_c = -0x10;
        } else if (iVar6 == ACTOR_TYPE_8_YETI_RIGHT && permObjX < 16000) {
            local_c = 0x10;
        } else {
            //            LAB_004044dd:
            if (playerActor) {
                pX = playerActor->xPosMaybe;
                pY = playerActor->yPosMaybe;
                //                    sVar3 = playerActor->xPosMaybe;
                //                    sVar4 = playerActor->yPosMaybe;
                //                if (iVar6 == 5 && sVar4 < -2000) {
                //                    if () goto LAB_00404539;
                //                }
                //                else if (iVar6 == 6 && sVar4 > 32000) {
                //                    if () goto LAB_00404539;
                //                }
                //                else if (iVar6 == 7 && sVar3 < -16000)

                if ((iVar6 == ACTOR_TYPE_5_YETI_TOP && pY < -2000) || (iVar6 == ACTOR_TYPE_6_YETI_BOTTOM && pY > 32000) || (iVar6 == ACTOR_TYPE_7_YETI_LEFT && pX < -16000) || (iVar6 == ACTOR_TYPE_8_YETI_RIGHT && pX > 16000)) {
                    dX = (int)pX - (int)permObjX;
                    dY = (int)pY - (int)permObjY;
                    //                    sVar9 = (short) windowWidth;
                    //                        iVar10 = (int) sVar9;
                    if (dX > windowWidth) {
                        //                        sVar9 = -sVar9;
                        //                        LAB_00404564:
                        permObject->maybeX = pX + -windowWidth;
                    } else if (dX < -windowWidth) { //(dX >= -sVar9) {
                        // goto LAB_00404564; // (SBORROW4(dX,-iVar10) != dX + iVar10 < 0) goto LAB_00404564;
                        permObject->maybeX = pX + windowWidth;
                    }
                    //                        iVar10 = (int) windowHeight;
                    if (dY > windowHeight) {
                        //                        sVar9 = playerActor->yPosMaybe - windowHeight;
                        //                        LAB_00404598:
                        permObject->maybeY = playerActor->yPosMaybe - windowHeight;
                    } else if (dY < -windowHeight) { //(SBORROW4(dY,-iVar10) != dY + iVar10 < 0) {
                        //                        sVar9 = playerActor->yPosMaybe + windowHeight;
                        permObject->maybeY = playerActor->yPosMaybe + windowHeight;
                        //                        goto LAB_00404598;
                    }
                    if (dX >= 0x10) {
                        dX = 0x10;
                    } else {
                        if (dX <= -0x10) {
                            dX = -0x10;
                        }
                    }
                    local_c = (short)dX;
                    if (dY >= 0x1a) {
                        dY = 0x1a;
                    } else {
                        if (dY <= -10) {
                            dY = -10;
                        }
                    }
                    sVar9 = (short)dY;
                    playSound(&sound_9);
                }
            }
        }
        //    uVar11 = (uint32_t)local_c;
        //    uVar8 = (uint32_t)sVar9;

        if (abs(local_c) > abs(sVar9)) {
            permObject->yVelocity = (short)((int)((int)permObject->xVelocity * sVar9) / (int)local_c);
            //        LAB_00404617:
            permObject->unk_0x1e = 1;
        } else if (sVar9 != 0) {
            permObject->xVelocity = (short)((int)((int)permObject->yVelocity * local_c) / (int)sVar9);
            //        goto LAB_00404617;
            permObject->unk_0x1e = 1;
        }
        permObject->yVelocity = sVar9;
        permObject->xVelocity = local_c;
        if (sVar9 < 0) {
            permObject->actorFrameNo = (actorFrameNo == 0x30) + 0x30;
            return;
        }
        if (local_c < 0) {
            permObject->actorFrameNo = (actorFrameNo == 0x2c) + 0x2c;
            return;
        }
        if ((local_c <= 0) && (sVar9 <= 0)) {
            //            uVar5 = ski_random(10);
            if (ski_random(10) != 0) {
                permObject->actorFrameNo = 0x2a;
                return;
            }
            permObject->unk_0x1e = 4;
            permObject->actorFrameNo = 0x2b;
            return;
        }
        actorFrameNo = (actorFrameNo == 0x2e) + 0x2e;
    }
    //    LAB_004046b8:
    permObject->actorFrameNo = actorFrameNo;
}

void FUN_004046e0(PermObjectList* permObjList) {
    short top;
    short bottom;
    PermObject* permObject;
    PermObject* pPVar4;

    top = ((short)windowClientRectWith120Margin.top - skierScreenYOffset) + -0x3c;
    bottom = ((short)windowClientRectWith120Margin.bottom - skierScreenYOffset) + 0x3c;
    permObject = permObjList->currentObj;
    ski_assert(permObjList, 2849);
    ski_assert(permObject >= permObjList->startingObject, 2850);
    ski_assert(permObject <= permObjList->nextObject, 2851);

    pPVar4 = permObjList->nextObject;
    for (; permObject < pPVar4; permObject++) {
        if ((int)permObject->maybeY - (int)playerY >= (int)top)
            break;
    }

    for (; permObject > permObjList->startingObject; permObject--) {
        if ((int)permObject->maybeY - (int)playerY < (int)top)
            break;
    }

    permObjList->currentObj = permObject;

    for (; permObject < permObjList->nextObject;) {
        if ((int)permObject->maybeY - (int)playerY >= bottom) {
            break;
        }
        addActorForPermObject(permObject++);
    }
}

BOOL resetGame(void) {
    currentTickCount = activeTickCount;
    lastResult = {};
    // todo
    //  srand(currentTickCount);
    randomState=nextSeed;
    setupActorList();
    playerActorPtrMaybe_1 = NULL;
    playerActor = NULL;
    totalAreaOfActorSprites = 0;
    resetPermObjectCount();
    isTurboMode = 0;
    playerY = 0;
    playerX = 0;
    DAT_0040c5d8 = 0;
    DAT_0040c714 = 0;
    stylePoints = 0;
    INT_0040c964 = 0;
    isSsGameMode = 0;
    INT_0040c960 = 0;
    isGsGameMode = 0;
    elapsedTime = 0;
    updateTimerDurationMillis = 40;
    redrawRequired = 1;
    return 1;
}

int getSkierGroundSpriteFromMousePosition(short param_1, short param_2) {
    short uVar1;

    if (param_2 > 0) {
        if (param_1 == 0) {
            return 0;
        }
        uVar1 = (((int)param_2 << 2) / (int)param_1);
        if (uVar1 <= -0xc) {
            return 0;
        }
        if (uVar1 <= -6) {
            return 1;
        }
        if (uVar1 <= -3) {
            return 2;
        }
        if (uVar1 <= -1) {
            return 3;
        }
        if (uVar1 >= 0xc) {
            return 0;
        }
        if (uVar1 >= 6) {
            return 4;
        }
        if (uVar1 >= 3) {
            return 5;
        }
        if (uVar1 >= 1) {
            return 6;
        }
    }
    return param_1 < 0 ? 3 : 6;
}

int getSkierInAirSpriteFromMousePosition(short param_1, short param_2) {
    if (param_1 < 0) {
        if (param_2 < 0) {
            return (param_2 < param_1) ? 16 : 14;
        }
        return (-param_2 < param_1) ? 13 : 14;
    }
    if (param_2 < 0) {
        return (-param_2 <= param_1) ? 15 : 16;
    }
    return (param_2 <= param_1) + 13;
}

void handleMouseClick() {
    uint32_t ActorframeNo;

    if (!playerActor) {
        handleGameReset();
        return;
    }
    ActorframeNo = playerActor->frameNo;
    if (ActorframeNo != 11) {
        if (playerActor->isInAir == 0) {
            playerActor->inAirCounter = 4;
            ActorframeNo = 0xd;
        } else if (ActorframeNo != 17) {
            switch (ActorframeNo) {
            case 0xd:
                ActorframeNo = 0x12;
                break;
            case 0x12:
                ActorframeNo = 0x13;
                break;
            case 0x13:
                ActorframeNo = 0xd;
                break;
            case 0xe:
                ActorframeNo = 0x14;
                break;
            case 0xf:
                ActorframeNo = 0x15;
                break;
            }
        }
    }
    if ((ActorframeNo != playerActor->frameNo) && (setActorFrameNo(playerActor, ActorframeNo), redrawRequired != 0)) {
        drawWindow(mainWindowDC, &windowClientRect);
        redrawRequired = FALSE;
    }
}

void handleMouseMoveMessage(short xPos, short yPos) {
    int ActorframeNo;
    short mouseSkierXDelta;
    short mouseSkierYDelta;

    if (((DAT_0040c760 != 0) && (((xPos != prevMouseX || (yPos != prevMouseY)) && (playerActor != NULL)))) && ((playerActor->frameNo != 0xb && (playerActor->frameNo != 0x11)))) {
        mouseSkierXDelta = xPos - (short)skierScreenXOffset;
        mouseSkierYDelta = yPos - (short)skierScreenYOffset;
        if (playerActor->isInAir == 0) {
            ActorframeNo = getSkierGroundSpriteFromMousePosition(mouseSkierXDelta, mouseSkierYDelta);
        } else {
            ActorframeNo = getSkierInAirSpriteFromMousePosition(mouseSkierXDelta, mouseSkierYDelta);
        }
        setActorFrameNo(playerActor, ActorframeNo);
    }
    prevMouseX = xPos;
    prevMouseY = yPos;
    DAT_0040c760 = 1;
}

void updateWindowSize(HWND) {
    DAT_0040c760=0; windowClientRect={0,0,640,360};
    updateActorsAfterWindowResize(320,120);
    windowClientRectWith120Margin={-120,-120,760,480};
    windowWidth=640;windowHeight=360;
    // Preserve the original expression, including its historical height/width typo.
    windowWithMarginTotalArea=(480-(-120))*(480-(-120));
}

void updateActorsAfterWindowResize(short centreX, short centreY) {
    Actor* actor;

    for (actor = actorListPtr; actor != NULL; actor = actor->next) {
        if (((actor->flags & FLAG_4) != 0) && ((actor->flags & FLAG_2) == 0)) {
            if ((actor->flags & FLAG_1) != 0) {
                duplicateAndLinkActor(actor);
            }
            actor->flags &= 0xfffffffb;
        }
    }

    skierScreenYOffset = centreY;
    skierScreenXOffset = centreX;
}

void updateRectForSpriteAtLocation(RECT* rect, Sprite* sprite, short newX, short newY, short param_5) {
    short spriteHeight;
    short spriteWidth;

    spriteWidth = sprite->width;
    spriteHeight = sprite->height;
    ski_assert(rect, 907);
    ski_assert(sprite, 908);

    rect->top = ((short)(newY + (short)((short)(skierScreenYOffset - playerY) - param_5)) - spriteHeight);
    rect->left = (newX + (short)((skierScreenXOffset - spriteWidth / 2) - playerX));
    rect->bottom = spriteHeight + rect->top;
    rect->right = spriteWidth + rect->left;
}

void formatAndPrintStatusStrings(HDC) { statusWindowLastUpdateTime=currentTickCount; }

PermObject* addPermObject(PermObjectList* objList, PermObject* permObject) {
    PermObject* pPVar1;

    pPVar1 = &permObjects[permObjectCount++]; // + uVar2;
    ski_assert(objList, 2587);
    ski_assert(permObject, 2588);
    ski_assert(permObjectCount <= NUM_PERM_OBJECTS, 2589);

    if (objList->startingObject == (PermObject*)0x0) {
        objList->currentObj = pPVar1;
        objList->nextObject = pPVar1;
        objList->startingObject = pPVar1;
    }
    ski_assert(objList->nextObject == pPVar1, 2592);
    objList->nextObject = objList->nextObject + 1;
    memcpy(pPVar1, permObject, sizeof(PermObject));
    pPVar1->actor = NULL;
    pPVar1->spritePtr = &sprites[(uint16_t)pPVar1->spriteIdx];

    return pPVar1;
}

void setupPermObjects() {
    BOOL bVar1;
    uint16_t uVar2;
    PermObject* pPVar3;
    short sVar4;
    PermObject permObject{};

    permObject.unk_0x1e = 0;
    permObject.yVelocity = 0;
    permObject.xVelocity = 0;
    permObject.unk_0x18 = 0;
    setPointerToNull(&PermObjectList_0040c630);
    permObject.actorTypeMaybe = ACTOR_TYPE_17_SIGN;

    // Slalom sign
    permObject.maybeX = ((short)playerX - skierScreenXOffset) + windowClientRect.left + 60;
    permObject.spriteIdx = 0x3d;
    if (permObject.maybeX < -320) {
        permObject.maybeX = -320;
    }
    permObject.maybeY = ((short)playerY - skierScreenYOffset) + windowClientRect.bottom - 60;
    if (640 < permObject.maybeY) {
        permObject.maybeY = 520;
    }
    addPermObject(&PermObjectList_0040c630, &permObject);

    // Slalom start sign left
    permObject.spriteIdx = 0x39;
    permObject.maybeX = -576;
    permObject.maybeY = 640;
    addPermObject(&PermObjectList_0040c630, &permObject);

    // Slalom start sign right
    permObject.spriteIdx = 0x3a;
    permObject.maybeX = -320;
    addPermObject(&PermObjectList_0040c630, &permObject);
    permObject.actorTypeMaybe = ACTOR_TYPE_12_SLALOM_FLAG;
    bVar1 = TRUE;
    firstSlalomFlagLeft = (PermObject*)0x0;
    /* slalom flags */
    sVar4 = 960;
    do {
        permObject.spriteIdx = bVar1 ? 0x17 : 0x18; // 0x18 - (uint16_t)bVar1;
        permObject.maybeX = bVar1 ? -496 : -400;    //(-(uint16_t)bVar1 & 0xffa0) + -400;
        bVar1 = !bVar1;
        permObject.maybeY = sVar4;
        pPVar3 = addPermObject(&PermObjectList_0040c630, &permObject);
        if (firstSlalomFlagLeft == (PermObject*)0x0) {
            firstSlalomFlagLeft = pPVar3;
        }
        sVar4 = sVar4 + 320;
    } while (sVar4 < 8640);
    permObject.actorTypeMaybe = ACTOR_TYPE_17_SIGN;
    permObject.spriteIdx = 0x3b;
    permObject.maybeX = -576;
    permObject.maybeY = 8640;
    addPermObject(&PermObjectList_0040c630, &permObject);
    permObject.spriteIdx = 0x3c;
    permObject.maybeX = -320;
    addPermObject(&PermObjectList_0040c630, &permObject);
    setPointerToNull(&PermObjectList_0040c5e0);
    permObject.actorTypeMaybe = ACTOR_TYPE_17_SIGN;
    permObject.spriteIdx = 0x3e;
    permObject.maybeX = ((short)windowClientRect.right - skierScreenXOffset) + -0x3c + (short)playerX;
    if (permObject.maybeX > 320) {
        permObject.maybeX = 320;
    }
    permObject.maybeY = ((short)windowClientRect.bottom - skierScreenYOffset) + playerY + -0x3c;
    if (0x280 < permObject.maybeY) {
        permObject.maybeY = 520;
    }
    addPermObject(&PermObjectList_0040c5e0, &permObject);
    permObject.spriteIdx = 0x39;
    permObject.maybeX = 320;
    permObject.maybeY = 640;
    addPermObject(&PermObjectList_0040c5e0, &permObject);
    permObject.spriteIdx = 0x3a;
    permObject.maybeX = 512;
    addPermObject(&PermObjectList_0040c5e0, &permObject);
    bVar1 = TRUE;
    FirstSlalomFlagRight = (PermObject*)0x0;
    /* slalom flags, right hand side */
    sVar4 = 0x410;
    do {
        permObject.actorTypeMaybe = ACTOR_TYPE_12_SLALOM_FLAG;
        permObject.spriteIdx = bVar1 ? 0x17 : 0x18;
        permObject.maybeX = bVar1 ? 400 : 432;
        bVar1 = !bVar1;
        permObject.maybeY = sVar4;
        pPVar3 = addPermObject(&PermObjectList_0040c5e0, &permObject);
        if (FirstSlalomFlagRight == (PermObject*)0x0) {
            FirstSlalomFlagRight = pPVar3;
        }
        permObject.actorTypeMaybe = ACTOR_TYPE_13_TREE;
        permObject.spriteIdx = getSpriteIdxForActorType(ACTOR_TYPE_13_TREE);
        uVar2 = ski_random(0x20);
        permObject.maybeX = uVar2 + 400;
        /* this should use the return value */
        ski_random(400);
        sVar4 = sVar4 + 400;
    } while (sVar4 < 0x4100);
    permObject.actorTypeMaybe = ACTOR_TYPE_17_SIGN;
    /* finish sign left */
    permObject.spriteIdx = 0x3b;
    permObject.maybeX = 320;
    permObject.maybeY = 16640;
    addPermObject(&PermObjectList_0040c5e0, &permObject);
    /* finish sign right */
    permObject.spriteIdx = 0x3c;
    permObject.maybeX = 512;
    addPermObject(&PermObjectList_0040c5e0, &permObject);
    setPointerToNull(&PermObjectList_0040c658);

    permObject.actorTypeMaybe = ACTOR_TYPE_17_SIGN;
    permObject.spriteIdx = 0x3f;
    permObject.maybeX = 0;
    permObject.maybeY = (windowClientRect.bottom - skierScreenYOffset);
    permObject.maybeY += playerY;
    permObject.maybeY -= 0x3c;
    if (0x280 < permObject.maybeY) {
        permObject.maybeY = 520;
    }
    addPermObject(&PermObjectList_0040c658, &permObject);
    permObject.spriteIdx = 0x39;
    permObject.maybeX = -160;
    permObject.maybeY = 640;
    addPermObject(&PermObjectList_0040c658, &permObject);
    permObject.spriteIdx = 0x3a;
    permObject.maybeX = 160;
    addPermObject(&PermObjectList_0040c658, &permObject);
    permObject.spriteIdx = 0x3b;
    permObject.maybeX = -160;
    permObject.maybeY = 16640;
    addPermObject(&PermObjectList_0040c658, &permObject);
    permObject.spriteIdx = 0x3c;
    permObject.maybeX = 160;
    addPermObject(&PermObjectList_0040c658, &permObject);
    INT_0040c968 = 0;
    isFsGameMode = 0;
    setPointerToNull(&PermObjectList_0040c738);
    /* ski lift poles??? */
    sVar4 = -1024;
    do {
        permObject.actorTypeMaybe = ACTOR_TYPE_13_TREE;
        permObject.spriteIdx = 0x40;
        permObject.maybeX = -128;
        permObject.maybeY = sVar4;
        permObject.unk_0x18 = 0;
        permObject.unk_0x1e = 0;
        permObject.yVelocity = 0;
        permObject.xVelocity = 0;
        addPermObject(&PermObjectList_0040c738, &permObject);
        sVar4 = sVar4 + 2048;
    } while (sVar4 <= 23552);
    setPointerToNull(&PermObjectList_0040c720);
    sVar4 = -1024;
    do {
        permObject.actorTypeMaybe = ACTOR_TYPE_4_CHAIRLIFT;
        permObject.spriteIdx = 0;
        permObject.unk_0x1e = 0;
        permObject.xVelocity = 0;
        permObject.unk_0x18 = 0x20;
        permObject.maybeY = sVar4;
        if (-1024 < sVar4) {
            permObject.actorFrameNo = 0x27;
            permObject.maybeX = -112;
            permObject.yVelocity = -2;
            addPermObject(&PermObjectList_0040c720, &permObject);
        }
        if (sVar4 < 23552) {
            permObject.actorFrameNo = 0x29;
            permObject.maybeX = -144;
            permObject.yVelocity = 2;
            addPermObject(&PermObjectList_0040c720, &permObject);
        }
        sVar4 = sVar4 + 2048;
    } while (sVar4 <= 23552);
    permObject.actorTypeMaybe = ACTOR_TYPE_7_YETI_LEFT;
    permObject.actorFrameNo = 0x2a;
    permObject.spriteIdx = 0;
    permObject.maybeX = -16060;
    permObject.unk_0x18 = 0;
    permObject.maybeY = 0;
    permObject.unk_0x1e = 0;
    permObject.yVelocity = 0;
    permObject.xVelocity = 0;
    addPermObject(&PermObjectList_0040c720, &permObject);
    permObject.actorTypeMaybe = ACTOR_TYPE_8_YETI_RIGHT;
    permObject.maybeX = 16060;
    addPermObject(&PermObjectList_0040c720, &permObject);
    permObject.actorTypeMaybe = ACTOR_TYPE_5_YETI_TOP;
    permObject.maybeX = 0;
    permObject.maybeY = -2060;
    addPermObject(&PermObjectList_0040c720, &permObject);
    permObject.actorTypeMaybe = ACTOR_TYPE_6_YETI_BOTTOM;
    permObject.maybeY = 32060;
    addPermObject(&PermObjectList_0040c720, &permObject);
}

void handleKeydownMessage(int key) {
    short sVar1;
    uint32_t actorframeNo;

    switch (key) {
    case 27:
        // ShowWindow(hSkiMainWnd, 6 /*SW_MINIMIZE*/);
        pauseGame();
        return;
    case 114:
        togglePausedState(); // TODO this is a jmp rather than a call in the original
        return;
    case 13:
        if (playerActor != (Actor*)0x0) {
            return;
        }
        handleGameReset(); // TODO this is a jmp rather than a call in the original
        return;
    case 113:
        handleGameReset(); // TODO this is a jmp rather than a call in the original
        return;
    default:
        break;
    }

    if (playerActor == NULL) {
        return;
    }
    actorframeNo = playerActor->frameNo;
    sVar1 = playerActor->isInAir;
    if ((actorframeNo != 0xb) && (actorframeNo != 0x11)) {
        switch (key) {
        case 37:
            /* numpad 4
               left */
            ski_assert(actorframeNo < 0x16, 0xf63);

            actorframeNo = playerTurnFrameNoTbl[actorframeNo].leftFrameNo;
            if (actorframeNo == 7) {
                //                    iVar2 = (int)playerActor->HorizontalVelMaybe - 8;
                //                    if (iVar2 <= -8) {
                //                        iVar2 = -8;
                //                    }
                //                    playerActor->HorizontalVelMaybe = (short) iVar2;
                playerActor->HorizontalVelMaybe = max_(playerActor->HorizontalVelMaybe - 8, -8);
            }
            break;

        case 39:
            /* numpad 6, Right */
            ski_assert(actorframeNo < 0x16, 3947);

            actorframeNo = playerTurnFrameNoTbl[actorframeNo].rightFrameNo;
            if (actorframeNo == 8) {
                //                    iVar2 = (int) playerActor->HorizontalVelMaybe + 8;
                //                    if (iVar2 >= 8) {
                //                        iVar2 = 8;
                //                    }
                //                    playerActor->HorizontalVelMaybe = (short) iVar2;

                playerActor->HorizontalVelMaybe = min_(playerActor->HorizontalVelMaybe + 8, 8);
            }
            break;

        case 40:
            if (sVar1 == 0) {
                actorframeNo = 0;
                break;
            }
            switch (actorframeNo) {
            case 0xd:
                actorframeNo = 0x13;
                break;
            case 0x14:
                actorframeNo = 0xe;
                break;
            case 0x15:
                actorframeNo = 0xf;
                break;
            case 0x12:
                actorframeNo = 0xd;
                break;
            case 0x13:
                actorframeNo = 0x12;
                break;
            }
            break;

        case 38:
            switch (actorframeNo) {
            case 0xd:
                //                switchD_0040628c_caseD_13:
                actorframeNo = 0x12;
                break;
            case 0x13:
                //                switchD_0040628c_caseD_12:
                actorframeNo = 0xd;
                break;
            case 0xe:
                actorframeNo = 0x14;
                break;
            case 0xf:
                actorframeNo = 0x15;
                break;
            case 3:
            case 7:
            case 0xc:
                if (playerActor->verticalVelocityMaybe == 0) {
                    actorframeNo = 9;
                    playerActor->verticalVelocityMaybe = -4;
                }
                break;
            case 6:
            case 8:
                if (playerActor->verticalVelocityMaybe == 0) {
                    actorframeNo = 10;
                    playerActor->verticalVelocityMaybe = -4;
                }
                break;
            case 0x12:
                //                switchD_0040628c_caseD_d:
                actorframeNo = 0x13;
                break;
            }
            break;

        case 103:
            if (sVar1 == 0) {
                actorframeNo = 3;
            }
            break;

        case 0x21:
        case 0x69:
            /* numpad 9
               Up right */
            if (sVar1 == 0) {
                actorframeNo = 6;
            }
            break;

        case 0x23:
        case 0x61:
            /* numpad 1
               down left */
            if (sVar1 == 0) {
                actorframeNo = 1;
            }
            break;

        case 0x22:
        case 99:
            /* numpad 3
               down right */
            if (sVar1 == 0) {
                actorframeNo = 4;
            }
            break;

        case 0x2d:
        case 0x60:
            /* numpad 0, Insert
               Jump. */
            if (sVar1 == 0) {
                playerActor->inAirCounter = 2;
                actorframeNo = 0xd;
                if (4 < playerActor->verticalVelocityMaybe) {
                    playerActor->verticalVelocityMaybe = playerActor->verticalVelocityMaybe + -4;
                }
            }
        }
    }

    if ((actorframeNo != playerActor->frameNo) && (setActorFrameNo(playerActor, actorframeNo), redrawRequired != 0)) {
        drawWindow(mainWindowDC, &windowClientRect);
        redrawRequired = 0;
    }
}

void updateGameState() {
    int iVar1;
    Actor* actor;
    RECT* ptVar6;
    RECT* rect2;
    Actor* pAVar7;

    DAT_0040c714 = DAT_0040c714 - (short)playerX;
    DAT_0040c5d8 = DAT_0040c5d8 - playerY;

    for (pAVar7 = actorListPtr; pAVar7 != NULL; pAVar7 = pAVar7->next) {
        if ((pAVar7->flags & (FLAG_2 | FLAG_8)) == 0) {
            pAVar7->flags &= 0xffffffdf;
            if ((pAVar7->permObject == NULL) && (pAVar7->typeMaybe < 0xb)) {
                updateActor(pAVar7);
            }
            if (((pAVar7->flags & FLAG_1) == 0) && (pAVar7 != playerActor)) {
                if ((pAVar7->flags & FLAG_4) != 0) {
                    ptVar6 = &pAVar7->someRect;
                } else {
                    ptVar6 = updateActorSpriteRect(pAVar7);
                }
                if (doRectsOverlap(ptVar6, &windowClientRectWith120Margin) == FALSE) {
                    totalAreaOfActorSprites = totalAreaOfActorSprites - pAVar7->spritePtr->totalPixels;
                    actorSetFlag8IfFlag1IsUnset(pAVar7);
                }
            }
        }
    }

    FUN_004046e0(&PermObjectList_0040c630);
    FUN_004046e0(&PermObjectList_0040c5e0);
    FUN_004046e0(&PermObjectList_0040c658);
    FUN_004046e0(&PermObjectList_0040c738);
    updateAllPermObjectsInList(&PermObjectList_0040c720);
    removeFlag8ActorsFromList();
    for (pAVar7 = actorListPtr; pAVar7 != NULL; pAVar7 = pAVar7->next) {
        if ((pAVar7->flags & FLAG_2) == 0) {
            if ((pAVar7->flags & FLAG_4) != 0) {
                ptVar6 = &pAVar7->someRect;
            } else {
                ptVar6 = updateActorSpriteRect(pAVar7);
            }
            // testing FLAG_20
            iVar1 = pAVar7->flags << 26;
            iVar1 = iVar1 >> 31; //(pAVar7->flags & FLAG_20) ? TRUE : FALSE;
            for (actor = actorListPtr; (actor != NULL && (pAVar7 != actor)); actor = actor->next) {
                if (((actor->flags & FLAG_2) == 0) && (iVar1 != 0 || ((actor->flags & FLAG_20) != 0))) {
                    if ((actor->flags & FLAG_4) != 0) {
                        rect2 = &actor->someRect;
                    } else {
                        rect2 = updateActorSpriteRect(actor);
                    }

                    if (doRectsOverlap(ptVar6, rect2)) {
                        handleActorCollision(pAVar7, actor);
                        if ((pAVar7->flags & FLAG_8) == 0 && (actor->flags & FLAG_8) == 0) {
                            handleActorCollision(actor, pAVar7);
                        }
                    }
                }
            }
        }
    }
    DAT_0040c714 = DAT_0040c714 + (short)playerX;
    for (DAT_0040c5d8 = DAT_0040c5d8 + playerY; 0x3c < DAT_0040c5d8;
         DAT_0040c5d8 = DAT_0040c5d8 + -0x3c) {
        addRandomActor(BORDER_BOTTOM);
    }
    for (; DAT_0040c5d8 < -0x3c; DAT_0040c5d8 = DAT_0040c5d8 + 0x3c) {
        addRandomActor(BORDER_TOP);
    }
    for (; 0x3c < DAT_0040c714; DAT_0040c714 = DAT_0040c714 + -0x3c) {
        addRandomActor(BORDER_RIGHT);
    }
    for (; DAT_0040c714 < -0x3c; DAT_0040c714 = DAT_0040c714 + 0x3c) {
        addRandomActor(BORDER_LEFT);
    }

    if (ski_random(0x29a) != 0) {
        return;
    }
    pAVar7 = addActorOfType(ACTOR_TYPE_3_SNOWBOARDER, 0x1f);

    /* top of screen */
    updateActorWithOffscreenStartingPosition(pAVar7, BORDER_TOP);
}

BOOL createBitmapSheets(HDC) {
    for(int id=1;id<90;++id) {
        sprites[id].width=spriteDimensions[id][0];sprites[id].height=spriteDimensions[id][1];
        sprites[id].totalPixels=sprites[id].width*sprites[id].height;
    }
    return TRUE;
}

void drawWindow(HDC hdc, RECT* windowRect) {
    RECT* rect1;
    RECT* ptVar3;
    uint32_t uVar4;
    Actor* pAVar6;
    Actor* pAVar7;

    // ski_assert(hdc, 1272);
    ski_assert(windowRect, 1273);

    for (pAVar7 = actorListPtr; pAVar7 != NULL; pAVar7 = pAVar7->next) {
        /* if FLAG_1 FLAG_2 FLAG_8 are unset */
        if ((pAVar7->flags & (FLAG_1 | FLAG_2 | FLAG_8)) == 0) {
            pAVar6 = pAVar7->linkedActor;
            if (pAVar6 != NULL && (pAVar6->flags & FLAG_1) != 0 && (pAVar6->flags & FLAG_2) != 0 && pAVar7->spriteIdx2 == pAVar6->spriteIdx2) {
                if ((pAVar6->flags & FLAG_4) != 0) {
                    ptVar3 = &pAVar6->someRect;
                } else {
                    ptVar3 = updateActorSpriteRect(pAVar6);
                }
                if ((pAVar7->flags & FLAG_4) != 0) {
                    rect1 = &pAVar7->someRect;
                } else {
                    rect1 = updateActorSpriteRect(pAVar7);
                }
                if (areRectanglesEqual(rect1, ptVar3)) {
                    pAVar7->flags |= FLAG_1;
                    pAVar6->flags = pAVar6->flags &= 0xfffffffe;
                    actorSetFlag8IfFlag1IsUnset(pAVar6);
                }
            }
        }
    }
    for (pAVar7 = actorListPtr; pAVar7 != NULL; pAVar7 = pAVar7->next) {
        if ((pAVar7->flags & FLAG_8) != 0) {
            pAVar7->flags &= 0xffffffef;
        } else {
            if ((pAVar7->flags & FLAG_4) != 0) {
                ptVar3 = &pAVar7->someRect;
            } else {
                ptVar3 = updateActorSpriteRect(pAVar7);
            }
            uVar4 = doRectsOverlap(ptVar3, windowRect);
            /* set FLAG_10 if rects overlap */
            pAVar7->flags = (pAVar7->flags & 0xffffffef) | ((uVar4 & 1) << 4);
            if ((uVar4 & 1) != 0) {
                (pAVar7->rect).left = ptVar3->left;
                (pAVar7->rect).top = ptVar3->top;
                (pAVar7->rect).right = ptVar3->right;
                (pAVar7->rect).bottom = ptVar3->bottom;
                pAVar7->actorPtr = NULL;
            }
        }
    }

    for (pAVar7 = actorListPtr; pAVar7 != NULL; pAVar7 = pAVar7->next) {
        if ((pAVar7->flags & FLAG_10) != 0) {
            pAVar6 = pAVar7->linkedActor;

            if (pAVar6 != NULL && (pAVar6->flags & FLAG_10) != 0 && doRectsOverlap(&pAVar7->rect, &pAVar6->rect)) {
                actorClearFlag10(pAVar7, pAVar6);
            }

            for (pAVar6 = actorListPtr; pAVar6 != NULL && pAVar6 != pAVar7; pAVar6 = pAVar6->next) {
                if ((pAVar6->flags & FLAG_10) != 0 && doRectsOverlap(&pAVar7->rect, &pAVar6->rect)) {
                    actorClearFlag10(pAVar7, pAVar6);
                    pAVar6 = actorListPtr;
                }
            }
        }
    }

    for (pAVar7 = actorListPtr; pAVar7 != NULL; pAVar7 = pAVar7->next) {
        if ((pAVar7->flags & FLAG_10) != 0) {
            drawActor(hdc, pAVar7);
        }
    }

    for (pAVar7 = actorListPtr; pAVar7 != NULL; pAVar7 = pAVar7->next) {
        if ((pAVar7->flags & FLAG_2) != 0) {
            actorSetFlag8IfFlag1IsUnset(pAVar7);
        }
    }
    removeFlag8ActorsFromList();
}

void drawActor(HDC hdc, Actor* actor) {
    Actor** ppAVar1;
    short y;
    RECT* rect;
    uint16_t uVar6;
    Actor* pAVar7;
    short sVar8;
    short newWidth;
    uint16_t newHeight;
    Actor* actor_00;
    short sVar10;
    Actor** ppAVar11;
    // DWORD rop;
    Actor* local_24;
    Actor* local_20;
    int local_1c;
    uint32_t local_14;
    uint32_t actorRectLeft;
    short sheetY;
    int local_c;
    uint32_t local_8 = 0;
    Actor** local_4 = nullptr;
    Sprite* sprite;
    short spriteWidth;
    short spriteHeight;

    uVar6 = *(uint16_t*)&(actor->rect).left;
    actorRectLeft = (actor->rect).left;

    y = *(short*)&(actor->rect).top;
    newWidth = *(short*)&(actor->rect).right - uVar6;
    newHeight = *(short*)&(actor->rect).bottom - y;

    local_c = 0;
    local_20 = actor;
    ski_assert(actor, 1133);
    // ski_assert(hdc, 1134);
    ski_assert((actor->flags & FLAG_10) != 0, 1135);

    if (actor == NULL) {
        return;
    }

    pAVar7 = actor;
    while ((pAVar7->flags & FLAG_1) << 1 != (pAVar7->flags & FLAG_2)) {
        pAVar7 = pAVar7->actorPtr;
        if (pAVar7 == NULL) {
            return;
        }
    }
    local_1c = 0;
    if (!changeScratchBitmapSize(newWidth, newHeight)) {
        // PatBlt(hdc, (int)(short)uVar6, (int)y, (int)newWidth, (int)(short)newHeight, 0xff0062);
        do {
            if ((actor->flags & FLAG_1) == 0 && (actor->flags & FLAG_2) == 0) {
                sprite = actor->spritePtr;
                if ((actor->flags & FLAG_4) == 0) {
                    rect = updateActorSpriteRect(actor);
                } else {
                    rect = &actor->someRect;
                }

                // SRCAND
                // BitBlt(hdc, rect->left, rect->top, (int)sprite->width, (int)sprite->height, sprite->sheetDC, 0, (int)sprite->sheetYOffset, 0x8800c6);
                actor->flags |= FLAG_1;
            } else {
                actor->flags &= 0xfffffffe;
            }
            actor = actor->actorPtr;
        } while (actor != NULL);
        return;
    }

    do {
        actor_00 = NULL;
        local_24 = NULL;
        pAVar7 = actor;
        ppAVar11 = &local_20;
        if (actor == NULL)
            break;
        do {
            ppAVar1 = &actor->actorPtr;
            if ((actor->flags & FLAG_2) == 0) {
                if ((actor->flags & FLAG_40) == 0) {
                    sVar8 = 0;
                } else {
                    sVar8 = actor->spritePtr->height;
                }
                uVar6 = actor->yPosMaybe - sVar8;
                if ((actor_00 == NULL) || ((short)uVar6 < (short)local_8)) {
                    actor_00 = actor;
                    local_24 = actor;
                    local_8 = uVar6; // actor->flags & 0xffff0000 | (uint32_t)uVar6;
                    local_4 = ppAVar11;
                }
            } else {
                if ((actor->flags & FLAG_1) != 0) {
                    local_c = 1;
                    actor->flags &= 0xfffffffe;
                }
                *ppAVar11 = *ppAVar1;
                pAVar7 = local_20;
            }
            actor = *ppAVar1;
            ppAVar11 = ppAVar1;
        } while (actor != NULL);
        actor = pAVar7;
        if (actor_00 != NULL) {
            sprite = actor_00->spritePtr;
            spriteWidth = sprite->width;
            spriteHeight = sprite->height;
            sheetY = sprite->sheetYOffset;
            if ((actor_00->flags & FLAG_4) == 0) {
                rect = updateActorSpriteRect(actor_00);
            } else {
                rect = &actor_00->someRect;
            }

            local_14 = rect->left - actorRectLeft;
            sVar10 = *(short*)&rect->top - y;
            ski_assert(rect->right - rect->left == spriteWidth, 1203);
            ski_assert(rect->bottom - rect->top == spriteHeight, 1204);
            ski_assert(local_14 >= 0, 1205);

            // if (sVar10 < 0) {
            //     printf("skipping because sVar10\n");
            //     return;
            // }

            ski_assert(sVar10 >= 0, 1206);
            ski_assert(newWidth >= spriteWidth, 1207);

            // if (sVar10 < 0) {
            //     printf("skipping because sVar10\n");
            //     return;
            // }
            if (newHeight < spriteHeight) {
                assertFailed(sourceFilename, 1208);
            }
            if (local_1c == 0) {
                local_1c = 1;
                if ((((0 < (short)local_14) || (0 < sVar10)) || (spriteWidth < newWidth)) || (spriteHeight < newHeight)) {
                    // PatBlt(bitmapSourceDC, 0, 0, (int)newWidth, (int)newHeight, 0xff0062);
                }
                // rop = 0xcc0020;
            } else {
                // SRCPAINT
                // BitBlt(bitmapSourceDC, (int)(short)local_14, (int)sVar10, spriteWidth, spriteHeight, sprite->sheetDC_1bpp, 0, (int)sheetY,
                //        0xee0086);
                // SRCAND
                // rop = 0x8800c6;
            }

            // BitBlt(bitmapSourceDC, (int)(short)local_14, (int)sVar10, spriteWidth, spriteHeight, sprite->sheetDC, 0, (int)sheetY, rop);

            drawCommands.push_back({static_cast<int>(actor_00->spriteIdx2),
                static_cast<int>(rect->left),static_cast<int>(rect->top),spriteWidth,spriteHeight});

            local_24->flags |= FLAG_1;
            *local_4 = local_24->actorPtr;
            actor = local_20;
        }
    } while (actor != NULL);
    if (local_1c != 0) {
        // SRCCOPY
        // BitBlt(hdc, (int)(short)actorRectLeft, (int)y, (int)newWidth, (int)(short)newHeight, bitmapSourceDC, 0, 0, 0xcc0020);

        return;
    }
    if (local_c != 0) {
        // PatBlt(hdc, (int)(short)actorRectLeft, (int)y, (int)newWidth, (int)(short)newHeight, 0xff0062);
    }
}

} // internal reconstruction
void reset(uint32_t seed) {
    nextSeed=seed;activeTickCount=0;activeClockFraction=0;accumulator=0;
    allocateMemory();createBitmapSheets(nullptr);
    resetGame();updateWindowSize(nullptr);setupGame();
    lastResult={};accumulator=0;initialized=true;
    mainWindowPaint(nullptr);
}
void advance(double seconds) {
    if(!initialized||!isGameTimerRunning||!std::isfinite(seconds)||seconds<=0)return;
    const double millis=seconds*1000.0+activeClockFraction;
    if(!std::isfinite(millis))return;
    const double whole=std::floor(millis+1e-9);
    activeClockFraction=std::max(0.0,millis-whole);
    // Preserve Win32's unsigned millisecond wrapping without out-of-range casts.
    activeTickCount+=static_cast<uint32_t>(std::fmod(whole,4294967296.0));
    accumulator+=seconds;
    if(accumulator+1e-12>=0.04) {
        accumulator=std::fmod(accumulator,0.04);
        if(accumulator<1e-12||0.04-accumulator<1e-12)accumulator=0;
        timerUpdateFunc();
    }
}
void pause(bool value) { if(!initialized)return;isPaused=value; if(value)pauseGame();else startGameTimer(); }
void key(Key key) {
    if(!initialized)return;
    if(key==Key::Fast){isTurboMode=!isTurboMode;return;}
    handleKeydownMessage(static_cast<int>(key));
}
void click() {if(initialized)handleMouseClick();}
void character(char value) {if(initialized)handleCharMessage(static_cast<unsigned char>(value));}
void pointer(int x,int y) {if(initialized)handleMouseMoveMessage(static_cast<short>(x),static_cast<short>(y));}
const std::vector<Draw>& draws() {if(initialized)mainWindowPaint(nullptr);return drawCommands;}
State state() {
    State s{};s.x=playerX;s.y=playerY;s.frame=playerActor?static_cast<int>(playerActor->frameNo):-1;
    s.alive=playerActor!=nullptr;s.fast=isTurboMode;s.paused=!isGameTimerRunning;
    s.style=stylePoints;s.elapsedMs=elapsedTime;s.clockMs=currentTickCount;
    s.course=isSsGameMode?Course::Slalom:isGsGameMode?Course::TreeSlalom:isFsGameMode?Course::Freestyle:Course::Free;
    int y=playerActor?playerActor->yPosMaybe:playerY;
    if(isSsGameMode)y=8640-y;else if(isGsGameMode||isFsGameMode)y=16640-y;
    s.distance=y/16;s.speed=playerActor&&timerFrameDurationInMillis?
        static_cast<int>(static_cast<int64_t>(playerActor->verticalVelocityMaybe)*1000/
                         (static_cast<int64_t>(timerFrameDurationInMillis)*16)):0;
    s.inAir=playerActor?playerActor->isInAir:0;
    for(Actor* a=actorListPtr;a;a=a->next)if(!(a->flags&(FLAG_2|FLAG_8)))++s.actors;
    s.result=lastResult;return s;
}
} // ski::classic

#ifdef SKIFREE_CLASSIC_TESTING
#include "../../tests/classic_fixtures.hpp"
#endif
