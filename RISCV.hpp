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
int jump=0,notjump=0;//跳和不跳分别的次数
double success=0,total=0;//预测成功的次数以及预测的总数；
int jumpflag=0,errorflag=0;//预测之后是否进行跳跃以及是否预测错误
int willjump[20000]={0};//利用哈希检测跳不跳
///都是operator所需要用到的东西
#endif //RISCV_BRANCH_PREDICTION_RISCV_H
