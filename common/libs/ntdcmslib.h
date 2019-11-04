#ifndef _NET_CMS_LIB_H_
#define _NET_CMS_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef unsigned short WORD;
//#define DYNAMIC_NTDCMSLIB 1

typedef unsigned char *	(*LPFNLOCK)();//function for get the pointer of output beffer
typedef void	(*LPFNUNLOCK)(int colorStart, int colorEnd, int monoStart, int monoEnd);//function for write data to pointer
typedef void	(*LPFNADDEMPTYLINE)(int n);//funciotn for insert enpty line

typedef struct 
{
	//for Color convert setting
	LPFNLOCK			Lock;//function for get the pointer of output beffer
	LPFNUNLOCK			Unlock;//function for write data to pointer
	LPFNADDEMPTYLINE	AddEmptyLine;//funciotn for insert enpty line
	int					HTMethod;//Color,Halftone and Image process setting as define
	int					HTWidth; //Input Image Width
	int					HTScaleX;//Scale ratio of X(10/100~100)*100
	int					HTScaleY;//Scale ratio of Y(10/100~100)*100//perceptual
	int					HTMediaI; //Input Image Media : 0:Default(Text/Phioto),1:Perceptual(Photo), 2:Colorimetric(WebPage), 3:Saturation(Graphic), 4:Colorimetric_A(POP),5:Nature Scan:6:Vivid Scan,7:Positive Film,8:Nagative Film,9:Monitor RGB
	int					HTMediaO; //Output Image Media: 0:Default(monitor),1:Plane, 2:Coate, 3:Photo, 4:transparency, 5:Glossy, 6:Matte, 9:Monitor RGB
	int					HTResolute; //For descreen resolution setting  1:50dpi, 2:100dpi, 3:150dpi, 4:200dpi, 5:250dpi, 6:300dpi.....
	int					HTDescreen; //For descreen quality 0:skip, 1:Fine Press, 2:Magazine, 3:News Paper
	unsigned char		HTColorI; //Input color mode   1:Gray, 2:RGB, 3:CMY, 4:CMYK, 5:CMYcm, 6:CMYKcm, 7:CMYKcmy, 8:CMcm, 9:Key, 10:YCC, 11:Gray16, 12:RGB16
	unsigned char		HTColorO; //Output color mode  1:Gray, 2:RGB, 3:CMY, 4:CMYK, 5:CMYcm, 6:CMYKcm, 7:CMYKcmy, 8:CMcm, 9:Key, 10:YCC, 11:Gray16, 12:RGB16
	//for Printer function setting
	unsigned char		HTQualityX; //X Resolution 0:300dpi, 1:600dpi, 2:1200dpi, 3:1800dpi, 4:2400dpi
	unsigned char		HTQualityY; //Y Resolution 0:300dpi, 1:600dpi, 2:1200dpi, 3:1800dpi, 4:2400dpi
	int					HTLevels;//Setting the level number for input image 2:Gray 1bit, 222:RGB 1bit, 444:RGB 2bits, 4444:CMYK 2bits, 2222:CMYK 1bit, 444444:CMYKcm 2bits,222222:CMYKcm 1bit
	int					HTFormat;//Format type for swatch 0:Sequence, 1:7 planes(Monza 2bit(CMY) 1bit(K)), 2:4 planes(paris 1bit(CMYK))
	char				*HTPath;//LUT path for convert
}NTDCOLOR;

