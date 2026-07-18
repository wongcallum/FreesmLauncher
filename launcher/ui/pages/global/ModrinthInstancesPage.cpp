// SPDX-License-Identifier: GPL-3.0-only

#include "ModrinthInstancesPage.h"
#include "ui_ModrinthInstancesPage.h"

#include <QHeaderView>
#include <QTableWidgetItem>

#include <algorithm>
#include <functional>

#include "modplatform/modrinth/ModrinthInstances.h"

ModrinthInstancesPage::ModrinthInstancesPage(QWidget* parent) : QWidget(parent), ui(new Ui::ModrinthInstancesPage)
{
    ui->setupUi(this);

    ui->instancesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->instancesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->instancesTable->setColumnWidth(0, 180);

    connect(ui->addButton, &QPushButton::clicked, this, &ModrinthInstancesPage::addInstance);
    connect(ui->removeButton, &QPushButton::clicked, this, &ModrinthInstancesPage::removeSelected);
    connect(ui->instancesTable, &QTableWidget::itemSelectionChanged, this, &ModrinthInstancesPage::updateButtonStates);

    loadSettings();
    updateButtonStates();
}

ModrinthInstancesPage::~ModrinthInstancesPage()
{
    delete ui;
}

QIcon ModrinthInstancesPage::icon() const
{
    auto icon = QIcon::fromTheme("worlds");
    if (icon.isNull())
        icon = QIcon::fromTheme("server");
    return icon;
}

void ModrinthInstancesPage::loadSettings()
{
    ui->instancesTable->setRowCount(0);
    for (const auto& inst : loadUserModrinthInstances())
        appendRow(inst.name, inst.apiUrl);
}

void ModrinthInstancesPage::appendRow(const QString& name, const QString& url)
{
    const int row = ui->instancesTable->rowCount();
    ui->instancesTable->insertRow(row);
    ui->instancesTable->setItem(row, 0, new QTableWidgetItem(name));
    ui->instancesTable->setItem(row, 1, new QTableWidgetItem(url));
}

void ModrinthInstancesPage::addInstance()
{
    appendRow(QString(), QString());
    const int row = ui->instancesTable->rowCount() - 1;
    ui->instancesTable->setCurrentCell(row, 0);
    ui->instancesTable->editItem(ui->instancesTable->item(row, 0));
}

void ModrinthInstancesPage::removeSelected()
{
    // Collect distinct rows first, then drop them top-down so earlier removals
    // don't shift the indices of the ones still to go.
    QList<int> rows;
    for (auto* item : ui->instancesTable->selectedItems())
        if (!rows.contains(item->row()))
            rows.append(item->row());

    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (int row : rows)
        ui->instancesTable->removeRow(row);
}

void ModrinthInstancesPage::updateButtonStates()
{
    ui->removeButton->setEnabled(!ui->instancesTable->selectedItems().isEmpty());
}

bool ModrinthInstancesPage::apply()
{
    QList<ModrinthUserInstance> instances;
    for (int row = 0; row < ui->instancesTable->rowCount(); ++row) {
        auto* nameItem = ui->instancesTable->item(row, 0);
        auto* urlItem = ui->instancesTable->item(row, 1);
        const QString name = nameItem ? nameItem->text().trimmed() : QString();
        const QString url = urlItem ? urlItem->text().trimmed() : QString();
        // saveUserModrinthInstances() drops entries missing a name or URL, so
        // half-filled rows are simply ignored rather than rejected.
        instances.append({ name, url });
    }

    saveUserModrinthInstances(instances);
    return true;
}

void ModrinthInstancesPage::retranslate()
{
    ui->retranslateUi(this);
}
