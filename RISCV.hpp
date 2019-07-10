//
// Created by 57402 on 2019/7/9.
//

#ifndef RISCV_BRANCH_PREDICTION_RISCV_H
#define RISCV_BRANCH_PREDICTION_RISCV_H
#include "operator.hpp"
const int maxn =0x3fffff;
unsigned char memory[maxn]={0};//内存
int reg[32]={0};//寄存器
int lock[32]={0};//寄存器是否被锁起来
int PC=0;//PC寄存器
bool PClock=0;//PC是否被锁起来
int saber=0;//缓存区，EX-result
int avalon=0;//缓存区，MEM-result
int loadlock=0;//标记是不是load以及load进行到的程度
///都是operator所需要用到的东西
#endif //RISCV_BRANCH_PREDICTION_RISCV_H
