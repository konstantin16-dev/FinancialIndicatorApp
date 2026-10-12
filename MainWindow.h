#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "DatabaseManager.h"
#include "ProfileDialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(const User& user, QWidget* parent = nullptr)
        : QMainWindow(parent), m_user(user) {
        setWindowTitle(QString("Финансовый мониторинг холдинга | Пользователь: %1 [%2]").arg(user.fullName(), user.role()));
        resize(1100, 680);

        auto* central = new QWidget(this);
        setCentralWidget(central);
        auto* mainLay = new QVBoxLayout(central);

        auto* topBar = new QHBoxLayout();
        topBar->addWidget(new QLabel("Предприятие:"));
        m_cbEnterprises = new QComboBox();
        m_cbEnterprises->setMinimumWidth(260);
        topBar->addWidget(m_cbEnterprises);

        topBar->addWidget(new QLabel("Период:"));
        m_cbPeriods = new QComboBox();
        m_cbPeriods->setMinimumWidth(160);
        topBar->addWidget(m_cbPeriods);

        auto* btnLoad = new QPushButton("Загрузить отчет (Read)");
        auto* btnProfile = new QPushButton("Мой профиль");
        topBar->addWidget(btnLoad);
        topBar->addWidget(btnProfile);
        topBar->addStretch();
        mainLay->addLayout(topBar);

        m_inputGroup = new QWidget(this);
        auto* grid = new QGridLayout(m_inputGroup);
        grid->addWidget(new QLabel("Выручка (руб):"), 0, 0);
        m_spRev = createSpinBox(150000000.0);
        grid->addWidget(m_spRev, 0, 1);

        grid->addWidget(new QLabel("Чистая прибыль (руб):"), 0, 2);
        m_spNp = createSpinBox(25000000.0);
        grid->addWidget(m_spNp, 0, 3);

        grid->addWidget(new QLabel("Оборотные активы (руб):"), 1, 0);
        m_spCa = createSpinBox(80000000.0);
        grid->addWidget(m_spCa, 1, 1);

        grid->addWidget(new QLabel("Краткосрочные обязательства (руб):"), 1, 2);
        m_spCl = createSpinBox(35000000.0);
        grid->addWidget(m_spCl, 1, 3);

        grid->addWidget(new QLabel("Собственный капитал (руб):"), 2, 0);
        m_spEq = createSpinBox(95000000.0);
        grid->addWidget(m_spEq, 2, 1);

        grid->addWidget(new QLabel("Всего активы (баланс, руб):"), 2, 2);
        m_spTa = createSpinBox(140000000.0);
        grid->addWidget(m_spTa, 2, 3);

        m_btnCalc = new QPushButton("Рассчитать и сохранить отчет (Create / Update)");
        m_btnCalc->setStyleSheet("background-color: #2e7d32; color: white; font-weight: bold; padding: 7px;");
        grid->addWidget(m_btnCalc, 3, 0, 1, 4);
        mainLay->addWidget(m_inputGroup);

        m_table = new QTableWidget(this);
        m_table->setColumnCount(6);
        m_table->setHorizontalHeaderLabels({"Код", "Показатель", "Значение", "Ед. изм.", "Норматив", "Оценка"});
        m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        mainLay->addWidget(m_table);

        auto* btmBar = new QHBoxLayout();
        m_btnAddEnt = new QPushButton("Добавить (Create)");
        m_btnEditEnt = new QPushButton("Изменить (Update)");
        m_btnDelEnt = new QPushButton("Удалить (Delete)");
        auto* btnExport = new QPushButton("Экспорт в CSV");

        btmBar->addWidget(m_btnAddEnt);
        btmBar->addWidget(m_btnEditEnt);
        btmBar->addWidget(m_btnDelEnt);
        btmBar->addStretch();
        btmBar->addWidget(btnExport);
        mainLay->addLayout(btmBar);

        if (m_user.role() != "ADMIN") {
            m_inputGroup->setEnabled(false);
            m_btnAddEnt->setEnabled(false);
            m_btnEditEnt->setEnabled(false);
            m_btnDelEnt->setEnabled(false);
        }

        connect(btnLoad, &QPushButton::clicked, this, &MainWindow::loadReport);
        connect(btnProfile, &QPushButton::clicked, this, &MainWindow::onOpenProfile);
        connect(m_btnCalc, &QPushButton::clicked, this, &MainWindow::calculateAndSave);
        connect(m_btnAddEnt, &QPushButton::clicked, this, &MainWindow::onAddEnterprise);
        connect(m_btnEditEnt, &QPushButton::clicked, this, &MainWindow::onEditEnterprise);
        connect(m_btnDelEnt, &QPushButton::clicked, this, &MainWindow::onDeleteEnterprise);
        connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportCsv);

        loadInitialData();
    }

