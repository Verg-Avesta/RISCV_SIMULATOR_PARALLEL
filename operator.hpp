//
// Created by 57402 on 2019/7/9.
//

#ifndef RISCV_BRANCH_PREDICTION_OPERATOR_H
#define RISCV_BRANCH_PREDICTION_OPERATOR_H
#include "RISCV.hpp"
int signedExtend(int data,int bits){
    if(data&(1<<bits))
        data|=0xffffffff>>bits<<bits;
    return data;
}

class instruction{
public:
    int rs1,rs2,rd,imm,f3,f7,opcode,code;
    int result;
    int bubble;
    instruction(){
        rs1=rs2=rd=imm=f3=f7=opcode=-1;
        code=0;
        bubble=0;
        result=-123;
    }
    instruction ID(){
        opcode=code&((1<<7)-1);
        int a,b,c,d;
        switch(opcode){
            case 55:
            case 23:
            case 111:rd=(code>>7)&((1<<5)-1);
                    imm=(code>>12);
                if(opcode!=111)imm=(imm<<12);
                else{
                    d=imm&((1<<8)-1);
                    c=(imm>>8)&1;
                    b=(imm>>9)&((1<<10)-1);
                    a=(imm>>19);
                    imm=(a<<20)+(b<<1)+(c<<11)+(d<<12);
                    imm=signedExtend(imm,20);
                }//U->ID
                break;
            case 51: rd=(code>>7)&((1<<5)-1);
                f3=(code>>12)&((1<<3)-1);
                rs1=(code>>15)&((1<<5)-1);
                rs2=(code>>20)&((1<<5)-1);
                f7=(code>>25);
                break;//R->ID
            case 35:
            case 99: rd=(code>>7)&((1<<5)-1);//靠右的imm
                f3=(code>>12)&((1<<3)-1);
                rs1=(code>>15)&((1<<5)-1);
                rs2=(code>>20)&((1<<5)-1);
                imm=(code>>25);//最后的imm
                if(opcode==35){
                    imm=(imm<<5)+rd;
                    imm=signedExtend(imm,11);
                }
                else{
                    a=rd&1;rd=(rd>>1);
                    b=(imm>>6);imm=imm&((1<<6)-1);
                    imm=(b<<12)+(a<<11)+(imm<<5)+(rd<<1);
                    imm=signedExtend(imm,12);
                }
                break;//S->ID
            case 3:
            case 19:
            case 103:rd=(code>>7)&((1<<5)-1);
                f3=(code>>12)&((1<<3)-1);
                rs1=(code>>15)&((1<<5)-1);
                imm=(code>>20);
                if(opcode==19&&f3==1||f3==5){
                    rs2=imm&((1<<5)-1);
                    f7=(imm>>5);
                }
                imm=signedExtend(imm,11);
                break;//I->ID
        }
        if(loadlock){
            instruction empty;
            if(rs1!=-1&&lock[rs1]!=0) {bubble=1;return empty;}//rs1 is locked.return a bubble.向上传气泡
            else{bubble=0;rs1=reg[rs1];}//last time may be a bubble.this time 取值.
            if(rs2!=-1&&lock[rs2]!=0) {bubble=1;return empty;}
            else{bubble=0;rs2=reg[rs2];}
        }
        else{
            if(rs1!=-1&&lock[rs1]==0) rs1=reg[rs1];
            else {
                if (lock[rs1] == 1) rs1 = saber;//rs1 is locked.return a bubble.向上传气泡
                else rs1 = avalon;
            }
            if(rs2!=-1&&lock[rs2]==0) rs2=reg[rs2];
            else {
                if (lock[rs2] == 1) rs2 = saber;
                else rs2 = avalon;
            }
            bubble=0;
        }

        if(opcode==111||opcode==103||opcode==99){
            PClock=1;
        }
        if(opcode==111){
            PClock=0;
            result=PC;
            PC+=imm-4;
        }
        return *this;
    }
    instruction EX(){
        switch(opcode){
            case 55: result=imm;if(rd>0)lock[rd]=1;break;//LUI
            case 23: result=imm;if(rd>0)lock[rd]=1;break;//AUIPC
            case 111:if(rd>0)lock[rd]=1;/*result=imm;*/break;//JAL
            case 51: switch(f3){
                    case 0:
                        if(f7==0) result=rs1+rs2;//ADD
                        else result=rs1-rs2;//SUB
                        break;
                    case 1:result=rs2&((1<<5)-1);result=rs1<<result;break;//SLL
                    case 2:result=(rs1<rs2);break;//SLT
                    case 3:result=((unsigned)rs1<(unsigned)rs2);break;//SLTU
                    case 4:result=rs1^rs2;break;//XOR
                    case 5:result=rs2&((1<<5)-1);//SRL&SRA
                        if (f7 == 0){result=(unsigned)rs1>>result;}//SRL
                        else result=rs1>>result;//SRA
                        break;
                    case 6: result=rs1|rs2;break;//OR
                    case 7: result=rs1&rs2;break;//AND
                }
                if(rd>0)lock[rd]=1;
                break;
            case 35: switch(f3) {
                    case 0:
                        result = (rs2 & ((1 << 8) - 1));
                        break;//SB
                    case 1:
                        result = (rs2 & ((1 << 16) - 1));
                        break;//SH
                    case 2:
                        result = rs2;
                        break;//SW
                }
                break;
            case 99: switch(f3){
                    case 0:result=(rs1==rs2);break;//BEQ
                    case 1:result=(rs1!=rs2);break;//BNE
                    case 4:result=(rs1<rs2);break;//BLT
                    case 5:result=(rs1>=rs2);break;//BGE
                    case 6:result=((unsigned)rs1<(unsigned)rs2);break;//BLTU
                    case 7:result=((unsigned)rs1>=(unsigned)rs2);break;//BGEU
                }
                break;
            case 3:result=rs1+imm;if(rd>0)lock[rd]=1;loadlock=1;break;//LB&LH&LW&LBU&LHU
            case 19:switch(f3){
                    case 0:result=rs1+imm;break;//ADDI
                    case 2:result=(rs1<imm);break;//SLTI
                    case 3:result=((unsigned)rs1<(unsigned)imm);break;//SLTIU
                    case 4:result=rs1^imm;break;//XORI
                    case 6:result=rs1|imm;break;//ORI
                    case 7:result=rs1&imm;break;//ANDI

                    case 1:result=(rs1<<imm);break;
                    case 5:
                        if(f7==0) result=((unsigned)rs1>>imm);
                        else result=(rs1>>imm);
                        break;
                }
                if(rd>0)lock[rd]=1;
                break;
            case 103:result=rs1+imm;break;//JALR
        }
        saber=result;
        //if(rd==8)
        //    std::cout<<"EXrd=8:"<<opcode<<' '<<f3<<' '<<rs1<<' '<<rs2<<' '<<saber<<' '<<avalon<<std::endl;
        return *this;
    }
    instruction MEM(){
        int a,b,c,d;
        switch(opcode){
            case 55: break;
            case 23: break;
            case 111:break;
            case 51: break;
            case 35: switch(f3){
                    case 0:memory[rs1+imm]=(char)result;break;
                    case 1:a=(result)&((1<<8)-1);b=result>>8;
                        memory[rs1+imm]=(char)a;memory[rs1+imm+1]=(char)b;
                        break;
                    case 2: a=(result)&((1<<8)-1);b=(result>>8)&((1<<8)-1);
                        c=(result>>16)&((1<<8)-1);d=result>>24;
                        memory[rs1+imm]=(char)a;memory[rs1+imm+1]=(char)b;
                        memory[rs1+imm+2]=(char)c;memory[rs1+imm+3]=(char)d;
                        break;
                }
                break;
            case 99: break;
            case 3: switch(f3){
                    case 0:result=(int)memory[result];break;
                    case 1:result=(int)memory[result]+((int)memory[result+1]<<8);break;
                    case 2:result=(int)memory[result]+((int)memory[result+1]<<8)+
                                  ((int)memory[result+2]<<16)+((int)memory[result+3]<<24);break;
                    case 4:result=(int)memory[result];break;
                    case 5:result=(int)memory[result]+((int)memory[result+1]<<8);break;
                }
                loadlock=0;
                break;
            case 19: break;
            case 103:break;
        }
        if(rd>0&&opcode!=35)lock[rd]=2;
        avalon=result;saber=0;
        //if(rd==8)
        //    std::cout<<"MEMrd=8:"<<opcode<<' '<<f3<<' '<<rs1<<' '<<rs2<<' '<<saber<<' '<<avalon<<std::endl;
        return *this;
    }
    void WB(){
        switch(opcode){
            case 55:if(rd!=0)reg[rd]=result;break;//LUI
            case 23:PC+=imm-4;if(rd!=0)reg[rd]=PC;break;//AUIPC
            case 111:if(rd!=0)reg[rd]=result;/*PC+=imm-4;PClock=0;*/break;//JAL
            case 51: if(rd!=0)
                { switch (f3) {
                        case 0:reg[rd]=result;break;//ADD&SUB
                        case 1:reg[rd]=result;break;//SLL
                        case 2:reg[rd]=result;break;//SLT
                        case 3:reg[rd]=result;break;//SLTU
                        case 4:reg[rd]=result;break;//XOR
                        case 5:
                            if (f7 == 0){reg[rd]=result;}//SRL
                            else reg[rd]=result;//SRA
                            break;
                        case 6:reg[rd]=result;break;//OR
                        case 7:reg[rd]=result;break;//AND
                    }
                }
                break;
            case 35:return;
            case 99: switch(f3){
                    case 0: PC=(result? PC+imm:PC+4)-4;break;
                    case 1: PC=(result? PC+imm:PC+4)-4;break;
                    case 4: PC=(result? PC+imm:PC+4)-4;break;
                    case 5: PC=(result? PC+imm:PC+4)-4;break;
                    case 6: PC=(result? PC+imm:PC+4)-4;break;
                    case 7: PC=(result? PC+imm:PC+4)-4;break;
                }
                PClock=0;
                break;
            case 3:if(rd!=0){
                    switch(f3){
                        case 0:reg[rd]=signedExtend(result,8);break;
                        case 1:reg[rd]=signedExtend(result,16);break;
                        case 2:reg[rd]=result;break;
                        case 4:reg[rd]=result;break;
                        case 5:reg[rd]=result;break;
                    }
            }
            break;
            case 19:if(rd!=0){
                    switch(f3){
                        case 0:
                        case 2:
                        case 3:
                        case 4:
                        case 6:
                        case 7:reg[rd]=result;break;

                        case 1:reg[rd]=result;break;
                        case 5:
                            if(f7==0) reg[rd]=result;
                            else reg[rd]=result;
                            break;
                    }
                }
                break;
            case 103:if(rd!=0)reg[rd]=PC;PC=result&0xfffffffe;PClock=0;break;
        }
        //if(code==0xfeb66ae3)
        //    std::cout<<"WBcode==0xfeb66ae3:"<<lock[11]<<' '<<lock[12]<<' '<<rs1<<' '<<rs2<<' '<<reg[11]<<' '<<reg[12]<<std::endl;
        if(rd!=-1)lock[rd]=0;
        avalon=0;
        code=0;
    }
};

#endif
//RISCV_BRANCH_PREDICTION_OPERATOR_H
