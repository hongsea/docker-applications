/*
Copyright (c) 2016, SODAQ
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Config.h"
#include "Command.h"
#include "FlashStorage.h"

#define DEFAULT_HEADER 0xBEEF

ConfigParams params;

FlashStorage(flash, ConfigParams);
static bool needsCommit;
static VoidCallbackMethodPtr configResetCallback;

static uint16_t crc16ccitt(const uint8_t *buf, size_t len)
{
    uint16_t crc = 0;
    while (len--) {
        crc ^= (*buf++ << 8);
        for (uint8_t i = 0; i < 8; ++i) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            }
            else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

void ConfigParams::read()
{
    flash.read(this);

    // check header and CRC
    uint16_t calcCRC16 = crc16ccitt((uint8_t*)this, (uint32_t)&params._crc16 - (uint32_t)&params._header);
    if (_header != DEFAULT_HEADER || _crc16 != calcCRC16) {
        reset();
    }
}

void ConfigParams::reset()
{
    _defaultFixInterval = 15;
    _alternativeFixInterval = 0;
    _alternativeFixFromHours = 0;
    _alternativeFixFromMinutes = 0;
    _alternativeFixToHours = 0;
    _alternativeFixToMinutes = 0;
    _gpsFixTimeout = 120;

    _isLedEnabled = 0;  // default is Led disabled
    _isOtaaEnabled = 0; // default is Otaa disabled and ABP enabled
    _isGpsEnabled = 0;  // default is Gps disabled
    _isAckEnabled = 0;  // default is Ack disabled
    _isDbgEnabled = 0;  // default is Ack disabled

    memset(_devAddrOrEUI, 0x30, sizeof(_devAddrOrEUI) - 1);
    _devAddrOrEUI[sizeof(_devAddrOrEUI) - 1] = '\0';

    memset(_appSKeyOrEUI, 0x30, sizeof(_appSKeyOrEUI) - 1);
    _appSKeyOrEUI[sizeof(_appSKeyOrEUI) - 1] = '\0';

    memset(_nwSKeyOrAppKey, 0x30, sizeof(_nwSKeyOrAppKey) - 1);
    _nwSKeyOrAppKey[sizeof(_nwSKeyOrAppKey) - 1] = '\0';

    _chargeOffset = 0;

    _coordinateUploadCount = 1;
    _repeatCount = 5;

    if (configResetCallback) {
        configResetCallback();
    }

    needsCommit = true;
}

/*
 * Write the configuration parameters to NVM / Dataflash
 */
void ConfigParams::commit(bool forced)
{
    if (!forced && !needsCommit) {
        return;
    }

    _header = DEFAULT_HEADER;
    _crc16 = crc16ccitt((uint8_t*)this, (uint32_t)&params._crc16 - (uint32_t)&params._header);

    flash.write(*this);

    needsCommit = false;
}

static const Command args[] = {
    { "Fix Interval (min)        ", "fi=", Command::set_uint16, Command::show_uint16, &params._defaultFixInterval },
    { "Alt. Fix Interval (min)   ", "afi=", Command::set_uint16, Command::show_uint16, &params._alternativeFixInterval },
    { "Alt. Fix From (HH)        ", "affh=", Command::set_uint8, Command::show_uint8, &params._alternativeFixFromHours },
    { "Alt. Fix From (MM)        ", "affm=", Command::set_uint8, Command::show_uint8, &params._alternativeFixFromMinutes },
    { "Alt. Fix To (HH)          ", "afth=", Command::set_uint8, Command::show_uint8, &params._alternativeFixToHours },
    { "Alt. Fix To (MM)          ", "aftm=", Command::set_uint8, Command::show_uint8, &params._alternativeFixToMinutes },
    { "GPS Fix Timeout (sec)     ", "gft=", Command::set_uint16, Command::show_uint16, &params._gpsFixTimeout },

    { "OTAA Mode (OFF=0 / ON=1)  ", "otaa=", Command::set_uint8, Command::show_uint8, &params._isOtaaEnabled },
    { "DevAddr / DevEUI          ", "dev=", Command::set_string, Command::show_string, params._devAddrOrEUI, sizeof(params._devAddrOrEUI) },
    { "AppSKey / AppEUI          ", "app=", Command::set_string, Command::show_string, params._appSKeyOrEUI, sizeof(params._appSKeyOrEUI) },
    { "NWSKey / AppKey           ", "key=", Command::set_string, Command::show_string, params._nwSKeyOrAppKey, sizeof(params._nwSKeyOrAppKey) },

    { "Accu charge offset        ", "cof=", Command::set_uint32, Command::show_uint32, &params._chargeOffset },

    { "Num Coords to Upload      ", "num=", Command::set_uint8, Command::show_uint8, &params._coordinateUploadCount },
    { "Repeat Count              ", "rep=", Command::set_uint8, Command::show_uint8, &params._repeatCount },
    { "Frame Up Count            ", "fup=", Command::set_uint32, Command::show_uint32, &params._frameUpCount },
    { "Frame Down Count          ", "fdw=", Command::set_uint32, Command::show_uint32, &params._frameDownCount },
    { "Status LED (OFF=0 / ON=1) ", "led=", Command::set_uint8, Command::show_uint8, &params._isLedEnabled },
    { "GPS mode (OFF=0 / ON=1)   ", "gps=", Command::set_uint8, Command::show_uint8, &params._isGpsEnabled },
    { "ACK mode (OFF=0 / ON=1)   ", "ack=", Command::set_uint8, Command::show_uint8, &params._isAckEnabled },
    { "Debug mode (OFF=0 / ON=1) ", "dbg=", Command::set_uint8, Command::show_uint8, &params._isDbgEnabled }
};

