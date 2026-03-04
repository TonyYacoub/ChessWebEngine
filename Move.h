// Move.h
#ifndef MOVEHISTORY_H
#define MOVEHISTORY_H

struct Move{
    int from;
    int to;
    char promotion;
    bool isCapture;
    bool isCheck;
    bool isMate;
};


#endif
