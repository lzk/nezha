#ifndef IDS_STRING_H
#define IDS_STRING_H

#define PS_READY								0x00
#define PS_PRINTING								0x01
#define PS_POWER_SAVING							0x02
#define PS_WARMING_UP							0x03
#define PS_PENDING_DELETION						0x04
#define PS_PAUSED								0x05
#define PS_WAITING								0x06
#define PS_PROCESSING							0x07
#define PS_BUSY									0x08
#define PS_OFFLINE								0x80
#define PS_TONER_LOW							0x81
#define PS_INITIALIZING							0x82
#define PS_UNKNOWN								0x83
#define PS_ACTIVE								0x84
#define PS_MANUAL_FEED_REQUIRED					0x85

#define PS_ERROR_PAPER_JAM						0xC0
#define PS_ERROR_DOOR_OPEN						0xC1
#define PS_ERROR_OUT_OF_MEMORY					0xC2
#define PS_ERROR_OUT_OF_PAPER					0xC3
#define PS_ERROR_PAPER_PROBLEM					0xC4
#define PS_ERROR_NO_TONER					0xC5
#define PS_ERROR_PAGE_ERROR					0xC6
#define PS_ERROR_NOT_AVAILABLE					0xC7
#define PS_ERROR_OUTPUT_BIN_FULL				0xC8
#define PS_ERROR_NOT_SUPPORT					0xC9
#define PS_ERROR_USER_INTERVENTION_REQUIRED			0xCA

#define PS_ERROR_ADF_COVER_OPEN					0xCB
#define PS_ERROR_ADF_PAPER_JAM					0xCC
#define PS_ERROR_TRAY_DETACHED					0xCD

#define PS_ERROR_POWER_OFF					0xCE
#define PS_ERROR_ERROR						0xCF


#define	IsStatusVirtual(S)	((S)==PS_ERROR_POWER_OFF || (S)==PS_PAUSED || (S)==PS_OFFLINE || (S)==PS_UNKNOWN || (S)==PS_ERROR_NOT_SUPPORT)
#define IsStatusTrans(S)	((S)==PS_POWER_SAVING || (S)==PS_PENDING_DELETION || (S)==PS_WAITING || (S)==PS_BUSY || (S)==PS_INITIALIZING || (S)==PS_ACTIVE)
#define IsStatusNormal(S)	(((S)&0x80)==0x00)
#define IsStatusAbnormal(S)	(((S)&0x80)==0x80)
#define IsStatusError(S)	(((S)&0xC0)==0xC0)
#define IsStatusPowerOff(S)	((S)==0xCE)
#define IsStatusReady(S)	((S)==PS_READY || (S)==PS_TONER_LOW)
#define IsStatusPrinting(S)	((S)==0x01)
#define IsStatusOfWarning(S)	((S)==PS_WARMING_UP || (S)==PS_OFFLINE || (S)==PS_UNKNOWN || (S)==PS_POWER_SAVING || (S)==PS_PENDING_DELETION || (S)==PS_INITIALIZING || (S)==PS_BUSY || (S)==PS_WAITING)
//#define IsStatusUnknownToner(S)	((S)==PS_ERROR_NOT_SUPPORT || (S)==PS_WARMING_UP || (S)==PS_OFFLINE || (S)==PS_ERROR_USER_INTERVENTION_REQUIRED || (S)==PS_INITIALIZING || (S)==PS_BUSY || (S)==PS_UNKNOWN || (S)==PS_ERROR_DOOR_OPEN ||(S)==PS_ERROR_PAPER_JAM || (S)==PS_ERROR_ERROR || (S)==PS_ERROR_ADF_COVER_OPEN || (S)==PS_ERROR_ADF_PAPER_JAM)
//#define IsStatusUnknownToner(S)	((S)==PS_ERROR_NOT_SUPPORT || (S)==PS_WARMING_UP || (S)==PS_OFFLINE || (S)==PS_ERROR_USER_INTERVENTION_REQUIRED || (S)==PS_INITIALIZING || (S)==PS_BUSY || (S)==PS_UNKNOWN || (S)==PS_ERROR_DOOR_OPEN || (S)==PS_ERROR_ERROR || (S)==PS_ERROR_ADF_COVER_OPEN || (S)==PS_ERROR_ADF_PAPER_JAM)
#define IsStatusUnknownToner(S)	((S)==PS_ERROR_NOT_SUPPORT || (S)==PS_WARMING_UP || (S)==PS_OFFLINE || (S)==PS_ERROR_USER_INTERVENTION_REQUIRED || (S)==PS_INITIALIZING || (S)==PS_BUSY || (S)==PS_UNKNOWN || (S)==PS_ERROR_ERROR || (S)==PS_ERROR_ADF_COVER_OPEN || (S)==PS_ERROR_ADF_PAPER_JAM)
#define IsStatusUnknownTray(S)	((S)==PS_ERROR_NOT_SUPPORT || IsStatusVirtual(S) || (S)==PS_WARMING_UP || (S)==PS_INITIALIZING || (S)==PS_BUSY || (S)==PS_ERROR_ERROR || (S)==PS_ERROR_ADF_COVER_OPEN || (S)==PS_ERROR_ADF_PAPER_JAM)


