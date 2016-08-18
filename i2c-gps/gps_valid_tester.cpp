#include "i2c-gps.h"
#include "serial-gps.h"
#include "Ublox.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
const char * test_data[] = {
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,19,85,55,153,32,86,65,323,*6C\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02882,N,11619.64081,E,100745.00,A,D*74\r\n",
		"$GNZDA,100745.00,18,08,2016,00,00*7B\r\n",
		"$GNRMC,100746.00,A,4000.02881,N,11619.64088,E,0.689,260.63,180816,,,D*7A\r\n",
		"$GNVTG,260.63,T,,M,0.689,N,1.277,K,D*23\r\n",
		"$GNGGA,100746.00,4000.02881,N,11619.64088,E,2,10,1.97,60.0,M,-8.8,M,,0000*6B\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,37,05,73,304,,06,27,109,32,07,11,072,40*73\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,16,15,11,204,17,19,04,158,*70\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,20,30,09,097,34*71\r\n",
		"$GPGSV,4,4,14,42,35,140,30,50,35,140,31*7E\r\n",
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,19,85,55,153,32,86,65,323,*6C\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02881,N,11619.64088,E,100746.00,A,D*7D\r\n",
		"$GNZDA,100746.00,18,08,2016,00,00*78\r\n",
		"$GNRMC,100747.00,A,4000.02882,N,11619.64105,E,0.670,261.08,180816,,,D*76\r\n",
		"$GNVTG,261.08,T,,M,0.670,N,1.241,K,D*2C\r\n",
		"$GNGGA,100747.00,4000.02882,N,11619.64105,E,2,10,1.97,60.2,M,-8.8,M,,0000*6F\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,37,05,73,304,,06,27,109,33,07,11,072,41*73\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,17,15,11,204,16,19,04,158,*70\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,21,30,09,097,34*70\r\n",
		"$GPGSV,4,4,14,42,35,140,30,50,35,140,31*7E\r\n",
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,20,85,55,153,32,86,65,323,*66\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02882,N,11619.64105,E,100747.00,A,D*7B\r\n",
		"$GNZDA,100747.00,18,08,2016,00,00*79\r\n",
		"$GNRMC,100748.00,A,4000.02884,N,11619.64118,E,0.676,261.03,180816,,,D*7E\r\n",
		"$GNVTG,261.03,T,,M,0.676,N,1.253,K,D*22\r\n",
		"$GNGGA,100748.00,4000.02884,N,11619.64118,E,2,10,1.97,60.3,M,-8.8,M,,0000*6B\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,37,05,73,304,,06,27,109,33,07,11,072,41*73\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,17,15,11,204,16,19,04,158,*70\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,21,30,09,097,34*70\r\n",
		"$GPGSV,4,4,14,42,35,140,30,50,35,140,31*7E\r\n",
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,20,85,55,153,32,86,65,323,*66\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02884,N,11619.64118,E,100748.00,A,D*7E\r\n",
		"$GNZDA,100748.00,18,08,2016,00,00*76\r\n",
		"$GNRMC,100749.00,A,4000.02887,N,11619.64137,E,0.482,,180816,,,D*60\r\n",
		"$GNVTG,,T,,M,0.482,N,0.892,K,D*35\r\n",
		"$GNGGA,100749.00,4000.02887,N,11619.64137,E,2,10,1.97,60.6,M,-8.8,M,,0000*61\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,37,05,73,304,,06,27,109,33,07,11,072,41*73\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,17,15,11,204,16,19,04,158,*70\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,22,30,09,097,34*73\r\n",
		"$GPGSV,4,4,14,42,35,140,30,50,35,140,31*7E\r\n",
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,20,85,55,153,32,86,65,323,*66\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02887,N,11619.64137,E,100749.00,A,D*71\r\n",
		"$GNZDA,100749.00,18,08,2016,00,00*77\r\n",
		"$GNRMC,100750.00,A,4000.02889,N,11619.64154,E,0.303,,180816,,,D*6D\r\n",
		"$GNVTG,,T,,M,0.303,N,0.561,K,D*3A\r\n",
		"$GNGGA,100750.00,4000.02889,N,11619.64154,E,2,10,1.97,60.8,M,-8.8,M,,0000*6C\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,37,05,73,304,,06,27,109,33,07,11,072,41*73\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,18,15,11,204,15,19,04,158,*7C\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,22,30,09,097,34*73\r\n",
		"$GPGSV,4,4,14,42,35,140,30,50,35,140,31*7E\r\n",
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,19,85,55,153,31,86,65,323,*6F\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02889,N,11619.64154,E,100750.00,A,D*72\r\n",
		"$GNZDA,100750.00,18,08,2016,00,00*7F\r\n",
		"$GNRMC,100751.00,A,4000.02890,N,11619.64163,E,0.324,,180816,,,D*65\r\n",
		"$GNVTG,,T,,M,0.324,N,0.600,K,D*3B\r\n",
		"$GNGGA,100751.00,4000.02890,N,11619.64163,E,2,10,1.97,60.9,M,-8.8,M,,0000*60\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,36,05,73,304,,06,27,109,33,07,11,072,41*72\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,19,15,11,204,16,19,04,158,*7E\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,22,30,09,097,34*73\r\n",
		"$GPGSV,4,4,14,42,35,140,30,50,35,140,31*7E\r\n",
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,19,85,55,153,32,86,65,323,*6C\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02890,N,11619.64163,E,100751.00,A,D*7F\r\n",
		"$GNZDA,100751.00,18,08,2016,00,00*7E\r\n",
		"$GNRMC,100752.00,A,4000.02892,N,11619.64180,E,0.362,,180816,,,D*6B\r\n",
		"$GNVTG,,T,,M,0.362,N,0.670,K,D*3E\r\n",
		"$GNGGA,100752.00,4000.02892,N,11619.64180,E,2,10,1.97,61.2,M,-8.8,M,,0000*66\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,36,05,73,304,,06,27,109,33,07,11,072,41*72\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,20,15,11,204,15,19,04,158,*77\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,22,30,09,097,34*73\r\n",
		"$GPGSV,4,4,14,42,35,140,31,50,35,140,31*7F\r\n",
		"$GLGSV,3,1,09,70,50,029,42,71,67,261,,72,18,232,,78,00,358,*6D\r\n",
		"$GLGSV,3,2,09,79,03,035,,84,06,149,19,85,55,153,32,86,65,323,*6C\r\n",
		"$GLGSV,3,3,09,87,09,328,*53\r\n",
		"$GNGLL,4000.02892,N,11619.64180,E,100752.00,A,D*73\r\n",
		"$GNZDA,100752.00,18,08,2016,00,00*7D\r\n",
		"$GNRMC,100753.00,A,4000.02893,N,11619.64194,E,0.392,,180816,,,D*61\r\n",
		"$GNVTG,,T,,M,0.392,N,0.727,K,D*32\r\n",
		"$GNGGA,100753.00,4000.02893,N,11619.64194,E,2,10,1.97,61.4,M,-8.8,M,,0000*65\r\n",
		"$GNGSA,A,3,02,06,07,30,09,42,50,,,,,,2.49,1.97,1.52*10\r\n",
		"$GNGSA,A,3,85,70,84,,,,,,,,,,2.49,1.97,1.52*1C\r\n",
		"$GPGSV,4,1,14,02,69,090,36,05,73,304,,06,27,109,33,07,11,072,41*72\r\n",
		"$GPGSV,4,2,14,09,07,038,35,13,35,178,19,15,11,204,14,19,04,158,*7C\r\n",
		"$GPGSV,4,3,14,20,24,260,,25,08,264,,29,39,306,22,30,09,097,35*72\r\n",
		"$GPGSV,4,4,14,42,35,140,30,50,35,140,31*7E\r\n",
};
unsigned char gps_config_change[] = {

		/* CFG_GNSS: GPS & GLONASS with SBAS */
		0xB5, 0x62, 0x06, 0x3E, 0x34, 0x00, 0x00, 0x00, 0x20,
		0x06, 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01,
		0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x03,
		0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x04, 0x00,
		0x08, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03,
		0x00, 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E, 0x00,
		0x01, 0x00, 0x01, 0x01, 0x15, 0xED,

		/* CFG_SBAS: MSAS */
		0xB5, 0x62, 0x06, 0x16, 0x08, 0x00, 0x01, 0x03, 0x03,
		0x00, 0x00, 0x02, 0x02, 0x00, 0x2F, 0xC3,

		/*timepulse*/
		/* 5Hz, 50% */
		0xB5, 0x62, 0x06, 0x31, 0x20,0x00,0x00,0x01,0x00,0x00,
		0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x00,
		0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0xAB,0x00,0x00,0x00,0x89,0xA2

};