private:
    QDoubleSpinBox* createSpinBox(double val) {
        auto* sb = new QDoubleSpinBox(this);
        sb->setRange(0.0, 1e12);
        sb->setDecimals(2);
        sb->setValue(val);
        return sb;
    }

private slots:
    void onOpenProfile() {
        ProfileDialog dlg(m_user, this);
        dlg.exec();
    }

    void loadInitialData() {
        m_cbEnterprises->clear();
        auto list = DatabaseManager::instance().getEnterprises();
        for (const auto& ent : list) {
            m_cbEnterprises->addItem(ent.title(), ent.id());
        }

        m_cbPeriods->clear();
        QSqlQuery q("SELECT id_period, period_name FROM reporting_periods ORDER BY id_period", DatabaseManager::instance().database());
        while (q.next()) {
            m_cbPeriods->addItem(q.value(1).toString(), q.value(0).toInt());
        }
    }

    void calculateAndSave() {
        int entId = m_cbEnterprises->currentData().toInt();
        int perId = m_cbPeriods->currentData().toInt();
        if (entId <= 0 || perId <= 0) return;

        double rev = m_spRev->value(), np = m_spNp->value();
        double ca = m_spCa->value(), cl = m_spCl->value();
        double eq = m_spEq->value(), ta = m_spTa->value();

        if (cl <= 0 || ta <= 0 || eq <= 0 || rev <= 0) {
            QMessageBox::warning(this, "Ошибка", "Обязательства, активы, капитал и выручка должны быть > 0!");
            return;
        }

        double cr = ca / cl;
        double ros = (np / rev) * 100.0;
        double roa = (np / ta) * 100.0;
        double roe = (np / eq) * 100.0;
        double eta = eq / ta;

        auto& db = DatabaseManager::instance().database();
        db.transaction();

        QSqlQuery q(db);
        q.prepare("INSERT INTO financial_reports (id_enterprise, id_period, id_user, revenue, net_profit, current_assets, current_liabilities, equity, total_assets) "
                  "VALUES (:e, :p, :u, :rev, :np, :ca, :cl, :eq, :ta) "
                  "ON CONFLICT (id_enterprise, id_period) DO UPDATE SET "
                  "revenue=:rev, net_profit=:np, current_assets=:ca, current_liabilities=:cl, equity=:eq, total_assets=:ta, id_user=:u;");
        q.bindValue(":e", entId); q.bindValue(":p", perId); q.bindValue(":u", m_user.id());
        q.bindValue(":rev", rev); q.bindValue(":np", np); q.bindValue(":ca", ca);
        q.bindValue(":cl", cl); q.bindValue(":eq", eq); q.bindValue(":ta", ta);
        q.exec();

        QSqlQuery qId(db);
        qId.prepare("SELECT id_report FROM financial_reports WHERE id_enterprise=:e AND id_period=:p;");
        qId.bindValue(":e", entId); qId.bindValue(":p", perId);
        qId.exec();
        int repId = qId.next() ? qId.value(0).toInt() : 0;

        struct Res { int id; double val; QString stat; };
        QList<Res> results = {
            {1, cr,  (cr >= 2.0 ? "В норме" : "Ниже норматива")},
            {2, ros, (ros >= 10.0 ? "В норме" : "Ниже норматива")},
            {3, roa, (roa >= 8.0 ? "Высокая" : "Умеренная")},
            {4, roe, (roe >= 15.0 ? "Эффективно" : "Низкая")},
            {5, eta, (eta >= 0.5 ? "Устойчиво" : "Риск потери автономии")}
        };

        for (const auto& r : results) {
            QSqlQuery qVal(db);
            qVal.prepare("INSERT INTO indicator_values (id_report, id_indicator, metric_value, status) "
                         "VALUES (:r, :ind, :v, :s) "
                         "ON CONFLICT (id_report, id_indicator) DO UPDATE SET metric_value=:v, status=:s;");
            qVal.bindValue(":r", repId); qVal.bindValue(":ind", r.id);
            qVal.bindValue(":v", r.val); qVal.bindValue(":s", r.stat);
            qVal.exec();
        }

        db.commit();
        loadReport();
        QMessageBox::information(this, "Успех", "Отчет рассчитан и сохранен в базе данных.");
    }

    void loadReport() {
        int entId = m_cbEnterprises->currentData().toInt();
        int perId = m_cbPeriods->currentData().toInt();

        QSqlQuery qRep(DatabaseManager::instance().database());
        qRep.prepare("SELECT revenue, net_profit, current_assets, current_liabilities, equity, total_assets FROM financial_reports WHERE id_enterprise=:e AND id_period=:p;");
        qRep.bindValue(":e", entId); qRep.bindValue(":p", perId);
        if (qRep.exec() && qRep.next()) {
            m_spRev->setValue(qRep.value(0).toDouble());
            m_spNp->setValue(qRep.value(1).toDouble());
            m_spCa->setValue(qRep.value(2).toDouble());
            m_spCl->setValue(qRep.value(3).toDouble());
            m_spEq->setValue(qRep.value(4).toDouble());
            m_spTa->setValue(qRep.value(5).toDouble());
        }

        m_table->setRowCount(0);
        QSqlQuery qTable(DatabaseManager::instance().database());
        qTable.prepare("SELECT i.indicator_code, i.title, v.metric_value, i.unit, i.norm_val, v.status "
                       "FROM indicator_values v "
                       "JOIN indicators i ON v.id_indicator = i.id_indicator "
                       "JOIN financial_reports r ON v.id_report = r.id_report "
                       "WHERE r.id_enterprise=:e AND r.id_period=:p ORDER BY i.id_indicator;");
        qTable.bindValue(":e", entId); qTable.bindValue(":p", perId);

        if (qTable.exec()) {
            int row = 0;
            while (qTable.next()) {
                m_table->insertRow(row);
                m_table->setItem(row, 0, new QTableWidgetItem(qTable.value(0).toString()));
                m_table->setItem(row, 1, new QTableWidgetItem(qTable.value(1).toString()));
                m_table->setItem(row, 2, new QTableWidgetItem(QString::number(qTable.value(2).toDouble(), 'f', 2)));
                m_table->setItem(row, 3, new QTableWidgetItem(qTable.value(3).toString()));
                m_table->setItem(row, 4, new QTableWidgetItem(qTable.value(4).toString()));

                auto* itemStat = new QTableWidgetItem(qTable.value(5).toString());
                if (qTable.value(5).toString().contains("Ниже") || qTable.value(5).toString().contains("Риск")) {
                    itemStat->setForeground(Qt::red);
                } else {
                    itemStat->setForeground(Qt::darkGreen);
                }
                m_table->setItem(row, 5, itemStat);
                row++;
            }
        }
    }

    void onAddEnterprise() {
        bool ok;
        QString title = QInputDialog::getText(this, "Создание (Create)", "Название предприятия:", QLineEdit::Normal, "", &ok);
        if (ok && !title.isEmpty()) {
            Enterprise ent(0, title, "77" + QString::number(rand() % 90000000 + 10000000), "770101001", "+74950000000", "Руководитель");
            DatabaseManager::instance().addEnterprise(ent);
            loadInitialData();
        }
    }

    void onEditEnterprise() {
        int id = m_cbEnterprises->currentData().toInt();
        if (id <= 0) return;
        bool ok;
        QString title = QInputDialog::getText(this, "Редактирование (Update)", "Новое название предприятия:", QLineEdit::Normal, m_cbEnterprises->currentText(), &ok);
        if (ok && !title.isEmpty()) {
            Enterprise ent(id, title, "7701234567", "770101001", "+74951002030", "Иванов И.И.");
            DatabaseManager::instance().updateEnterprise(ent);
            loadInitialData();
        }
    }

    void onDeleteEnterprise() {
        int id = m_cbEnterprises->currentData().toInt();
        if (id > 0) {
            DatabaseManager::instance().deleteEnterprise(id);
            loadInitialData();
            m_table->setRowCount(0);
        }
    }

    void onExportCsv() {
        if (m_table->rowCount() == 0) {
            QMessageBox::warning(this, "Экспорт", "Нет данных для экспорта!");
            return;
        }

        QString filePath = QFileDialog::getSaveFileName(this, "Сохранить отчет", "Отчет.csv", "CSV Files (*.csv)");
        if (filePath.isEmpty()) return;

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            file.write("\xEF\xBB\xBF"); // UTF-8 BOM для Excel
            out << "Организация:;" << m_cbEnterprises->currentText() << "\n";
            out << "Период:;" << m_cbPeriods->currentText() << "\n";
            out << "Код;Показатель;Значение;Ед. изм.;Норматив;Оценка\n";
            for (int r = 0; r < m_table->rowCount(); ++r) {
                QStringList row;
                for (int c = 0; c < m_table->columnCount(); ++c) row << m_table->item(r, c)->text();
                out << row.join(";") << "\n";
            }
            file.close();
            QMessageBox::information(this, "Экспорт", "Отчет сохранен в файл CSV!");
        }
    }

private:
    User m_user;
    QComboBox *m_cbEnterprises, *m_cbPeriods;
    QWidget *m_inputGroup;
    QDoubleSpinBox *m_spRev, *m_spNp, *m_spCa, *m_spCl, *m_spEq, *m_spTa;
    QPushButton *m_btnCalc, *m_btnAddEnt, *m_btnEditEnt, *m_btnDelEnt;
    QTableWidget *m_table;
};