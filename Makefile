#
# Makefile
#

# 最終目的ファイル
TARGET = debuger.exe

# ソースファイル一覧(*.c)の一覧
SRCS = debuger.c game.c

# オブジェクトファイル一覧(*.o)
OBJS = ${SRCS:.c=.o}

# ヘッダファイルの一覧(何もなし)
HEADERS = 

# コンパイラ・リンカの指定
CC = gcc
CCFLAGS = -Wall -g -O0 -I/usr/include/opengl
LD = gcc
LDFLAGS =
LIBS = -lglpng -lglut32 -lglu32 -lopengl32 -lm

#OBJSからTARGETを作る方法
$(TARGET) : $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET) $(LIBS)

# *.cから*.oを作る方法
.c.o:
	$(CC) $(CCFLAGS) -c $<

# *.oはHEADERSとMakefileに依存
$(OBJS) : $(HEADERS) Makefile

# make cleanとしたときに実行されるコマンド
clean :
	rm -f $(TARGET) $(OBJS) core *~