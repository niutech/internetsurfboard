/****************************************************************************
**
**  Internet Surfboard (Webbrowser)
**
**  Copyright (C) 2010  Philipp Ruppel
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"
#include "desktopwindowmanager.h"

#include <QNetworkProxy>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{


    ui->setupUi(this);


    ui->proxyTypeComboBox->addItem("");
    ui->proxyTypeComboBox->addItem("");
    ui->proxyTypeComboBox->addItem("");
    ui->proxyTypeComboBox->addItem("");

    ui->proxyTypeComboBox->setItemText(QNetworkProxy::NoProxy, tr("No Proxy"));
    ui->proxyTypeComboBox->setItemText(QNetworkProxy::DefaultProxy, tr("System Default"));
    ui->proxyTypeComboBox->setItemText(QNetworkProxy::Socks5Proxy, tr("Socks 5"));
    ui->proxyTypeComboBox->setItemText(QNetworkProxy::HttpProxy, tr("HTTP"));

    load();



    if(DesktopWindowManager::isSupported())
    {
        setObjectName("xsettingsdialog");
        ui->widget->setObjectName("xsettingscontainer");
    } else {
        setObjectName("settingsdialog");
        ui->widget->setObjectName("settingscontainer");
    }

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::isSupported());
    //ui->widget->setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::isSupported());
    //ui->buttonBox->setAttribute(Qt::WA_TranslucentBackground, DesktopWindowManager::isSupported());


    DesktopWindowManager::setBlurBehind(this);

    //setStyleSheet("#asdf{}");
}

void SettingsDialog::load()
{
    ui->jsCheckBox->setChecked(Settings::getStatic()->value("javascript/enabled", true).toBool());
    ui->pluginsCheckBox->setChecked(Settings::getStatic()->value("plugins/enabled", true).toBool());

    QNetworkProxy proxy = QNetworkProxy::applicationProxy();
    //ui->proxyGroupBox->setChecked(((QNetworkProxy::ProxyType)Settings::getStatic()->value("proxy/type", (int)proxy.type()).toInt()) != QNetworkProxy::DefaultProxy);
    ui->proxyTypeComboBox->setCurrentIndex(Settings::getStatic()->value("proxy/type", /*(int)proxy.type()*/  QNetworkProxy::DefaultProxy).toInt());
    ui->proxyHostLineEdit->setText(Settings::getStatic()->value("proxy/host", proxy.hostName()).toString());
    ui->proxyPortSpinBox->setValue(Settings::getStatic()->value("proxy/port", proxy.port()).toInt());
    ui->proxyUserLineEdit->setText(Settings::getStatic()->value("proxy/user", proxy.user()).toString());
    ui->proxyPasswordLineEdit->setText(Settings::getStatic()->value("proxy/password", proxy.password()).toString());

    ui->homeLineEdit->setText(Settings::getStatic()->value("general/home").toString());
}

void SettingsDialog::apply()
{
    Settings::getStatic()->setValue("javascript/enabled", ui->jsCheckBox->isChecked());
    Settings::getStatic()->setValue("plugins/enabled", ui->pluginsCheckBox->isChecked());

    //Settings::getStatic()->setValue("proxy/type", ui->proxyGroupBox->isChecked() ? ui->proxyTypeComboBox->currentIndex() : QNetworkProxy::DefaultProxy);
    Settings::getStatic()->setValue("proxy/type", ui->proxyTypeComboBox->currentIndex());
    Settings::getStatic()->setValue("proxy/host", ui->proxyHostLineEdit->text());
    Settings::getStatic()->setValue("proxy/port", ui->proxyPortSpinBox->value());
    Settings::getStatic()->setValue("proxy/user", ui->proxyUserLineEdit->text());
    Settings::getStatic()->setValue("proxy/password", ui->proxyPasswordLineEdit->text());

    Settings::getStatic()->setValue("general/home", ui->homeLineEdit->text());

    Settings::getStatic()->apply();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    switch(ui->buttonBox->buttonRole(button))
    {
    case QDialogButtonBox::AcceptRole:
    case QDialogButtonBox::ApplyRole:
    case QDialogButtonBox::YesRole:
        apply();
        break;

    case QDialogButtonBox::RejectRole:
    case QDialogButtonBox::NoRole:
        load();
        break;

    case QDialogButtonBox::ResetRole:
    case QDialogButtonBox::DestructiveRole:
        Settings::getStatic()->loadDefault();
        load();
        Settings::getStatic()->apply();
        break;
    }
}

void SettingsDialog::on_proxyTypeComboBox_currentIndexChanged(int index)
{
    bool b = index != QNetworkProxy::NoProxy && index != QNetworkProxy::DefaultProxy;

    ui->proxyHostLineEdit->setEnabled(b);
    ui->proxyPasswordLineEdit->setEnabled(b);
    ui->proxyPortSpinBox->setEnabled(b);
    ui->proxyUserLineEdit->setEnabled(b);
}
