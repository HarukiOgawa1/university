// gcc q_learning.c -o q_learning -lGL -lGLU -lglut -lm
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <signal.h>

// --- 設定パラメータ ---
#define BOUNDARY 60.0       
#define RADIUS 5.0
#define GRAVITY 0.08
#define WALL_DAMPING 0.7
#define TIME_STEP 16
#define PI 3.1415926535

// --- 強化学習パラメータ ---
#define NUM_STATES 50
#define NUM_ACTIONS 2       // 0:Wait, 1:Kick
#define ALPHA 0.2
#define GAMMA 0.95
#define EPSILON_START 0.8
#define EPSILON_DECAY 0.998

// --- グラフ描画用パラメータ ---
#define HISTORY_SIZE 100    
#define GRAPH_X 10          
#define GRAPH_Y 10          
#define GRAPH_W 200         
#define GRAPH_H 100         

// --- ★追加：リプレイ機能用パラメータ ---
#define MAX_STEPS 5000      // 1エピソードの最大フレーム数

// --- 構造体 ---
typedef struct { GLdouble x, y; } Point;
Point circleModel[36];

typedef struct {
    GLdouble x, y;
    GLdouble vx, vy;
    GLdouble ang;
    GLdouble omega;
    GLfloat r, g, b;
} Ball;

typedef struct {
    GLdouble baseX, baseY;
    GLdouble angle;
    int state;
} Foot;

typedef struct {
    GLdouble left, right;
    GLdouble bottom, top;
} Goal;

// --- グローバル変数 ---
Ball ball;
Foot foot;
Goal goal;

// RL変数
double Q_Table[NUM_STATES][NUM_ACTIONS];
int episodeCount = 0;
double epsilon = EPSILON_START;
int isTraining = 1;
int fastMode = 0;
int testMode = 0;

// エピソード状態管理
int hasTouchedEpisode = 0;
int currentKickHit = 0;
int bounceCount = 0;
int hasBouncedEpisode = 0;

// 報酬倍率（1.0=ノーバウンド、0.5=ワンバウンド以上）
double kickRewardMultiplier = 1.0; 

// 報酬記録用
double rewardHistory[HISTORY_SIZE];
double currentEpisodeReward = 0.0;

// --- ★追加：リプレイ機能用変数 ---
int currentActions[MAX_STEPS]; // 今回のエピソードの行動履歴
int bestActions[MAX_STEPS];    // 過去最高スコア時の行動履歴
int currentStep = 0;           // 現在のステップ数
double bestRewardRecord = -99999.0; // 過去最高の報酬額
int playbackMode = 0;          // 1ならリプレイ再生中
int stepsInBestRun = 0;        // ベストランの長さ

// --- プロトタイプ ---
void timer(int value);
void resetGame(void);
void cleanup(int sig);
void saveQTable();
void loadQTable();

void onExit() {
    saveQTable();
}

void handleSignal(int sig) {
    exit(0);
}

void saveQTable() {
    FILE *fp = fopen("q_table.txt", "w");
    if (fp == NULL) return;
    for (int i = 0; i < NUM_STATES; i++) {
        fprintf(fp, "%lf %lf\n", Q_Table[i][0], Q_Table[i][1]);
    }
    fclose(fp);
    printf("\n--- Model Saved (Episode: %d) ---\n", episodeCount);
}

void loadQTable() {
    FILE *fp = fopen("q_table.txt", "r");
    if (fp == NULL) {
        printf("No save file. New training.\n");
        return;
    }
    for (int i = 0; i < NUM_STATES; i++) {
        if (fscanf(fp, "%lf %lf", &Q_Table[i][0], &Q_Table[i][1]) != 2) break;
    }
    fclose(fp);
    printf("Loaded.\n");
}

