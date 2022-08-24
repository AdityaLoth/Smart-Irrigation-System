#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2); 

int servoPin1 = 13; 
int servoPin2 = 3; 
int servoPin3 = 12; 
int servoPin4 = 2; 
Servo Servo1;
Servo Servo2; 
Servo Servo3;
Servo Servo4;

const int ldrPin1 = A8;
const float GAMMAA = 0.7;
const float RL10 = 50;

const int dhtPin1= 49;
const int dhtPin2= 47;
const int dhtPin3= 53;
const int dhtPin4= 43;
DHT dht1(dhtPin1, DHT22);
DHT dht2(dhtPin2, DHT22);
DHT dht3(dhtPin3, DHT22);
DHT dht4(dhtPin4, DHT22);

int pertodeg(int a){
  return a*180/100;
}

float relu(float x)
{
    // Relu Activation Funtion --
    // MAX(0,x)
    if(x>0)
    return x;
    return 0;
}

float predict(float humidity, float temperature)
{
    // weights and bias of model. 
    float layer1_weights[2][3] = {{-0.491866  , -1.0614316 , -0.4307963 }, {-0.01059842,  0.07211161,  1.3132615 }};
    float layer1_bias[1][3] = {0.        , 0.        , 0.08891865};
    
    float layer2_weights[3][2] = {{-0.87787795,  0.5774244 }, { 1.0744598 ,  0.7851182 },{-0.44219005,  0.95790935}};
    float layer2_bias[1][2] = {0.        , 0.11467338};
    
    float layer3_weights[2] = {-0.6685886, 0.7450541};
    float layer3_bias = 0.13448663;
    
    // make prediction
    float layer1_op[3];
    float layer2_op[2];
    /// Calculate layer one output.
    for(int i=0; i<3; i++)
    {
        float x = humidity*layer1_weights[0][i] + temperature*layer1_weights[1][i] + layer1_bias[0][i];
        layer1_op[i] = relu(x);
    }
    /// Calculate layer two output.
    for(int i=0; i<2; i++)
    {
        float x = layer1_op[0]*layer2_weights[0][i]  + layer1_op[1]*layer2_weights[1][i]  + layer1_op[2]*layer2_weights[2][i]  + layer2_bias[0][i];
        layer2_op[i] = relu(x);
    }
    
    // predict waterflow percentage.
    float prediction = layer3_weights[0]*layer2_op[0] + layer3_weights[1]*layer2_op[1] + layer3_bias;
    if(temperature <= 35)
      return(0);
    return(prediction);
}





void setup() {
 
  Wire.begin();
  Serial.begin(9600);
  Servo1.attach(servoPin1); 
  Servo2.attach(servoPin2); 
  Servo3.attach(servoPin3); 
  Servo4.attach(servoPin4);

  pinMode(ldrPin1, INPUT); 
  dht1.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();

  lcd.init();    
  lcd.backlight();
}


void loop() {
  // put your main code here, to run repeatedly:
int ldr = analogRead(ldrPin1);

float voltage = ldr / 1024. * 5;
float resistance = 2000 * voltage / (1 - voltage / 5);
float lux = pow(RL10 * 1e3 * pow(10,0.7) / resistance, (1 / 0.7));

if (lux<=50) 
{
  Servo1.write(0); 
  Servo2.write(0);
  Servo3.write(0);
  Servo4.write(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1:");
  lcd.print("0");
  lcd.setCursor(10, 0);
  lcd.print("2:"); 
  lcd.print("0");
  lcd.setCursor(0, 12); 
  lcd.print("3:");
  lcd.print("0");
  lcd.setCursor(10, 12); 
  lcd.print("4:");
  lcd.print("0");

}
else{
  float t1 = dht1.readTemperature(); // Gets the values of the temperature
  float h1 = dht1.readHumidity(); // Gets the values of the humidity
  delay(50);
  float t2 = dht2.readTemperature(); // Gets the values of the temperature
  float h2 = dht2.readHumidity();
  delay(50);
  float t3 = dht3.readTemperature(); // Gets the values of the temperature
  float h3 = dht3.readHumidity();
  delay(50);
  float t4 = dht4.readTemperature(); // Gets the values of the temperature
  float h4 = dht4.readHumidity();

  float p1=predict(h1,t1);
  float p2=predict(h2,t2);
  float p3=predict(h3,t3);
  float p4=predict(h4,t4);

lcd.setCursor(0, 0);
lcd.print("1:"); 
lcd.print(p1);
lcd.setCursor(10, 0);
lcd.print("2:"); 
lcd.print(p2);
lcd.setCursor(0, 12); 
lcd.print("3:");
lcd.print(p3);
lcd.setCursor(10, 12); 
lcd.print("4:");
lcd.print(p4);
int d1=pertodeg(p1);
int d2=pertodeg(p2);
int d3=pertodeg(p3);
int d4=pertodeg(p4);  
  Servo1.write(d1); 
  Servo2.write(d2);
  Servo3.write(d3);
  Servo4.write(d4);
}
delay(700);



}

