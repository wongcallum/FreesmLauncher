// SPDX-License-Identifier: GPL-3.0-only
//
// Settings page for managing extra Modrinth-API backends browsed alongside the
// public modrinth.com instance (see modplatform/modrinth/ModrinthInstances.h).

#pragma once

#include <QWidget>

#include "ui/pages/BasePage.h"

namespace Ui {
class ModrinthInstancesPage;
}

class ModrinthInstancesPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit ModrinthInstancesPage(QWidget* parent = nullptr);
    ~ModrinthInstancesPage() override;

    QString displayName() const override { return tr("Modrinth Instances"); }
    QIcon icon() const override;
    QString id() const override { return "modrinth-instances"; }
    QString helpPage() const override { return "Mod-platform"; }
    bool apply() override;
    void retranslate() override;

   private slots:
    void addInstance();
    void removeSelected();
    void updateButtonStates();

   private:
    void loadSettings();
    void appendRow(const QString& name, const QString& url);

    Ui::ModrinthInstancesPage* ui;
};
