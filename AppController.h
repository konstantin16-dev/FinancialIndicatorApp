#pragma once
#include <QCryptographicHash>
#include <QRegularExpression>
#include "DatabaseManager.h"
#include "Models.h"

class AppController {
public:
    static QString hashPassword(const QString& password) {
        return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
    }

    static bool validatePassword(const QString& password) {
        static const QRegularExpression regex("^(?=.*[A-ZА-Я])(?=.*\\d)(?=.*[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>/?]).{8,}$");
        return regex.match(password).hasMatch();
    }

    static bool authenticate(const QString& login, const QString& password, User& outUser) {
        QString hash = hashPassword(password);
        QSqlQuery q(DatabaseManager::instance().database());
        q.prepare("SELECT id_user, login, role_name, full_name, phone FROM system_users WHERE login = :l AND password_hash = :p;");
        q.bindValue(":l", login.trimmed());
        q.bindValue(":p", hash);

        if (q.exec() && q.next()) {
            outUser = User(q.value(0).toInt(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString());
            return true;
        }
        return false;
    }

    static bool registerUser(const QString& login, const QString& password, const QString& fullName, const QString& phone) {
        if (!validatePassword(password)) return false;

        QSqlQuery q(DatabaseManager::instance().database());
        q.prepare("INSERT INTO system_users (login, password_hash, role_name, full_name, phone) "
                  "VALUES (:l, :p, 'USER', :fn, :ph);");
        q.bindValue(":l", login.trimmed());
        q.bindValue(":p", hashPassword(password));
        q.bindValue(":fn", fullName.trimmed());
        q.bindValue(":ph", phone.trimmed());
        return q.exec();
    }
};