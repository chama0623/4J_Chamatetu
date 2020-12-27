#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"

char Map[YMAX][XMAX+1] = { //NULL文字に気を付ける
        "AAAAAAAAAAAAAAA",
        "AAP-B-B---CAAAAA",
        "AA|A|AAAAAAAAAA",
        "AAP-B--C--PAAAA",
        "AAAA|AAAAA|AAAA",
        "AAAAMAABAA|AAAA",
        "AAAA|AA|AA|AAAA",
        "AAAA|AA|AA|AAAA",
        "AC--M--P--BAAAAA",
        "AAAAAAAAAAAAAAA"
};

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
                            ,"6","7","8","9","ex","mx","ox","px"
                            };

int playercolor[3][3]={{30,144,255},
                       {255,20,147},
                       {255,255,0}};
int pluscolor[3] = {0,0,205};
int minuscolor[3] = {220,20,60};
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
        keyboardflg=1;
    }
}

void DiceTimer(int t)
{
    glutPostRedisplay();
    dice=rand()%DICEMAX;
    recount=dice;
    if(diceflg==1){
        glutTimerFunc(DICETIME, DiceTimer, 0);
    }
}

void RandTimer(int t)
{
    int range;
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
//
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

void nextStation(int x,int y){
    int xa=x/IMGSIZE;
    int ya=y/IMGSIZE;
    //printf("Cal Next Station(%d,%d)\n",xa,ya);
    if(direction==0){
        while(1){
            ya--;
            if(getmapnum(xa,ya)!=5){
                break;
            }
        }
    }

    if(direction==1){
        while(1){
            xa++;
            if(getmapnum(xa,ya)!=6){
                break;
            }
        }
    }

    if(direction==2){
        while(1){
            ya++;
            if(getmapnum(xa,ya)!=5){
                break;
            }
        }
    }

    if(direction==3){
        while(1){
            xa--;
            if(getmapnum(xa,ya)!=6){
                break;
            }
        }
    }
    nx=xa;
    ny=ya;
}

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

void keyboard(unsigned char key,int x,int y){
    int r = 623;
    if(turnstatus==1){
        if(key=='e'){
            diceflg=2;
        }
    }

    if((turnstatus==2)&&(keyboardflg==1)){
    if(key=='w'){
        direction=0;
    }
    if(key=='d'){
        direction=1;
    }
    if(key=='s'){
        direction=2;
    }
    if(key=='a'){
        direction=3;
    }
    r=isMovable(players[turn].x,players[turn].y);
        if(r){
            massRecord[dice-recount][0]=players[turn].x/IMGSIZE;
            massRecord[dice-recount][1]=players[turn].y/IMGSIZE;
            nextStation(players[turn].x,players[turn].y);
            keyboardflg=2;
            glutTimerFunc(MOVETIME, MoveTimer, 0);
        }
    }

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
                printf("%s\n",stations[wherestation].plist[selectpos-1].name);
                if(ispurchase(wherestation,selectpos-1)==1){
                    printf("買えるよ!\n");
                    players[turn].assets+=stations[wherestation].plist[selectpos-1].price;
                    players[turn].money-=stations[wherestation].plist[selectpos-1].price;
                    stations[wherestation].plist[selectpos-1].holder=turn+1;
                    dispPlayer(1);
                    dispStation(0);
                    }else{
                    printf("買えないよ!\n");
                }
            }
        }

    }
    if((turnstatus==5)&&(randflg==1)){
        if(key=='e'){
            randflg=2;
        }        
    }
    if((turnstatus==5)&&(randflg==2)){
        if(key=='w'){
            randflg=3;
        }        
    }   

        if((turnstatus==6)&&(randflg==1)){
        if(key=='e'){
            randflg=2;
        }        
    }
    if((turnstatus==6)&&(randflg==2)){
        if(key=='w'){
            randflg=3;
        }        
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
            PutSprite(mapimg[img_num], drawx, drawy, &mapinfo[img_num],1);
        }
    }
}

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

void drawString(char *string,int kh,int color,int xo,int yo,double scale){
    int i,j;
    int len = strlen(string)-1;
    int x=xo;
    int y=yo;
    int numberflg;
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
            i++;
        }
        x+=IMGSIZE*scale;
        if(x+IMGSIZE*scale>InitWidth){
            x=xo;
            y+=IMGSIZE*scale;
        }
    }
}

void InitPlayer(void){
    int i,j;
    for(i=0;i<PLAYERNUM;i++){
        sprintf(players[i].name,"debug%d",i+1); // for debug
        players[i].x=INITX;
        players[i].y=INITY;
        players[i].money=INITMONEY;
        players[i].assets=0;
        players[i].isBonby=0;
        players[i].cardnum=0;
        for(j=0;j<NAMEMAX;j++){
            players[i].nameAttribute[j]=0;
        }
    }
}

