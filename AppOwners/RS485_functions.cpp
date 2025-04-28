
#include "RS485_functions.h"
#include "auxiliary_functions.h"
#include <unordered_map>
#include <string>


void StartModbusReading(fs::FS& fs,JsonDocument& registerMap , JsonDocument& bucketTemp,const char * bucket_path,ESP32_RS485** configRS485ref, int lintItem){ 
        Serial.println("StartModbusReading");

        int bucketSize = 0;
        int sucessDevices = 0;
        int erroDevices = 0;
        std::unordered_map<std::string, std::string> larrAbntId;
        int i = lintItem;
        //for (int i = 0; i < registerMap[0]["list"].size(); i++) {

                Serial.printf("Lendo ID %s\r\n",registerMap[0]["list"][i]["id"].as<String>());
                Serial.printf("Registro %s\r\n",registerMap[0]["list"][i]["reg"].as<String>());
                String addr = "";
                String cleanedStr="";
                String strError;
                String strRegister;
                if (registerMap[0]["list"][i]["prot"] == 1) {
                        
                        for (int iTry = 1; iTry <= 2; iTry++) {
                                Serial.println("TENTIVA :" + String(iTry)  );              
                                strRegister = configRS485ref[0]->modbus_read_register(registerMap[0]["list"][i]["id"].as<int>(), registerMap[0]["list"][i]["func"].as<int>(), registerMap[0]["list"][i]["addr"].as<int>(), registerMap[0]["list"][i]["reg"].as<int>(),registerMap[0]["list"][i]["check_length"].as<int>(),registerMap[0]["list"][i]["check_crc16"].as<int>(), strError);
                                Serial.println("REGISTER :" + String(strRegister) + " ERROR:" + strError  ); 

                                if( strRegister == "FFFF" && (strError == "00F0" || strError=="00F3") ){
                                        delay(1000);
                                }else{
                                        break;
                                }
                        }


                        cleanedStr = removeSpecialCharacters(strRegister);

                        int tamanhoID = registerMap[0]["list"][i]["id"].as<String>().length();
                        String id = registerMap[0]["list"][i]["id"].as<String>();
                        if (tamanhoID <= 2) {
                                id = "0" + id;
                        }
                        int tamanhoFunc = registerMap[0]["list"][i]["func"].as<String>().length();
                        String func = registerMap[0]["list"][i]["func"].as<String>();
                        if (tamanhoFunc <= 2) {
                                func = "0" + func;
                        }
                        int tamanhoPort = registerMap[0]["list"][i]["port"].as<String>().length();
                        String port = registerMap[0]["list"][i]["port"].as<String>();
                        if (tamanhoPort <= 2) {
                                port = "0" + port;
                        }
                        addr = registerMap[0]["list"][i]["addr"].as<String>();
                        int tamanhoAddr = addr.length();
                        if (tamanhoAddr < 4) {
                                int zerosToAdd = 4 - tamanhoAddr;
                                addr = String("0" + addr);
                                for (int ii = 0; ii < zerosToAdd - 1; ii++) {
                                        addr = String("0" + addr);
                                }
                        }
                        
                        sucessDevices += 1;
                        bucketTemp[bucketSize]["id"] = String(id);
                        bucketTemp[bucketSize]["addr"] = String(addr);
                        bucketTemp[bucketSize]["func"] = String(func);
                        bucketTemp[bucketSize]["port"] = String(port);
                        bucketTemp[bucketSize]["n_reg"] = String(registerMap[0]["list"][i]["reg"].as<int>());
                        bucketTemp[bucketSize]["register"] = String(cleanedStr.c_str());
                        bucketTemp[bucketSize]["error"] =String(strError.c_str());
                        bucketSize += 1;

                        Serial.println("Bucket :" + String(bucketSize)  ); 
                }
                if (registerMap[0]["list"][i]["prot"] == 2) {
                        String current_id   = registerMap[0]["list"][i]["id"].as<String>();
                        int    current_func = registerMap[0]["list"][i]["func"].as<int>();
                        
                       
                        std::string key = std::string(current_id.c_str()) + "-" + std::to_string(current_func);
                        std::string strRegister = larrAbntId[key];
                        
                        if ( strRegister == "") {
                        
                                for(int i = 1; i <= 3 ; i++){
                                        String strRegisterTmp = configRS485ref[0]->abnt1452_read_register(current_func, current_id, false);
                                        strRegister = strRegisterTmp.c_str();

                                        if(strRegister.length() >= 247){
                                                Serial.println("SUCESS TENTIVA " + String(i) + " DE 3 " + String(current_id) + " FUNC " + String(current_func));
                                                Serial.println("READ " + String(strRegister.c_str()));
                                                break;
                                        }else{
                                                Serial.println("TENTIVA " + String(i) + " DE 3 " + String(current_id) + " FUNC " + String(current_func));
                                                strRegister = "";
                                        }
                                        delay(500);
                                }
                                if ( strRegister != "") {
                                        larrAbntId[key] = strRegister;
                                }
                        }
                        
                        
                        
                        int addr = registerMap[0]["list"][i]["addr"].as<int>();
                        int n_reg = registerMap[0]["list"][i]["reg"].as<int>();
                        
                        bucketTemp[bucketSize]["id"]    =  String(current_id);
                        bucketTemp[bucketSize]["addr"]  = registerMap[0]["list"][i]["addr"].as<String>();
                        bucketTemp[bucketSize]["func"]  = registerMap[0]["list"][i]["func"].as<String>();
                        bucketTemp[bucketSize]["port"]  = registerMap[0]["list"][i]["port"].as<String>();
                        bucketTemp[bucketSize]["n_reg"] = registerMap[0]["list"][i]["n_reg"].as<String>();

                        
                        if ( strRegister != "") {
                                bucketTemp[bucketSize]["register"] = String(strRegister.c_str()).substring(addr,addr + n_reg);
                        } else {
                                bucketTemp[bucketSize]["register"] = "FFFF";
                                bucketTemp[bucketSize]["error"] = "00F3";
                        }
                        bucketSize += 1;
                }   

                delay(1000);
        //}

        delay(2000);      
        jsonWriteFile(fs, bucket_path, bucketTemp);
        Serial.println("EndModbusReading");

        
}

