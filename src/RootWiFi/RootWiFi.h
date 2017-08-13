#include <WiFiEsp.h>
#include <WiFiEspClient.h>

class RootWiFi {
	private:
		char* ssid;
		char* password;

		char* apName;
		char* apPass;
	public:
		char* getSSID();
		char* getPassword();
		void setSSID(char *ssid, char* password);
		
};