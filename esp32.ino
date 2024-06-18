#include <WiFi.h>
#include <PubSubClient.h>

//Case de exemplo:
const float densidade = 1000.0; // Densidade da água em kg/m³
const float gravidade = 9.81; // Aceleração da gravidade em m/s²
int vazao; // Vazão em m³/s - O valor seria substituído pelo sensor;
const float altura = 10.0; // Altura da queda d'água em metros - 10 metros - O valor seria substituído pelo sensor;
float eficiencia; // Eficiência do sistema (valor entre 0 a 100%) - 85% 
float energia_acumulada; // Potência hidráulica em watts


// Configuração do WIFI
const char* ssid = "";
const char* password = "";


// Configuração do MQTT
const char* server = "";
const char* topic_energy = "/energy";
const char* topic_vazao = "/vazao";
const char* topic_price = "/price";
const char* clientName = "globalsolution";

String payload;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void wifiConnect() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttReConnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientName)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttEmit(String topic, String value)
{
  client.publish((char*) topic.c_str(), (char*) value.c_str());
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  wifiConnect();
  client.setServer(server, 1883);
  delay(1500);
}

void loop() {
  if (!client.connected()) {
    mqttReConnect();
  }

  //Calculo de energia
  int efficiencyPercentage = random(80, 90); 
  eficiencia = efficiencyPercentage / 100.0;
  vazao = random(2 , 10);
  energia_acumulada = round((densidade * gravidade * vazao * altura * eficiencia)) / 10;

  float energia_kWh = energia_acumulada / 1000.0; // Converter watts para kilowatts
  float custoEnergia = energia_kWh * 0.40773;
  int new_energia = round(energia_kWh);
  int new_custo = round(custoEnergia);

/*
  Serial.print("O valor de energia em kW: ");
  Serial.println(new_energia);
  Serial.print("Vazão em m/s: ");
  Serial.println(vazao);
  Serial.print("Precificação da energia gerada: R$");
  Serial.println(new_custo);
*/
  mqttEmit(topic_energy, (String) new_energia);
  mqttEmit(topic_vazao, (String) vazao);
  mqttEmit(topic_price, (String) new_custo);

  delay(5000);
}