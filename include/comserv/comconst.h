#ifndef __COMCONST_H_
#define __COMCONST_H_

#include "comtypedef.h"

const INT INFO_BUF_SIZE          = 8096;//Êä³öÐÅÏ¢»º³åÇø´óÐ¡¡£¾ö¶¨ÁËÊä³öÐÅÏ¢µÄ×î´ó³¤¶È¡£ 
const INT INFO_PRE_SIZE          = 96;  //Êä³öÐÅÏ¢Ç°×ºµÄ³¤¶È¡£
const INT MAX_INFO_SIZE          = (INFO_BUF_SIZE-INFO_PRE_SIZE);
//´«Èëprint(),errorLogµÈ·½·¨µÄ×Ö·û´®Ó¦¸Ã
//´óÐ¡Ó¦¸Ã<(INFO_BUF_SIZE-INFO_PRE_SIZE)
//¶ÔÓÚCVarChar£¬Ó¦¸ÃÉè¶¨Ò»¸ö×î´ó³¤¶È£¬·ñÔòÈç¹ûsocket´«ËÍÓÐÎó£¬length³ö´í£¬Ôò¿ÉÄÜ½âÂëÊ±Ôì³ÉÔ½½ç£¬´Ó¶øcore dump.
const INT MAX_VCHAR_LENGTH       = 8192;//4096; modified by lxm. 2008.12.31
const INT MAX_FCHAR_LENGTH       = 256; //added by lxm. 2009-02-26
//added by Long Xiangming. 2008.04.02
const INT MAX_ARRAY_SIZE         = 1024;  //ÏûÏ¢¶¨ÒåÖÐ£¬Êý×éµÄ×î´ó³¤¶È¡££¨CVarCharÔòÊ¹ÓÃMAX_VCHAR_LENGTHÏÞÖÆ£©
//µ±socket´«ÊäÊ§°ÜÊ±£¬½âÂëÊý×éÊ±£¬lengthÔ½½ç¿ÉÄÜµ¼ÖÂ¶Î´íÎó¡£Òò´Ë¹æ¶¨Ò»¸ö×î´ó³¤¶ÈÏÞÖÆ¡£

#ifdef WIN32
#define MAXINT 65535  //used in func.h, Unix already pre-defined
#endif

//added by Long Xiangming. 2007.10.20
//ÏµÍ³¶¨Ê±Æ÷£º1¡«9
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
const BYTE COM_MSG_LENGTH       = 4;      /* Bytes */ //¿¿¿¿¿¿¿¿¿¿¿
const BYTE COM_MSG_LENGTH_LENGTH = COM_MSG_LENGTH;
const I32  MaxMsgLength          = 81920;    // 80K. (8192*10). Modified by Long Xiangming. 2007.04.25.
const I32  MAX_MSG_BUF_SIZE      = MaxMsgLength; //¿¿socket¿buffer¿¿¿
// ¿¿MSRP¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿Buffer¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
const I32  MaxResendLength       =  81920; //80K.
const I32  MaxRecvLength           =  81920; //80K.
const I32  MaxEmergencyLength   = 40960;//40K

#endif
