/*
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (c) 2026 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Logger.h"
#include "LoggableItem.h"

#include "../DataModel/DataModel.h"
#include "../DataModel/DataModelNode.h"
#include "../DataModel/DataModelStringLeaf.h"

#include <Arduino.h>
#include <QNEthernet.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/string_view.h>
#include <etl/string_stream.h>

#include <stddef.h>
#include <stdint.h>

Logger::Logger(DataModel &dataModel)
    : lineStream(lineString),
      lineIsDebug(false),
      logging(false),
      logNode("log", &dataModel.sysNode()),
      log1Leaf("1", &logNode, log1Buffer),
      log2Leaf("2", &logNode, log2Buffer),
      log3Leaf("3", &logNode, log3Buffer),
      log4Leaf("4", &logNode, log4Buffer),
      log5Leaf("5", &logNode, log5Buffer),
      logPosition(0),
      logLeaves { &log1Leaf, &log2Leaf, &log3Leaf,
                  &log4Leaf, &log5Leaf } {
}

Logger & Logger::operator << (const Debug &debug) {
    if (!dontLog()) {
        lineIsDebug = true;
    }

    return *this;
}

Logger & Logger::operator << (uint8_t value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (uint16_t value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (uint32_t value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (char value) {
    if (!dontLog()) {
        char cString[2];
        cString[0] = value;
        cString[1] = 0;

        lineStream << cString;
    }

    return *this;
}

Logger & Logger::operator << (int16_t value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (int32_t value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (int value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (size_t value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (float value) {
    if (!dontLog()) {
        lineStream << value;
    }

    return *this;
}

Logger & Logger::operator << (const char *string) {
    if (!dontLog()) {
        lineStream << (string != nullptr ? string : "(nil)");
    }

    return *this;
}

Logger & Logger::operator << (const etl::istring &string) {
    if (!dontLog()) {
        lineStream << string;
    }

    return *this;
}

Logger & Logger::operator << (const etl::string_view &stringView) {
    if (!dontLog()) {
        lineStream << stringView;
    }

    return *this;
}

Logger & Logger::operator << (IPAddress &ip) {
    if (!dontLog()) {
        lineStream << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3];
    }

    return *this;
}

Logger & Logger::operator << (const LoggableItem &item) {
    if (!dontLog()) {
        item.log(*this);
    }

    return *this;
}

Logger & Logger::operator << (const LoggableItem *item) {
    if (!dontLog()) {
        item->log(*this);
    }

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::base_spec &format) {
    if (!dontLog()) {
        lineStream << format;
    }

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::width_spec &format) {
    if (!dontLog()) {
        lineStream << format;
    }

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::fill_spec<char> &format) {
    if (!dontLog()) {
        lineStream << format;
    }

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::precision_spec &format) {
    if (!dontLog()) {
        lineStream << format;
    }

    return *this;
}

Logger & Logger::operator << (const EndOfLine &eol) {
    // If the act of logging a message causes a log to be triggered, we don't want to
    // cause recursion. We also don't want to have the line we're logging get stomped on.
    if (!logging) {
        logging = true;

        if (!lineIsDebug || debugging) {
            Serial.println(lineString.data());
        }
        if (!lineIsDebug) {
            addLineToLog();
        }

        lineIsDebug = false;
        lineStream.str("");

        logging = false;
    }

    return *this;
}

bool Logger::dontLog() const {
    return logging || (lineIsDebug && !debugging);
}

void Logger::addLineToLog() {
    if (logPosition == logDepth) {
        scrollUpLog();
        logPosition = logDepth - 1;
    }

    *(logLeaves[logPosition]) = lineString;

    logPosition++;
}

void Logger::scrollUpLog() {
    for (size_t pos = 0; pos < logDepth - 1; pos++) {
        *(logLeaves[pos]) = *(logLeaves[pos + 1]);
    }
}
