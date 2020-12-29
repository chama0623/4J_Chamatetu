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
// 収益計算用
int shueki[3];

// ウィンドウサイズ変更時に座標を再設定
void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glScaled(1, -1, 1);
    glTranslated(0, -h, 0);
}

// 画面更新用タイマー
void Timer(int t)
{
    glutPostRedisplay();
    glutTimerFunc(RESHAPETIME, Timer, 0);
}

// 乱数を一定時間ごとに生成するタイマー
void RandTimer(int t)
{
    glutPostRedisplay();
    dummyresult = rand()%range;
    if(randflg==1){
        glutTimerFunc(RANDTIME, RandTimer, 0);
    }
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
        if(randresult-recount>0){
            if((massRecord[randresult-recount-1][0]==players[turn].x/IMGSIZE)&&(massRecord[randresult-recount-1][1]==players[turn].y/IMGSIZE)){
                recount++;
            }else{
                recount--;
            }
        }else{
            recount--;
        }
        keyboardflg=0;
    }
}

// eを押したか判定
int isE(unsigned char key){
    if(key=='e'){
        return 1;
    }else{
        return 0;
    }
}

//キーボード入力管理タイマー 
void keyboardTimer(int t)
{
    glutPostRedisplay();
    keyboardflg=0;
}

// (x,y)に大きさscaleの画像を表示
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
int ispurchase(int id){
    int flg=1;
    // 既に誰かの物件
    if(stations[stid].plist[id].holder!=0){
        flg=0;
    }
    // 値段が足りない
    if(players[turn].money < stations[stid].plist[id].price){ 
        flg=0;
    }
    return flg;
}

//物件購入処理
void purchase(int id){
    int i;
    int monopolyCheck=0;
    players[turn].assets+=stations[stid].plist[selectpos].price;
    players[turn].money-=stations[stid].plist[selectpos].price;
    stations[stid].plist[selectpos].holder=turn+1;    
    for(i=0;i<propertynum;i++){
        if(stations[stid].plist[i].holder==turn+1){
            monopolyCheck++;
        }
    }    
    if(monopolyCheck==propertynum){
        stations[stid].ismonopoly=turn+1;
    }    
}