#define IDS_STRPRINTERSELECTTION_EXIT "退出"
#define IDS_STATUSReady         "就绪"
#define IDS_STATUSPrinting      "正在打印"
#define IDS_STATUSPowerSaving   "节电中"
#define IDS_STATUSWarningUp     "正在预热"
#define IDS_STATUSInitializing  "正在初始化"
#define IDS_STATUSPaused        "暂停"
#define IDS_STATUSWaiting       "等待中"
#define IDS_STATUSBusy          "繁忙"
#define IDS_STATUSOffline       "脱机"
#define IDS_STATUSTonerLow      "墨粉量少"
#define IDS_STATUSOutOfPaper    "没有纸张"
#define IDS_STATUSNoToner       "无墨粉"
#define IDS_STATUSDoorOpen      "盖子开着"
#define IDS_STATUSNotAV         "不可用"
#define IDS_STATUSUserIR        "需要用户干预"
#define IDS_STATUSPaperProblem  "纸张问题"
#define IDS_STATUSPageError     "纸张错误"
#define IDS_STATUSPaperJam      "卡纸"
#define IDS_STATUSOutOfM        "内存不足"
#define IDS_STATUSError         "错误"
#define IDS_STRStatusWindow     "状态窗口"
#define IDS_STRPendingDeletion  "正在删除"
#define IDS_STRProcessing       "正在处理"
#define IDS_STRUnknow           "未知"
#define IDS_STRPOWEROFF         "电源关闭"
#define IDS_STRCommunityName    "输入的团体名称中有无效字符。"
#define IDS_STRSelectPrinter    "选择打印机。"
#define IDS_STATUSManualFeedRequired "需要手动送纸"

/////////////////////////////////////////////////////////////////////////////
// Chinese (P.R.C.) resources

//#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_CHS)
//#ifdef _WIN32
//LANGUAGE LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED
//#//ENDif //_WIN32

/////////////////////////////////////////////////////////////////////////////
//
// String Table
//

//STRINGTABLE
//BEGIN
#define IDS_PAPERSIZE_STATEMENT "Statement"
#define IDS_PAPERSIZE_POSTCARD_JPN "明信片"
#define IDS_MEDIATYPE_POSTCARD_JPN "日本明信片"
#define IDS_MEDIATYPE_WPOSTCARD_JPN "W-明信片"
#define IDS_MEDIATYPE_YOUGATA2  "2号西式信封"
#define IDS_MEDIATYPE_YOUGATA3  "3号西式信封"
#define IDS_MEDIATYPE_YOUGATA4  "4号西式信封"
#define IDS_MEDIATYPE_YOUGATA6  "6号西式信封"
#define IDS_MEDIATYPE_NAGAGATA3 "3号长信封"
#define IDS_MEDIATYPE_NAGAGATA4 "4号长信封"
#define IDS_MEDIATYPE_BOND      "债券纸"
#define IDS_MEDIATYPE_YOUNAGA3  "3号西式长信封"
#define IDS_PAPERSIZE_A4        "A4"
#define IDS_PAPERSIZE_A5        "A5"
//END

//STRINGTABLE
//BEGIN
#define IDS_UNKONWN_PAPERSIZE_PAPERTYPE "未知"
//END

//STRINGTABLE
//BEGIN
#define IDS_PAPERSIZE_B5        "B5"
#define IDS_PAPERSIZE_LETTER    "Letter"
#define IDS_PAPERSIZE_FOILO     "Folio"
#define IDS_PAPERSIZE_LEGAL     "Legal"
#define IDS_PAPERSIZE_EXECUTIVE "Executive"
#define IDS_PAPERSIZE_COM10     "Envelope #10"
#define IDS_PAPERSIZE_MONARCH   "Monarch"
#define IDS_PAPERSIZE_DL        "DL"
#define IDS_PAPERSIZE_C5        "C5"
#define IDS_PAPERSIZE_CUSTOM    "自定义纸张尺寸"
#define IDS_MEDIATYPE_COATED    "铜版纸(106-163g/m²)"
#define IDS_MEDIATYPE_COATED_SIDE "涂层纸 - 反面"
#define IDS_MEDIATYPE_COVER1    "封面(106-163g/m²)"
#define IDS_MEDIATYPE_COVER1_SIDE "厚纸1 - 反面"
#define IDS_MEDIATYPE_ENVELOPE  "信封"
#define IDS_MEDIATYPE_LABEL     "标签纸"
//END

