#include <iostream>
#include<iomanip>
#include<bitset>
#include "operator.hpp"
#include "RISCV.hpp"
using namespace std;

instruction buff[4];
instruction IF(int code){
    /*int opcode;
    opcode=code&((1<<7)-1);
    switch(opcode){
        case 55:
        case 23:
        case 111:U ins;  ins.code=code; ins.ID(); ins.EX(); ins.MEM(); ins.WB(); cout<<"U"<<endl;break;
        case 51: R ins1; ins1.code=code;ins1.ID();ins1.EX();ins1.MEM();ins1.WB();cout<<"R"<<endl;break;
        case 35:
        case 99: S ins2; ins2.code=code;ins2.ID();ins2.EX();ins2.MEM();ins2.WB();cout<<"S"<<endl;break;
        case 3:
        case 19:
        case 103:I ins3; ins3.code=code;ins3.ID();ins3.EX();ins3.MEM();ins3.WB();cout<<"I"<<endl;break;
    }*/
    instruction ins;
    ins.code=code;
    return ins;
}
int main() {
    unsigned char tmp[15];
    int pos=0,temp=0;
    while(cin>>tmp){
        if(tmp[0]=='@'){
            int i;
            for(i=1;i<9;i++){
                if(tmp[i]>='A'&&tmp[i]<='Z')
                    temp+=(tmp[i]-'A'+10)<<(4*(8-i));
                else
                    temp+=(tmp[i]-'0')<<(4*(8-i));
            }
            pos=temp;temp=0;continue;
        }

        if(tmp[0]>='A'&&tmp[0]<='Z')
            memory[pos]=(unsigned char)((tmp[0]-'A'+10)<<4);
        else
            memory[pos]=(unsigned char)((tmp[0]-'0')<<4);

        if(tmp[1]>='A'&&tmp[1]<='Z')
            memory[pos]+=(tmp[1]-'A'+10);
        else
            memory[pos]+=(tmp[1]-'0');
        pos++;
    }//将数据输入到内存中

    /* for(int i=0;i<maxn;i++)if(memory[i]!=0)cout<<hex<<i<<':'<<(int)memory[i]<<' ';
     cout<<endl<<length<<endl;*/
    int Code;
    int i=0;
    instruction e1,e2,e3,e4;
    while(i<3000){
        Code=(int)((memory[PC+3]<<24)+(memory[PC+2]<<16)+(memory[PC+1]<<8)+memory[PC]);
        /*cout<<i++<<' ';
        cout<<hex<<PC<<' ';
        //cout<<bitset<32>(Code)<<endl;
        cout<<hex<<(buff[3].code)<<endl;
        cout<<hex<<(int)memory[PC]<<(int)memory[PC+1]<<(int)memory[PC+2]<<(int)memory[PC+3]<<endl;*/
        if(buff[3].code!=0)buff[3].WB();//lock off
        if(buff[3].opcode==103||buff[3].opcode==99){
            buff[0]=e1;
            buff[1]=e2;
            buff[2]=e3;
            buff[3]=e4;
            Code=(int)((memory[PC+3]<<24)+(memory[PC+2]<<16)+(memory[PC+1]<<8)+memory[PC]);
        }
        if(buff[2].code!=0)buff[3]=buff[2].MEM();
        else buff[3]=e3;
        if(buff[1].code!=0)buff[2]=buff[1].EX();//lock on
        else buff[2]=e2;
        if(buff[0].code!=0){buff[1]=buff[0].ID();}//产生气泡
        else buff[1]=e1;
        if(buff[0].opcode==111){
            Code=(int)((memory[PC+3]<<24)+(memory[PC+2]<<16)+(memory[PC+1]<<8)+memory[PC]);
            buff[0]=IF(Code);
            PC+=4;
        }
        if(!buff[0].bubble&&!PClock&&buff[1].opcode!=111){buff[0]=IF(Code);PC+=4;}
        if(PClock) buff[0]=e1;//把下面的一个变成气泡,之后正常向上走
        /*for(int j=0;j<32;j++)std::cout<<reg[j]<<' ';
        std::cout<<std::endl;*/
        if(memory[0x30004]!=0)  {cout<<((reg[10])&((1<<8)-1))<<endl;break;}
    }

    /*for(int j=0;j<32;j++)cout<<reg[j]<<' ';*/
    return 0;
}
/**第0个寄存器始终为0，尝试改变x0的操作必须失败*/
