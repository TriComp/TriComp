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

public slots:
    void createNewKnit(QAbstractButton* b);
    void changeChoice();

private:
    Ui::newKnitDialog *ui;
    QString knit;
};

#endif // NEWKNITDIALOG_H
