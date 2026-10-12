#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTabWidget>
#include "AppController.h"

class AuthDialog : public QDialog {
    Q_OBJECT
public:
    explicit AuthDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Авторизация | Финансовый мониторинг");
        setFixedSize(380, 340);

        auto* tabWidget = new QTabWidget(this);

        auto* tabLogin = new QWidget();
        auto* lLay = new QVBoxLayout(tabLogin);
        m_loginEdit = new QLineEdit();
        m_passEdit = new QLineEdit();
        m_passEdit->setEchoMode(QLineEdit::Password);
        auto* btnLogin = new QPushButton("Войти");
        lLay->addWidget(new QLabel("Логин:"));
        lLay->addWidget(m_loginEdit);
        lLay->addWidget(new QLabel("Пароль:"));
        lLay->addWidget(m_passEdit);
        lLay->addWidget(btnLogin);
        tabWidget->addTab(tabLogin, "Вход");

        auto* tabReg = new QWidget();
        auto* rLay = new QVBoxLayout(tabReg);
        m_regLogin = new QLineEdit();
        m_regPass = new QLineEdit();
        m_regPass->setEchoMode(QLineEdit::Password);
        m_regName = new QLineEdit();
        m_regPhone = new QLineEdit();
        auto* btnReg = new QPushButton("Зарегистрироваться");
        rLay->addWidget(new QLabel("Логин:"));
        rLay->addWidget(m_regLogin);
        rLay->addWidget(new QLabel("Пароль (8+ знаков, спецсимвол, цифра, заглавная):"));
        rLay->addWidget(m_regPass);
        rLay->addWidget(new QLabel("ФИО:"));
        rLay->addWidget(m_regName);
        rLay->addWidget(new QLabel("Телефон:"));
        rLay->addWidget(m_regPhone);
        rLay->addWidget(btnReg);
        tabWidget->addTab(tabReg, "Регистрация");

        auto* mainLay = new QVBoxLayout(this);
        mainLay->addWidget(tabWidget);

        connect(btnLogin, &QPushButton::clicked, this, &AuthDialog::onLogin);
        connect(btnReg, &QPushButton::clicked, this, &AuthDialog::onRegister);
    }

    User currentUser() const { return m_authenticatedUser; }

private slots:
    void onLogin() {
        if (AppController::authenticate(m_loginEdit->text(), m_passEdit->text(), m_authenticatedUser)) {
            accept();
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль!");
        }
    }

    void onRegister() {
        if (!AppController::validatePassword(m_regPass->text())) {
            QMessageBox::critical(this, "Сложность пароля",
                                  "Пароль не соответствует требованиям безопасности!\n"
                                  "- Не менее 8 символов\n- Хотя бы одна цифра\n"
                                  "- Хотя бы одна заглавная буква\n- Хотя бы один спецсимвол");
            return;
        }

        if (AppController::registerUser(m_regLogin->text(), m_regPass->text(), m_regName->text(), m_regPhone->text())) {
            QMessageBox::information(this, "Успех", "Регистрация успешна! Войдите под своей учетной записью.");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось зарегистрировать пользователя (возможно, логин занят).");
        }
    }

private:
    QLineEdit *m_loginEdit, *m_passEdit, *m_regLogin, *m_regPass, *m_regName, *m_regPhone;
    User m_authenticatedUser;
};