typedef struct 
{
	int Flag;	// 0:NO Adjustment
				// 1:Temperature and Gamma(profile setting)
				// 2:Icc Profile(profile setting)
				// 4:Brightness, Contrast and Chroma
				// 8:Balance(Balance setting)
				//16:Thermal RGB Gamma 

	char* ProfilePathI;	//Input profile path	
	char* ProfilePathO;	//Output profile path	
	int Temperature;	//Temperature(0:RawDate; 1:5000k; 2:6500k; 3:9300k) and Intent: 16: LCS_GM_IMAGES; 32: LCS_GM_GRAPHICS; 48: LCS_GM_BUSINESS;	64: LCS_GM_ABS_COLORIMETRIC
	int GammaValue;		//Gamma(0:1.0, 1:1.4, 2:1.8, 3:2.2, 4:2.6) 
	int Brightness[4];	//Brightness setting for background,text, graphic, photo
	int Contrast[4];	//Contrast setting for background,text, graphic, photo
	int Chroma[4];		//Chroma setting for background,text, graphic, photo
						//									height Density			medium Density		low Density
						//				  12bit index	[1536,1280,1024,768,512,256]+[ 96,80,64,48,32,16]+[ 6, 5, 4, 3, 2, 1] 
	int BalanceK;		//Balance setting for Black		[	3,	 2,	  1, -1, -2, -3]+[  3, 2, 1,-1,-2,-3]+[ 3, 2, 1,-1,-2,-3]
	int BalanceC;		//Balance setting for Cyan		[	3,	 2,	  1, -1, -2, -3]+[  3, 2, 1,-1,-2,-3]+[ 3, 2, 1,-1,-2,-3]
	int BalanceM;		//Balance setting for Magenta	[	3,	 2,	  1, -1, -2, -3]+[  3, 2, 1,-1,-2,-3]+[ 3, 2, 1,-1,-2,-3]
	int BalanceY;		//Balance setting for Yellow	[	3,	 2,	  1, -1, -2, -3]+[  3, 2, 1,-1,-2,-3]+[ 3, 2, 1,-1,-2,-3]
}ADJUSTSTR;

#ifdef DYNAMIC_NTDCMSLIB
int (*xInitNTDCMS)(NTDCOLOR *);
int (*xStartFilterJob)(int JobNum, int FilterMode, int Gain, int Clamp, int XSize, int YSize, char *Array);
int (*xStartAdjustJob)(int JobNum, ADJUSTSTR *IntputStr);
int (*xStartAdjustCurveJob)(int JobNum, unsigned char *RGBcurve, ADJUSTSTR *IntputStr);
int (*xCreateNTDCMS)(int JobNum);
int (*xNTDCMS)(int JobNum,unsigned char *Src,unsigned char *Info,int Bandsize,int LineIndex);
int (*xEndNTDCMS)(int JobNum);
int (*xStartCisInfoJob)(int JobNum ,int ChipMode,int GapSize,int GapOffset,int ChipSize,int RegistMode,int YSize,char *Array);
int (*xUpdateMatrixConvert)(int JobNum,double *Matrix,int GammaSize, WORD *GammaR, WORD *GammaG, WORD *GammaB);
int (*xCreateRGBCurve16)(int CurveType, int Gamma[3], int Brightness[3], int Contrast[3], int Minlevel[3], int Maxlevel[3], WORD Curve[65536*3]);
#else
int InitNTDCMS(NTDCOLOR *IMGAPP);
int CreateNTDCMS(int JobNum);
int NTDCMS(int JobNum,unsigned char *Src,unsigned char *Info,int Bandsize,int LineIndex);
int EndNTDCMS(int JobNum);
//int AutoCrop(LPBYTE lpDIB,int WidthX, int WidthY ,int Channel,int CropNum ,FILTER_IMAGE *CropArea, int CropThreshold);
//int Draw_CropArea(LPBYTE lpDIB,int WidthX, int WidthY ,int Channel,FILTER_IMAGE CropArea);
int StartCisInfoJob(int JobNum,int ChipMode,int GapSize,int GapOffset,int ChipSize,int RegistMode,int YSize,char *Array);
int StartFilterJob(int JobNum,int FilterMode, int Gain,int Clamp, int XSize,int YSize,char *Array);
int StartAdjustJob(int JobNum, ADJUSTSTR *IntputStr);
int StartAdjustCurveJob(int JobNum,unsigned char *Curve, ADJUSTSTR *IntputStr);
int UpdateMatrixConvert(int JobNum,double *Matrix,int GammaSize, WORD *GammaR, WORD *GammaG, WORD *GammaB);
int CreateRGBCurve(int CurveType,int Gamma[3],int Brightness[3],int Contrast[3],int Minlevel[3],int Maxlevel[3],unsigned char Curve[768]);
#endif
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif	// _NET_CMS_LIB_H_
