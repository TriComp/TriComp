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

private:
    Ui::newKnitDialog *ui;
};

#endif // NEWKNITDIALOG_H
