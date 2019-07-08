//
// Created by 57402 on 2019/7/8.
//

#ifndef RISCV_PARALLEL_RISCV_H
#define RISCV_PARALLEL_RISCV_H

#include "operator.hpp"
const int maxn =0x3fffff;
unsigned char memory[maxn]={0};//内存
int reg[32]={0};//寄存器
bool lock[32]={0};//寄存器是否被锁起来
int PC=0;//PC寄存器
bool PClock=0;//PC是否被锁起来
/**operator所需要用到的东西*/
#endif //RISCV_PARALLEL_RISCV_H
