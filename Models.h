#pragma once
#include <QString>

class User {
public:
    User() : m_id(0) {}
    User(int id, QString login, QString role, QString fullName, QString phone)
        : m_id(id), m_login(std::move(login)), m_role(std::move(role)),
        m_fullName(std::move(fullName)), m_phone(std::move(phone)) {}

    int id() const { return m_id; }
    QString login() const { return m_login; }
    QString role() const { return m_role; }
    QString fullName() const { return m_fullName; }
    QString phone() const { return m_phone; }

private:
    int m_id;
    QString m_login, m_role, m_fullName, m_phone;
};

class Enterprise {
public:
    Enterprise() : m_id(0) {}
    Enterprise(int id, QString title, QString inn, QString kpp, QString phone, QString contactPerson)
        : m_id(id), m_title(std::move(title)), m_inn(std::move(inn)),
        m_kpp(std::move(kpp)), m_phone(std::move(phone)), m_contactPerson(std::move(contactPerson)) {}

    int id() const { return m_id; }
    QString title() const { return m_title; }
    QString inn() const { return m_inn; }
    QString kpp() const { return m_kpp; }
    QString phone() const { return m_phone; }
    QString contactPerson() const { return m_contactPerson; }

private:
    int m_id;
    QString m_title, m_inn, m_kpp, m_phone, m_contactPerson;
};

class IndicatorResult {
public:
    IndicatorResult(QString code, QString title, double value, QString unit, QString norm, QString status)
        : m_code(std::move(code)), m_title(std::move(title)), m_value(value),
        m_unit(std::move(unit)), m_norm(std::move(norm)), m_status(std::move(status)) {}

    QString code() const { return m_code; }
    QString title() const { return m_title; }
    double value() const { return m_value; }
    QString unit() const { return m_unit; }
    QString norm() const { return m_norm; }
    QString status() const { return m_status; }

private:
    QString m_code, m_title;
    double m_value;
    QString m_unit, m_norm, m_status;
};