//STRINGTABLE
//BEGIN
#define IDS_MEDIATYPE_PLAIN     "普通纸"
#define IDS_MEDIATYPE_PLAINTHICK "普通纸(厚)"
#define IDS_MEDIATYPE_PLANE1_SIDE "普通纸 - 反面"
#define IDS_MEDIATYPE_RECYCLED  "再生纸"
#define IDS_MEDIATYPE_RECYCLED_SIDE "再生纸 - 反面"
#define IDS_MEDIATYPE_THICK_SIDE "债券纸 - 反面"
#define IDS_MEDIATYPE_LIGHT_CARD "厚纸1"
#define IDS_MEDIATYPE_KAKUGATA3 "3号方信封"
#define IDS_MEDIATYPE_LIGHT_GLOSSY_CARD "涂层纸"
#define IDS_MEDIATYPE_JAP_GLOSSY_CARD "涂层纸(薄)"
#define IDS_PAPERSIZE_STATEMENT_G2 "Statement"
//END

//STRINGTABLE
//BEGIN
#define IDS_PRINTER_STATUS_TITLE "打印机状态"
#define IDS_STATUS              "状态"
#define IDS_CONSUMABLES         "消耗品"
//END

//STRINGTABLE
//BEGIN
#define IDS_TONER_IS_GETTING_LOW "墨粉即将耗尽。"
//END

//STRINGTABLE
//BEGIN
#define IDS_DELETING_JOB        "正在删除打印作业。"
//END

//STRINGTABLE
//BEGIN
#define IDS_PS_READY            "打印就绪。"
#define IDS_UNKNOWN_STATUS      "无法获取打印机状态。"
#define IDS_NOT_SUPPORT_PRINTER "不支持已连接的打印机。"
//END

//STRINGTABLE
//BEGIN
#define IDS_REFRESH             "刷新(&R)"
#define IDS_VIEW_PRINT_QUEUE    "查看打印队列(&V)"
#define IDS_CLOSE               "关闭"
#define IDS_HELP                "帮助(&H)..."
#define IDS_USERS_GUIDE         "使用说明书(&G)..."
//END

//STRINGTABLE
//BEGIN
#define IDS_PRINTING_STATUS     "打印状态："
#define IDS_TONER_TEXT          "估计的墨粉量："
#define IDS_NORMAL_READY        "就绪"
#define IDS_NORMAL_PRINTING     "正在打印"
#define IDS_NORMAL_POWER_SAVING "节电中"
#define IDS_NORMAL_WARMING_UP   "正在预热"
#define IDS_NORMAL_PAUSED       "暂停"
#define IDS_NORMAL_WAITING      "等待中"
#define IDS_NORMAL_BUSY         "繁忙"
#define IDS_ERROR_INITIALIZING  "正在初始化"
#define IDS_ERROR_OFFLINE       "脱机"
#define IDS_ERROR_TONER_LOW     "墨粉量少"
#define IDS_WARN_OUT_OF_PAPER   "没有纸张"
#define IDS_WARN_NO_TONER       "无墨粉"
//END

//STRINGTABLE
//BEGIN
#define IDS_WARN_DOOR_OPEN      "盖子开着"
#define IDS_WARN_NOT_AVAILABLE  "不可用"
#define IDS_WARN_USER_INTERVENTION "需要用户干预"
#define IDS_WARN_PAPER_PROBLEM  "纸张问题"
#define IDS_WARN_PAPER_ERROR    "纸张错误"
#define IDS_WARN_PAPER_JAM      "卡纸"
#define IDS_WARN_OUT_OF_MEMORY  "内存不足"
#define IDS_WARN_ERROR          "错误"
#define IDS_TONER_NEED_REPLACE  "更换墨粉来进行彩色打印。"
#define IDS_NORMAL_PROCESSING   "正在处理"
//END

//STRINGTABLE
//BEGIN
#define IDS_MANUAL_FEED_REQUIRED "需要手动送纸"
#define IDS_USER_INTERVENTION_REQUIRED "需要用户干预"
#define IDS_POWER_OFF           "电源关闭"
#define IDS_ERROR_RECOVERY      "错误恢复："
#define IDS_POWER_OFF_DESC      "打印机已关机或未连接电缆。"
#define IDS_MPF_READY           "纸盘：就绪"
#define IDS_MPF_UNKNOWN         "纸盘： --"
#define IDS_MPF_NOPAPER         "纸盘：无纸"
//END

//STRINGTABLE
//BEGIN
#define IDS_PAPER_SRC           "纸张来源："
#define IDS_SHOWMEHOW           "显示操作方式(&S)"
#define IDS_BLACK               "黑色"
#define IDS_CYAN                "青色"
#define IDS_MAGENTA             "洋红色"
#define IDS_YELLOW              "黃色"
//END

//STRINGTABLE
//BEGIN
#define IDS_MEDIATYPE_LIGHTWEIGHT1 "涂层纸1(60-105g/m²)"
#define IDS_MEDIATYPE_LIGHTWEIGHT_RELOADED "涂层纸1(60-105g/m²) - 反面"
#define IDS_MEDIATYPE_POSTCARD_RELOADED "日本明信片 - 反面"
#define IDS_USERS_GUIDE_1       "使用说明书(&G)..."
//END

#//ENDif    // Chinese (P.R.C.) resources
/////////////////////////////////////////////////////////////////////////////

#endif // IDS_STRING_H
