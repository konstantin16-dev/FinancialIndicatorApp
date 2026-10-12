#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QList>
#include <QDebug>
#include "Models.h"

class DatabaseManager {
public:
    static DatabaseManager& instance() {
        static DatabaseManager inst;
        return inst;
    }

    bool open(const QString& host = "localhost", int port = 5432,
              const QString& dbName = "holding_db",
              const QString& user = "postgres",
              const QString& pass = "postgres") {
        if (m_db.isOpen()) return true;

        m_db = QSqlDatabase::addDatabase("QPSQL");
        m_db.setHostName(host);
        m_db.setPort(port);
        m_db.setDatabaseName(dbName);
        m_db.setUserName(user);
        m_db.setPassword(pass);

        if (!m_db.open()) {
            qCritical() << "Ошибка подключения к PostgreSQL:" << m_db.lastError().text();
            return false;
        }
        return true;
    }

    QSqlDatabase& database() { return m_db; }

    QList<Enterprise> getEnterprises() {
        QList<Enterprise> list;
        QSqlQuery q("SELECT id_enterprise, title, inn, kpp, phone, contact_person FROM enterprises ORDER BY id_enterprise", m_db);
        while (q.next()) {
            list.append(Enterprise(q.value(0).toInt(), q.value(1).toString(), q.value(2).toString(),
                                   q.value(3).toString(), q.value(4).toString(), q.value(5).toString()));
        }
        return list;
    }

    bool addEnterprise(const Enterprise& ent) {
        QSqlQuery q(m_db);
        q.prepare("INSERT INTO enterprises (title, inn, kpp, phone, contact_person) VALUES (:t, :inn, :kpp, :ph, :cp);");
        q.bindValue(":t", ent.title());
        q.bindValue(":inn", ent.inn());
        q.bindValue(":kpp", ent.kpp());
        q.bindValue(":ph", ent.phone());
        q.bindValue(":cp", ent.contactPerson());
        return q.exec();
    }

    bool updateEnterprise(const Enterprise& ent) {
        QSqlQuery q(m_db);
        q.prepare("UPDATE enterprises SET title=:t, inn=:inn, kpp=:kpp, phone=:ph, contact_person=:cp WHERE id_enterprise=:id;");
        q.bindValue(":t", ent.title());
        q.bindValue(":inn", ent.inn());
        q.bindValue(":kpp", ent.kpp());
        q.bindValue(":ph", ent.phone());
        q.bindValue(":cp", ent.contactPerson());
        q.bindValue(":id", ent.id());
        return q.exec();
    }

    bool deleteEnterprise(int id) {
        QSqlQuery q(m_db);
        q.prepare("DELETE FROM enterprises WHERE id_enterprise = :id;");
        q.bindValue(":id", id);
        return q.exec();
    }

    bool updateUserProfile(int userId, const QString& fullName, const QString& phone, const QString& newHash = "") {
        QSqlQuery q(m_db);
        if (newHash.isEmpty()) {
            q.prepare("UPDATE system_users SET full_name=:fn, phone=:ph WHERE id_user=:id;");
        } else {
            q.prepare("UPDATE system_users SET full_name=:fn, phone=:ph, password_hash=:pw WHERE id_user=:id;");
            q.bindValue(":pw", newHash);
        }
        q.bindValue(":fn", fullName);
        q.bindValue(":ph", phone);
        q.bindValue(":id", userId);
        return q.exec();
    }

private:
    DatabaseManager() = default;
    ~DatabaseManager() { if (m_db.isOpen()) m_db.close(); }
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
};