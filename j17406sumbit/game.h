// ウィンドウサイズ
#define InitWidth 480
#define InitHeight 320
// 画像サイズ
#define IMGSIZE 32
// マップ配列のサイズ
#define XMAX 30
#define YMAX 30
// マップ画像の枚数
#define MAP_NUM 7
// イベント画像の枚数
#define SP_NUM 4
// 季節画像の枚数
#define SEASON_NUM 4

// 再描画タイマー秒数
#define RESHAPETIME 100
// 移動タイマー秒数
#define MOVETIME 100
// 乱数表示秒数
#define RANDTIME 100

// 日本語+特殊文字
#define JPMAX 75
// 特殊文字
#define SPMAX 24
// 名前の最大長
#define NAMEMAX 20
// 駅名,物件名の最大長
#define STRMAX 60

// マスIDの定義
#define PLUSMASU 0 // プラス駅
#define MINUSMASU 1 // マイナス駅
#define CARDMASU 2 // カード駅
#define PROPERTYMASU 3 // 物件駅
#define SENRO1 4 // 線路(縦)
#define SENRO2 5 // 線路(横)
#define DIST 6 // 目的地駅
#define WALL 623 // 草原

// ターン中のプレイヤーを真ん中に描画するための座標
#define CX 7
#define CY 5
#define CENTX CX*IMGSIZE
#define CENTY CY*IMGSIZE
// 一回の動く距離
#define MOVESIZE 16

// プレイヤー人数
#define PLAYERNUM 3 
// 初期プレイヤー座標
#define INITX 13*IMGSIZE
#define INITY 7*IMGSIZE
// 初期所持金
#define INITMONEY 10000

// サイコロの出目の最大値
#define DICEMAX 6
// サイコロの最大数
#define SAIKOROMAX 4
// 駅の数
#define STATIONNUM 21
// 最大物件数
#define PROPERTMAX 6
// カードの最大枚数
#define CARDMAX 5

#define CARDNUM 8
// カード番号の定義
#define KYUKO 1 // 急行カード
#define TOKKYU 2 // 特急カード
#define SINKANSEN 3 // 新幹線カード
#define SAMMIT 4 // サミットカード 
#define BUTTOBI 5 // ぶっとびカード
#define JUOKU 6 // 十億円カード
#define TOKUSEIREI 7 // 徳政令カード
#define GOUSOKKYU 8 // 剛速球カード

// 月の最大値
#define MAXMONTH 12

// プレイヤーの情報構造体
struct playerstatus{
    char name[NAMEMAX]; // プレイヤー名
    int money; // 所持金
    int assets; // 総資産
    int x; // x座標(実描画座標)
    int y; // y座標(実描画座標)
    int cardnum; // 持っているカード枚数
    int card[CARDMAX]; // カードの番号記憶
};

typedef struct playerstatus player;
player players[PLAYERNUM]; // 人数分の配列を確保

// 物件情報構造体
struct propertystatus{
    char name[STRMAX]; // 物件名
    int holder; // 物件所持者
    int price; // 価格
    int earnings; // 収益率
};

typedef struct propertystatus property;

// 駅情報構造体
struct stationstatus{
    char name[STRMAX]; // 駅名
    int x; // x座標
    int y; // y座標
    int ismonopoly; // 独占フラグ
    int propertynum; // 物件数
    property plist[PROPERTMAX]; // 物件情報構造体の配列
};

typedef struct stationstatus station;
station stations[STATIONNUM]; // 駅の数分の配列を確保
station distination; // 目的地配列

// 画像用変数
// 季節画像
GLuint seasonimg[SEASON_NUM];
pngInfo seasoninfo[SEASON_NUM];
// マップ画像
GLuint mapimg[MAP_NUM];
pngInfo mapinfo[MAP_NUM];
// プレイヤー画像
GLuint playerimg[PLAYERNUM];
pngInfo playerinfo[PLAYERNUM];
// サイコロ画像
GLuint diceimg[DICEMAX];
pngInfo diceinfo[DICEMAX];
// イベント画像
GLuint spimg[SP_NUM];
pngInfo spinfo[SP_NUM];

// 日本語画像
// ひらがな黒
GLuint hblackimg[JPMAX+SPMAX];
pngInfo hblackinfo[JPMAX+SPMAX];
// ひらがな赤
GLuint hredimg[JPMAX+SPMAX];
pngInfo hredinfo[JPMAX+SPMAX];
// カタカナ黒
GLuint kblackimg[JPMAX];
pngInfo kblackinfo[JPMAX];
// カタカナ赤
GLuint kredimg[JPMAX];
pngInfo kredinfo[JPMAX];

// 月,年,季節
int month,year,season;
// 誰のターンか判別
int turn;
// ターンの状況
int turnstatus;
int inflg;
// ゴールフラグ
int goalflg;
// 向き
int direction;
// 次の停車位置座標
int nx;
int ny;
// ターン中のプレイヤーを真ん中に描くための変数
int tx,ty;

// キーボードフラグ
int keyboardflg;
// 残り移動可能マス
int recount;

//サイコロの数
int saikoro;
int rcard;
int nextflg;

// 乱数生成用
int randflg;
int range;
int randresult;
int randresulttmp[SAIKOROMAX];
int dummynum;
int dummyresult[SAIKOROMAX];

// 収益計算用
int tmpmoney;
// 借金計算用 
int rdebet;

// 物件を買うときの変数
// 選択ポジション
int selectpos;
// 駅のインデックス
int stid;
// 物件数
int propertynum;

// 移動したマスを記録
int massRecord[DICEMAX*SAIKOROMAX][2];

void Reshape(int, int);
void PutSprite(int, int, int, pngInfo *,double);
void Timer(int);
void keyboardTimer(int);
void MoveTimer(int);
void RandTimer(int);

int getmapnum(int,int);
int isMovable(int,int);
void move(void);
void nextStation(int,int);
void dispmassRecord(void);

void readImg(void);
void readStation(void);
void readProperty(void);

void drawMap(void);
void drawPlayer(void);
void drawChar(int,int,int,int,int,double);
void drawString(char *,int,int,int,double);
void drawDialog(int,int,int,int);
void drawQUAD(int,int,int,int);
void drawMoney(int,int,int,int,double);
void drawText(char *,int,int,int,int,int);
void drawStation(void);

void Initvalue(void);
void InitPlayer(void);

void keyboard(unsigned char,int x,int y);
void calseason(void);
int sale(int,int);
void kessan(void);
int debtprocess(void);
void Display(void);

void Initvalue(void);
void startgame(void);
void desisionDist(void);
void startTurn(void);
void rollDice(void);
void moveMass(void);
void checkMass(void);
void purchaseProperty(void);
void plusMass();
void minusMass(void);
void cardMass(void);
void endTurn(void);
void processKessan();
void endgame(void);

void dispPlayer(int);
void dispStation(int);