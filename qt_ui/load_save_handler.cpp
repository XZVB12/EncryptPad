//**********************************************************************************
//EncryptPad Copyright 2016 Evgeny Pokhilko 
//<http://www.evpo.net/encryptpad>
//
//This file is part of EncryptPad
//
//EncryptPad is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 2 of the License, or
//(at your option) any later version.
//
//EncryptPad is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with EncryptPad.  If not, see <http://www.gnu.org/licenses/>.
//**********************************************************************************
#include "load_save_handler.h"
#include <QtGui>
#include <QtGlobal>
#include <QString>
#include <QObject>
#include <QMessageBox>
#include <QDialog>
#include <QFile>
#include "file_encryption.h"
#include "file_name_helper.h"
#include "get_passphrase_dialog.h"
#include "confirm_passphrase_dialog.h"
#include "get_passphrase_dialog.h"
#include "set_encryption_key.h"
#include "get_passphrase_or_key_dialog.h"

namespace EncryptPad
{
    const char *kKeyFilePassphraseWindowTitle = QT_TRANSLATE_NOOP("LoadSaveHandler", "Passphrase for Key File");

    bool LoadHandler::LoadFile(const QString &fileName, bool force_kf_passphrase_request)
    {
        bool can_be_passphrase_protected = true;

        bool is_wad_file = false;
        std::string key_file;
        bool isGpg = IsGpgFormat(fileName);

        if(!QFile::exists(fileName))
        {
            QMessageBox::warning(
                    parent,
                    "EncryptPad",
                    qApp->translate("LoadSaveHandler", "Cannot open the file because it does not exist"));

            return false;
        }

        // Only check if it's an encrypt pad format
        if(!isGpg)
        {
            can_be_passphrase_protected = EncryptPad::CheckIfPassphraseProtected(
                        fileName.toStdString(), is_wad_file, key_file);
        }

        assert(!can_be_passphrase_protected || !is_wad_file);

        bool isPwd = !client.IsPassphraseNotSet();
        bool isKey = !client.EncryptionKeyFile().isEmpty();

        if(!can_be_passphrase_protected && !is_wad_file && (isPwd || isKey))
        {
            // this is an unencrypted file or a corrupted encrypted file. Reset encryption parameters.
            client.SetIsPlainText();
            client.PersistEncryptionKeyPath(false);
            client.EncryptionKeyFile("");
            client.UpdateEncryptionKeyStatus();
        }

        std::string passphrase;
        if(isGpg)
        {
            // Passphrase and key file are both or none. We need to display the key or passphrase dialog.
            if(!(isPwd ^ isKey))
            {
                GetPassphraseOrKeyDialog dlg(parent, client.GetFileRequestService());
                if(dlg.exec() == QDialog::Rejected)
                    return false;

                client.EncryptionKeyFile("");
                client.PersistEncryptionKeyPath(false);
                client.SetIsPlainText();

                if(dlg.IsPassphraseSelected())
                {
                    QString pwdString = dlg.GetPassphrase();
                    QByteArray byte_array = pwdString.toUtf8();
                    const char *pwd = byte_array.constData();
                    passphrase = pwd;
                    if(passphrase.size() > 0)
                        client.SetPassphrase(pwd, metadata);
                }
                else
                {
                    client.EncryptionKeyFile(dlg.GetKeyFilePath());
                    client.PersistEncryptionKeyPath(dlg.GetPersistKeyPath());
                }

                client.UpdateEncryptionKeyStatus();
            }
        }
        else if(client.IsPassphraseNotSet() && can_be_passphrase_protected && !OpenPassphraseDialog(false, &passphrase))
        {
            return false;
        }

        if(is_wad_file) 
        {
            if(!key_file.empty())
            {
                client.EncryptionKeyFile(QString::fromStdString(key_file));
                client.PersistEncryptionKeyPath(true);
                client.UpdateEncryptionKeyStatus();
            }
            else if(client.EncryptionKeyFile().isEmpty() && !OpenSetEncryptionKeyDialogue())
            {
                return false;
            }
        }

        isPwd = !client.IsPassphraseNotSet();
        isKey = !client.EncryptionKeyFile().isEmpty();

        std::string kf_passphrase;
        if((isKey && !client.HasKeyFilePassphrase() && CheckIfKeyFileMayRequirePassphrase(client.EncryptionKeyFile().toStdString()))
                || force_kf_passphrase_request)
        {
            if(!OpenPassphraseDialog(false, &kf_passphrase, false, qApp->translate("LoadSaveHandler", kKeyFilePassphraseWindowTitle)))
                return false;
        }

        if(!isPwd && isKey)
        {
            metadata.key_only = true;
        }

        client.StartLoad(fileName, client.EncryptionKeyFile(), passphrase, metadata, kf_passphrase);
        std::fill(std::begin(passphrase), std::end(passphrase), '0');
        std::fill(std::begin(kf_passphrase), std::end(kf_passphrase), '0');
        return true;
    }

