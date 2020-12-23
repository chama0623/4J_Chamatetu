#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

char Map[YMAX][XMAX+1] = { //NULL文字に気を付ける
        "AAAAAAAAAA",
        "AAP-B-BAAA",
        "AA|A|AAAAA",
        "AAP-B--CAA",
        "AAAA|AAAAA",
        "AAAAMAAAAA",
        "AAAAAAAAAA"
};

//
//  ウィンドウのサイズ変更が発生したときに座標系を再設定する関数
//
void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glScaled(1, -1, 1);
    glTranslated(0, -h, 0);
}

//
//  タイマー割り込みで画面を描画し直す
//
void Timer(int t)
{
    glutPostRedisplay();
    glutTimerFunc(t, Timer, 0);
}

//
//  num番のPNG画像を座標(x,y)に表示する
//
void PutSprite(int num, int x, int y, pngInfo *info)
{
    int w, h;  //  テクスチャの幅と高さ

    w = info->Width;   //  テクスチャの幅と高さを取得する
    h = info->Height;

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, num);
    glColor4ub(255, 255, 255, 255);

    glBegin(GL_QUADS);  //  幅w, 高さhの四角形

    glTexCoord2i(0, 0); 
    glVertex2i(x, y);

    glTexCoord2i(0, 1);
    glVertex2i(x, y + h);

    glTexCoord2i(1, 1);
    glVertex2i(x + w, y + h);

    glTexCoord2i(1, 0);
    glVertex2i(x + w, y);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

int isWall(int x,int y){
    if(getmapnum(x,y)==0){
        return 1;
    }
    return 0;
}

// 0 : 上
// 1 : 右
// 2 : 下
// 3 : 左 
//
// 進めるとき 1
// 進めないとき 0
int isMovable(int x,int y,int d){
    int xa=x/IMGSIZE;
    int ya=y/IMGSIZE;
    if(d==0){ // 上
        if(isWall(xa,ya-1)){
            return 0;
        }
    }else if(d==1){ //右
        if(isWall(xa+1,ya)){
            return 0;
        }    
    }else if(d==2){ //下
        if(isWall(xa,ya+1)){
            return 0;
        }   
    }else{ //左
        if(isWall(xa-1,ya)){
            return 0;
        }   
    }
    return 1;
}

void move(int *x,int *y,int d){
    switch (d)
    {
    case 0:
        *y-=1;
        break;
    case 1:
        *x+=1;
        break;
    case 2:
        *y+=1;
        break;
    case 3:
        *x-=1;
        break;
    default:
        break;
    }
}

