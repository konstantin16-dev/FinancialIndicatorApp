#pragma once
#include <QString>

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