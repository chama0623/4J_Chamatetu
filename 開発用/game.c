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
       //012345678901234567890123456789      
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", // 0
        "AAAAAAAAAAAP-M--M-BAAAAAAAAAAA", // 1
        "AAAAAAAAAAA|A|AAAAAAAAAAAAAAAA", // 2
        "AAAAAAAAAC-M-BAAAAAAAAAAAAAAAA", // 3
        "AAAAAAAAA|AAA|AAAAAAAAAAAAAAAA", // 4
        "AAAAAAAAAP-M-P--CAAAAAAAAAAAAA", // 5
        "AAAAAAAAA|AAA|AA|AAAAAAAAAAAAA", // 6
        "AAAB-C---C---B--P--BAAAAAAAAAA", // 7
        "AAA|AAAAA|AAAAAA|AA|AAAAAAAAAA", // 8
        "AAAMAAAAABAAAAAAM--P-CAAAAAAAA", // 9
        "AAA|AAAAA|AABAAAAAA|A|AAAAAAAA", // 0
        "AAAM-MAAA|AA|AAAAAAB-CAAAAAAAA", // 1
        "AAA|A|AAAB--M--PAAA|AAAAAAAAAA", // 2
        "AAAM-MAAA|AAAAAAAAAMAAAAAAAAAA", // 3
        "AAA|AAAAA|AAAAAAAAA|AAAAAAAAAA", // 4
        "AAA|AAAAAB-P-M-M-P-B-P-BAAAAAA", // 5
        "AAAMAAAAA|AAA|AAAAAAAAA|AAAAAA", // 6
        "AAA|AAAAAPAAAPAAAAAAAAA|AAAAAA", // 7
        "AAAB-MAAA|AAA|AAAAAAAAABAAAAAA", // 8
        "AAA|AAAAAP---CAAAAAAAAA|AAAAAA", // 9
        "AAABAAAAA|AAAAAAAAAAAAA|AAAAAA", // 0
        "AAA|AAAAA|AAAAAAAAAAAAA|AAAAAA", // 1
        "AAAM-M---B--C--P-MAAAAA|AAAAAA", // 2
        "AAAAAAAAA|AAAAAAA|AAP--PAAAAAA", // 3
        "AAAAAAAAA|AAAAAAA|AA|AA|AAAAAA", // 4
        "AAABAC-M-C-----M-B--C--P--PAAA", // 5
        "AAA|A|A|A|AAAAA|A|AAAAA|AAAAAA", // 6
        "AAAM-P-P-B-CAAA|A|AAAAABAAAAAA", // 7
        "AAAAAAAAAAAAAAAM-P--BAAAAAAAAA", // 8
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"  // 9
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

// カード駅の色
int cardcolor[3] = {255,255,33};

// プラス駅の月別乱数表
int plusarray[MAXMONTH][2] = {{400,1200}, // 1月
                               {200,800}, // 2月
                               {400,1500}, // 3月
                               {2000,4000}, // 4月
                               {3000,7000}, // 5月
                               {4000,8000}, // 6月
                               {12000,30000}, // 7月
                               {35000,70000}, // 8月
                               {18000,32000}, // 9月
                               {6000,12000}, // 10月
                               {3000,7000}, // 11月
                               {2000,4000}}; // 12月

// マイナス駅の月別乱数表
int minusarray[MAXMONTH][2] = {{10000,25000}, // 1月
                               {20000,80000}, // 2月
                               {10000,25000}, // 3月
                               {500,1500}, // 4月
                               {400,1200}, // 5月
                               {300,1000}, // 6月
                               {200,600}, // 7月
                               {100,400}, // 8月
                               {500,1500}, // 9月
                               {2000,4000}, // 10月
                               {3000,8000}, // 11月
                               {4000,9000}}; // 12月

char cardname[CARDNUM][50]={"kiluuukouullkallmslldoll",
                         "toltkiluuullkallmslldoll",
                         "sinnkannsennllkallmslldoll",
                         "llsamilttokallmslldoll",
                         "bulttobillkallmslldoll",
                         "10oxexllkallmslldoll",
                         "tokuseiireiillkallmslldoll",
                         "gouusoltkiluuullkallmslldoll"};
// 収益計算用
int shueki[3];

// デバッグ用関数 	
// プレイヤー構造体を表示	
// detail : 0 全部表示 , else その番号の駅を表示	
void dispPlayer(int detail){	
    int i,j;	
    if(detail==0){	
        for(i=0;i<PLAYERNUM;i++){	
            printf("--------------------\n");	
            printf("%s社長 (%d,%d)\n",players[i].name,players[i].x,players[i].y);	
            printf("\n");	
            printf("所持金 : %d\n",players[i].money);	
            printf("総資産 : %d\n",players[i].assets);
            printf("カード枚数 : %d\n",players[i].cardnum);
            for(j=0;j<CARDMAX;j++){
                printf("%d ",players[i].card[j]);
            }	
            printf("\n--------------------\n\n");	
        }	
    }else{	
        printf("--------------------\n");	
        printf("%s社長 (%d,%d)\n",players[detail-1].name,players[detail-1].x,players[detail-1].y);	
        printf("\n");	
        printf("所持金 : %d\n",players[detail-1].money);	
        printf("総資産 : %d\n",players[detail-1].assets);
        printf("カード枚数 : %d\n",players[detail-1].cardnum);
        for(j=0;j<CARDMAX;j++){
            printf("%d ",players[detail-1].card[j]);
        }		
        printf("\n--------------------\n\n");   	
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
                printf("%s %d %d %d\n",stations[i].plist[j].name,stations[i].plist[j].price,stations[i].plist[j].earnings,stations[i].plist[j].holder);	
            }	
            printf("--------------------\n\n");	
        }	
        }else{	
        printf("--------------------\n");	
        printf("%s駅 (%d,%d)\n",stations[detail-1].name,stations[detail-1].x,stations[detail-1].y);	
        printf("独占フラグ : %d   物件数 : %d\n",stations[detail-1].ismonopoly,stations[detail-1].propertynum);	
        for(j=0;j<stations[detail-1].propertynum;j++){	
            printf("%s %d %d %d\n",stations[detail-1].plist[j].name,stations[detail-1].plist[j].price,stations[detail-1].plist[j].earnings,stations[detail-1].plist[j].holder);	
        }	
        printf("--------------------\n\n");     	
    }	
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

// ウィンドウサイズ変更時の処理
void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glScaled(1, -1, 1);
    glTranslated(0, -h, 0);
    //windowサイズ固定 
    glutReshapeWindow(InitWidth,InitHeight);
}