// 画像読み込み
void readImg(void){
    int i;
    char fname[100];
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
    char fname[100];
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
    int len = strlen(string);
    int x=xo;
    int y=yo;
    int flg;
    int kh=0;
    for(i=0;i<len;i++){
        flg=string[i]-'0';
        if((flg>=0)&&(flg<=9)){ // 数字描画
            drawChar(JPMAX+flg,0,color,x,y,scale);
            flg=1;
        }else{
            for(j=0;j<JPMAX;j++){ //日本語描画
                if((jpProtcol[j][0]==string[i])&&(jpProtcol[j][1]==string[i+1])){
                    drawChar(j,kh,color,x,y,scale);
                    break;
                }
            }       
            for(j=JPMAX+10;j<JPMAX+SPMAX;j++){ //特殊文字描画
                if((jpProtcol[j][0]==string[i])&&(jpProtcol[j][1]==string[i+1])){
                    drawChar(j,kh,color,x,y,scale);
                    break;
                }
            }
            flg=1;
            if((string[i]=='l')&&(string[i+1]=='l')){ //ひらがな/カタカナ切り替え
                kh=1-kh;
                flg=0;
            }
            if((string[i]=='x')&&(string[i+1]=='x')){ // 改行
                x=xo;
                flg=0;
                y+=IMGSIZE*scale;
            }     
            i++;
        }
        if(flg==1){
            x+=IMGSIZE*scale;
            if(x>InitWidth-22){
                x=xo;
                y+=IMGSIZE*scale;
            }
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

// ダイアログを画面に描画
void drawDialog(int x,int y,int width,int height){
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

// 数字を描画
void drawMoney(int money,int x,int y,int color,double scale){
    char fname[100];
    int oku,man;
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

// 四角形を描画
void drawQUAD(int x,int y,int width,int height){
    glBegin(GL_QUADS);
    glVertex2i(x,y);
    glVertex2i(x,y+height);
    glVertex2i(x+width,y+height);
    glVertex2i(x+width,y);
    glEnd();       
}

// テキスト表示
void drawText(char *string,int x,int y,int width,int height,int color){
    glColor3ub(255,245,238);
    drawDialog(x,y,width,height);
    drawString(string,color,x+5,y+5,0.5);
}

// 物件情報を描画
void drawStation(void){
    char fname[100];
    int i,j;
    int holder;
    int color;
    int transx = players[turn].x/IMGSIZE;
    int transy = players[turn].y/IMGSIZE;
    // どの駅か識別
    for(i=0;i<STATIONNUM;i++){
         if((stations[i].x==transx)&&(stations[i].y==transy)){
            propertynum = stations[i].propertynum; 
            stid = i;
         }
     }
    
    // 駅名表示
    glColor3ub(255,245,238);
    drawDialog(11,11,InitWidth-22,42);
    sprintf(fname,"%seeki",stations[stid].name);
    drawString(fname,0,16,16,1);
    
    // 所持金表示
    glColor3ub(playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
    drawDialog(11,61,InitWidth-22,34);
    drawString("silozikinn",0,16,61+8,0.5);
    drawMoney(players[turn].money,2*InitWidth/4,61+8,0,0.5);
    
    // 物件表示
    glColor3ub(255,245,238);
    drawDialog(11,103,InitWidth-22,11+17*stations[stid].propertynum);
    for(j=0;j<propertynum;j++){
        
        // 収益率を文字列に変換
        sprintf(fname,"%dpx",stations[stid].plist[j].earnings);
        
        // 物件の所有者がいるとき,所有者カラーで物件を囲む
        holder = stations[stid].plist[j].holder;
        if(holder!=0){
            glColor3ub(playercolor[holder-1][0],playercolor[holder-1][1],playercolor[holder-1][2]);
            drawQUAD(16,108+j*17,InitWidth-32,17);
        }

        // セレクトポジションを表示
        if(selectpos == j){
            glColor3ub(255,0,0);
            drawQUAD(16,108+j*17,InitWidth-32,17);                 
        }
        // 物件の表示色設定
        if(ispurchase(j)){
            color=0;
        }else{
            color=1;
        }
        // 物件を表示
        drawString(stations[stid].plist[j].name,color,18,42+11+50+7+17*j,0.5);
        drawMoney(stations[stid].plist[j].price,InitWidth/2,42+11+50+7+17*j,color,0.5);
        drawString(fname,color,3*InitWidth/4,42+11+50+7+17*j,0.5);
    }
    // 独占ダイアログ表示
    if(stations[stid].ismonopoly!=0){
        sprintf(fname,"%ssilatilouunodokusenndesumr",players[stations[stid].ismonopoly-1].name);
        drawText(fname,11,210,InitWidth-22,42,0);
        sprintf(fname,"xqsssiluuurilouuxxxesskouuniluuu");
        drawText(fname,11,260,InitWidth-22,42,0);
    }else{ // 操作ダイアログ表示
            sprintf(fname,"xqsssiluuurilouuxxxesskouuniluuu");
            drawText(fname,11,225,InitWidth-22,42,0);
    }
}

// 決算処理
void kessan(void){
    int i,j,k;
    int dokusen;
    for(i=0;i<PLAYERNUM;i++){
        shueki[i]=0;
        for(j=0;j<STATIONNUM;j++){
            if(stations[j].ismonopoly==i+1){
                dokusen=2;
            }else{
                dokusen=1;
            }
            for(k=0;k<stations[j].propertynum;k++){
                if(stations[j].plist[k].holder==i+1){
                    shueki[i]+=dokusen*stations[j].plist[k].price*stations[j].plist[k].earnings/100;
                }
            }
        }
    }
}

// キーボード入力管理
void keyboard(unsigned char key,int x,int y){
    int locktime =500;
if(keyboardflg==0){
    if(turnstatus==2){ // 目的地設定
        if(isE(key)){
            inflg++;
        }
    }else if(turnstatus==3){ // ターンのはじめ
        if(isE(key)){
            inflg++;
        }
    }else if(turnstatus==4){ // サイコロをふるとき
        if(isE(key)){
            inflg++;
        }
    }else if(turnstatus==5){ //移動
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
            if(isMovable(players[turn].x,players[turn].y)){
                massRecord[randresult-recount][0]=players[turn].x/IMGSIZE;
                massRecord[randresult-recount][1]=players[turn].y/IMGSIZE;
                nextStation(players[turn].x,players[turn].y);
                keyboardflg=1;
                glutTimerFunc(MOVETIME, MoveTimer, 0);
            }
        }        
    }else if(turnstatus==7){ // 物件購入
        locktime=200;
        if(key=='s'){ //ポジションを下へ
            if(selectpos<propertynum-1){
                selectpos+=1;
            }            
        }else if(key=='w'){ // ポジションを上へ
            if(selectpos>=1){
                selectpos-=1;
            }            
        }else if(isE(key)){
            if(ispurchase(selectpos)){
                purchase(selectpos);
            }
        }else if(key=='q'){
                inflg++;
        }
    }else if(turnstatus==8){
        if(isE(key)){
            inflg++;
        }        
    }else if(turnstatus==9){
        if(isE(key)){
            inflg++;
        }        
    }else if(turnstatus==16){
        if(isE(key)){
            inflg++;
        }        
    }

    if(turnstatus!=5){
        keyboardflg=1;
        glutTimerFunc(locktime, keyboardTimer, 0);
    }
}
}

void Initvalue(void){
    keyboardflg=0;
    direction=-1;
    selectpos=0;
    onetime=0;
    inflg=0;
}

// 売却する物件を決める関数
//ismonopoly : 独占の物件を売るか
// high : 借金より高い物件から売るか
int sale(int ismonopoly,int high){
    int i,j;
    int nearest=99999;
    int nearestst=-1;
    int nearestid=-1;
    int count=0;
    while(1){
        for(i=0;i<STATIONNUM;i++){ // 全ての駅について
            if(ismonopoly){
                for(j=0;j<stations[i].propertynum;j++){ // 全ての物件について
                    if(stations[i].plist[j].holder==turn+1){ // ターン中のププレイヤーの物件のとき
                        // 物件の価格が借金の絶対値より大きいとき
                        if(high){
                        if(stations[i].plist[j].price>=-players[turn].money){
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                nearest = abs(-players[turn].money-stations[i].plist[j].price);
                                nearestst = i;
                                nearestid = j;                              
                            }
                        }
                        }else{
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                nearest = abs(-players[turn].money-stations[i].plist[j].price);
                                nearestst = i;
                                nearestid = j;                              
                            }
                        }
                    }
                }
            }else{
                if(stations[i].ismonopoly!=turn+1){ // 独占でないとき
                for(j=0;j<stations[i].propertynum;j++){ // 全ての物件について
                    if(stations[i].plist[j].holder==turn+1){ // ターン中のププレイヤーの物件のとき
                        // 物件の価格が借金の絶対値より大きいとき
                        // 物件の価格が借金の絶対値より大きいとき
                        if(high){
                        if(stations[i].plist[j].price>=-players[turn].money){
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                nearest = abs(-players[turn].money-stations[i].plist[j].price);
                                nearestst = i;
                                nearestid = j;                                 
                            }
                        }
                        }else{
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                nearest = abs(-players[turn].money-stations[i].plist[j].price);
                                nearestst = i;
                                nearestid = j;                                 
                            }
                        }
                    }
                }
            }
            }
        }
        if(nearestst==-1){
            break;
        }else{
            //printf("%sを売却しました\n",stations[nearestst].plist[nearestid].name);
            count++;
            stations[nearestst].plist[nearestid].holder=0;
            players[turn].money+=stations[nearestst].plist[nearestid].price;
            players[turn].assets-=stations[nearestst].plist[nearestid].price;
            if(players[turn].money>=0){
                return count;
            }
        }      
    }
    return count;
}

// 物件売却処理
int debtprocess(void){
    int i,j;
    int count=0;
    // 資産がないとき
    if(players[turn].assets==0){
        return -1;
    }

    // 借金が資産より大きいとき,全ての物件を売却
    if(abs(players[turn].money)>=players[turn].assets){
        for(i=0;i<STATIONNUM;i++){
            // 独占解除
            if(stations[i].ismonopoly==turn+1){
                stations[i].ismonopoly=0;
            }
            for(j=0;j<stations[i].propertynum;j++){
            if(stations[i].plist[j].holder==turn+1){
                stations[i].plist[j].holder=0;
                players[turn].money+=stations[i].plist[j].price;
                players[turn].assets-=stations[i].plist[j].price;
                count++;
                }
            }
        }  
        return count;
    }

    // 借金が資産より小さいとき
    count +=sale(0,1);
    if(players[turn].money>=0){
        return count;
    }
    count +=sale(0,0);
    if(players[turn].money>=0){
        return count;
    }
    count +=sale(1,1);
    if(players[turn].money>=0){
        return count;
    }
    count +=sale(1,0);
    printf("count = %d\n",count);
    return count;
}
// ディスプレイ関数
void Display(void){
    int st; //止まった駅の番号を保持
    int transx,transy;
    int i;
    char fname[100];

    glClear(GL_COLOR_BUFFER_BIT);
    //printf("inflg = %d\n",inflg);
    //printf("turnstatus = %d\n",turnstatus);
    if(turnstatus==0){ // ゲーム初期化処理
        Initvalue();
        month=4;
        year=0;
        turn=0;
        onetime=0;
        goalflg=0;
        inflg=0;
        turnstatus=1;

    }else if(turnstatus==1){ // 未設定

        onetime=0;
        inflg=0;
        turnstatus=2;

    }else if(turnstatus==2){ // 目的地設定

        if(inflg==0){ // 乱数生成用の設定
            range=STATIONNUM;
            randflg=1;
            glutTimerFunc(RANDTIME, RandTimer, 0);
            inflg=1;
        }else if(inflg==1){ // ランダム表示
            glColor3ub(23,194,230);
            drawQUAD(0,0,InitWidth,InitHeight);
            drawString("mokutekiti",0,InitWidth/2-80,InitHeight/2-64,1);
            drawString(stations[dummyresult].name,0,InitWidth/2-80,InitHeight/2-16,1);            
        }else if(inflg==2){
            if(onetime==0){
                randflg=0;
                randresult=rand()%range;
                distination.x=stations[randresult].x;
                distination.y=stations[randresult].y;
                onetime=1;
            }
            glColor3ub(23,194,230);
            drawQUAD(0,0,InitWidth,InitHeight);
            drawString("mokutekiti",0,InitWidth/2-80,InitHeight/2-64,1);
            drawString(stations[randresult].name,0,InitWidth/2-80,InitHeight/2-16,1);  
        }else if(inflg==3){
            onetime=0;
            inflg=0;
            if(goalflg==1){
                turnstatus=7;
            }else{
                turnstatus=3;
            }
        }

    }else if(turnstatus==3){ // ターンのはじめ

        drawMap();
        drawPlayer();    
        if(inflg==0){
        // 所持金表示
            glColor3ub(playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
            drawDialog(11,11,InitWidth-22,34+16);
            sprintf(fname,"%dnennmess%dgatu",year,month);
            drawString(fname,0,16,11+8,0.5);
            drawMoney(players[turn].money,InitWidth/2,11+8+16,0,0.5);
            sprintf(fname,"%ssilatilouussssss",players[turn].name);
            drawString(fname,0,16,11+8+16,0.5);

            sprintf(fname,"%ssilatilouunobanndesumrxxxedellsaiikorollwomawasitekudasaiimr",players[turn].name);
            drawText(fname,11,225,InitWidth-22,42,0);            
        }else if(inflg==1){
            inflg=0;
            turnstatus=4;
        }    

    }else if(turnstatus==4){ // サイコロをふる処理

        drawMap();
        drawPlayer();    
        if(inflg==0){
            range=DICEMAX;
            randflg=1;
            glutTimerFunc(RANDTIME, RandTimer, 0);
            inflg=1;
        }else if(inflg==1){
            PutSprite(diceimg[dummyresult], 416, 32, &diceinfo[dummyresult],1);
            sprintf(fname,"xedellsaiikorollwotomemasumr");
            drawText(fname,11,225,InitWidth-22,42,0);   
        }else if(inflg==2){
            randflg=0;
            if(onetime==0){
                randresult = rand()%range;
                recount=randresult;
                onetime=1;
            }
            PutSprite(diceimg[randresult], 416, 32, &diceinfo[randresult],1);
            sprintf(fname,"xewooositekudasaiimr");
            drawText(fname,11,225,InitWidth-22,42,0);   
        }else if(inflg==3){
            inflg=0;
            onetime=0;
            turnstatus=5;
        }

    }else if(turnstatus==5){ // マス移動

        drawMap();
        drawPlayer();  
        sprintf(fname,"aatoxxss%dxxssssllmasu",recount+1);
        drawText(fname,390,40,74,60,0);  
        if(recount==-1){
            turnstatus=6;
        }

    }else if(turnstatus==6){

        transx = players[turn].x/IMGSIZE;
        transy = players[turn].y/IMGSIZE;
        st = getmapnum(transx,transy);
        if(st==3){ // 物件に止まったとき
            if((transx == distination.x)&&(transy == distination.y)){ // 目的地なら
                goalflg=1;
                turnstatus=2;
            }else{ //目的地でないなら
                turnstatus=7;
            }
        }else if(st==1){ // プラス駅に止まったとき
            turnstatus=8;
        }else if(st==2){ // マイナス駅に止まったとき
            turnstatus=9;
        }

    }else if(turnstatus==7){ // 物件購入処理

        drawMap();
        drawPlayer(); 
        if(inflg==0){
            selectpos=0;
            inflg++;
        }else if(inflg==1){
            drawStation();
        }else if(inflg==2){
            inflg=0;
            turnstatus=15;
        }

    }else if(turnstatus==8){ // プラス駅の処理

        drawMap();
        drawPlayer(); 
        if(inflg==0){
            range=plusarray[month-1][1]-plusarray[month-1][0];
            randflg=1;
            glutTimerFunc(RANDTIME, RandTimer, 0);
            inflg=1;            
        }else if(inflg==1){
            glColor3ub(pluscolor[0],pluscolor[1],pluscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(plusarray[month-1][0]+dummyresult,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);            
        }else if(inflg==2){
            if(onetime==0){
                randflg=0;
                randresult = rand()%range;
                tmpmoney = plusarray[month-1][0]+randresult;
                players[turn].money+=tmpmoney;
                onetime=1;
            }
            glColor3ub(pluscolor[0],pluscolor[1],pluscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(tmpmoney,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
            // 所持金表示
            glColor3ub(playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
            drawDialog(11,220,InitWidth-22,42);
            drawMoney(players[turn].money,InitWidth/2,225,0,0.5);
            sprintf(fname,"silozikinn");
            drawString(fname,0,16,225,0.5);               
        }else if(inflg==3){
            onetime=0;
            inflg=0;
            turnstatus=15;
        }
    }else if(turnstatus==9){ //マイナス駅の処理
        drawMap();
        drawPlayer(); 
        if(inflg==0){
            range=minusarray[month-1][1]-minusarray[month-1][0];
            randflg=1;
            glutTimerFunc(RANDTIME, RandTimer, 0);
            inflg=1;            
        }else if(inflg==1){
            glColor3ub(minuscolor[0],minuscolor[1],minuscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(-minusarray[month-1][0]-dummyresult,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);            
        }else if(inflg==2){
            if(onetime==0){
                randflg=0;
                randresult = rand()%range;
                tmpmoney = -minusarray[month-1][0]-randresult;
                players[turn].money+=tmpmoney;
                onetime=1;
            }
            glColor3ub(minuscolor[0],minuscolor[1],minuscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(tmpmoney,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
            // 所持金表示
            glColor3ub(playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
            drawDialog(11,220,InitWidth-22,42);
            drawMoney(players[turn].money,InitWidth/2,225,0,0.5);
            sprintf(fname,"silozikinn");
            drawString(fname,0,16,225,0.5);               
        }else if(inflg==3){
            if(players[turn].money<0){
                onetime=0;
                inflg=4;
            }else{
                onetime=0;
                inflg=0;
                turnstatus=15;
            }
        }else if(inflg==4){
            if(onetime==0){
                rdebet = debtprocess();
                onetime=1;
            }
            if(rdebet==-1){
                sprintf(fname,"uurerubultkenngaaarimasennmrsilaltkinnwoseoolttesimaiimasitamr");
            }else{
                sprintf(fname,"silaltkinnwoseoolttesimaiimasitamr%dkennnobultkennwobaiikilakusimasitamr",rdebet);
            }
            drawText(fname,11,225,InitWidth-22,42,0); 
        }else if(inflg==5){
            inflg=0;
            turnstatus=15;
        }
    }else if(turnstatus==15){
        // ターン終了処理
        turn++;
        onetime=0;
        if(turn==3){
            month++;
        }
        // 3月の場合は決算を行う
        if((turn==3)&&(month==4)){
            turn=0;
            turnstatus=16;
        }else{
            if(turn==3){
                turn=0;
            }
            if(month==13){
                month=1;
            }
            turnstatus=3;
        }
    }else if(turnstatus==16){
        if(inflg==0){
            kessan();
            for(i=0;i<PLAYERNUM;i++){
                players[i].money+=shueki[i];
            }
            inflg++;
        }else if(inflg==1){
            glColor3ub(23,194,230);
            drawQUAD(0,0,InitWidth,InitHeight);
            drawString("keltsann",0,InitWidth/2-64,InitHeight/2-16,1);            
        }else if(inflg==2){
            glColor3ub(23,194,230);
            drawQUAD(0,0,InitWidth,InitHeight);
            drawString("keltsann",0,InitWidth/2-64,11,1);
            drawString("siluuueekigaku",0,11,43,0.7);
            for(i=0;i<PLAYERNUM;i++){
                sprintf(fname,"%s",players[i].name);
                drawString(fname,0,11,75+25*i,0.7);
                drawMoney(shueki[i],InitWidth/2,75+25*i,0,0.7);
            }            
        }else if(inflg==3){
            glColor3ub(23,194,230);
            drawQUAD(0,0,InitWidth,InitHeight);
            drawString("keltsann",0,InitWidth/2-64,11,1);
            drawString("souusisann",0,11,43,0.7);
            for(i=0;i<PLAYERNUM;i++){
                sprintf(fname,"%s",players[i].name);
                drawString(fname,0,11,75+25*i,0.7);
                drawMoney(players[i].assets,InitWidth/2,75+25*i,0,0.7);
            }
        }else if(inflg==4){
            inflg=0;
            year++;
            turnstatus=3;
        }
    }
    glFlush();
}