void drawString(const char* str, int x, int y) {
    glRasterPos2i(x, y);
    int len = strlen(str);
    for (int i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
}

void drawRewardGraph() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.0, 0.5);
    glRectd(GRAPH_X, GRAPH_Y, GRAPH_X + GRAPH_W, GRAPH_Y + GRAPH_H);
    glDisable(GL_BLEND);

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(GRAPH_X, GRAPH_Y);
    glVertex2i(GRAPH_X + GRAPH_W, GRAPH_Y);
    glVertex2i(GRAPH_X + GRAPH_W, GRAPH_Y + GRAPH_H);
    glVertex2i(GRAPH_X, GRAPH_Y + GRAPH_H);
    glEnd();

    double zeroY = GRAPH_Y + GRAPH_H / 2.0;
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex2i(GRAPH_X, (int)zeroY);
    glVertex2i(GRAPH_X + GRAPH_W, (int)zeroY);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    double scaleY = 0.2;
    for (int i = 0; i < HISTORY_SIZE; i++) {
        double val = rewardHistory[i];
        double plotY = zeroY + val * scaleY;
        if(plotY < GRAPH_Y) plotY = GRAPH_Y;
        if(plotY > GRAPH_Y + GRAPH_H) plotY = GRAPH_Y + GRAPH_H;
        double plotX = GRAPH_X + (double)i / (HISTORY_SIZE - 1) * GRAPH_W;
        glVertex2d(plotX, plotY);
    }
    glEnd();
    
    drawString("Reward History", GRAPH_X, GRAPH_Y + GRAPH_H + 5);
}

int getStateIndex(double y) {
    double min_y = -BOUNDARY;
    double max_y = BOUNDARY;
    int idx = (int)((y - min_y) / (max_y - min_y) * NUM_STATES);
    if (idx < 0) idx = 0;
    if (idx >= NUM_STATES) idx = NUM_STATES - 1;
    return idx;
}

void init(void) {
    int i;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    srand(time(NULL));

    atexit(onExit);
    signal(SIGINT, handleSignal);

    for(int s=0; s<NUM_STATES; s++) {
        Q_Table[s][0] = 0.0; 
        Q_Table[s][1] = 0.0; 
    }
    loadQTable();

    for(int j=0; j<HISTORY_SIZE; j++) rewardHistory[j] = 0.0;

    for (i = 0; i < 36; i++) {
        double angle = (double)i / 36.0 * 2.0 * PI;
        circleModel[i].x = cos(angle);
        circleModel[i].y = sin(angle);
    }

    goal.left = 30.0; goal.right = 50.0;
    goal.bottom = -20.0; goal.top = -10.0;

    resetGame();
}

void resetGame(void) {
    // --- ★追加：ベストスコア更新チェック ---
    // 再生モードでなく、かつ今回のスコアが過去最高なら記録を保存
    if (!playbackMode && currentEpisodeReward > bestRewardRecord) {
        bestRewardRecord = currentEpisodeReward;
        stepsInBestRun = currentStep;
        // 行動履歴をコピー
        for(int i=0; i<currentStep && i<MAX_STEPS; i++) {
            bestActions[i] = currentActions[i];
        }
        printf("!!! New Best Record: %.0f (Steps: %d) !!!\n", bestRewardRecord, currentStep);
    }
    // ------------------------------------

    for(int i=0; i<HISTORY_SIZE-1; i++) {
        rewardHistory[i] = rewardHistory[i+1];
    }
    rewardHistory[HISTORY_SIZE-1] = currentEpisodeReward;

    currentEpisodeReward = 0.0;
    ball.x = -30.0; ball.y = 40.0;
    ball.vx = 0.0; ball.vy = 0.0;
    ball.ang = 0.0; ball.omega = 0.0;
    ball.r = 1.0; ball.g = 0.9; ball.b = 0.9;

    foot.baseX = -30.0; foot.baseY = 0.0;   
    foot.angle = -45.0;
    foot.state = 0;

    hasTouchedEpisode = 0;
    currentKickHit = 0;
    bounceCount = 0;
    hasBouncedEpisode = 0;
    
    kickRewardMultiplier = 1.0;

    // ★追加：ステップリセット
    currentStep = 0;

    episodeCount++;
    // 再生モード中は学習率を減衰させない
    if (!testMode && !playbackMode && epsilon > 0.05) epsilon *= EPSILON_DECAY;

    if (!fastMode) glutPostRedisplay();
}

void drawCircle(void) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 36; i++) glVertex2d(circleModel[i].x, circleModel[i].y);
    glEnd();
}

