import requests

url = "https://api.thingspeak.com/channels/1061866/feeds.json?results=2"

response = requests.get(url)

json = response.json()

info = f'{json["channel"]["field1"]}\t: {json["feeds"][0]["field1"]}' + '\t[°C]\n' + f'{json["channel"]["field2"]}\t: {json["feeds"][0]["field2"]}' + '\t[%RH]\n' + f'{json["channel"]["field3"]}\t: {json["feeds"][0]["field3"]}' + '\t[hPa]\n\n' + f'Data pull date\t: {json["feeds"][0]["created_at"]}'

print(info)

"""

{
    "channel":
    {
        "id":1061866,"name":"weatherStationofKorkmaz","latitude":"0.0","longitude":"0.0","field1":"Temperature","field2":"Humidity","field3":"Pressure","created_at":"2020-05-18T02:23:51Z","updated_at":"2020-05-18T02:25:47Z","last_entry_id":174
    },
    
    "feeds":
        [
            {"created_at":"2020-05-21T00:21:04Z","entry_id":173,"field1":"23.63534","field2":"42.85483","field3":"802.22998"},
            {"created_at":"2020-05-21T00:21:19Z","entry_id":174,"field1":"23.64606","field2":"43.15237","field3":"802.21997"}
        ]
}


"""