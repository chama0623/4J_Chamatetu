#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "game.h"

// マップ配列
char Map[YMAX][XMAX+1] = { //NULL文字に気を付ける
        "AAAAAAAAAAAAAAA",
        "AAP-B-B---PAAAAA",
        "AA|A|AAAAAAAAAA",
        "AAP-B--P--PAAAA",
        "AAAA|AAAAA|AAAA",
        "AAAAMAABAA|AAAA",
        "AAAA|AA|AA|AAAA",
        "AAAA|AA|AA|AAAA",
        "AP--M--P--BAAAAA",
        "AAAAAAAAAAAAAAA"
};

// 日本語プロトコル
char jpProtcol[JPMAX+SPMAX][3] = {"aa","ii","uu","ee","oo",
                            "ka","ki","ku","ke","ko",
                            "sa","si","su","se","so",
                            "ta","ti","tu","te","to",
                            "na","ni","nu","ne","no",
                            "ha","hi","hu","he","ho",
                            "ma","mi","mu","me","mo",
                            "ya","yu","yo",
                            "ra","ri","ru","re","ro",
                            "wa","wo","nn",
                            "lt","la","lu","lo",
                            "ga","gi","gu","ge","go",
                            "za","zi","zu","ze","zo",
                            "da","di","du","de","do",
                            "ba","bi","bu","be","bo",
                            "pa","pi","pu","pe","po",
                            "0","1","2","3","4","5"
                            ,"6","7","8","9",
                            "ex","mx","ox","px","ms","ps",
                            "mr","tn","xq","xw","xe","xa","xs","xd"
                            };

// プレイヤーカラー
int playercolor[3][3]={{30,144,255},
                       {255,20,147},
                       {255,255,0}};
// プラス駅の色
int pluscolor[3] = {0,0,205};
// マイナス駅の色
int minuscolor[3] = {220,20,60};
// プラス駅の月別乱数表
int plusarray[MAXMONTH][2] = {{400,800},
                               {200,600},
                               {400,600},
                               {1000,2000},
                               {1500,4000},
                               {4000,8000},
                               {6000,12000},
                               {8000,22000},
                               {6000,15000},
                               {1500,6000},
                               {1000,3000},
                               {600,1200}};
// マイナス駅の月別乱数表
int minusarray[MAXMONTH][2] = {{4000,12000},
                               {8000,25000},
                               {4000,8000},
                               {500,1500},
                               {400,1200},
                               {300,1000},
                               {200,600},
                               {100,400},
                               {300,1000},
                               {400,1200},
                               {800,3000},
                               {2000,8000}};
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
    glutTimerFunc(RESHAPETIME, Timer, 0);
}
//キーボード入力管理タイマー 
void keyboardTimer(int t)
{
    glutPostRedisplay();
    keyboardflg=0;
}
// 駅移動管理タイマー
void MoveTimer(int t)
{
    glutPostRedisplay();
    move();
    if((players[turn].x/IMGSIZE != nx)||(players[turn].y/IMGSIZE != ny)){
        glutTimerFunc(MOVETIME, MoveTimer, 0);
    }else if((players[turn].x%IMGSIZE!=0)||(players[turn].y%IMGSIZE!=0)){
        glutTimerFunc(MOVETIME, MoveTimer, 0);
    }else{
        if(dice-recount>0){
            //printf("%d (%d,%d)\n",dice-recount-1,massRecord[dice-recount-1][0],massRecord[dice-recount-1][1]);
            //printf("(%d,%d)\n",players[turn].x/IMGSIZE,players[turn].y/IMGSIZE);
            if((massRecord[dice-recount-1][0]==players[turn].x/IMGSIZE)&&(massRecord[dice-recount-1][1]==players[turn].y/IMGSIZE)){
                recount++;
            }else{
                recount--;
            }
        }else{
            recount--;
        }
        keyboardflgformove=1;
    }
}
// サイコロタイマー
void DiceTimer(int t)
{
    glutPostRedisplay();
    dice=rand()%DICEMAX;
    recount=dice;
    if(diceflg==1){
        glutTimerFunc(DICETIME, DiceTimer, 0);
    }
}
// ランダムタイマー
void RandTimer(int t)
{
    if(pm==0){ //プラス駅
        range = plusarray[month-1][1]-plusarray[month-1][0];
        randreturn = plusarray[month-1][0] + rand()%range;
    }else{
        range = minusarray[month-1][1]-minusarray[month-1][0];
        randreturn = minusarray[month-1][0] + rand()%range;
    }
    glutPostRedisplay();
    if(randflg==1){
        glutTimerFunc(RANDTIME, RandTimer, 0);
    }
}