#define SIZE_PER_PLATFORM 44
unsigned char gps_platform_model_data[] = {
		/* CFG_NAV5: Platform model: 2-Stationary */
		0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x02,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
		0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E,
		0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4B, 0xD2,

		/* CFG_NAV5: Platform model: 3-Pedestrian */
		0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x03,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
		0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E,
		0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0xF4,

		/* CFG_NAV5: Platform model: 4-Automotive */
		0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x04,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
		0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E,
		0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x16,

		/* CFG_NAV5: Platform model: 0-Portable */
		0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
		0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E,
		0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x8E,

};

#define MAX_PLATFORM_MODEL_SN	3
int gps_platform_model_sn = 0;
bool changeGpsplatformFlag = 0;

void* gpsdata_decode_loop(void * parm) {
	Ublox *M8_Gps_ = (Ublox*)parm;
	i2cgps gps;
	serialGps gps_serial;
	bool ret;

	if(changeGpsplatformFlag) {
		gps.write_gps_config(gps_platform_model_data + (gps_platform_model_sn * SIZE_PER_PLATFORM), SIZE_PER_PLATFORM);
	} else {
		printf("changeGpsplatformFlag error!\n");
	}

	printf("%d\n",gps.write_gps_config(gps_config_change, sizeof(gps_config_change)));

	int bytesread;
	while(1) {
		bytesread = gps_serial.get_gps_data2buf();
	    if(bytesread < 0){
	    	printf("gpsdata_decode_loop: bytes<0!\n");
	    	exit(0);
	    }

		for (int i = 0; i < bytesread; i++) {
			M8_Gps_->encode((char)(gps_serial.gpsdata_buf())[i]);

		}

		pthread_testcancel();
	}
}

