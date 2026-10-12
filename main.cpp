#include <QApplication>
#include <QMessageBox>
#include "DatabaseManager.h"
#include "AuthDialog.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if (!DatabaseManager::instance().open("127.0.0.1", 5432, "holding_db", "postgres", "1234")) {
        QMessageBox::critical(nullptr, "Ошибка СУБД PostgreSQL",
                              "Не удалось подключиться к серверу PostgreSQL (holding_db)!\n"
                              "Убедитесь, что служба PostgreSQL запущена, база holding_db создана,\n"
                              "а библиотеки драйвера QPSQL доступны.");
        return -1;
    }

    AuthDialog auth;
    if (auth.exec() == QDialog::Accepted) {
        MainWindow mainWin(auth.currentUser());
        mainWin.show();
        return app.exec();
    }

    return 0;
}