// detail : 0 全部表示 , else その番号の駅を表示
void dispPlayer(int detail){
    int i,j;
    if(detail==0){
        for(i=0;i<PLAYERNUM;i++){
            printf("--------------------\n");
            printf("%s社長 (%d,%d)\n",players[i].name,players[i].x,players[i].y);
            for(j=0;j<NAMEMAX;j++){
                printf("%d",players[i].nameAttribute[j]);
            }
            printf("\n");
            printf("所持金 : %d\n",players[i].money);
            printf("総資産 : %d\n",players[i].assets);
            printf("ボンビーフラグ : %d\n",players[i].isBonby);
            printf("カード枚数 : %d\n",players[i].cardnum);
            printf("--------------------\n\n");
        }
    }else{
        printf("--------------------\n");
        printf("%s社長 (%d,%d)\n",players[detail-1].name,players[detail-1].x,players[detail-1].y);
        for(j=0;j<NAMEMAX;j++){
            printf("%d",players[detail-1].nameAttribute[j]);
        }
        printf("\n");
        printf("所持金 : %d\n",players[detail-1].money);
        printf("総資産 : %d\n",players[detail-1].assets);
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

void dispmassRecord(void){
    int i;
    printf("\n---dispmass Record---\n");
    for(i=0;i<=dice-recount;i++){
        printf("%d (%d,%d)\n",i,massRecord[i][0],massRecord[i][1]);
    }
    printf("----------\n");
}

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

void drawQUAD(int x,int y,int width,int height){
    glBegin(GL_QUADS);
    glVertex2i(x,y);
    glVertex2i(x,y+height);
    glVertex2i(x+width,y+height);
    glVertex2i(x+width,y);
    glEnd();       
}

void drawMoney(int money,int x,int y,int color,double scale){
    int oku,man;
    char fname[40];
    oku = money/10000;
    man = money%10000;
    if(oku!=0){
        if(man!=0){
            sprintf(fname,"%dox%dmxex",oku,man);
        }else{
            sprintf(fname,"%doxex",oku);
        }
    }else{
        sprintf(fname,"%dmxex",man);
    }
    drawString(fname,0,color,x,y,scale);
}

void drawStation(int x,int y){
    int i,j;
    char fname[40];
    int holder;
    int color;
     for(i=0;i<STATIONNUM;i++){
         if((stations[i].x==x)&&(stations[i].y==y)){
            propertynum = stations[i].propertynum; 
            wherestation = i;
            // 駅名表示
            drawDialog(11,11,InitWidth-22,42,255,245,238);
            drawString(stations[i].name,0,0,16,16,1);

            // 所持金表示
            drawDialog(11,61,InitWidth-22,34,playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
            drawString("silozikinn",0,0,16,61+8,0.5);
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
                drawString(stations[i].plist[j].name,stations[i].plist[j].nameAttribute,color,18,42+11+50+7+17*j,0.5);
                drawMoney(stations[i].plist[j].price,InitWidth/2,42+11+50+7+17*j,color,0.5);
                drawString(fname,0,color,3*InitWidth/4,42+11+50+7+17*j,0.5);
            }
         }
     }
}

void Display(void){
    int stopstation;
    glClear(GL_COLOR_BUFFER_BIT);
    drawMap();
    drawPlayer();
    
    //printf("turnstatus = %d\n",turnstatus);
    // ターン開始のとき
    if(turnstatus==0){
        selectpos=-623;
        printf("%s社長のターンです\n",players[turn].name);
        printf("%d年%d月目\n",year,month);
        dispPlayer(turn+1);
        turnstatus=1;
    }

    //サイコロをふるとき
    if(turnstatus==1){
        if(diceflg==0){
            diceflg=1;
            glutTimerFunc(DICETIME, DiceTimer, 0);
        }
        if(diceflg==1){
            PutSprite(diceimg[dice], 448, 32, &diceinfo[dice],1);
        }
        if(diceflg==2){
            printf("dice = %d\n",dice);
            turnstatus=2;
        }
    }

    // マス移動のとき
    if(turnstatus==2){
        PutSprite(diceimg[dice], 448, 32, &diceinfo[dice],1);
        PutSprite(diceimg[recount], 448, 64, &diceinfo[recount],1);
        if(keyboardflg==0){
            keyboardflg=1;
        }else{
            //printf("残り移動可能マス数 = %d\n",recount);
            if(recount==-1){
                keyboardflg=0;
                direction=-1;
                turnstatus=3;
            }
        }
    }
    
    // 駅に止まったとき
    if(turnstatus==3){
        stopstation = getmapnum(players[turn].x/IMGSIZE,players[turn].y/IMGSIZE);
        if(stopstation==4){ // 物件にとまったとき
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
        printf("selectpos = %d\n",selectpos);
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
            drawMoney(randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
        }
        if(randflg==2){ 
            if(calflg==0){
                players[turn].money-=randreturn;
                dispPlayer(turn+1);
                drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
                drawMoney(randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
                calflg=1;
            }
            glColor3ub(minuscolor[0],minuscolor[1],minuscolor[2]);
            drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
            drawMoney(randreturn,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
        }   
        if(randflg==3){
            turnstatus=100;
        }
    }

    //ターン終了処理
    if(turnstatus==100){
        selectpos=-623;
        keyboardflg=0;
        turn++;
        if(turn==3){
            month+=1;
            if(month==13){
                year+=1;
                month=0;
            }
            turn =0;
        }
        keyboardflg=0;
        direction=-1;
        randflg=0;
        diceflg=0;
        turnstatus=0;
    }

    glFlush();
}