//
//  num番のPNG画像を座標(x,y)に表示する
void PutSprite(int num, int x, int y, pngInfo *info,double scale)
{
    int w, h;  //  テクスチャの幅と高さ

    w = info->Width*scale;   //  テクスチャの幅と高さを取得する
    h = info->Height*scale;

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
// 行先が壁かどうか判定
// 1 : 壁
// 0 : 壁でない
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
int isMovable(int x,int y){
    int xa=x/IMGSIZE;
    int ya=y/IMGSIZE;
    if(direction==0){ // 上
        if(isWall(xa,ya-1)){
            return 0;
        }
    }else if(direction==1){ //右
        if(isWall(xa+1,ya)){
            return 0;
        }    
    }else if(direction==2){ //下
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
// 移動処理
void move(){
    if(direction==0){
        players[turn].y-=MOVESIZE;
    }
    if(direction==1){
        players[turn].x+=MOVESIZE;
    }
    if(direction==2){
        players[turn].y+=MOVESIZE;
    }
    if(direction==3){
        players[turn].x-=MOVESIZE;
    }
}
// 次の駅を取得
void nextStation(int x,int y){
    int xa=x/IMGSIZE;
    int ya=y/IMGSIZE;
    //printf("Cal Next Station(%d,%d)\n",xa,ya);
    if(direction==0){
        while(1){
            ya--;
            if(getmapnum(xa,ya)!=4){
                break;
            }
        }
    }

    if(direction==1){
        while(1){
            xa++;
            if(getmapnum(xa,ya)!=5){
                break;
            }
        }
    }

    if(direction==2){
        while(1){
            ya++;
            if(getmapnum(xa,ya)!=4){
                break;
            }
        }
    }

    if(direction==3){
        while(1){
            xa--;
            if(getmapnum(xa,ya)!=5){
                break;
            }
        }
    }
    nx=xa;
    ny=ya;
}
// 物件が購入できるか取得
// 1 : 取得可能
// 0 : 取得不可能
int ispurchase(int where,int id){
    int flg=1;
    // 既に誰かの物件
    if(stations[where].plist[id].holder!=0){
        flg=0;
    }
    // 値段が足りない
    if(players[turn].money < stations[where].plist[id].price){ 
        flg=0;
    }
    return flg;
}
// キーボード割り込み処理
void keyboard(unsigned char key,int x,int y){
    int r = 623;
    int i,count;

    if(turnstatus==4){
        if(key=='q'){
            turnstatus=100;
        }

        if(key=='s'){
            if(selectpos<propertynum){
                selectpos+=1;
            }
        }        
        
        if(key=='w'){
            if(selectpos>=1){
                selectpos-=1;
            }
        }
        if(key=='e'){
            if(selectpos!=0){
                //printf("%s\n",stations[wherestation].plist[selectpos-1].name);
                if(ispurchase(wherestation,selectpos-1)==1){
                    players[turn].assets+=stations[wherestation].plist[selectpos-1].price;
                    players[turn].money-=stations[wherestation].plist[selectpos-1].price;
                    stations[wherestation].plist[selectpos-1].holder=turn+1;
                    count=0;
                    for(i=0;i<stations[wherestation].propertynum;i++){
                        if(stations[wherestation].plist[i].holder==turn+1){
                            count++;
                        }
                    }
                    if(count==stations[wherestation].propertynum){
                        stations[wherestation].ismonopoly=turn+1;
                    }
                    }
            }
        }
    }

    if(keyboardflg==0){
    if(turnstatus==0){
        if(key=='e'){
            turnstatus=1;
        }
    }else if(turnstatus==1){
        if(key=='e'){
            if(diceflg==1){
                diceflg=2;
            }else if(diceflg==2){
                diceflg=3;
            }
        }
    }else if((turnstatus==2)&&(keyboardflgformove==1)){
    if(key=='w'){
        direction=0;
    }else if(key=='d'){
        direction=1;
    }else if(key=='s'){
        direction=2;
    }else if(key=='a'){
        direction=3;
    }else {
        direction=623;
    }
    if(direction!=623){
        r=isMovable(players[turn].x,players[turn].y);
        if(r){
            massRecord[dice-recount][0]=players[turn].x/IMGSIZE;
            massRecord[dice-recount][1]=players[turn].y/IMGSIZE;
            nextStation(players[turn].x,players[turn].y);
            keyboardflgformove=2;
            glutTimerFunc(MOVETIME, MoveTimer, 0);
        }
    }
    }else if((turnstatus==5)&&(randflg==1)){
        if(key=='e'){
            randflg=2;
        }        
    }else if((turnstatus==5)&&(randflg==2)){
        if(key=='e'){
            randflg=3;
        }        
    }else if((turnstatus==6)&&(randflg==1)){
        if(key=='e'){
            randflg=2;
        }        
    }else if((turnstatus==6)&&(randflg==2)){
        if(key=='e'){
            randflg=3;
        }        
    }  
    keyboardflg=1;
    glutTimerFunc(500, keyboardTimer, 0);
    }
}
// 画像読み込み
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

    // read diceimage
    for(i=0;i<DICEMAX;i++){
        sprintf(fname,".\\dice\\dice%d.png",i+1);
        diceimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &diceinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    //read Hiragana black
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\h%sblack.png",jpProtcol[i]);
        hblackimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hblackinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }

    //read Hiragana red
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\h%sred.png",jpProtcol[i]);
        hredimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hredinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    //read Katakana black
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\k%sblack.png",jpProtcol[i]);
        kblackimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &kblackinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    //read Katakana red
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\k%sred.png",jpProtcol[i]);
        kredimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &kredinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    //read Special Str red
    for(i=JPMAX;i<JPMAX+SPMAX;i++){
        sprintf(fname,".\\charimg\\%sred.png",jpProtcol[i]);
        hredimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hredinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    //read Special Str black
    for(i=JPMAX;i<JPMAX+SPMAX;i++){
        sprintf(fname,".\\charimg\\%sblack.png",jpProtcol[i]);
        hblackimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hblackinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }

}
// マップの画像番号を取得
int getmapnum(int x,int y){
    int img_num;
    switch (Map[y][x]){
    case 'A': // 草原
        img_num=0;
        break;
    case 'B': // 物件
        img_num=3;
         break;
    case '|': // 線路(縦)
        img_num=4;
        break; 
    case '-': // 線路(横)
        img_num=5;
        break;
    case 'P': // プラス駅
        img_num=1;
        break;
    case 'M': // マイナス駅
        img_num=2;
        break;
    }
    return img_num;
}
// マップを描画
void drawMap(void){
    int x,y;
    int drawx,drawy;
    int img_num;
    for(y=0;y<YMAX;y++){
        for(x=0;x<XMAX;x++){
            drawx = x*IMGSIZE;
            drawy = y*IMGSIZE;
            img_num = getmapnum(x,y);
            PutSprite(mapimg[img_num], drawx, drawy, &mapinfo[img_num],1);
        }
    }
}
// プレイヤーを描画
// 最上面にターン中のプレイヤーを描画
void drawPlayer(void){
    int i;
    for(i=0;i<PLAYERNUM;i++){
        if(i!=turn){
            PutSprite(playerimg[i], players[i].x,players[i].y, &playerinfo[i],1);
        }
    }
    // ターン中のプレイヤーを最上レイヤーに表示
    PutSprite(playerimg[turn], players[turn].x,players[turn].y, &playerinfo[turn],1);
}

// 1文字の日本語を表示
// int kh : 0,Hiragana 1,Katakana
// int color 0,black 1,red
void drawChar(int num,int kh,int color,int x,int y,double scale){
    if(kh==0){
        if(color==0){ // hiragana black
            PutSprite(hblackimg[num], x, y, &hblackinfo[num],scale);
        }else{ //hiragana red
            PutSprite(hredimg[num], x, y, &hredinfo[num],scale);
            }
        }else{ 
        if(color==0){ // katakana black
            PutSprite(kblackimg[num], x, y, &kblackinfo[num],scale);
        }else{ // katakana red
            PutSprite(kredimg[num], x, y, &kredinfo[num],scale);
        }
    }
}
// 引数stringの文字列を表示
void drawString(char *string,int color,int xo,int yo,double scale){
    int i,j;
    int len = strlen(string)-1;
    int x=xo;
    int y=yo;
    int numberflg;
    int kh=0;
    for(i=0;i<len;i++){
        numberflg=-1;
        switch (string[i])
        {
        case '0': numberflg=0; break;
        case '1': numberflg=1; break;
        case '2': numberflg=2; break;
        case '3': numberflg=3; break;
        case '4': numberflg=4; break;
        case '5': numberflg=5; break;
        case '6': numberflg=6; break;
        case '7': numberflg=7; break;
        case '8': numberflg=8; break;
        case '9': numberflg=9; break;
        default : numberflg=-1; break;
        }
        if(numberflg>=0){
            drawChar(JPMAX+numberflg,0,color,x,y,scale);
        }else{
            for(j=0;j<JPMAX;j++){
                if((jpProtcol[j][0]==string[i])&&(jpProtcol[j][1]==string[i+1])){
                    drawChar(j,kh,color,x,y,scale);
                    break;
                }
            }       
            for(j=JPMAX+10;j<JPMAX+SPMAX;j++){
                if((jpProtcol[j][0]==string[i])&&(jpProtcol[j][1]==string[i+1])){
                    drawChar(j,kh,color,x,y,scale);
                    break;
                }
            }
            if((string[i]=='l')&&(string[i+1]=='l')){
                kh=1-kh;
                x-=IMGSIZE*scale;
            }
            if((string[i]=='x')&&(string[i+1]=='x')){
                x=xo;
                x-=IMGSIZE*scale;
                y+=IMGSIZE*scale;
            }     
            i++;
        }
        x+=IMGSIZE*scale;
        if(x+IMGSIZE*scale>InitWidth-22){
            x=xo;
            y+=IMGSIZE*scale;
        }
    }
}
// プレイヤー構造体を初期化
void InitPlayer(void){
    int i;
    for(i=0;i<PLAYERNUM;i++){
        sprintf(players[i].name,"debaltgu%d",i+1); // for debug
        players[i].x=INITX;
        players[i].y=INITY;
        players[i].money=INITMONEY;
        players[i].assets=0;
    }
}
// デバッグ用関数 
// プレイヤー構造体を表示
// detail : 0 全部表示 , else その番号の駅を表示
void dispPlayer(int detail){
    int i;
    if(detail==0){
        for(i=0;i<PLAYERNUM;i++){
            printf("--------------------\n");
            printf("%s社長 (%d,%d)\n",players[i].name,players[i].x,players[i].y);
            printf("\n");
            printf("所持金 : %d\n",players[i].money);
            printf("総資産 : %d\n",players[i].assets);
            printf("--------------------\n\n");
        }
    }else{
        printf("--------------------\n");
        printf("%s社長 (%d,%d)\n",players[detail-1].name,players[detail-1].x,players[detail-1].y);
        printf("\n");
        printf("所持金 : %d\n",players[detail-1].money);
        printf("総資産 : %d\n",players[detail-1].assets);
        printf("--------------------\n\n");   
    }
}
// ファイルから駅情報を取得
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
// ファイルから物件情報を取得
void readProperty(void){
    FILE *fp;
    int i,j;
    char fname[80];
    for(i=0;i<STATIONNUM;i++){
        sprintf(fname,".\\property\\%s.txt",stations[i].name);
        fp=fopen(fname,"r");
        j=0;
        if(fp==NULL){
            printf("file not found in %s",stations[i].name);
            exit(0);
        }else{
            while(fscanf(fp,"%s %d,%d,%d",stations[i].plist[j].name,&stations[i].plist[j].price,&stations[i].plist[j].earnings,&stations[i].plist[j].nameAttribute)!=EOF){
            j++;
        }
        stations[i].propertynum=j;
        stations[i].plist[j].holder=0;
        fclose(fp);
    }

    }   
}
// デバッグ用関数
// 駅情報を表示
void dispStation(int detail){
    int i,j;
    if(detail==0){
        for(i=0;i<STATIONNUM;i++){
            printf("--------------------\n");
            printf("%s駅 (%d,%d)\n",stations[i].name,stations[i].x,stations[i].y);
            printf("独占フラグ : %d   物件数 : %d\n",stations[i].ismonopoly,stations[i].propertynum);
            for(j=0;j<stations[i].propertynum;j++){
                printf("%s %d %d %d %d\n",stations[i].plist[j].name,stations[i].plist[j].price,stations[i].plist[j].earnings,stations[i].plist[j].nameAttribute,stations[i].plist[j].holder);
            }
            printf("--------------------\n\n");
        }
        }else{
        printf("--------------------\n");
        printf("%s駅 (%d,%d)\n",stations[detail-1].name,stations[detail-1].x,stations[detail-1].y);
        printf("独占フラグ : %d   物件数 : %d\n",stations[detail-1].ismonopoly,stations[detail-1].propertynum);
        for(j=0;j<stations[detail-1].propertynum;j++){
            printf("%s %d %d %d %d\n",stations[detail-1].plist[j].name,stations[detail-1].plist[j].price,stations[detail-1].plist[j].earnings,stations[detail-1].plist[j].nameAttribute,stations[detail-1].plist[j].holder);
        }
        printf("--------------------\n\n");     
    }
}
// デバッグ用関数
// 残り移動可能マス数を表示
void dispmassRecord(void){
    int i;
    printf("\n---dispmass Record---\n");
    for(i=0;i<=dice-recount;i++){
        printf("%d (%d,%d)\n",i,massRecord[i][0],massRecord[i][1]);
    }
    printf("----------\n");
}
// ダイアログを画面に描画
void drawDialog(int x,int y,int width,int height,int red,int blue,int green){
    glColor3ub(red,blue,green);
    glBegin(GL_QUADS);
    glVertex2i(x,y);
    glVertex2i(x,y+height);
    glVertex2i(x+width,y+height);
    glVertex2i(x+width,y);
    glEnd();

    glColor3ub(139,69,19);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x,y);
    glVertex2i(x,y+height);
    glVertex2i(x+width,y+height);
    glVertex2i(x+width,y);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex2i(x+5,y+5);
    glVertex2i(x+5,y+height-5);
    glVertex2i(x+width-5,y+height-5);
    glVertex2i(x+width-5,y+5);
    glEnd();
}
// 四角形を描画
void drawQUAD(int x,int y,int width,int height){
    glBegin(GL_QUADS);
    glVertex2i(x,y);
    glVertex2i(x,y+height);
    glVertex2i(x+width,y+height);
    glVertex2i(x+width,y);
    glEnd();       
}
// 数字を描画
void drawMoney(int money,int x,int y,int color,double scale){
    int oku,man;
    char fname[40];
    oku = money/10000;
    man = money%10000;
    if(money>=0){
        if(oku!=0){
            if(man!=0){
                sprintf(fname,"%dox%dmxex",oku,man);
            }else{
                sprintf(fname,"%doxex",oku);
            }
        }else{
            sprintf(fname,"%dmxex",man);
        }
    }else{
        oku*=-1;
        man*=-1;
        if(oku!=0){
            if(man!=0){
                sprintf(fname,"ms%dox%dmxex",oku,man);
            }else{
                sprintf(fname,"ms%doxex",oku);
            }
        }else{
            sprintf(fname,"ms%dmxex",man);
        }        
    }
    drawString(fname,color,x,y,scale);
}

// テキスト表示
void drawText(char *string,int y,int color){
    drawDialog(11,y,InitWidth-22,42,255,245,238);
    drawString(string,color,16,y+5,0.5);
}

// 物件情報を描画
void drawStation(int x,int y){
    int i,j;
    char fname[100];
    int holder;
    int color;
     for(i=0;i<STATIONNUM;i++){
         if((stations[i].x==x)&&(stations[i].y==y)){
            propertynum = stations[i].propertynum; 
            wherestation = i;
            // 駅名表示
            drawDialog(11,11,InitWidth-22,42,255,245,238);
            sprintf(fname,"%seeki",stations[i].name);
            drawString(fname,0,16,16,1);

            // 所持金表示
            drawDialog(11,61,InitWidth-22,34,playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
            drawString("silozikinn",0,16,61+8,0.5);
            drawMoney(players[turn].money,2*InitWidth/4,61+8,0,0.5);
            
            // 物件表示
            drawDialog(11,103,InitWidth-22,11+17*stations[i].propertynum,255,245,238);
            for(j=0;j<stations[i].propertynum;j++){
                // earings
                sprintf(fname,"%dpx",stations[i].plist[j].earnings);
                holder = stations[i].plist[j].holder;

                if(holder!=0){
                    glColor3ub(playercolor[holder-1][0],playercolor[holder-1][1],playercolor[holder-1][2]);
                    drawQUAD(16,108+j*17,InitWidth-32,17);
                }

                if(selectpos-1 == j){
                    glColor3ub(255,0,0);
                    drawQUAD(16,108+j*17,InitWidth-32,17);                 
                }
                if(ispurchase(wherestation,j)){
                    color=0;
                }else{
                    color=1;
                }
                drawString(stations[i].plist[j].name,color,18,42+11+50+7+17*j,0.5);
                drawMoney(stations[i].plist[j].price,InitWidth/2,42+11+50+7+17*j,color,0.5);
                drawString(fname,color,3*InitWidth/4,42+11+50+7+17*j,0.5);
            }
        if(stations[i].ismonopoly!=0){
            sprintf(fname,"%ssilatilouunodokusenndesumr",players[stations[i].ismonopoly-1].name);
            drawText(fname,191,0);
            sprintf(fname,"xqsssiluuurilouuxxxesskouuniluuu");
            drawText(fname,241,0);
        }else{
                sprintf(fname,"xqsssiluuurilouuxxxesskouuniluuu");
                drawText(fname,225,0);
        }
        }
     }
}

int debtprocess(void){
    int i,j;
    int count=0;
    int nearest;
    int nearests,nearestid;
    for(i=0;i<STATIONNUM;i++){
        for(j=0;j<stations[i].propertynum;j++){
            if(turn+1==stations[i].plist[j].holder){
                count++;
                //printf("%s\n",stations[i].plist[j].name);
            }
        }
    }
    if(count!=0){
        // 借金が資産より大きいときすべて売却
        if(-players[turn].money>=players[turn].assets){
            printf("全ての物件を売却しました\n");
            count=-623;
            for(i=0;i<STATIONNUM;i++){
                if(stations[i].ismonopoly==turn+1){
                    stations[i].ismonopoly=0;
                }
                for(j=0;j<stations[i].propertynum;j++){
                if(turn+1==stations[i].plist[j].holder){
                    stations[i].plist[j].holder=0;
                    players[turn].money+=stations[i].plist[j].price;
                    players[turn].assets-=stations[i].plist[j].price;
                    }
                }
            }            
        }else{ // 借金が資産より小さいとき
        count=0;
            // 独占でない物件から売却
            // 借金より価格が高く,借金に一番値段が近い物件を売却
            while(1){
                nearest = 9999999;
                nearests =-1;
                nearestid = -1;
                for(i=0;i<STATIONNUM;i++){
                    if(stations[i].ismonopoly!=turn+1){ // 独占でないなら
                        for(j=0;j<stations[i].propertynum;j++){
                            if(turn+1==stations[i].plist[j].holder){
                                // 借金より資産が大きいとき
                                if(stations[i].plist[j].price>=-players[turn].money){
                                    if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                        nearest = abs(-players[turn].money-stations[i].plist[j].price);
                                        nearests = i;
                                        nearestid = j;       
                                    }
                                }
                            }
                        }
                    }
                }  
                if(nearests!=-1){
                    printf("%sを売却しました\n",stations[nearests].plist[nearestid].name);
                    count++;
                    stations[nearests].plist[nearestid].holder=0;
                    players[turn].money+=stations[nearests].plist[nearestid].price;
                    players[turn].assets-=stations[nearests].plist[nearestid].price;
                    if(players[turn].money>=0){
                        break;
                    }
                }else{
                    break;
                }
            }
            // 借金より価格が低く,借金に一番値段が近い物件を売却
            if(players[turn].money<0){
            while(1){
                nearest = 9999999;
                nearests =-1;
                nearestid = -1;
                for(i=0;i<STATIONNUM;i++){
                    if(stations[i].ismonopoly!=turn+1){ // 独占でないなら
                        for(j=0;j<stations[i].propertynum;j++){
                            if(turn+1==stations[i].plist[j].holder){
                                // 借金より資産が小さいとき
                                    if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                        nearest = abs(-players[turn].money-stations[i].plist[j].price);   
                                        nearests = i;
                                        nearestid = j;       
                                    }
                            }
                        }
                    }
                }  
                if(nearests!=-1){
                    printf("%sを売却しました\n",stations[nearests].plist[nearestid].name);
                    count++;
                    stations[nearests].plist[nearestid].holder=0;
                    players[turn].money+=stations[nearests].plist[nearestid].price;
                    players[turn].assets-=stations[nearests].plist[nearestid].price;
                    if(players[turn].money>=0){
                        break;
                    }
                }else{
                    break;
                }
            }
            }
            // 独占の物件を売却
            if(players[turn].money<0){
            while(1){
                nearest = 9999999;
                nearests =-1;
                nearestid = -1;
                for(i=0;i<STATIONNUM;i++){
                        for(j=0;j<stations[i].propertynum;j++){
                            if(turn+1==stations[i].plist[j].holder){
                                // 借金より資産が大きいとき
                                if(stations[i].plist[j].price>=-players[turn].money){
                                    if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                        nearest = abs(-players[turn].money-stations[i].plist[j].price);  
                                        nearests = i;
                                        nearestid = j;       
                                    }
                                }
                            }
                        }
                }  
                if(nearests!=-1){
                    printf("%sを売却しました\n",stations[nearests].plist[nearestid].name);
                    printf("独占が崩れました\n");
                    count++;
                    stations[nearests].plist[nearestid].holder=0;
                    stations[nearests].ismonopoly=0;
                    players[turn].money+=stations[nearests].plist[nearestid].price;
                    players[turn].assets-=stations[nearests].plist[nearestid].price;
                    if(players[turn].money>=0){
                        break;
                    }
                }else{
                    break;
                }
            }
            }
            if(players[turn].money<0){
            while(1){
                nearest = 9999999;
                nearests =-1;
                nearestid = -1;
                for(i=0;i<STATIONNUM;i++){
                        for(j=0;j<stations[i].propertynum;j++){
                            if(turn+1==stations[i].plist[j].holder){
                                // 借金より資産が小さいとき
                                    if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                        nearest = abs(-players[turn].money-stations[i].plist[j].price);  
                                        nearests = i;
                                        nearestid = j;       
                                    }
                            }
                        }
                }  
                if(nearests!=-1){
                    printf("%sを売却しました\n",stations[nearests].plist[nearestid].name);
                    printf("独占が崩れました\n");
                    count++;
                    stations[nearests].plist[nearestid].holder=0;
                    stations[nearests].ismonopoly=0;
                    players[turn].money+=stations[nearests].plist[nearestid].price;
                    players[turn].assets-=stations[nearests].plist[nearestid].price;
                    if(players[turn].money>=0){
                        break;
                    }
                }else{
                    break;
                }
            }
            }
            }
        }
    return count;
}

