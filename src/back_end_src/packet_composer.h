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
#pragma once
#include <iostream>
#include <iterator>
#include <algorithm>
#include <memory>
#include <vector>
#include <functional>
#include "botan.h"
#include "packet_typedef.h"
#include "encryptmsg/algo_spec.h"
#include "algo_defaults.h"
#include "packet_stream.h"
#include "key_service.h"

namespace EncryptPad
{
    struct ProgressEvent
    {
        stream_length_type total_bytes;
        stream_length_type complete_bytes;
        bool cancel;
        ProgressEvent():
            total_bytes(0),
            complete_bytes(0),
            cancel(false)
        {
        }

        ProgressEvent(stream_length_type total_bytes, stream_length_type complete_bytes):
            total_bytes(total_bytes),
            complete_bytes(complete_bytes),
            cancel(false)
        {
        }
    };

    using ProgressCallback = std::function<void(ProgressEvent&)>;

    void DefaultProgressCallback(ProgressEvent &event);

    struct EncryptParams;

    // Secret parameters for encryption and decryption
    struct EncryptParams
    {
        // Passphrase for decryption
        // It needs to be a passphrase because we don't know the salt yet. We'll read it from the file.
        // If passphrase is nullptr, then we'll try to find the key in key_service by salt.
        const std::string *passphrase;

        KeyService *key_service;

        // Encryption parameters to decrypt the key file if it is encrypted
        // If this EncryptParams is for the key file, this field should be null because the key file is never encrypted 
        // with another key file.
        EncryptParams *key_file_encrypt_params;

        // Path to libcurl executable, which is used to download the key file from a remote location such as SSH
        const std::string *libcurl_path;
        const std::string *libcurl_parameters;
        size_t memory_buffer;
        ProgressCallback progress_callback;

        EncryptParams():
            passphrase(nullptr),
            key_service(nullptr),
            key_file_encrypt_params(nullptr),
            libcurl_path(nullptr),
            libcurl_parameters(nullptr),
            memory_buffer(kDefaultMemoryBuffer),
            progress_callback(ProgressCallback(DefaultProgressCallback))
        {}
    };

    // Packets RFC 4880
    // Encrypted Message = {packets below}
    // Symmetric Key Encrypted Session Key Packet = {}, Symmetrically Encrypted Integrity Protected Data Packet = {packets below}
    // Compressed Data Packet = {packets below}, Modification Detection Code Packet (SHA-1 hash function against the data and the prefix replacing iv)
    // Literal Data Packet (see page 46 RFC 4880)

    // 4 bytes' date representation. Not decided yet what it's going to be.
    typedef unsigned int FileDate;

    // This information is not secret. It will be saved into a file unencrypted
    struct PacketMetadata
    {
        unsigned int iterations;
        EncryptMsg::Compression compression;
        EncryptMsg::CipherAlgo cipher_algo;
        EncryptMsg::HashAlgo hash_algo;
        std::string file_name;
        FileDate file_date;
        bool is_binary;
        bool is_armor;
        bool cannot_use_wad;
        EncryptMsg::Salt salt;

        // Key file
        std::string key_file;
        bool key_only;
        bool persist_key_path;

        PacketMetadata()
            :iterations(0), compression(EncryptMsg::Compression::Unknown),
            cipher_algo(EncryptMsg::CipherAlgo::Unknown), hash_algo(EncryptMsg::HashAlgo::Unknown),
            file_name(), file_date(0), is_binary(false), is_armor(false), cannot_use_wad(false),
            salt(), key_file(), key_only(false), persist_key_path(false)
        {
        }
    };
}
