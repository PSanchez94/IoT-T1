#include <stdio.h>
#include <string.h>



unsigned char * serialize_char(unsigned char *buffer, char value)
{
  buffer[0] = value;
  return buffer + 1;
}


/*
unsigned char * serialize_bytes(unsigned char *buffer, int value) {
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;
  return buffer + 4;
}

/*
typedef struct Header {

    int16_t id_device;
    int64_t mac; // 8 bytes instead of 6
    int8_t id_protocol;
    int16_t len;
    
};


typedef struct Data0 {
    int8_t status=1;
    int8_t batt_lvl;
    int8_t timestamp;
};

typedef struct Data1 {
    int8_t temp;
    int64_t press;
    int8_t hum;
    int64_t co;
};

typedef struct Data2 {
    int64_t rms;  
};

typedef struct Data3 {
    int64_t x_amp;
    int64_t x_frec;
    int64_t y_amp;
    int64_t y_frec;
    int64_t z_amp;
    int64_t z_frec;
};
/**/


/*
typedef struct ConfigData {
    int8_t status;
    int8_t ID_Protocol;
    int32_t BMI270_Sampling;
    int32_t BMI270_Acc_Sensibility; 
    int32_t BMI270_Gyro_Sensibility; 
    int32_t BME688_Sampling;
    int32_t Discontinuous_Time;
    int32_t Port_TCP;
    int32_t Port_UDP;
    int32_t Host_Ip_Addr;
    int32_t Ssid;
    int32_t Pass;
} SensorData;


typedef struct SensorData {
    int8_t Batt_level;
    int8_t Timestamp[4];
    int8_t Temp;
    int8_t Press[4];
    int8_t Hum;
    int8_t Co[4];
    int8_t RMS[4];
    int8_t Amp_x_bmi[4];
    int8_t Frec_x_bmi[4];
    int8_t Amp_y_bmi[4];
    int8_t Frec_y_bmi[4];
    int8_t Amp_z_bmi[4];
    int8_t Frec_z_bmi[4];

    int16_t BMI270_Acc_x[2000]; 
    int16_t BMI270_Acc_y[2000];
    int16_t BMI270_Acc_z[2000];
    int16_t BMI270_Gir_x[2000];
    int16_t BMI270_Gir_y[2000];
    int16_t BMI270_Gir_z[2000];
} SensorData;
/**/