// ディスプレイ関数
void Display(void){
    int stopstation;
    char fname[100];
    glClear(GL_COLOR_BUFFER_BIT);
    drawMap();
    drawPlayer();
    
    //printf("turnstatus = %d\n",turnstatus);
    // ターン開始のとき
    if(turnstatus==0){
        selectpos=-623;
        //printf("%s社長のターンです\n",players[turn].name);
        //printf("%d年%d月目\n",year,month);

        // 所持金表示
        drawDialog(11,11,InitWidth-22,34+16,playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
        sprintf(fname,"%dnennmess%dgatu",year,month);
        drawString(fname,0,16,11+8,0.5);
        drawMoney(players[turn].money,2*InitWidth/4,11+8+16,0,0.5);
        sprintf(fname,"%ssilatilouussssss",players[turn].name);
        drawString(fname,0,16,11+8+16,0.5);

        sprintf(fname,"%ssilatilouunobanndesumrxxxedellsaiikorollwomawasitekudasaiimr",players[turn].name);
        drawText(fname,225,0);
    }

    //サイコロをふるとき
    if(turnstatus==1){
        if(diceflg==0){
            diceflg=1;
            dispPlayer(1);
            //dispStation(0);
            glutTimerFunc(DICETIME, DiceTimer, 0);
        }
        if(diceflg==1){
            PutSprite(diceimg[dice], 416, 32, &diceinfo[dice],1);
            sprintf(fname,"xewooositekudasaiimr");
            drawText(fname,225,0);
        }
        if(diceflg==2){
            PutSprite(diceimg[dice], 416, 32, &diceinfo[dice],1);
            sprintf(fname,"%ddesumrxewooositekudasaiimr",recount+1);
            drawText(fname,225,0);
        }
        if(diceflg==3){
            //printf("dice = %d\n",dice);
            turnstatus=2;
        }
    }

    // マス移動のとき
    if(turnstatus==2){
        sprintf(fname,"aato%dllmasullssssxwssxassxsssxddeiidouudekimasumr",recount+1);
        drawText(fname,225,0);
        if(keyboardflgformove==0){
            keyboardflgformove=1;
        }else{
            //printf("残り移動可能マス数 = %d\n",recount);
            if(recount==-1){
                keyboardflgformove=0;
                direction=-1;
                turnstatus=3;
            }
        }
    }
    
    // 駅に止まったとき
    if(turnstatus==3){
        stopstation = getmapnum(players[turn].x/IMGSIZE,players[turn].y/IMGSIZE);
        if(stopstation==3){ // 物件にとまったとき
            turnstatus=4;
        }else if(stopstation==1){ // プラスマス
            turnstatus=5;
        }else if(stopstation==2){ //マイナスマス
            turnstatus=6;
        }
    }
    
    // 物件購入操作
    if(turnstatus==4){
        if(selectpos==-623){
            selectpos=1;
        }
        drawStation(players[turn].x/IMGSIZE,players[turn].y/IMGSIZE);
        //printf("selectpos = %d\n",selectpos);
    }

    //プラスマスに止まったとき
    if(turnstatus==5){
        if(randflg==0){ 
            pm=0;
            randflg=1;
            calflg=0;
            glutTimerFunc(RANDTIME, RandTimer, 0);
        }   
        if(randflg==1){
            glColor3ub(pluscolor[0],pluscolor[1],pluscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
        }
        if(randflg==2){ 
            if(calflg==0){
                range = plusarray[month-1][1]-plusarray[month-1][0];
                randreturn = plusarray[month-1][0] + rand()%range;
                players[turn].money+=randreturn;
                dispPlayer(turn+1);
                drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
                drawMoney(randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
                calflg=1;
            }
            glColor3ub(pluscolor[0],pluscolor[1],pluscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
        }   
        if(randflg==3){
            turnstatus=100;
        }
    }

    //マイナスマスに止まったとき
    if(turnstatus==6){
        if(randflg==0){ 
            pm=1;
            randflg=1;
            calflg=0;
            glutTimerFunc(RANDTIME, RandTimer, 0);
        }   
        if(randflg==1){
            glColor3ub(minuscolor[0],minuscolor[1],minuscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(-randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
        }
        if(randflg==2){ 
            if(calflg==0){
                range = minusarray[month-1][1]-minusarray[month-1][0];
                randreturn = minusarray[month-1][0] + rand()%range;
                players[turn].money-=randreturn;
                dispPlayer(turn+1);
                drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
                drawMoney(-randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
                if(players[turn].money<0){
                    sold=debtprocess();
                }
                calflg=1;
            }
            glColor3ub(minuscolor[0],minuscolor[1],minuscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(-randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
            if(sold!=-1){
                if(sold==-623){
                    sprintf(fname,"silaltkinnwoseoolttesimaiimasitamrxxsubetenobultkennwobaiikilakusimasitamr");
                    drawText(fname,225,0);
                }else{
                    sprintf(fname,"silaltkinnwoseoolttesimaiimasitamrxx%dkennnobultkennwobaiikilakusimasitamr",sold);
                    drawText(fname,225,0);
                }
            }
        }   
        if(randflg==3){
            turnstatus=100;
        }
    }

    //ターン終了処理
    if(turnstatus==100){
        selectpos=-623;
        sold=-1;
        keyboardflgformove=0;
        turn++;
        if(turn==3){
            month+=1;
            if(month==13){
                year+=1;
                month=1;
            }
            turn =0;
        }
        keyboardflgformove=0;
        direction=-1;
        randflg=0;
        diceflg=0;
        turnstatus=0;
    }

    glFlush();
}