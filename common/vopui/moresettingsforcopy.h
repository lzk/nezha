#ifndef MORESETTINGSFORCOPY_H
#define MORESETTINGSFORCOPY_H

#include <QDialog>
#include <QMouseEvent>
#include <qdebug.h>
#include <QtCore/qglobal.h>
#include <QTimer>
#include <qpushbutton.h>
#include "uinterface.h"

namespace Ui {
class MoreSettingsForCopy;
}

enum DocType_Copy {DocType_Copy_Photo=0, DocType_Copy_Text};
enum DocSize_Copy {DocSize_Copy_A4=0, DocSize_Copy_A5, DocSize_Copy_B5, DocSize_Copy_Letter, DocSize_Copy_Executive};
enum DocDpi_Copy {DocDpi_Copy_DPI300=0, DocDpi_Copy_DPI600};
enum OutPutSize_Copy {OutPutSize_Copy_letter=0, OutPutSize_Copy_A4, OutPutSize_Copy_A5, OutPutSize_Copy_A6, OutPutSize_Copy_B5, OutPutSize_Copy_B6, OutPutSize_Copy_Executive, OutPutSize_Copy_16K};
enum MediaType_Copy {MediaType_Copy_Plain=0, MediaType_Copy_Recycled, MediaType_Copy_Thick, MediaType_Copy_Thin, MediaType_Copy_Label};
enum MultiMode_Copy {OneInOne=0,TwoInOne,FourInOne, NineInOne, IDCardCopyFlag};
enum MultiMode_Copy_ID {A4Mode1=0,A4Mode2,A4Mode3,A5Mode};
enum Duplex_Copy_Mode {Flip_Long_Edge=0,Flip_Short_Edge};

struct Prompt {
    bool isIDCard;
    bool isMultible;
    int isDuplex;
};

struct Param_Copy {

    int scaling;
    DocType_Copy docType;          //doctype
    DocSize_Copy docSize;          //docsize
    DocDpi_Copy docDpi;            //docDpi
    OutPutSize_Copy outputSize;    //outputSize
    MediaType_Copy paperType;      //paperType
    bool isMultiPage;         //multi paper In One
    MultiMode_Copy multiMode;
    int idCardCopyMode;//Credentials Duplex Copy Mode
    int duplexMode;

    // infor support : bool idCard; bool nInOne;

    Prompt promptInfo;

};

static copycmdset default_copy_parameter =
{
//    .Density   = 3,
//    .copyNum   = 1,
//    .scale     = 100
    2,//UINT8 Density         ; // 0  -   0~6
    1,//UINT8 copyNum         ; // 1  -   1~99
    0,//UINT8 scanMode        ; // 2  -   0: Photo, 1: Text, 2: ID card
    0,//UINT8 orgSize         ; // 3  -   0: A4, 1: A5, 2: B5, 3: Letter, 4: Executive
    1,//UINT8 paperSize       ; // 4  -   0: Letter, 1: A4, 2: A5, 3: A6, 4: B5, 5: B6, 6: Executive, 7: 16K
    1,//UINT8 nUp             ; // 5  -   0:1up, 1: 2up, 3: 4up, 4: 9up
    0,//UINT8 dpi             ; // 6  -   0: 300*300, 1: 600*600
    0,//UINT8 mediaType       ; // 7  -   0: plain paper 1: Recycled paper 2: Thick paper 3: Thin paper 4: Label
    100,//UINT16 scale          ; // 8  -   25~400, disabled for 2/4/9up
};

class MoreSettingsForCopy : public QDialog
{
    Q_OBJECT

public:
    explicit MoreSettingsForCopy(QWidget *parent = 0,bool duplexCopyFlag = false, bool idCardFlay = false,Param_Copy *pParam = NULL ,int mode = 0);
    ~MoreSettingsForCopy();

private:
    Ui::MoreSettingsForCopy *ui;

    bool _idCardFlag;
    bool _duplexCopyFlag;
    Param_Copy defParam;
    Param_Copy *ParamForCopy;

    QPoint last;
    QTimer *timer;
    int timeCount;

    int _idCardCopyMode;
    MultiMode_Copy _multiMode;


protected:
    bool isPress;
    void selectMode(MultiMode_Copy mode);
    void setDefault();
    void showParam(Param_Copy *param);

public slots:
    bool eventFilter(QObject *,QEvent *);
private slots:
    void on_btOK_clicked();
    void on_btDefault_clicked();

    void on_btReduce_clicked();
    void on_btAdd_clicked();


    void on_scaling_editingFinished();
    void on_timeout_add();
    void on_timeout_reduce();

    void on_isNinOne_toggled(bool checked);
    void on_bt2in1_clicked();
    void on_bt4in1_clicked();
    void on_bt9in1_clicked();
    void on_btID_clicked(bool checked);
    void on_btNInOne_clicked(bool checked);
    void on_btn_duplex_clicked(bool checked);
//    void on_btText_toggled(bool checked);
//    void on_btPicture_toggled(bool checked);
    void on_docSizeList_currentIndexChanged(int index);
    void on_dpiList_currentIndexChanged(int index);
    void on_outPutSizeList_currentIndexChanged(int index);
//    void on_paperTypeList_currentIndexChanged(int index);
    void on_btReduce_pressed();
    void on_btReduce_released();
    void on_btAdd_pressed();
    void on_btAdd_released();
    void on_scaling_textEdited(const QString &arg1);
    int getScalingValue(int outputSize, int inputSize);

    void enableIDCardCopyMode(bool checked);
    void selectIDCardCopyMode(int mode);
    void on_btA4_1_clicked();
    void on_btA4_2_clicked();
    void on_btA4_3_clicked();
    void on_btA4_4_clicked();

};

#endif // MORESETTINGSFORCOPY_H