void readImg(void){
    int i;
    char fname[40];
    //read mapimage
    for(i=0;i<MAP_NUM;i++){
        sprintf(fname,".\\mapparts\\map%d.png",i+1);
        mapimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &mapinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    //read playerimage
    for(i=0;i<PLAYERNUM;i++){
        sprintf(fname,".\\eventparts\\player%d.png",i+1);
        playerimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &playerinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
}

int getmapnum(int x,int y){
    int img_num;
    switch (Map[y][x]){
    case 'A': // 草原
        img_num=0;
        break;
    case 'B': // 物件
        img_num=4;
         break;
    case '|': // 線路(縦)
        img_num=5;
        break; 
    case '-': // 線路(横)
        img_num=6;
        break;
    case 'P': // プラス駅
        img_num=1;
        break;
    case 'M': // マイナス駅
        img_num=2;
        break;
    case 'C': // カード駅
        img_num=3;
        break;
    }
    return img_num;
}

void drawMap(void){
    int x,y;
    int drawx,drawy;
    int img_num;
    for(y=0;y<YMAX;y++){
        for(x=0;x<XMAX;x++){
            drawx = x*IMGSIZE;
            drawy = y*IMGSIZE;
            img_num = getmapnum(x,y);
            PutSprite(mapimg[img_num], drawx, drawy, &mapinfo[img_num]);
        }
    }
}

void drawPlayer(void){
    int i;
    for(i=0;i<PLAYERNUM;i++){
        if(i!=turn){
            PutSprite(playerimg[i], players[i].x,players[i].y, &playerinfo[i]);
        }
    }
    // ターン中のプレイヤーを最上レイヤーに表示
    PutSprite(playerimg[turn], players[turn].x,players[turn].y, &playerinfo[turn]);
}

void InitPlayer(void){
    int i;
    for(i=0;i<PLAYERNUM;i++){
        sprintf(players[i].name,"debug%d",i+1); // for debug
        players[i].x=INITX;
        players[i].y=INITY;
        players[i].money=INITMONEY;
        players[i].isBonby=0;
        players[i].cardnum=0;
    }
}

// detail : 0 全部表示 , else その番号の駅を表示
void dispPlayer(int detail){
    int i;
    if(detail==0){
        for(i=0;i<PLAYERNUM;i++){
            printf("--------------------\n");
            printf("%s社長 (%d,%d)\n",players[i].name,players[i].x,players[i].y);
            printf("所持金 : %d\n",players[i].money);
            printf("ボンビーフラグ : %d\n",players[i].isBonby);
            printf("カード枚数 : %d\n",players[i].cardnum);
            printf("--------------------\n\n");
        }
    }else{
        printf("--------------------\n");
        printf("%s社長 (%d,%d)\n",players[detail-1].name,players[detail-1].x,players[detail-1].y);
        printf("所持金 : %d\n",players[detail-1].money);
        printf("ボンビーフラグ : %d\n",players[detail-1].isBonby);
        printf("カード枚数 : %d\n",players[detail-1].cardnum);
        printf("--------------------\n\n");   
    }
}

void readStation(void){
    FILE *fp;
    int i=0;
    fp=fopen("property.txt","r");
    if(fp==NULL){
        printf("file not found");
        exit(0);
    }else{
        while(fscanf(fp,"%s %d,%d",stations[i].name,&stations[i].x,&stations[i].y)!=EOF){
            stations[i].ismonopoly=0;
            i++;
        }
        fclose(fp);
    }
}

void readProperty(void){
    FILE *fp;
    int i,j;
    char fname[50];
    for(i=0;i<STATIONNUM;i++){
        sprintf(fname,".\\property\\%s.txt",stations[i].name);
        fp=fopen(fname,"r");
        j=0;
        if(fp==NULL){
            printf("file not found");
            exit(0);
        }else{
            while(fscanf(fp,"%s %d,%d",stations[i].plist[j].name,&stations[i].plist[j].price,&stations[i].plist[j].earnings)!=EOF){
            j++;
        }
        stations[i].propertynum=j;
        stations[i].plist[j].holder=0;
        fclose(fp);
    }

    }   
}

void dispStation(int detail){
    int i,j;
    if(detail==0){
        for(i=0;i<STATIONNUM;i++){
            printf("--------------------\n");
            printf("%s駅 (%d,%d)\n",stations[i].name,stations[i].x,stations[i].y);
            printf("独占フラグ : %d   物件数 : %d\n",stations[i].ismonopoly,stations[i].propertynum);
            for(j=0;j<stations[i].propertynum;j++){
                printf("%s %d %d\n",stations[i].plist[j].name,stations[i].plist[j].price,stations[i].plist[j].earnings);
            }
            printf("--------------------\n\n");
        }
        }else{
        printf("--------------------\n");
        printf("%s駅 (%d,%d)\n",stations[detail-1].name,stations[detail-1].x,stations[detail-1].y);
        printf("独占フラグ : %d   物件数 : %d\n",stations[detail-1].ismonopoly,stations[detail-1].propertynum);
        for(j=0;j<stations[detail-1].propertynum;j++){
            printf("%s %d %d\n",stations[detail-1].plist[j].name,stations[detail-1].plist[j].price,stations[detail-1].plist[j].earnings);
        }
        printf("--------------------\n\n");     
    }
}

void Display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    drawMap();
    drawPlayer();
    printf("上 : %d\n ",isMovable(players[0].x,players[0].y,0));
    printf("右 : %d\n ",isMovable(players[0].x,players[0].y,1));
    printf("下 : %d\n ",isMovable(players[0].x,players[0].y,2));
    printf("左 : %d\n ",isMovable(players[0].x,players[0].y,3));

    glFlush();
}