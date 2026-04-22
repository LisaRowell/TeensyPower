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

#include "NetworkInterface.h"

#include "../Util/Logger.h"
#include "../Util/Error.h"

#include <Arduino.h>
#include <QNEthernet.h>

#include <Embedded_Template_Library.h>
#include <etl/pool.h>

namespace qn = qindesign::network;

void NetworkInterface::setup() {
    qn::Ethernet.onLinkState(linkStateChanged);
    qn::Ethernet.onInterfaceStatus(interfaceStatusChanged);
    qn::Ethernet.onAddressChanged(ethernetAddressChanged);

    if (!qn::Ethernet.begin()) {
        fatalError("Failed to initialize Ethernet");
    }

    httpServer.begin(80);
}

void NetworkInterface::service() {
    checkForHTTPClients();

    // It's not clear if this is actually needed, but it seems like a good idea
    // to make sure that the stack gets to run.
    qn::Ethernet.loop();
}

void NetworkInterface::checkForHTTPClients() {
    qn::EthernetClient client = httpServer.available();

    if (client) {
        Serial.println("HTTP client connected");

        // An http request ends with a blank line.
        bool currentLineIsBlank = true;

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                if (c == '\n' && currentLineIsBlank) {
                    // If we've gotten to the end of the line (received a newline
                    // character) and the line is blank, the http request has ended,
                    // so we can send a reply.
                    sendHTTPReply(client);
                    break;
                } else if (c == '\n') {
                    // Starting a new line.
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    // Read a character on the current line.
                    currentLineIsBlank = false;
                }
            }
        }

        // Close the connection.
        client.stop();
    }
}


void NetworkInterface::sendHTTPReply(qn::EthernetClient &client) {
    // Send a website that connects to the websocket server and allows to
    // communicate with the us.

    const char* header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n";

    const char* document = 
        "<!DOCTYPE html>\n"
        "<title>Teensy 4.1 Websockets</title>\n"
        "<meta charset='UTF-8'>\n"
        "<style>\n"
        "  body {\n"
        "    display: grid;\n"
        "    grid-template: min-content auto / auto min-content;\n"
        "    grid-gap: 1em;\n"
        "    margin: 0;\n"
        "    padding: 1em;\n"
        "    height: 100vh;\n"
        "    box-sizing: border-box;\n"
        "  }\n"
        "  #output {\n"
        "    grid-column-start: span 2;\n"
        "    overflow-y: scroll;\n"
        "    padding: 0.1em;\n"
        "    border: 1px solid;\n"
        "    font-family: monospace;\n"
        "  }\n"
        "</style>\n"
        "<input type='text' id='message' placeholder='Send a message and Teensy will echo it back!'>\n"
        "<button id='send-message'>send</button>\n"
        "<div id='output'></div>\n"
        "<script>\n"
        "  const url = `ws://${window.location.host}:3000`\n"
        "  const ws = new WebSocket(url)\n"
        "  let connected = false\n"
        "  const sendMessage = document.querySelector('#send-message')\n"
        "  const message = document.querySelector('#message')\n"
        "  const output = document.querySelector('#output')\n"
        "  function log(message, color = 'black') {\n"
        "    const el = document.createElement('div')\n"
        "    el.innerHTML = message\n"
        "    el.style.color = color\n"
        "    output.append(el)\n"
        "    output.scrollTop = output.scrollHeight\n"
        "  }\n"
        "  ws.addEventListener('open', () => {\n"
        "    connected = true\n"
        "    log('(✔️) Open', 'green')\n"
        "  })\n"
        "  ws.addEventListener('close', () => {\n"
        "    connected = false\n"
        "    log('(❌) Close', 'red')\n"
        "  })\n"
        "  ws.addEventListener('message', ({ data }) =>\n"
        "    log(`(💌) ${data}`)\n"
        "  )\n"
        "  sendMessage.addEventListener('click', () => {\n"
        "    connected && ws.send(message.value)\n"
        "  })\n"
        "  message.addEventListener('keyup', ({ keyCode }) => {\n"
        "     connected && keyCode === 13 && ws.send(message.value)\n"
        "  })\n"
        "  log(`(📡) Connecting to ${url} ...`, 'blue')\n"
        "</script>\n";

    client.write(header);
    client.write(document);  
}

void NetworkInterface::linkStateChanged(bool linkState) {
    if (linkState) {
        logger << debug << "Ethernet link UP" << eol;
    } else {
        logger << debug << "Ethernet link DOWN" << eol;
    }
}

void NetworkInterface::interfaceStatusChanged(bool InterfaceStatus) {
    if (InterfaceStatus) {
        logger << debug << "Ethernet interface UP" << eol;
    } else {
        logger << debug << "Ethernet interface DOWN" << eol;
    }
}

void NetworkInterface::ethernetAddressChanged() {
    IPAddress ip = qn::Ethernet.localIP();
    if (ip != INADDR_NONE) {
        IPAddress subnetMask = qn::Ethernet.subnetMask();
        IPAddress gatewayIP = qn::Ethernet.gatewayIP();
        logger << debug << "Ethernet address changed:" << eol;
        logger << debug << "    Local IP = " << ip << eol;
        logger << debug << "    Subnet   = " << subnetMask << eol;
        logger << debug << "    Gateway  = " << gatewayIP << eol;
        IPAddress dnsServer = qn::Ethernet.dnsServerIP();
        if (dnsServer != INADDR_NONE) {
            logger << debug << "    DNS      = " << dnsServer << eol;
        }
    } else {
        logger << debug << "Ethernet address changed: No IP address" << eol;
    }
}
