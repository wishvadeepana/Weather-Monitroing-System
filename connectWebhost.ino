void connect_webhost(){
  
//we have changing variable here, so we need to first build up our URL packet
URL_withPacket = URL_webhost;//pull in the base URL
//URL_withPacket += String(unit_id);//unit id value
//URL_withPacket += "?humidity=";// web address part
URL_withPacket += String(h);//sensor value
URL_withPacket += "&Temparature=";//web address part
URL_withPacket += String(t);//sensor value
URL_withPacket += "&Rainfall=";//web address part
URL_withPacket += String(raindropVal);//sensor value
URL_withPacket += payload_closer;
  
  
  /// This builds out the payload URL - not really needed here, but is very handy when adding different arrays to the payload
  counter=0;//keeps track of the payload size
  payload_size=0;
  for(int i=0; i<(URL_withPacket.length()); i++){//using a string this time, so use .length()
   payload[payload_size+i] = URL_withPacket[i];//build up the payload
    counter++;//increment the counter
  }//for int
   payload_size = counter+payload_size;//payload size is just the counter value - more on this when we need to build out the payload with more data
   //payload_size = sizeof(payload);
  Serial.print(payload_size);
    for(int i=0; i<payload_size; i++)//print the payload to the ESP
    Serial.print(payload[i]);
   connect_ESP();//this calls 'connect_ESP()' and expects a '1' back if successful
}//connect web host
