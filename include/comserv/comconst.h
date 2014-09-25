#ifndef __COMCONST_H_
#define __COMCONST_H_

#include "comtypedef.h"

const INT INFO_BUF_SIZE          = 8096;//�����Ϣ��������С�������������Ϣ����󳤶ȡ� 
const INT INFO_PRE_SIZE          = 96;  //�����Ϣǰ׺�ĳ��ȡ�
const INT MAX_INFO_SIZE          = (INFO_BUF_SIZE-INFO_PRE_SIZE);
//����print(),errorLog�ȷ������ַ���Ӧ��
//��СӦ��<(INFO_BUF_SIZE-INFO_PRE_SIZE)
//����CVarChar��Ӧ���趨һ����󳤶ȣ��������socket��������length��������ܽ���ʱ���Խ�磬�Ӷ�core dump.
const INT MAX_VCHAR_LENGTH       = 8192;//4096; modified by lxm. 2008.12.31
const INT MAX_FCHAR_LENGTH       = 256; //added by lxm. 2009-02-26
//added by Long Xiangming. 2008.04.02
const INT MAX_ARRAY_SIZE         = 1024;  //��Ϣ�����У��������󳤶ȡ���CVarChar��ʹ��MAX_VCHAR_LENGTH���ƣ�
//��socket����ʧ��ʱ����������ʱ��lengthԽ����ܵ��¶δ�����˹涨һ����󳤶����ơ�

#ifdef WIN32
#define MAXINT 65535  //used in func.h, Unix already pre-defined
#endif

//added by Long Xiangming. 2007.10.20
//ϵͳ��ʱ����1��9
const INT T_SYS_MON_CPU          = 1;
const INT T_SYS_MON_MEM          = 2;   
const INT T_MONITOR_SERVERSTATE  = 3;
const INT T_MONITOR_PROCESSSTATE = 4;
const INT T_MONITOR_DBSTATE      = 5;

//for amehandler(command processing)
const I32  MaxArgC               = 20;
const I32  MaxArgVLen            = 255;
const I32  MaxKeyBufferLength    = 512;
const I32  MaxCmdNum             = 100 ;
const I32  MaxCmdLen             = 8000;//255; modified by lxm.2008.12.31 
const I32  MaxCmdResponseLen     = 8000;

//for socket msg
const BYTE COM_MSG_LENGTH       = 4;      /* Bytes */ //�����������
const BYTE COM_MSG_LENGTH_LENGTH = COM_MSG_LENGTH;
const I32  MaxMsgLength          = 81920;    // 80K. (8192*10). Modified by Long Xiangming. 2007.04.25.
const I32  MAX_MSG_BUF_SIZE      = MaxMsgLength; //��socket�buffer���
// ��MSRP������������������������������Buffer����������������
const I32  MaxResendLength       =  81920; //80K.
const I32  MaxRecvLength           =  81920; //80K.
const I32  MaxEmergencyLength   = 40960;//40K

#endif