void drawFoot(void) {
    glPushMatrix();
    glTranslatef(foot.baseX, foot.baseY, 0.0);
    glRotatef(foot.angle, 0.0, 0.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    double thickness = 4.0;
    double legLength = 25.0;
    glRectd(-thickness/2.0, -legLength, thickness/2.0, 0.0);
    glRectd(-thickness/2.0, -legLength - thickness/2.0, 10.0, -legLength + thickness/2.0);
    glPopMatrix();
}

void drawGoal(void) {
    glColor3f(1.0, 1.0, 1.0);
    double wallThick = 1.0; 
    glRectd(goal.left - wallThick, goal.bottom, goal.left, goal.top);
    glRectd(goal.right, goal.bottom, goal.right + wallThick, goal.top);
    glRectd(goal.left - wallThick, goal.bottom - wallThick, goal.right + wallThick, goal.bottom);
}

void display(void) {
    if (fastMode) return;
    glClear(GL_COLOR_BUFFER_BIT);
    drawGoal();
    drawFoot();
    glColor3f(ball.r, ball.g, ball.b);
    glPushMatrix();
    glTranslatef(ball.x, ball.y, 0.0);
    glScaled(RADIUS, RADIUS, 1.0);
    glRotatef(ball.ang, 0.0, 0.0, 1.0);
    drawCircle();
    glPopMatrix();

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, 500, 0, 500); 
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    glColor3f(0.0, 1.0, 0.0);
    char info[256];
    char modeStr[32];
    
    // ★変更：モード表示
    if (playbackMode) sprintf(modeStr, ">> PLAYBACK BEST <<");
    else if (testMode) sprintf(modeStr, "BEST PLAY (No Learn)");
    else sprintf(modeStr, "TRAINING (Eps: %.2f)", epsilon);

    // ★変更：ベストスコア表示追加
    sprintf(info, "Mode: %s  Ep: %d  Rw: %.0f (Best: %.0f)", modeStr, episodeCount, currentEpisodeReward, bestRewardRecord);
    drawString(info, 10, 480);

    drawRewardGraph();

    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
    
    glutSwapBuffers();
}

double toRad(double deg) { return deg * PI / 180.0; }

int checkKickCollision() {
    if (foot.state != 1) return 0;
    if (currentKickHit) return 0;

    double rad = toRad(foot.angle);
    double legLength = 25.0;
    double footTipX = foot.baseX + legLength * sin(rad);
    double footTipY = foot.baseY - legLength * cos(rad);
    double dx = ball.x - footTipX;
    double dy = ball.y - footTipY;
    double dist = sqrt(dx*dx + dy*dy);

    if (dist < RADIUS + 8.0) { 
        double kickPower = 2.5;
        double kickAngle = rad + toRad(90.0);
        ball.vx = cos(kickAngle) * kickPower;
        ball.vy = sin(kickAngle) * kickPower;
        ball.x += ball.vx * 2.0;
        ball.y += ball.vy * 2.0;
        currentKickHit = 1;
        hasTouchedEpisode = 1;

        if (hasBouncedEpisode) {
            kickRewardMultiplier = 0.5; // ワンバウンド以上
        } else {
            kickRewardMultiplier = 1.0; // ノーバウンド
        }

        return 1;
    }
    return 0;
}

int checkGoalCollision() {
    int status = 0;
    if (ball.x > goal.left && ball.x < goal.right) {
        if (ball.y - RADIUS < goal.bottom && ball.y > goal.bottom) {
            ball.y = goal.bottom + RADIUS;
            ball.vy = -ball.vy * 0.5;
            ball.vx *= 0.9;
            return 2; 
        }
    }
    int hitWall = 0;
    if (ball.y < goal.top && ball.y > goal.bottom) {
        if ((ball.x+RADIUS > goal.left && ball.x < goal.left) || 
            (ball.x-RADIUS < goal.left && ball.x > goal.left)) {
            ball.vx *= -0.7; 
            ball.x = (ball.x < goal.left) ? goal.left-RADIUS : goal.left+RADIUS;
            hitWall = 1;
        }
        if ((ball.x-RADIUS < goal.right && ball.x > goal.right) || 
            (ball.x+RADIUS > goal.right && ball.x < goal.right)) {
            ball.vx *= -0.7; 
            ball.x = (ball.x > goal.right) ? goal.right+RADIUS : goal.right-RADIUS;
            hitWall = 1;
        }
    }
    if (hitWall) status = 1;
    return status;
}