void ConfigParams::showConfig(Stream* stream)
{
    stream->println();
    stream->println("Settings:");
    for (size_t i = 0; i < sizeof(args) / sizeof(args[0]); ++i) {
        const Command* a = &args[i];
        if (a->show_func) {
            a->show_func(a, stream);
        }
    }
}

/*
 * Execute a command from the commandline
 *
 * Return true if it was a valid command
 */
bool ConfigParams::execCommand(const char* line)
{
    bool done = Command::execCommand(args, sizeof(args) / sizeof(args[0]), line);
    if (done) {
        needsCommit = true;
    }

    return done;
}

/*
 * Check if all required config parameters are filled in
 */
bool ConfigParams::checkConfig(Stream& stream)
{
    bool fail = false;

    if (_alternativeFixFromHours > 23) {
        stream.println("\n\nERROR: \"Alt. Fix From (HH)\" must not be more than 23");

        fail = true;
    }

    if (_alternativeFixToHours > 23) {
        stream.println("\n\nERROR: \"Alt. Fix To (HH)\" must not be more than 23");

        fail = true;
    }


    if (_alternativeFixFromMinutes > 59) {
        stream.println("\n\nERROR: \"Alt. Fix From (MM)\" must not be more than 59");

        fail = true;
    }

    if (_alternativeFixToMinutes > 59) {
        stream.println("\n\nERROR: \"Alt. Fix To (MM)\" must not be more than 59");

        fail = true;
    }

    if (_alternativeFixToMinutes > 59) {
        stream.println("\n\nERROR: \"Alt. Fix To (MM)\" must not be more than 59");

        fail = true;
    }

    if (_coordinateUploadCount < 1 || _coordinateUploadCount > 4) {
        stream.println("\n\nERROR: \"Num Coords to Upload\" must be between 1 and 4");

        fail = true;
    }

    if (_repeatCount < 0 || _repeatCount > 10) {
        stream.println("\n\nERROR: \"Repeat count\" must be between 0 and 10");

        fail = true;
    }

    if (_frameUpCount < 0) {
        stream.println("\n\nERROR: \"Frame Up count\" must be bigger then or equal to 0");

        fail = true;
    }

    if (_frameDownCount < 0) {
        stream.println("\n\nERROR: \"Frame Down count\" must be bigger then or equal to 0");

        fail = true;
    }

    if (_isOtaaEnabled > 1 || _isOtaaEnabled < 0) {
        stream.println("OTAA Mode must be either 0 or 1");
        fail = true;
    }
    if (_isLedEnabled > 1 || _isLedEnabled < 0) {
        stream.println("Led Mode must be either 0 or 1");
        fail = true;
    }
    if (_isGpsEnabled > 1 || _isGpsEnabled < 0) {
        stream.println("GPS Mode must be either 0 or 1");
        fail = true;
    }
    if (_isAckEnabled > 1 || _isAckEnabled < 0) {
        stream.println("ACK Mode must be either 0 or 1");
        fail = true;
    }
    if (_isDbgEnabled > 1 || _isDbgEnabled < 0) {
        stream.println("Debug Mode must be either 0 or 1");
        fail = true;
    }

    return !fail;
}

void ConfigParams::setConfigResetCallback(VoidCallbackMethodPtr callback)
{
    configResetCallback = callback;
}

