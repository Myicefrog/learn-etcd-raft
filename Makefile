# vim: set fdm=marker:
    
# -------------------------------
# KDS Makefile 模版
# -------------------------------

# 声明三个伪目标
.PHONY: default help clean clean-all force

CC=cc
CXX=g++
CFLAGS=-Wall -c -std=c++11

# TODO: 这里加入所有默认需要编译的目标。
TARGETS=raft
default: $(TARGETS)

# 全局的编译和链接选项都放在这里
CC_OPTS=-I /usr/local/include/ -I /usr/include/
LD_OPTS=-L /usr/local/lib/ -L /usr/lib64/


SOURCES=$(wildcard *.cc)
OBJECTS=$(patsubst %.cc, %.o,$(SOURCES))

# 下面是每个目标的生成过程
force:

LIBS=-lprotobuf -lpthread
 
raft: $(OBJECTS)
	$(CXX)  $(OBJECTS) $(CC_OPTS) $(LD_OPTS) $(LIBS) -o $(TARGETS)
%.o : %.cc
	$(CC) $(CFLAGS) $(CC_OPTS) $(LD_OPTS) $< -o $@
%.o : %.cpp
	$(CXX) $(CFLAGS) $(CC_OPTS) $(LD_OPTS) $< -o $@
	
	
install:

clean:
	rm -f *.o
	rm -f $(TARGETS)

clean-all:
	rm -f *.o
	rm -f $(TARGETS)
