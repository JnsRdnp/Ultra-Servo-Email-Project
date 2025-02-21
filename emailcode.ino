#include <Arduino.h>
#if defined(ESP32)
  #include <Wifi.h>
#elif defined(ESP8266)
  #include <ESP8266Wifi.h>
#endif
#include <ESP_Mail_Client.h>


#define WIFI_SSID "kk"
#define WIFI_PASSWORD ""

#define SMTP_HOST "smpt.gmail.com"
#define SMTP_PORT 465

/* Sign in credentials */
#define AUTHOR_EMAIL "emailservolcdrfidproject@gmail.com"
#define AUTHOR_PASSWORD ""

#define RECIPIENT_EMAIL "emailservolcdrfidproject@gmail.com"

/* SMTP Session object for sending email*/
SMTPSession smpt;

/* Callback function to get Email sending status */
void smtpCallback(SMTP_Status status);


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status()!= WL_CONNECTED) {
    Serial.print(".");
    delay(200)
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback funtion to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session configuration */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare message class */
  SMTP_Message message;

  /* Message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Arduino!", RECIPIENT_EMAIL);

  //Send raw text message
  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = 
  Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::
  esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | 
  esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  // Connect to server with session config
  if (!smtp.connect(&session))
    return;
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending email, " + smtp.errorReason());

}

void loop() {
  // put your main code here, to run repeatedly:

}

void smtpCallback(SMTP_Status status) {
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
  }
}
