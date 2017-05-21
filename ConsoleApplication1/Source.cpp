#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
typedef unsigned char uchar;

typedef struct            //MIDI文件
{
	char MIDid[4];                 //文件头块标志MThd,4个字节
	char length1[4];               //头部文件长度，4个字节
	char format[2];                //格式，2个字节
	char tracks[2];                //音轨数目,2个字节
	char ticks[2];                 //基本时间格式类型，每个四分音符的tick数，2个字节
	char MTrk[4];                  //音轨块标志MTrk
	char length2[4];               //音轨块长度
	char start[5];                 //音色定义00 C0 01，音色大钢琴，编号001；最开始的dt和开音00 90
	uchar* mid;             //音轨块实际数据
	char end[3];                   //音轨块结束标志 ff 2f 00
} MIDI;




void CreateMidi(uchar tone[], uchar power[], uchar delta[], int length);

int main()
{
	uchar tone[] = { 0X15+0x30,0X18+0x30,0X17+0x30,0X13+0x30,0X0b+0x30,0X0C+0x30,0X0E + 0x30,0X10+0x30,0X13+0x30};
	uchar delta[] = { 0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0X78};
	uchar power[] = { 0XB1,0X39,0XF0,0XB1,0X39,0X39,0X39,0X39,0X39};

	//音符，响度，延时
	CreateMidi(tone, power, delta, 9);

	printf("Finished");
	return 0;
}

void CreateMidi(uchar tone[], uchar power[], uchar delta[], int length)
{
	int i = 0, j=0,k = 0,n=0,temp=0;
	int tackLength = 0;
	/*定义各部分程序变量*/
	FILE *file;

	MIDI midi;
	midi.mid = (uchar*)malloc(sizeof(uchar)*length*3);
	memset(midi.mid,0, length*3);
	j = length * 3;
	/*转化成MIDI文件*/
	//  file=fopen("TEST.mid", "wb");
	file = fopen("newTEST.rmi", "wb");

	midi.MIDid[0] = 0x4d;          /*文件头块标志MThd*/
	midi.MIDid[1] = 0x54;
	midi.MIDid[2] = 0x68;
	midi.MIDid[3] = 0x64;

	midi.length1[0] = 0x00;        /*文件头块长度，固定为6*/
	midi.length1[1] = 0x00;
	midi.length1[2] = 0x00;
	midi.length1[3] = 0x06;

	midi.format[0] = 0x00;         //单音轨
	midi.format[1] = 0x00;

	midi.tracks[0] = 0x00;         //音轨数目
	midi.tracks[1] = 0x01;

	midi.ticks[0] = 0x01;
	midi.ticks[1] = 0xe0;     //每个4分音符delta-time节奏数


	midi.MTrk[0] = 0x4d;           /*音轨块标志MTrk*/
	midi.MTrk[1] = 0x54;
	midi.MTrk[2] = 0x72;
	midi.MTrk[3] = 0x6b;



	



	//事件开始

	midi.start[0] = 0x00;       //delta-time 0      
	midi.start[1] = 0xC0;      //音色定义，大钢琴声，编号002
	midi.start[2] = 0x02;


	midi.start[3] = 0x00;             /*第一个deltatime*/
	midi.start[4] = 0x90;             //通道1音符打开

	for (i = 0; i < length; i++) {
		midi.mid[n++] = tone[i];
		midi.mid[n++] = power[i];

		temp  = delta[i]/128;
		k = delta[i] % 128;

		if (temp > 0) {
			midi.mid[n++] = 0x80 + temp;
			midi.mid = (uchar*)realloc(midi.mid, j++);
			midi.mid[n++] = k;
		}
		else
		{
			midi.mid[n++] = k;
		}
		/*while (1)
		{
			k = temp % 128;
			temp = temp / 128;
			
			if (temp >= 128) {
				midi.mid[n++] = 0x80+k;
				midi.mid=(char*)realloc(midi.mid, j++);
			}
			else if (temp>0)
			{
				midi.mid[n++] = 0x80 + temp;
				midi.mid= (char*)realloc(midi.mid, j++);
			}
			else
			{
				midi.mid[n++] = k;
				break;

			}
		}*/
	}
	

	midi.end[0] = 0xff;           /*MIDI文件结束标志*/
	midi.end[1] = 0x2f;
	midi.end[2] = 0x00;

	tackLength = j + 5 + 3;
	midi.length2[0] = tackLength >> 24;    // 音轨长度
	midi.length2[1] = tackLength >> 16;
	midi.length2[2] = tackLength >> 8;
	midi.length2[3] = tackLength;
	
	fwrite(midi.MIDid, 4, 1, file);
	fwrite(midi.length1, 4, 1, file);
	fwrite(midi.format, 2, 1, file);
	fwrite(midi.tracks, 2, 1, file);
	fwrite(midi.ticks, 2, 1, file);
	fwrite(midi.MTrk, 4, 1, file);
	fwrite(midi.length2, 4, 1, file);
	fwrite(midi.start, 5, 1, file);
	fwrite(midi.mid, j, 1, file);
	fwrite(midi.end, 3, 1, file);
	
	
	
	fclose(file);
}