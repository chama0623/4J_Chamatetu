#define InitWidth 480
#define InitHeight 320
#define XMAX 10
#define YMAX 7
#define MOVESIZE 16
#define IMGSIZE 32

#define CARDMAX 5
#define PROPERTMAX 6

#define PLAYERNUM 3 
#define STATIONNUM 3

#define INITX 4*IMGSIZE
#define INITY 3*IMGSIZE
#define INITMONEY 1000

#define MAP_NUM 7

#define RESHAPETIME 100
#define MOVETIME 200

// プレイヤーの情報構造体
struct playerstatus{
    char name[20];
    int money;
    int x;
    int y;
    int isBonby;
    int cardnum;
    int cardlist[CARDMAX];
    int cardoption[CARDMAX];
};

typedef struct playerstatus player;
player players[PLAYERNUM];

// 物件情報構造体
struct propertystatus{
    char name[40];
    int holder;
    int price;
    int earnings;
};

typedef struct propertystatus property;

// 駅情報構造体
struct stationstatus{
    char name[20];
    int x;
    int y;
    int ismonopoly;
    int propertynum;
    property plist[PROPERTMAX];
};

typedef struct stationstatus station;
station stations[STATIONNUM];

// 画像用変数
GLuint mapimg[MAP_NUM];
pngInfo mapinfo[MAP_NUM];
GLuint playerimg[PLAYERNUM];
pngInfo playerinfo[PLAYERNUM];

// 誰のターンか判別
int turn;
int direction;
int nx;
int ny;

void Reshape(int, int);
void Timer(int);
void MoveTimer(int);
void PutSprite(int, int, int, pngInfo *);
int isMovable(int,int);
void move();
void nextStation(int,int);
void keyboard(unsigned char,int x,int y);

void Display(void);

void readImg(void);
int getmapnum(int,int);
void drawMap(void);
void drawPlayer(void);

void InitPlayer(void);
void dispPlayer(int);

void readStation(void);
void readProperty(void);
void dispStation(int);
