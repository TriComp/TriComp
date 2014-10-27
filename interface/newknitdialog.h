#ifndef NEWKNITDIALOG_H
#define NEWKNITDIALOG_H

#include <QDialog>
#include <QAbstractButton>


namespace Ui {
class newKnitDialog;
}

class newKnitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newKnitDialog(QWidget *parent = 0);
    ~newKnitDialog();
    QString getChoice();

signals:
    void newKnit();

public slots:
    void changeChoice();
    void ok();

private:
    Ui::newKnitDialog *ui;
    QString knit;
};

#endif // NEWKNITDIALOG_H