// 画面更新用タイマー
void Timer(int t)
{
    glutPostRedisplay();
    glutTimerFunc(RESHAPETIME, Timer, 0);
}

// ダミー乱数を一定時間ごとに生成するタイマー
void RandTimer(int t)
{ // (0,range-1)の範囲の乱数を生成
    int i;
    for(i=0;i<dummynum;i++){
        dummyresult[i] = rand()%range;
    }
    if(randflg==1){ // randflgがたっているときタイマー継続
        glutTimerFunc(RANDTIME, RandTimer, 0);
    }
}

// 駅移動管理タイマー
void MoveTimer(int t)
{
    int transx;
    int transy;
    // 移動処理
    move();
    // 座標変換
    transx = players[turn].x/IMGSIZE;
    transy = players[turn].y/IMGSIZE;
    if((transx != nx)||(transy != ny)){ // 次の駅の座標と同じか
        glutTimerFunc(MOVETIME, MoveTimer, 0); // 同じでないときタイマー継続
    }else if((players[turn].x%IMGSIZE!=0)||(players[turn].y%IMGSIZE!=0)){  // 余りが0でないとき
        glutTimerFunc(MOVETIME, MoveTimer, 0); // タイマー継続
    }else{
        if(randresult-recount>0){ // まだ移動可能かどうか
        // 移動可能のとき,以前の移動履歴をチェック
            if((massRecord[randresult-recount-1][0]==transx)&&(massRecord[randresult-recount-1][1]==transy)){
                recount++; // 戻ったとき残りカウント数増加
            }else{
                recount--; // 進んだとき残りカウント数減少
            }
        }else{
            recount--; // カウント減少
        }
        if(recount==0){ // カウント終了のときキーボード入力をロック
            keyboardflg=1;
        }else{ // キーボード入力のロックを解除
            keyboardflg=0;
        }
    }
}


//キーボード入力管理タイマー 
void keyboardTimer(int t)
{
    // キーボード入力ロックを解除
    keyboardflg=0;
}

// eを押したか判定
// 1: キーがE
// 0 : キーがEでない
int isE(unsigned char key){
    if(key=='e'){
        return 1;
    }else{
        return 0;
    }
}

