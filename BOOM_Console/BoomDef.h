#ifndef _BOOM_DEFINE_STRUCT_H
#define _BOOM_DEFINE_STRUCT_H

#define	TAG_READ		0xAEFCBD88DEADC0DE
#define	TAG_WRITE		0xAEFCBD88E0CDC0DE
#define TAG_GETMODULE	0xAEFCBD88C0EDC0DE
#define TAG_PROTECT		0xAEFCBD88ECADC0DE
#define TAG_PROBEREAD	0xAEFCBD880DADC0DE


typedef struct _BOOM_PROCESS_OPERA{
	ULONG64		tag;		//���
	ULONG		flags;		//���
	ULONG64		pid;		//target process
	ULONG64		address;	//��ȡ��ַ
	UCHAR		*buf;		//������ָ��
	ULONG		size;		//��Ҫ��ȡ��С
}BOOM_PROCESS_OPERA,*PBOOM_PROCESS_OPERA;


#endif