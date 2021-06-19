#ifndef NEWPROCESSWINDOW_H
#define NEWPROCESSWINDOW_H

#include <QDialog>
#include "OS.h"

class MainWindow;

namespace Ui {
class NewProcessWindow;
}

class NewProcessWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewProcessWindow(QWidget *parent = nullptr);
    ~NewProcessWindow();

    void set_os_ptr(OS*);

    friend class MainWindow;

private slots:
    void on_randomBtn_clicked();

    void on_confirmBtn_clicked();

    void on_propsSelect_activated(int index);

    void on_clearBtn_clicked();

private:
    Ui::NewProcessWindow *ui;
    OS* os;

    void init();
    void before_show();
    bool verify();

signals:
    void addProcessSignal(QString, int, int, int, int, Process*, Process*);
};

#endif // NEWPROCESSWINDOW_H