    bool LoadHandler::SaveFile(const QString &fileName)
    {
        bool passphraseSet = !client.IsPassphraseNotSet();
        bool isGpg = IsGpgFormat(fileName);
        bool isArmor = IsArmorFormat(fileName);

        bool isEncryptedFormat = IsEncryptPadFormat(fileName) || isGpg;

        if(isGpg)
        {
            if(client.PersistEncryptionKeyPath())
            {
                auto ret = QMessageBox::warning(
                        parent,
                        "EncryptPad",
                        qApp->translate("LoadSaveHandler", "GPG format does not support persistent key path.") 
                            + QString("\n") +
                            qApp->translate("LoadSaveHandler", "Do you want to disable it?"),
                        QMessageBox::Ok | QMessageBox::Cancel
                        );

                if(ret == QMessageBox::Cancel)
                    return false;

                client.PersistEncryptionKeyPath(false);
                client.UpdateEncryptionKeyStatus();
            }

            if(passphraseSet && !client.EncryptionKeyFile().isEmpty())
            {
                QMessageBox::warning(
                        parent,
                        "EncryptPad",
                        qApp->translate("LoadSaveHandler", "GPG format does not support double protection by passphrase and key file.")
                            + QString("\n") +
                        qApp->translate( "LoadSaveHandler", "Use EPD format or disable either passphrase or key protection."));

                return false;
            }
            else if(!passphraseSet && client.EncryptionKeyFile().isEmpty() && !OpenPassphraseDialog(true))
            {
                return false;
            }

            assert(client.IsPassphraseNotSet() || client.EncryptionKeyFile().isEmpty());
            assert(!client.PersistEncryptionKeyPath());
        }
        else if(!passphraseSet && isEncryptedFormat && !OpenPassphraseDialog(true))
        {
            return false;
        }

        passphraseSet = !client.IsPassphraseNotSet();

        if(isEncryptedFormat && !passphraseSet && client.EncryptionKeyFile().isEmpty())
        {
            auto ret = QMessageBox::warning(
                    parent,
                    "EncryptPad",
                    qApp->translate("LoadSaveHandler", "Neither a key file nor passphrase is set. The file is going to be saved UNENCRYPTED."),
                    QMessageBox::Ok | QMessageBox::Cancel
                    );

            if(ret == QMessageBox::Cancel)
                return false;
        }

        std::string kf_passphrase;

        if(!client.EncryptionKeyFile().isEmpty() && !client.HasKeyFilePassphrase() &&
                CheckIfKeyFileMayRequirePassphrase(client.EncryptionKeyFile().toStdString()) &&
                !OpenPassphraseDialog(false, &kf_passphrase, false, qApp->translate("LoadSaveHandler", kKeyFilePassphraseWindowTitle)))
        {
            return false;
        }

        metadata.cannot_use_wad = isGpg;
        metadata.is_armor = (isGpg && isArmor);

        client.StartSave(fileName, kf_passphrase);
        std::fill(std::begin(kf_passphrase), std::end(kf_passphrase), '0');
        return true;
    }

    bool LoadHandler::OpenPassphraseDialog(bool confirmationEnabled, std::string *passphrase, bool set_client_passphrase, const QString &title)
    {
        if(passphrase)
            passphrase->clear();

        QString pwdString;
        if(confirmationEnabled)
        {
            ConfirmPassphraseDialog dlg(parent);
            if(!title.isEmpty())
                dlg.setWindowTitle(title);
            if(dlg.exec() == QDialog::Rejected)
                return false;
            pwdString = dlg.GetPassphrase();
        }
        else
        {
            GetPassphraseDialog dlg(parent);
            if(!title.isEmpty())
                dlg.setWindowTitle(title);
            if(dlg.exec() == QDialog::Rejected)
                return false;
            pwdString = dlg.GetPassphrase();
        }

        if(pwdString.isEmpty())
        {
            if(set_client_passphrase)
                client.SetIsPlainText();
            return true;
        }

        QByteArray byte_array = pwdString.toUtf8();
        const char *pwd = byte_array.constData();

        if(set_client_passphrase)
            client.SetPassphrase(pwd, metadata);

        if(passphrase)
            *passphrase = pwd;
        return true;
    }

    bool LoadHandler::OpenSetEncryptionKeyDialogue()
    {
        EncryptionKeySelectionResult selection;
        if(!SetEncryptionKey(parent, client.EncryptionKeyFile(), client.PersistEncryptionKeyPath(),
                             client.GetFileRequestService(), selection))
        {
            return false;
        }

        client.EncryptionKeyFile(selection.key_file_path);
        client.PersistEncryptionKeyPath(selection.persist_key_path);

        client.UpdateEncryptionKeyStatus();

        return true;
    }
}
