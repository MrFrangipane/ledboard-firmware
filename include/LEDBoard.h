#ifndef PLATFORMIO_LEDBOARD_H
#define PLATFORMIO_LEDBOARD_H


#include <LittleFS.h>

#include <ArtnetEtherENC.h>

#include "Callbacks.h"
#include "ILEDBoard.h"
#include "SerialProtocol.h"
#include "SerialCommunicator.h"
#include "WireOled.h"


namespace Frangitron {

    class LEDBoard : public ILEDBoard {
    public:
        void init() {
            // DISPLAY
            display.init();
            display.setContrast(0);

            // SERIAL COMMUNICATION
            serialCommunicator.setCallbackParent(this);
            serialCommunicator.registerSendCallback(
                SerialProtocol::DataTypeCode::BoardSettingsCode,
                sendSettings
            );
            serialCommunicator.registerReceiveCallback(
                SerialProtocol::DataTypeCode::BoardSettingsCode,
                receiveSettings
            );

            // FILESYSTEM
            LittleFS.begin();
            loadSettings();

            // NETWORK
            const IPAddress ip(
                settings.ipAddress[0],
                settings.ipAddress[1],
                settings.ipAddress[2],
                settings.ipAddress[3]
            );
            pico_unique_board_id_t boardId;
            pico_get_unique_board_id(&boardId);
            uint8_t mac[] = {
                    boardId.id[2],
                    boardId.id[3],
                    boardId.id[4],
                    boardId.id[5],
                    boardId.id[6],
                    boardId.id[7]
            };

            Ethernet.begin(mac, ip);
//            lwipPollingPeriod(10);
//            eth.config(ip);
//            eth.begin(mac);

            // WELCOME
            display.write("Hello " + String(sizeof(settings)));
            display.write(1, 0,
                String(settings.ipAddress[0]) + "." +
                String(settings.ipAddress[1]) + "." +
                String(settings.ipAddress[2]) + "." +
                String(settings.ipAddress[3])
            );

            // ArtNet
            artnetReceiver.subscribeArtDmx(receiveArtNet);
            artnetReceiver.begin();
        }

        void loop() {
            artnetReceiver.parse();
        }

        void loop1() {
            display.pollScreensaver();
            serialCommunicator.poll();

            displayWrite(0, 0, Ethernet.localIP().toString() + "  ");
            displayWrite(1, 10, " " + String(loopCount) + " ");
            loopCount++;
        }

        void displayWrite(uint8_t row, uint8_t column, String text) override {
            display.write(row, column, text);
        }

        const void* getSettings() override {
            return reinterpret_cast<void*>(&settings);
        }

        void setSettings(const void* settings1) override {
            memcpy(&settings, settings1, sizeof(settings));

            const IPAddress ip1(
                settings.ipAddress[0],
                settings.ipAddress[1],
                settings.ipAddress[2],
                settings.ipAddress[3]
            );
            displayWrite(1, 0, ip1.toString() + "   ");
//            netif_set_ipaddr(eth.getNetIf(), ip1);

            if (settings.doSave == 1) {
                saveSettings();
            }
        }

        void loadSettings() override {
            if (!LittleFS.exists("settings.bin")) {
                File f = LittleFS.open("settings.bin", "w");
                f.write(reinterpret_cast<char*>(&settings), sizeof(settings));
                f.close();
            } else {
                File f = LittleFS.open("settings.bin", "r");
                if (f.size() == sizeof(settings)) {
                    f.readBytes(reinterpret_cast<char*>(&settings), sizeof(settings));
                }
                f.close();
            }
        }

        void saveSettings() override {
            File f = LittleFS.open("settings.bin", "w");
            f.write(reinterpret_cast<char*>(&settings), sizeof(settings));
            f.close();
            // rp2040.reboot();
        }

    private:
        SerialCommunicator serialCommunicator;
        WireOled display;
        SerialProtocol::BoardSettings settings;
        ArtnetReceiver artnetReceiver;
        int loopCount = 0;
    };
}

#endif //PLATFORMIO_LEDBOARD_H