void StartAbnt14522Reading(fs::FS& fs, JsonDocument& registerMap, JsonDocument& bucketTemp, const char* bucket_path, ESP32_RS485** configRS485ref) {
    int bucketSize = 0;
    int successDevices = 0;
    int errorDevices = 0;

    String last_id;
    int last_func = -1; 

    for (int i = 0; i < registerMap[0]["list"].size(); i++) {

        int    current_func = registerMap[0]["list"][i]["func"].as<int>();
        String current_id   = registerMap[0]["list"][i]["id"].as<String>();

        if (current_func != last_func || current_id != last_id) {
            String strRegister;
            for(int i = 1; i <= 3 ; i++){
                strRegister = configRS485ref[0]->abnt1452_read_register(current_func, current_id, false);
                if(strRegister.length() >= 247){
                        Serial.println("SUCESS ");
                        Serial.println("TENTIVA  " + strRegister + "DE 3" );
                        Serial.println("READ " + strRegister);
                        break;
                }else{
                        Serial.println("ERROR ");
                        Serial.println("READ " + strRegister);
                        Serial.println("TENTIVA  " + strRegister + "DE 3" );
                        strRegister = "";
                }
            }

            if(strRegister = ""){
                    Serial.println("ERROR  DEVICE" + current_id + "FUNC" + current_func);
                    errorDevices++;
            }else{
                    Serial.println("SUCESS  DEVICE" + current_id + "FUNC" + current_func);
                    successDevices++;
            }

            for (int j = 0; j < registerMap[0]["list"].size(); j++) {

                if (registerMap[0]["list"][j]["id"].as<String>() == current_id && registerMap[0]["list"][j]["func"].as<int>() == current_func) {

                    int addr  = registerMap[0]["list"][j]["addr"].as<int>();
                    int n_reg = registerMap[0]["list"][j]["n_reg"].as<int>();
                    
                    bucketTemp[bucketSize]["id"]        = current_id;
                    bucketTemp[bucketSize]["addr"]      = addr;
                    bucketTemp[bucketSize]["register"]  = "AAA";
                    bucketSize++;
                }

            }
            last_id   = current_id;
            last_func = current_func;
        }

    }
    delay(2000);
    jsonWriteFile(fs, bucket_path, bucketTemp);
    
    for (int j = 0; j < bucketTemp.size(); j++) {

        Serial.println(bucketTemp[j]["id"].as<String>());
        Serial.println(bucketTemp[j]["addr"].as<String>());
        // Serial.println(bucketTemp[j]["register"].as<String>());

    }
    delay(50000);

}