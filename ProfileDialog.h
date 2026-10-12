#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "AppController.h"

class ProfileDialog : public QDialog {
    Q_OBJECT
public:
    ProfileDialog(const User& user, QWidget* parent = nullptr) : QDialog(parent), m_user(user) {
        setWindowTitle("Редактирование профиля");
        setFixedSize(360, 260);

        auto* lay = new QVBoxLayout(this);
        m_nameEdit = new QLineEdit(user.fullName());
        m_phoneEdit = new QLineEdit(user.phone());
        m_passEdit = new QLineEdit();
        m_passEdit->setEchoMode(QLineEdit::Password);
        m_passEdit->setPlaceholderText("Оставьте пустым, если не меняете");

        lay->addWidget(new QLabel("ФИО:"));
        lay->addWidget(m_nameEdit);
        lay->addWidget(new QLabel("Телефон:"));
        lay->addWidget(m_phoneEdit);
        lay->addWidget(new QLabel("Новый пароль (по желанию):"));
        lay->addWidget(m_passEdit);

        auto* btnSave = new QPushButton("Сохранить изменения");
        lay->addWidget(btnSave);

        connect(btnSave, &QPushButton::clicked, this, &ProfileDialog::onSave);
    }

private slots:
    void onSave() {
        QString newPass = m_passEdit->text();
        QString hash = "";
        if (!newPass.isEmpty()) {
            if (!AppController::validatePassword(newPass)) {
                QMessageBox::critical(this, "Ошибка", "Пароль не удовлетворяет требованиям (8+ знаков, цифра, заглавная, спецсимвол)!");
                return;
            }
            hash = AppController::hashPassword(newPass);
        }

        if (DatabaseManager::instance().updateUserProfile(m_user.id(), m_nameEdit->text().trimmed(), m_phoneEdit->text().trimmed(), hash)) {
            QMessageBox::information(this, "Успех", "Данные успешно обновлены!");
            accept();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить изменения.");
        }
    }

private:
    User m_user;
    QLineEdit *m_nameEdit, *m_phoneEdit, *m_passEdit;
};