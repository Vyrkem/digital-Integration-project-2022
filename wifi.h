/*
 * wifi.h
 *
 * Created: 2022-01-24 10:54:33
 *  Author: cedric.cloutier
 */ 


#ifndef WIFI_H_
#define WIFI_H_

#define MEG							1048576.0

#define WIFI_QAP					66
#define WIFI_STATION				68
#define WIFI_STOP					70
#define WIFI_GO						72
#define WIFI_CONNECTING				74
#define WIFI_LOCAL_IP				76
#define WIFI_CONNECTED				78

#define SSID						"Tech-11"
#define PASSWD						"243-32687"
#define PORT5000					5000
#define PORT5001					5001
#define PORT5002					5002
#define PORT5003					5003

#define OK							31
#define CONNECTED					33
#define NOTCONNECTED				35
#define IP_RECUE					37
#define DECONNEXION					39

#define IPNODERED					"172.31.158.105"

#endif /* WIFI_H_ */

void init_wifi(void);
uint8_t wifi_event(char *ssid, char *passwd);

uint8_t wifi_etat;
uint8_t wifi_answ;