#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QSslSocket>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    if (!QSslSocket::supportsSsl())
    {
        QMessageBox diag;
        diag.setText("OpenSSL not installed. Please install OpenSSL to use this program. The program will now close.");
        diag.exec();

        return 1;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