//void* gpsdata_decode_loop(void * parm) {
//	Ublox *m8_Gps = (Ublox*)parm;
////	i2cgps gps;
////	serialGps gps_serial;
//	bool ret;
//
//	int bytesread;
//	int i;
//	for(i=0;i<110;i++){
//		bytesread = strlen(test_data[i]);
//		printf("%d\n", i+1);
//		for (int j = 0; j < bytesread; j++) {
//			m8_Gps->encode(test_data[i][j]);
//		}
//		if(1){
//			printf("%f, %f, fix_type: %d\n", m8_Gps->latitude, m8_Gps->longitude, m8_Gps->fixtype);
//		} else {
//			printf("position invalid!!\n");
//		}
//
//		if(1){
//			printf("%d-%d-%d, %d:%d:%d\n", m8_Gps->datetime.year, m8_Gps->datetime.month, m8_Gps->datetime.day,
//					m8_Gps->datetime.hours, m8_Gps->datetime.minutes, m8_Gps->datetime.seconds);
//		} else {
//			printf("time invalid!!\n");
//		}
//	}
//
//}

int usage(){
	printf("Parameter Error!\n");
	printf("-P [gps platform model (0~3)] : 0-Stationary, 1-Pedestrian, 2-Automotive, 3-Portabl, \n");
	return -1;
}

int main(int argc, char **argv){
	pthread_t gpstid;
	argc--;
	argv++;

	while(argc > 0) {
		if(argv[0][0]=='-' && argv[0][1]=='P') {
			if (isdigit(argv[0][2])) {
				return usage();
			} else {
				if(argc < 2 || argv[0][2] != '\0') return usage();
				if(gps_platform_model_sn < 0 || gps_platform_model_sn > MAX_PLATFORM_MODEL_SN) {
					printf("Unsupported GPS platform model!\n");
					return usage();
				}
				gps_platform_model_sn = atoi(argv[1]);
				changeGpsplatformFlag = 1;
				argc--;
				argv++;
			}
		}
		argc--;
		argv++;
	}
	if(!changeGpsplatformFlag){
		return usage();
	}
	Ublox *m8_Gps = new Ublox();
	pthread_create(&gpstid, NULL, gpsdata_decode_loop, (void*)m8_Gps);

	while(1){
		system("clear");
		if(m8_Gps->fixtype > 0){
			printf("%f, %f, fix_type: %d\n", m8_Gps->latitude, m8_Gps->longitude, m8_Gps->fixtype);
		} else {
			printf("position invalid!!\n");
		}

		if(m8_Gps->datetime.valid){
			printf("%d-%d-%d, %d:%d:%d\n", m8_Gps->datetime.year, m8_Gps->datetime.month, m8_Gps->datetime.day,
					m8_Gps->datetime.hours, m8_Gps->datetime.minutes, m8_Gps->datetime.seconds);
		} else {
			printf("time invalid!!\n");
		}

		usleep(600 * 1000);
	}
}