// 行先が壁かどうか判定
// 1 : 壁
// 0 : 壁でない
int isWall(int x,int y){
    if(getmapnum(x,y)==WALL){
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
    if(direction==0){ // 上
        if(isWall(x,y-1)){
            return 0;
        }
    }else if(direction==1){ //右
        if(isWall(x+1,y)){
            return 0;
        }    
    }else if(direction==2){ //下
        if(isWall(x,y+1)){
            return 0;
        }   
    }else{ //左
        if(isWall(x-1,y)){
            return 0;
        }   
    }
    return 1;
}

// 移動処理
void move(void){
    if(direction==0){ // 上
        players[turn].y-=MOVESIZE;
    }
    if(direction==1){ // 右
        players[turn].x+=MOVESIZE;
    }
    if(direction==2){ // 下
        players[turn].y+=MOVESIZE;
    }
    if(direction==3){ // 左
        players[turn].x-=MOVESIZE;
    }
}

// 次の駅を取得
void nextStation(int x,int y){
    int sx=x;
    int sy=y;
    if(direction==0){ // 上
        while(1){
            sy--;
            if(getmapnum(sx,sy)!=SENRO1){ // 駅を発見したら
                break;
            }
        }
    }else if(direction==1){ // 右
        while(1){
            sx++;
            if(getmapnum(sx,sy)!=SENRO2){ // 駅を発見したら
                break;
            }
        }
    }else if(direction==2){ // 下
        while(1){
            sy++;
            if(getmapnum(sx,sy)!=SENRO1){ // 駅を発見したら
                break;
            }
        }
    }else if(direction==3){ // 左
        while(1){
            sx--;
            if(getmapnum(sx,sy)!=SENRO2){ // 駅を発見したら
                break;
            }
        }
    }else{ // エラーチェック用
        sx=-623;
        sy=-623;
    }
    // 次の駅の座標をセット
    nx=sx;
    ny=sy;
}

// 物件が購入できるか取得
// 1 : 取得可能
// 0 : 取得不可能
int ispurchase(int id){
    int flg=1;
    // 既に誰かの物件のとき
    if(stations[stid].plist[id].holder!=0){
        flg=0;
    }
    // 値段が足りないとき
    if(players[turn].money < stations[stid].plist[id].price){ 
        flg=0;
    }
    return flg;
}

//物件購入処理
void purchase(int id){
    int i;
    int monopolyCheck=0; // 独占チェック用
    // 総資産を計算
    players[turn].assets+=stations[stid].plist[selectpos].price;
    // 財布を計算
    players[turn].money-=stations[stid].plist[selectpos].price;
    // 購入済みフラグをたてる
    stations[stid].plist[selectpos].holder=turn+1;    
    // 独占チェック
    for(i=0;i<propertynum;i++){
        if(stations[stid].plist[i].holder==turn+1){
            monopolyCheck++;
        }
    }    
    // 独占のとき
    if(monopolyCheck==propertynum){
        // 独占フラグをたてる
        stations[stid].ismonopoly=turn+1;
    }    
}

// 画像読み込み
void readImg(void){
    int i;
    char fname[100];

    // イベントマップ読み込み
    for(i=0;i<SP_NUM;i++){
        sprintf(fname,".\\eventparts\\sp%d.png",i+1);
        spimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &spinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);        
    }

   // 季節マップ読み込み 
    for(i=0;i<SEASON_NUM;i++){
        sprintf(fname,".\\mapparts\\season%d.png",i+1);
        seasonimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &seasoninfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);        
    }

    // マップイメージ読み込み
    for(i=0;i<=MAP_NUM;i++){
        sprintf(fname,".\\mapparts\\map%d.png",i+1);
        mapimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &mapinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    // プレイヤー画像を読み込み
    for(i=0;i<PLAYERNUM;i++){
        sprintf(fname,".\\eventparts\\player%d.png",i+1);
        playerimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &playerinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }

    // サイコロの画像を読み込み
    for(i=0;i<DICEMAX;i++){
        sprintf(fname,".\\dice\\dice%d.png",i+1);
        diceimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &diceinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    // read Hiragana black
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\h%sblack.png",jpProtcol[i]);
        hblackimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hblackinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }

    // read Hiragana red
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\h%sred.png",jpProtcol[i]);
        hredimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hredinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    // read Katakana black
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\k%sblack.png",jpProtcol[i]);
        kblackimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &kblackinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    // read Katakana red
    for(i=0;i<JPMAX;i++){
        sprintf(fname,".\\charimg\\k%sred.png",jpProtcol[i]);
        kredimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &kredinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    // read Special Str red
    for(i=JPMAX;i<JPMAX+SPMAX;i++){
        sprintf(fname,".\\charimg\\%sred.png",jpProtcol[i]);
        hredimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hredinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
    // read Special Str black
    for(i=JPMAX;i<JPMAX+SPMAX;i++){
        sprintf(fname,".\\charimg\\%sblack.png",jpProtcol[i]);
        hblackimg[i] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &hblackinfo[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
}

// ファイルから駅情報を取得
// stations構造体を初期化
void readStation(void){
    FILE *fp;
    int i=0;
    fp=fopen("property.txt","r");
    if(fp==NULL){ // 開けなかったとき
        printf("file not found");
        exit(0);
    }else{ // 駅名と座標を取得
        while(fscanf(fp,"%s %d,%d",stations[i].name,&stations[i].x,&stations[i].y)!=EOF){
            stations[i].ismonopoly=0; // 独占フラグ初期化
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
        if(fp==NULL){ // 開けなかったとき
            printf("file not found in %s",stations[i].name);
            exit(0);
        }else{
            // 物件名,値段,収益率を取得
            while(fscanf(fp,"%s %d,%d",stations[i].plist[j].name,&stations[i].plist[j].price,&stations[i].plist[j].earnings)!=EOF){
            stations[i].plist[j].holder=0; // 購入フラグ初期化
            j++;
        }
        stations[i].propertynum=j; // 物件数を保存
        fclose(fp);
    }

    }   
}

// マップの画像番号を取得
int getmapnum(int x,int y){
    int img_num;
    // 配列番号をはみ出した場合
    if((x<0)||(x>=XMAX)){ 
        return 623; // 草原マップを返す
    }else if((y<0)||(y>=YMAX)){
        return 623; // 草原マップを返す
    }

    switch (Map[y][x]){
    case 'A': // 草原
        img_num=623;
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
        img_num=0;
        break;
    case 'M': // マイナス駅
        img_num=1;
        break;
    case 'C': // マイナス駅
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
    for(y=0;y<InitHeight/IMGSIZE;y++){
        for(x=0;x<InitWidth/IMGSIZE;x++){
            drawx = x*IMGSIZE;
            drawy = y*IMGSIZE;
            img_num = getmapnum(x+tx,y+ty);
            if((distination.x==x+tx)&&(distination.y==y+ty)){ // 目的地のとき
                // 目的地画像を描画
                PutSprite(mapimg[DIST], drawx, drawy, &mapinfo[DIST],1);
            }else if(img_num==WALL){ // 草原マップのとき
                // 季節にあった草原を描画
                PutSprite(seasonimg[season], drawx, drawy, &seasoninfo[season],1);
            }else{
                // マップ描画
                PutSprite(mapimg[img_num], drawx, drawy, &mapinfo[img_num],1);
            }
        }
    }
}

// プレイヤーを描画
// 最上面にターン中のプレイヤーを描画
void drawPlayer(void){
    int transx,transy;
    int i;
    for(i=0;i<PLAYERNUM;i++){
        if(i!=turn){ // ターン中のプレイヤー以外を描画
            transx = players[i].x/IMGSIZE;
            transy = players[i].y/IMGSIZE;
            PutSprite(playerimg[i], (transx-tx)*IMGSIZE,(transy-ty)*IMGSIZE, &playerinfo[i],1);
        }
    }
    // ターン中のプレイヤーを最上レイヤーに表示
    //PutSprite(playerimg[turn], players[turn].x,players[turn].y, &playerinfo[turn],1);
    PutSprite(playerimg[turn], CENTX,CENTY, &playerinfo[turn],1);
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
        flg=string[i]-'0'; // インデクス計算
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
        if(flg==1){ // 次の座標に移動
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
    int i,j;
    for(i=0;i<PLAYERNUM;i++){
        //プレイヤーhoge
        sprintf(players[i].name,"llpureiiyallms%d",i+1);
        players[i].x=INITX;
        players[i].y=INITY;
        players[i].money=INITMONEY;
        players[i].assets=0;
        players[i].cardnum=0;
        for(j=0;j<CARDMAX;j++){
            players[i].card[j]=0;
        }
    }
}

// ダイアログを画面に描画
void drawDialog(int x,int y,int width,int height){
    // ダイアログの背景を描画
    glBegin(GL_QUADS);
    glVertex2i(x,y);
    glVertex2i(x,y+height);
    glVertex2i(x+width,y+height);
    glVertex2i(x+width,y);
    glEnd();

    // 外側の四角を縁取り
    glColor3ub(139,69,19);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x,y);
    glVertex2i(x,y+height);
    glVertex2i(x+width,y+height);
    glVertex2i(x+width,y);
    glEnd();

    // 内側の四角を縁取り
    glBegin(GL_LINE_LOOP);
    glVertex2i(x+5,y+5);
    glVertex2i(x+5,y+height-5);
    glVertex2i(x+width-5,y+height-5);
    glVertex2i(x+width-5,y+5);
    glEnd();
}

// 数字を描画
void drawMoney(int money,int x,int y,int color,double scale){
    char fname[50];
    int oku,man;
    // 億の桁,万の桁を計算
    oku = money/10000;
    man = money%10000;
    if(money>=0){ // お金がプラスの時
        if(oku!=0){
            if(man!=0){
                sprintf(fname,"%dox%dmxex",oku,man);
            }else{
                sprintf(fname,"%doxex",oku);
            }
        }else{
            sprintf(fname,"%dmxex",man);
        }
    }else{ // お金がマイナスの時
    // 数字部分の符号を反転
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
    // 画面出力
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
        // 駅の座標が一致したら
         if((stations[i].x==transx)&&(stations[i].y==transy)){
            // 物件数を取得
            propertynum = stations[i].propertynum; 
            // 配列番号を取得
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
        drawMoney(stations[stid].plist[j].price,InitWidth/2-16,42+11+50+7+17*j,color,0.5);
        drawString(fname,color,16+3*InitWidth/4,42+11+50+7+17*j,0.5);
    }
    // 独占ダイアログ表示
    if(stations[stid].ismonopoly!=0){
        sprintf(fname,"%ssilatilouunodokusenndesumr",players[stations[stid].ismonopoly-1].name);
        drawText(fname,11,223,InitWidth-22,32,0);
        sprintf(fname,"xqsssiluuurilouuxxxesskouuniluuu");
        drawText(fname,11,273,InitWidth-22,42,0);
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
        shueki[i]=0; // 収益初期化
        for(j=0;j<STATIONNUM;j++){
            // 独占チェック
            if(stations[j].ismonopoly==i+1){
                dokusen=2; // 独占のとき
            }else{
                dokusen=1; // 独占でないとき
            }
            for(k=0;k<stations[j].propertynum;k++){
                if(stations[j].plist[k].holder==i+1){
                    // 価格*収益率を計算
                    shueki[i]+=dokusen*stations[j].plist[k].price*stations[j].plist[k].earnings/100;
                }
            }
        }
    }
}

// キーボード入力管理
void keyboard(unsigned char key,int x,int y){
    int locktime =500;
    int transx = players[turn].x/IMGSIZE;
    int transy = players[turn].y/IMGSIZE;
if(keyboardflg==0){ // キーボード入力がロックされていないとき
    if(turnstatus==0){ //タイトル
        if(isE(key)){
            inflg++;
        }
    }else if(turnstatus==1){ // 目的地設定
        if(isE(key)){
            inflg++;
        }
    }else if(turnstatus==2){ // ターンのはじめ
    if(inflg==1){
        if(key=='w'){
            if(selectpos==1){
                selectpos=0;
            }   
        }else if(key=='s'){
            if(selectpos==0){
                selectpos=1;
            }
        }else if(isE(key)){
            inflg++;
        }
    }else if(inflg==4){
        if(isE(key)){
            inflg=1;
        }        
    }else if(inflg==5){
        if(key=='w'){
            if(selectpos>=1){
                selectpos--;
            }   
        }else if(key=='s'){
            if(selectpos<players[turn].cardnum){
                selectpos++;
            }
        }else if(isE(key)){
            inflg++;
        }        
    }else{
        if(isE(key)){
            inflg++;
        }        
    }
    }else if(turnstatus==4){ // サイコロをふるとき
        if(isE(key)){
            inflg++;
        }
    }else if(turnstatus==5){ //移動
        if(key=='w'){ // 上
            direction=0;
        }else if(key=='d'){ // 右
            direction=1;
        }else if(key=='s'){ // 下
            direction=2;
        }else if(key=='a'){ // 左
            direction=3;
        }else { // それ以外
            direction=-623;
        }
        if(direction!=-623){
            if(isMovable(transx,transy)){
                // 移動履歴を書き込み
                massRecord[randresult-recount][0]=transx;
                massRecord[randresult-recount][1]=transy;
                // 次の駅を計算
                nextStation(transx,transy);
                keyboardflg=1; // キーボード入力ロック
                glutTimerFunc(MOVETIME, MoveTimer, 0); //移動タイマー起動
            }
        }   
    }else if(turnstatus==6){ // 到着
        if(inflg>=1){
            inflg++;
        }
    }else if(turnstatus==7){ // 物件購入
        locktime=200;
        if(key=='s'){ // ポジションを下へ
            if(selectpos<propertynum-1){
                selectpos+=1;
            }            
        }else if(key=='w'){ // ポジションを上へ
            if(selectpos>=1){
                selectpos-=1;
            }            
        }else if(isE(key)){ //　物件購入
            if(ispurchase(selectpos)){
                purchase(selectpos);
            }
        }else if(key=='q'){ // 購入終了
                inflg++;
        }
    }else if(turnstatus==8){ // プラスマス
        if(isE(key)){
            inflg++;
        }        
    }else if(turnstatus==9){ // マイナスマス
        if(isE(key)){
            inflg++;
        }        
    }else if(turnstatus==10){ // カードマス
        if(isE(key)){
            inflg++;
        }        
    }else if(turnstatus==16){ // 決算
        if(isE(key)){
            inflg++;
        }        
    }else if(turnstatus==17){ // 最終成績
        if(inflg==4){
            if(key=='q'){ // ゲーム終了
                exit(0);
            }
        }else{
            if(isE(key)){
                inflg++;
            }
        }        
    }

    if(turnstatus!=5){
        keyboardflg=1; // キーボード入力ロック
        glutTimerFunc(locktime, keyboardTimer, 0); // ロック解除タイマー
    }
}
}

// 物件を売却する関数
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
            if(ismonopoly){ // 独占の物件を売るとき
                for(j=0;j<stations[i].propertynum;j++){ // 全ての物件について
                    if(stations[i].plist[j].holder==turn+1){ // ターン中のププレイヤーの物件のとき
                        // 物件の価格が借金の絶対値より大きいとき
                        if(high){
                        // 借金より高い物件かチェック
                        if(stations[i].plist[j].price>=-players[turn].money){
                            // 値段が近いとき
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                // 候補として更新
                                nearest = abs(-players[turn].money-stations[i].plist[j].price);
                                nearestst = i;
                                nearestid = j;                              
                            }
                        }
                        }else{
                            // 値段が近いとき
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                // 候補として更新
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
                        if(high){
                        // 借金より高い物件かチェック
                        if(stations[i].plist[j].price>=-players[turn].money){
                            // 値段が近いとき
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                // 候補として更新
                                nearest = abs(-players[turn].money-stations[i].plist[j].price);
                                nearestst = i;
                                nearestid = j;                                 
                            }
                        }
                        }else{
                            // 値段が近いとき
                            if(abs(-players[turn].money-stations[i].plist[j].price)<nearest){
                                // 候補として更新
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
        if(nearestst==-1){ // 売れる物件がない
            break;
        }else{
            count++; // 売った物件の数を追加
             // 購入フラグ解除
            stations[nearestst].plist[nearestid].holder=0;
            // 所持金を計算
            players[turn].money+=stations[nearestst].plist[nearestid].price;
            // 総資産を計算
            players[turn].assets-=stations[nearestst].plist[nearestid].price;
            // 借金が回復したとき
            if(players[turn].money>=0){
                return count;
            }
        }      
    }
    return count;
}

//季節番号を計算
// 0:春 3~5月
// 1:夏 6~8月
// 2:秋 9~11月
// 3:冬 12~2月
void calseason(void){
    if((3<=month)&&(month<=5)){
        season=0;
    }else if((6<=month)&&(month<=8)){
        season=1;
    }else if((9<=month)&&(month<=11)){
        season=2;
    }else{
        season=3;
    }
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
                // 購入フラグ初期化
                stations[i].plist[j].holder=0;
                // 所持金を計算
                players[turn].money+=stations[i].plist[j].price;
                // 総資産を計算
                players[turn].assets-=stations[i].plist[j].price;
                // 売った物件数をカウント
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
    return count;
}

// カード処理
int cardprocess(int num){
    int r=1;
    int i,j,randst;
    if(num==KYUKO){ // 急行カード
        saikoro=2;
    }else if(num==TOKKYU){ // 特急カード
        saikoro=3;
    }else if(num==SINKANSEN){ // 新幹線カード
        saikoro=4;
    }else if(num==SAMMIT){ // サミットカード
        if(rand()%3!=0){
            for(i=0;i<PLAYERNUM;i++){
                players[i].x=players[turn].x;
                players[i].y=players[turn].y;
            }
        }else{
            r=0;
        }
    }else if(num==BUTTOBI){ // ぶっとびカード
        randst = rand()%STATIONNUM;
        players[turn].x = stations[randst].x*IMGSIZE;
        players[turn].y = stations[randst].y*IMGSIZE;
    }else if(num==JUOKU){ // 10億円カード
        players[turn].money+=100000;
    }else if(num==TOKUSEIREI){ // 徳政令カード
        for(i=0;i<PLAYERNUM;i++){
            if(players[i].money<0){
                players[i].money=0;
            }
        }
    }else if(num==GOUSOKKYU){ // 剛速球カード
        if(rand()%2){
        for(i=0;i<PLAYERNUM;i++){
            if(i!=turn){
                players[i].cardnum=0;
                for(j=0;j<CARDMAX;j++){
                    players[i].card[j]=0;
                }
            }
        }
        }else{
           r=0; 
        }        
    }
    dummynum=saikoro;
    return r;
}

// 変数初期化
void Initvalue(void){
    int i;
    keyboardflg=0;
    for(i=0;i<SAIKOROMAX;i++){
        dummyresult[i]=0;
    }
    direction=-1;
    selectpos=0;
    inflg=0;
}
// ゲーム開始時の処理
void startgame(void){
    if(inflg==0){
        Initvalue(); // 変数初期化
        month=4; // 4月にセット
        year=1; // 1年目にセット
        calseason(); // 季節計算
        turn=0; // プレイヤー1のターンにセット
        goalflg=0; // ゴールフラグ初期化
        inflg++;
    }else if(inflg==1){
        PutSprite(spimg[3],0,0,&spinfo[3],1);
    }else if(inflg==2){
        inflg=0;
        turnstatus=1;
    }
}
// 目的地決定処理
void desisionDist(void){
    char fname[150];
    PutSprite(spimg[2],0,0,&spinfo[2],1); // 背景表示
    if(inflg==0){
        if(goalflg==0){ // 初めて目的地をセットするとき
            // さいしょのもくてきちをきめます．
            // Eでルーレットをまわしてください．
            sprintf(fname,"saiisilonomokutekitiwokimemasumrxxxedellrullmsllrelttollwomawasitekudasaiimr");
            drawText(fname,11,225,InitWidth-22,42,0);             
        }else if(goalflg==1){
            // つぎのもくてきちをきめます．
            // Eでルーレットをまわしてください．                
            sprintf(fname,"tuginomokutekitiwokimemasumrxxxedellrullmsllrelttollwomawasitekudasaiimr");
            drawText(fname,11,225,InitWidth-22,42,0);                    
            }
    }else if(inflg==1){ // 乱数生成用の設定
        dummynum=1;
        dummyresult[0]=0;
        range=STATIONNUM; // rangeを駅の数にセット
        randflg=1; // ダミータイマーロック解除
        //タイマー呼び出し
        glutTimerFunc(RANDTIME, RandTimer, 0);
        inflg++;
    }else if(inflg==2){ // ダミーリザルトを表示
        drawString(stations[dummyresult[0]].name,0,InitWidth/2-80,105,1); 
        // Eでとめます．
        sprintf(fname,"xedetomemasumr");
        drawText(fname,11,225,InitWidth-22,42,0);             
    }else if(inflg==3){
        randflg=0; // タイマー停止
        randresult=rand()%range; // 結果を計算
        // 目的地の座標,名前を格納
        distination.x=stations[randresult].x;
        distination.y=stations[randresult].y;
        sprintf(distination.name,"%s",stations[randresult].name);
        inflg++;
    }else if(inflg==4){
        // 目的地を画面出力
        // もくてきちはhogeです.
        // Eをおしてください.
        sprintf(fname,"mokutekitiha%sdesumrxxxewooositekudasaiimr",distination.name);
        drawText(fname,11,225,InitWidth-22,42,0); 
        drawString(distination.name,0,InitWidth/2-80,105,1);  
    }else if(inflg==5){ // status更新
        inflg=0;
        if(goalflg==1){
            turnstatus=7;
        }else{
            turnstatus=2;
        }
    }
}
// ターン開始時の処理
void startTurn(void){
    int i;
    char fname[150];
    drawMap(); // マップ描画
    drawPlayer(); // プレイヤー描画
    if(inflg==0){
        saikoro=1;
        dummynum=1;
        keyboardflg=0;
        selectpos=0;
        inflg++;
    }else if(inflg==1){
        // プレイヤーカラーでダイアログ生成
        glColor3ub(playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
        drawDialog(11,11,InitWidth-22,34+16);
        // hogeねんめ hugaがつ
        sprintf(fname,"%dnennmess%dgatu",year,month);
        drawString(fname,0,16,11+8,0.5);
        // 所持金表示
        drawMoney(players[turn].money,InitWidth/2,11+8+16,0,0.5);
        // hogeしゃちょう
        sprintf(fname,"%ssilatilouussssss",players[turn].name);
        drawString(fname,0,16,11+8+16,0.5);   
        // サイコロ
        // カード
        sprintf(fname,"llsaiikoroxxkallmslldoll");
        drawDialog(11,175,74,42);
        // セレクトポジション表示
        if(selectpos == 0){
            glColor3ub(255,0,0);
            drawQUAD(16,180,64,16);                 
        }else if(selectpos==1){
            glColor3ub(255,0,0);
            drawQUAD(16,196,64,16);               
        }   
        drawString(fname,0,16,180,0.5);
        // hogeしゃちょうのばんです．
        sprintf(fname,"%ssilatilouunobanndesumr",players[turn].name);
        drawText(fname,11,225,InitWidth-22,42,0);      
    }else if(inflg==2){
        if(selectpos==0){
            inflg++;
        }else{
            if(players[turn].cardnum==0){
                inflg=4;
            }else{
                selectpos=0;
                inflg=5;
            }
        }
    }else if(inflg==3){ // status更新(サイコロをふる)
        inflg=0;
        turnstatus++;
    }else if(inflg==4){ // カードがないとき
        sprintf(fname,"llkallmslldollga1maiimoaarimasennmr");
        drawText(fname,11,225,InitWidth-22,42,0);               
    }else if(inflg==5){
        glColor3ub(255,245,238);
        drawDialog(155,50,10+10*16,10+16*(players[turn].cardnum+1));
        // セレクトポジション表示
        glColor3ub(255,0,0);
        drawQUAD(160,55+selectpos*16,10*16,16);                 
        
        for(i=0;i<players[turn].cardnum;i++){
            if(players[turn].card[i]!=0){
                drawString(cardname[players[turn].card[i]-1],0,160,55+i*16,0.5);                   
            }
        }
        drawString("modoru",0,160,55+players[turn].cardnum*16,0.5);
    }else if(inflg==6){
        if(selectpos==players[turn].cardnum){ // もどるのとき
            selectpos=0;
            inflg=1;
        }else{
            rcard = cardprocess(players[turn].card[selectpos]);
            inflg++;
        }
    }else if(inflg==7){
        if(rcard==0){
            sprintf(fname,"kouugekigakawasaretamr");
            nextflg=1;
        }else if(players[turn].card[selectpos]==KYUKO){
            sprintf(fname,"llsaiikorollga2tuninalttamr");
            nextflg=0;
        }else if(players[turn].card[selectpos]==TOKKYU){
            sprintf(fname,"llsaiikorollga3tuninalttamr");
            nextflg=0;
        }else if(players[turn].card[selectpos]==SINKANSEN){
            sprintf(fname,"llsaiikorollga4tuninalttamr");
            nextflg=0;
        }else if(players[turn].card[selectpos]==SAMMIT){
            sprintf(fname,"zennsilatilouuga%ssilatilouunomotoniaatumalttamr",players[turn].name);
            nextflg=1;
        }else if(players[turn].card[selectpos]==BUTTOBI){
            sprintf(fname,"%ssilatilouuhaiilttaiidokohe",players[turn].name);
            nextflg=1;
        }else if(players[turn].card[selectpos]==JUOKU){
            sprintf(fname,"%ssilatilouunillpurasull10oxexmr",players[turn].name);
            nextflg=1;
        }else if(players[turn].card[selectpos]==TOKUSEIREI){
            sprintf(fname,"zennsilatilouunosilaltkinngatilouukesininalttamr");
            nextflg=1;
        }else if(players[turn].card[selectpos]==GOUSOKKYU){
            sprintf(fname,"hokanosilatilouunollkallmslldollganakunalttamr");
            nextflg=1;
        }
        drawText(fname,11,225,InitWidth-22,42,0);      
    }else if(inflg==8){ // 
        // 使ったカードの消去
        for(i=selectpos;i<players[turn].cardnum-1;i++){
            players[turn].card[i]=players[turn].card[i+1];
        }
        players[turn].cardnum--;
        inflg=0;
        if(nextflg==1){
            turnstatus=15; // ターン終了 
        }else{
            turnstatus++; // サイコロをふる処理
        }     
    }
}
// サイコロをふる処理
void rollDice(void){
    int i;
    char fname[150];
    drawMap();
    drawPlayer();    
    if(inflg==0){
        // ダミーサイコロを起動
        for(i=0;i<SAIKOROMAX;i++){
            dummyresult[i]=0;
        }
        range=DICEMAX;
        randflg=1;
        glutTimerFunc(RANDTIME, RandTimer, 0);
        inflg=1;
    }else if(inflg==1){
        // サイコロ描画
        for(i=0;i<dummynum;i++){
            PutSprite(diceimg[dummyresult[i]], 416, 32+32*i, &diceinfo[dummyresult[i]],1);
        }
        // Eでサイコロをとめます．
        sprintf(fname,"xedellsaiikorollwotomemasumr");
        drawText(fname,11,225,InitWidth-22,42,0);   
    }else if(inflg==2){
        // サイコロ結果処理
        randflg=0;
        recount=0;
        for(i=0;i<saikoro;i++){
            randresulttmp[i] = rand()%range;
            recount+=randresulttmp[i]+1;
        }
        randresult=recount;
        inflg++;
    }else if(inflg==3){
        for(i=0;i<saikoro;i++){
            PutSprite(diceimg[randresulttmp[i]], 416, 32+32*i, &diceinfo[randresulttmp[i]],1);
        }       
        // Eをおしてください.
        sprintf(fname,"xewooositekudasaiimr");
        drawText(fname,11,225,InitWidth-22,42,0);
    }else if(inflg==4){
        inflg=0;
        turnstatus=5;
    }
}
// 駅移動処理
void moveMass(void){
    char fname[150];
    drawMap();
    drawPlayer();  
    // あと
    // 　　hoge
    // 　　　マス
    // もくてきち
    // huga
    sprintf(fname,"aatoxxss%dxxssssllmasullxxmokutekitixx%s",recount,distination.name);
    drawText(fname,340,40,125,94,0);  
    if(recount==0){ // 移動マスを消費したらstatus更新
        turnstatus=6;
    }
}
// 停車駅の判定と処理の分岐
void checkMass(void){
    int st; //止まった駅の番号を保持
    int transx,transy; // プレイヤーの座標変換用
    char fname[200];
    drawMap();
    drawPlayer();  
    if(inflg==0){
        keyboardflg=0; // キーボードロック解除
        transx = players[turn].x/IMGSIZE;
        transy = players[turn].y/IMGSIZE;
        st = getmapnum(transx,transy);
        if(st==PROPERTYMASU){ // 物件に止まったとき
            if((transx == distination.x)&&(transy == distination.y)){ // 目的地なら
                goalflg=1; // ゴールフラグをたてる
                players[turn].money+=30000; // プラス3億円
                inflg++;
            }else{ //目的地でないなら
                turnstatus=7;
            }
        }else if(st==PLUSMASU){ // プラス駅に止まったとき
            turnstatus=8;
        }else if(st==MINUSMASU){ // マイナス駅に止まったとき
            turnstatus=9;
        }else if(st==CARDMASU){ // マイナス駅に止まったとき
            turnstatus=10;
        }
    }else if(inflg==1){
        // hogeしゃちょうがhugaに1ばんのり.おめでとうございます.
        // hogeしゃちょうにプラス3億円.
        sprintf(fname,"%ssilatilouuga%sni1bannnorimroomedetouugozaiimasumr%ssilatilouunillpurasull3oxexmr",players[turn].name,distination.name,players[turn].name);
        drawText(fname,11,225,InitWidth-22,42,0);              
    }else if(inflg==2){
        inflg=0;
        turnstatus=1; // 目的地再設定
    }
}
// 物件購入処理
void purchaseProperty(void){
    drawMap();
    drawPlayer(); 
    if(inflg==0){
        keyboardflg=0;
        selectpos=0;
        inflg++;
    }else if(inflg==1){
        drawStation(); // 物件情報描画
    }else if(inflg==2){
        inflg=0;
        turnstatus=15;
    }
}
// プラス駅の処理
void plusMass(){
    char fname[150];
    drawMap();
    drawPlayer(); 
    if(inflg==0){
        dummynum=1;
        // ダミータイマー起動
        dummyresult[0]=0;
        keyboardflg=0;
        range=plusarray[month-1][1]-plusarray[month-1][0];
        randflg=1;
        glutTimerFunc(RANDTIME, RandTimer, 0);
        inflg=1;            
    }else if(inflg==1){
        // ダミー出力
        glColor3ub(pluscolor[0],pluscolor[1],pluscolor[2]);
        drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
        drawMoney(plusarray[month-1][0]+dummyresult[0],InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);            
    }else if(inflg==2){
        // 結果を計算
        randflg=0;
        randresult = rand()%range;
        tmpmoney = plusarray[month-1][0]+randresult;
        players[turn].money+=tmpmoney;
        inflg++;
    }else if(inflg==3){
        // 所持金ダイアログ表示
        glColor3ub(pluscolor[0],pluscolor[1],pluscolor[2]);
        drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
        drawMoney(tmpmoney,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
        // 所持金表示
        glColor3ub(playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
        drawDialog(11,220,InitWidth-22,42);
        drawMoney(players[turn].money,InitWidth/2,225,0,0.5);
        // しょじきん
        sprintf(fname,"silozikinn");
        drawString(fname,0,16,225,0.5);               
    }else if(inflg==4){
        inflg=0;
        turnstatus=15;
    }
}
// マイナス駅の処理
void minusMass(void){
    char fname[150];
    drawMap();
    drawPlayer(); 
    if(inflg==0){
        // ダミータイマー起動
        dummynum=1;
        dummyresult[0]=0;
        keyboardflg=0;
        range=minusarray[month-1][1]-minusarray[month-1][0];
        randflg=1;
        glutTimerFunc(RANDTIME, RandTimer, 0);
        inflg=1;            
    }else if(inflg==1){
        // ダミー出力
        glColor3ub(minuscolor[0],minuscolor[1],minuscolor[2]);
        drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
        drawMoney(-minusarray[month-1][0]-dummyresult[0],InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);            
    }else if(inflg==2){
        // 結果を計算
        randflg=0;
        randresult = rand()%range;
        tmpmoney = -minusarray[month-1][0]-randresult;
        players[turn].money+=tmpmoney;
        inflg++;
    }else if(inflg==3){
        // 所持金ダイアログ表示
        glColor3ub(minuscolor[0],minuscolor[1],minuscolor[2]);
        drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
        drawMoney(tmpmoney,InitWidth/2-IMGSIZE*3,InitHeight/2-16,0,1);
        // 所持金表示
        glColor3ub(playercolor[turn][0],playercolor[turn][1],playercolor[turn][2]);
        drawDialog(11,220,InitWidth-22,42);
        drawMoney(players[turn].money,InitWidth/2,225,0,0.5);
        sprintf(fname,"silozikinn");
        drawString(fname,0,16,225,0.5);               
    }else if(inflg==4){
        if(players[turn].money<0){ //借金を背負ったとき
            inflg++;
        }else{ // それ以外
            inflg=0;
            turnstatus=15;
        }
    }else if(inflg==5){ // 借金返済処理
        rdebet = debtprocess(); // 売却した物件数を取得
        inflg++;
    }else if(inflg==6){
        if(rdebet==-1){
            // うれるぶっけんがありません．しゃっきんをせおってしまいました.
            sprintf(fname,"uurerubultkenngaaarimasennmrsilaltkinnwoseoolttesimaiimasitamr");
        }else{
            // しゃっきんをせおってしまいました.hogeけんのぶっけんをばいきゃくしました.
            sprintf(fname,"silaltkinnwoseoolttesimaiimasitamr%dkennnobultkennwobaiikilakusimasitamr",rdebet);
        }
        drawText(fname,11,225,InitWidth-22,42,0); 
    }else if(inflg==7){
        inflg=0;
        turnstatus=15;
    }
}
// カード駅の処理
void cardMass(void){
    char fname[150];
    drawMap();
    drawPlayer(); 
    if(inflg==0){
        if(players[turn].cardnum==5){
            inflg=5;
        }else{
            dummynum=1;
            // ダミータイマー起動
            dummyresult[0]=0;
            keyboardflg=0;
            range=CARDNUM;
            randflg=1;
            glutTimerFunc(RANDTIME, RandTimer, 0);
            inflg=1;
        }            
    }else if(inflg==1){
        // ダミー出力
        glColor3ub(cardcolor[0],cardcolor[1],cardcolor[2]);
        drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
        drawString(cardname[1+dummyresult[0]],0,InitWidth/2-IMGSIZE*4,InitHeight/2-16,1);           
    }else if(inflg==2){
        // 結果を計算
        randflg=0;
        randresult = 1 + rand()%range;
        players[turn].card[players[turn].cardnum]=randresult;
        players[turn].cardnum++;
        inflg++;
    }else if(inflg==3){
        // 入手したカードを表示
        glColor3ub(cardcolor[0],cardcolor[1],cardcolor[2]);
        drawQUAD(0,InitHeight/2-16,InitWidth,IMGSIZE);
        drawString(cardname[randresult-1],0,InitWidth/2-IMGSIZE*4,InitHeight/2-16,1); 
        // hogeをてにいれました.
        sprintf(fname,"%swoteniiiremasitamr",cardname[randresult-1]);
        drawText(fname,11,225,InitWidth-22,42,0);                     
    }else if(inflg==4){
        inflg=0;
        turnstatus=15;
    }else if(inflg==5){
        // これいじょうカードをもてません.
        sprintf(fname,"koreiizilouullkallmslldollwomotemasennmr");
        drawText(fname,11,225,InitWidth-22,42,0);            
    }else if(inflg==6){
        inflg=4;
    }
};
// ターン終了処理
void endTurn(void){
// ターン終了処理
    turn++;
    if(turn==3){
        month++;
    }
    // 決算月かどうか判別
    if((turn==3)&&(month==4)){
        if(year==3){
            turnstatus=17; // 最終成績
        }else{
            turn=0;
            turnstatus=16;  // 決算
        }
    }else{
        if(turn==3){ // ターン初期化
            turn=0;
        }
        if(month==MAXMONTH+1){
            month=1; // 12月まできたら1月にリセット
        }
        calseason(); // 季節再計算
        turnstatus=2; // ターンのはじめにもどる
    }
}
// 決算の処理
void processKessan(){
    int i;
    char fname[150];
    if(inflg==0){
        kessan(); // 決算処理
        // 収益を所持金に追加
        for(i=0;i<PLAYERNUM;i++){
            players[i].money+=shueki[i];
        }
        inflg++;
    }else if(inflg==1){
        // 決算のタイトル画像を表示
        glColor3ub(23,194,230);
        drawQUAD(0,0,InitWidth,InitHeight);
        PutSprite(spimg[0], 0, 0, &spinfo[0],1);           
    }else if(inflg==2){
        // 背景表示
        glColor3ub(23,194,230);
        drawQUAD(0,0,InitWidth,InitHeight);
        PutSprite(spimg[1], 0, 0, &spinfo[1],1);
        // 決算
        drawString("keltsann",0,InitWidth/2-64,11,1);
        // しゅうえきがく
        drawString("siluuueekigaku",0,11,43,0.7);
        for(i=0;i<PLAYERNUM;i++){
            // 名前表示
            sprintf(fname,"%s",players[i].name);
            drawString(fname,0,11,75+25*i,0.7);
            // 収益額表示
            drawMoney(shueki[i],InitWidth/2,75+25*i,0,0.7);
        }            
    }else if(inflg==3){
        glColor3ub(23,194,230);
        drawQUAD(0,0,InitWidth,InitHeight);
        // 背景
        PutSprite(spimg[1], 0, 0, &spinfo[1],1);
        // けっさん
        drawString("keltsann",0,InitWidth/2-64,11,1);
        // そうしさん
        drawString("souusisann",0,11,43,0.7);
        for(i=0;i<PLAYERNUM;i++){
        // 名前表示
            sprintf(fname,"%s",players[i].name);
            drawString(fname,0,11,75+25*i,0.7);
            // 総資産表示
            drawMoney(players[i].assets+players[i].money,InitWidth/2,75+25*i,0,0.7);
        }
    }else if(inflg==4){
        inflg=0;
        year++; // 1年経過
        turnstatus=2;
    }
}
// ゲーム終了処理
void endgame(void){
    int i;
    char fname[150];
    if(inflg==0){
        kessan();
        for(i=0;i<PLAYERNUM;i++){
            players[i].money+=shueki[i];
        }
        inflg++;
    }else if(inflg==1){
        glColor3ub(23,194,230);
        drawQUAD(0,0,InitWidth,InitHeight);
        PutSprite(spimg[0], 0, 0, &spinfo[0],1);            
    }else if(inflg==2){
        glColor3ub(23,194,230);
        drawQUAD(0,0,InitWidth,InitHeight);
        PutSprite(spimg[1], 0, 0, &spinfo[1],1);
        // さいしゅうせいせき
        drawString("saiisiluuuseiiseki",0,InitWidth/2-144,11,1);
        // しゅうえきがく
        drawString("siluuueekigaku",0,11,43,0.7);
        for(i=0;i<PLAYERNUM;i++){
            sprintf(fname,"%s",players[i].name);
            drawString(fname,0,11,75+25*i,0.7);
            drawMoney(shueki[i],InitWidth/2,75+25*i,0,0.7);
        }            
    }else if(inflg==3){
        glColor3ub(23,194,230);
        drawQUAD(0,0,InitWidth,InitHeight);
        PutSprite(spimg[1], 0, 0, &spinfo[1],1);
        // さいしゅうせいせき
        drawString("saiisiluuuseiiseki",0,InitWidth/2-144,11,1);
        // しゅうえきがく
        drawString("souusisann",0,11,43,0.7);
        for(i=0;i<PLAYERNUM;i++){
        sprintf(fname,"%s",players[i].name);
            drawString(fname,0,11,75+25*i,0.7);
            drawMoney(players[i].assets+players[i].money,InitWidth/2,75+25*i,0,0.7);
        }
    }else if(inflg==4){
        glColor3ub(23,194,230);
        drawQUAD(0,0,InitWidth,InitHeight);
        // おつかれさまでした
        drawString("ootukaresamadesita",0,InitWidth/2-144,InitHeight/2-48,1);
        // Qでゲームをしゅうりょうします.
        drawString("xqdellgellmsllmullwosiluuurilouusimasu",0,InitWidth/2-120,InitHeight/2+16,0.5);          
    }
}

// ディスプレイ関数
void Display(void){
    tx = players[turn].x/IMGSIZE-CX; // 中央座標計算
    ty = players[turn].y/IMGSIZE-CY;
    glClear(GL_COLOR_BUFFER_BIT); // 描画クリア
    if(turnstatus==0){ // ゲーム初期化処理
        startgame();
    }else if(turnstatus==1){ // 目的地設定
        desisionDist();
    }else if(turnstatus==2){ // ターンのはじめ
        startTurn();
    }else if(turnstatus==3){ // ターンのはじめ
        turnstatus++;
    }else if(turnstatus==4){ // サイコロをふる処理
        rollDice();
    }else if(turnstatus==5){ // マス移動
        moveMass();
    }else if(turnstatus==6){ // 条件分岐
        checkMass();
    }else if(turnstatus==7){ // 物件購入処理
        purchaseProperty();
    }else if(turnstatus==8){ // プラス駅の処理
        plusMass();
    }else if(turnstatus==9){ // マイナス駅の処理
        minusMass();
    }else if(turnstatus==10){ // カード駅の処理
        cardMass();
    }else if(turnstatus==15){ // 月別分岐
        endTurn();
    }else if(turnstatus==16){ // 決算月
        processKessan();
    }else if(turnstatus==17){ // 最終成績
        endgame();
    }
    glFlush();
}