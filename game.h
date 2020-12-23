#define InitWidth 480
#define InitHeight 320
#define XMAX 10
#define YMAX 7
#define IMGSIZE 32

#define CARDMAX 5
#define PROPERTMAX 6

#define PLAYERNUM 3 
#define STATIONNUM 3

#define INITX 4
#define INITY 3
#define INITMONEY 1000

#define MAP_NUM 7

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

GLuint mapimg[MAP_NUM];
pngInfo mapinfo[MAP_NUM];

void Reshape(int, int);
void Timer(int);
void PutSprite(int, int, int, pngInfo *);
int isMovable(int,int,int);
void move(int *,int *,int);

void Display(void);

void readImg(void);
int getmapnum(int,int);
void drawMap(void);

void InitPlayer(void);
void dispPlayer(int);

void readStation(void);
void readProperty(void);
void dispStation(int);
