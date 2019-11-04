#ifndef ANIMATIONDLG_H
#define ANIMATIONDLG_H

#include <QDialog>
#include <QLabel>
#include <QMovie>
#include <QDir>
#include <qstringlist.h>
#include <QString>
#include <QFile>
#include <QTranslator>
#include "uinterface.h"

namespace Ui {
class AnimationDlg;
}

class AnimationDlg : public QDialog
{
    Q_OBJECT

public:
    /********************************
     * enNext: true: would not show this dlg next time;
     *status: 0:jam  1:_IDCardScan  2:_Nin1Copy 0xBD:OutOfPaper 0xBC/0xBE:Jam   0xBF/0xC0:JamExit
    * *******************************************************/
        explicit AnimationDlg(QWidget *parent = 0, int status = 0, bool *enNext = NULL);
        ~AnimationDlg();


private slots:
    void getFinished();
    void on_bt_pause_play_clicked();
    void on_bt_next_clicked();
    void on_bt_back_clicked();
    //void getFilePath();

    void on_radioButton_toggled(bool checked);

    void hideLabel(bool isHidePlayButton);

//    void on_btCancel_clicked();

//    void on_btOK_clicked();

private:
    Ui::AnimationDlg *ui;
    QLabel *label;
    QMovie *movie;
    int index;
    int fquantity;
    bool isPause;
    QDir *videoDir;
    QFileInfoList files;
    bool *enNextShow;
    QMap<QString, QString> map;
};

#endif // ANIMATIONDLG_H
