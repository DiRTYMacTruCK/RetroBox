#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , settings("MyJukeboxApp", "Settings")
{
    ui->setupUi(this);

    // Load saved settings
    QSettings settings("MyJukeboxApp", "Settings");
    ui->libraryPathEdit->setText(settings.value("libraryPath", "").toString());

    // Connect buttons
    connect(ui->btnBrowse, &QPushButton::clicked, this, &SettingsDialog::browseLibraryPath);
    connect(ui->btnSave, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

// Function to browse for a folder
void SettingsDialog::browseLibraryPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Music Library", QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui->libraryPathEdit->setText(dir);
    }
}

// Function to save settings
void SettingsDialog::saveSettings()
{
    QSettings settings("MyJukeboxApp", "Settings");
    settings.setValue("libraryPath", ui->libraryPathEdit->text());
    settings.sync();

    accept(); // Close the dialog
}