void update(void) {
    int state = getStateIndex(ball.y);
    int action = 0;
    double reward = 0.0;

    double dx = ball.x - foot.baseX;
    double dy = ball.y - foot.baseY;
    double dist = sqrt(dx*dx + dy*dy);
    int isReachable = (dist < 40.0); 

    if (foot.state == 0) {
        currentKickHit = 0; 

        // --- ★変更：行動決定ロジック ---
        if (playbackMode) {
            // 再生モード：記録された配列から行動を取り出す
            if (currentStep < stepsInBestRun) {
                action = bestActions[currentStep];
            } else {
                action = 0; // 記録切れ
            }
        } 
        else {
            // 通常モード（学習 or テスト）
            if (isReachable) {
                if (testMode) {
                    action = (Q_Table[state][0] > Q_Table[state][1]) ? 0 : 1;
                } else {
                    if ((double)rand()/RAND_MAX < epsilon) action = rand() % NUM_ACTIONS;
                    else action = (Q_Table[state][0] > Q_Table[state][1]) ? 0 : 1;
                }
            } else {
                action = 0;
            }
        }
        
        // ★追加：行動を記録（通常モード時のみ）
        if (!playbackMode && currentStep < MAX_STEPS) {
            currentActions[currentStep] = action;
        }

        if (action == 1) {
            foot.state = 1;
            reward -= 50.0;
        }
    }

    // ★追加：ステップを進める
    currentStep++;

    ball.vy -= GRAVITY;
    ball.x += ball.vx;
    ball.y += ball.vy;

    if (foot.state == 1) {
        foot.angle += 15.0;
        if (foot.angle > 45.0) foot.state = 2;
    } else if (foot.state == 2) {
        foot.angle -= 5.0;
        if (foot.angle < -45.0) { foot.angle = -45.0; foot.state = 0; }
    }

    if (checkKickCollision()) {
        reward += 50.0;
        if (ball.vx > 0) reward += 30.0;
        else reward -= 10.0;
    }

    int done = 0;
    int goalStatus = checkGoalCollision(); 
    
    if (goalStatus == 2) {
        reward += 200.0 * kickRewardMultiplier;
        done = 1;
    } else if (goalStatus == 1) {
        if (!hasBouncedEpisode) {
            reward += 100.0;
        }
    }

    if (ball.x > BOUNDARY || ball.x < -BOUNDARY || ball.y > BOUNDARY) {
        done = 1;
    } else if (ball.y - RADIUS < -BOUNDARY) {
        ball.y = -BOUNDARY + RADIUS;
        ball.vy = -ball.vy * WALL_DAMPING;
        ball.vx *= 0.9;
        hasBouncedEpisode = 1;
        bounceCount++;
        if (bounceCount >= 3) done = 1;
    }

    if (done && !hasTouchedEpisode) {
        reward -= 30.0;
    }

    currentEpisodeReward += reward;
    int nextState = getStateIndex(ball.y);
    double maxQ_next = (Q_Table[nextState][0] > Q_Table[nextState][1]) ? Q_Table[nextState][0] : Q_Table[nextState][1];

    // ★変更：再生モード中は学習更新しない
    if (isTraining && !testMode && !playbackMode) {
        int updateAction = (foot.state != 0) ? 1 : 0;
        Q_Table[state][updateAction] += ALPHA * (reward + GAMMA * maxQ_next - Q_Table[state][updateAction]);
    }

    if (done) resetGame();
    else if (!fastMode) glutPostRedisplay();
}

void timer(int value) {
    int loops = fastMode ? 50 : 1;
    for(int i=0; i<loops; i++) update();
    glutTimerFunc(fastMode ? 1 : TIME_STEP, timer, 0);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(-BOUNDARY, BOUNDARY, -BOUNDARY, BOUNDARY, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's': case 'S': saveQTable(); break;
        case 't': case 'T': 
            if (playbackMode) { printf("Cannot use Turbo in Playback\n"); break; }
            fastMode = !fastMode; 
            printf("Turbo: %d\n", fastMode); 
            break;
        case 'b': case 'B': 
            testMode = !testMode;
            if (testMode) fastMode = 0; 
            playbackMode = 0; // テストモードにしたら再生モードは解除
            printf("Mode Changed: %s\n", testMode ? "BEST PLAY (No Learn)" : "TRAINING");
            break;
        
        // ★追加：Pキーでベスト再生モード
        case 'p': case 'P':
            if (bestRewardRecord == -99999.0) {
                printf("No best record yet.\n");
            } else {
                playbackMode = !playbackMode;
                fastMode = 0; // 再生時は等倍速
                resetGame();  // 強制リセットして最初から再生
                printf("Playback Mode: %d (Best Reward: %.0f)\n", playbackMode, bestRewardRecord);
            }
            break;

        case 'r': case 'R': 
            epsilon = EPSILON_START; 
            for(int s=0; s<NUM_STATES; s++) { Q_Table[s][0]=0; Q_Table[s][1]=0; }
            bestRewardRecord = -99999.0; // ベスト記録もリセット
            resetGame(); 
            break;
        case '\x1b': exit(0); break;
    }
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RL Soccer: Replay Best");
    init();
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutTimerFunc(TIME_STEP, timer, 0);
    glutMainLoop();
    return 0;
}