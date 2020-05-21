import requests

url = "https://api.thingspeak.com/channels/1061866/feeds.json?results=2"

response = requests.get(url)

json = response.json()

info = f'{json["channel"]["field1"]}\t: {json["feeds"][0]["field1"]}' + '\t[°C]\n' + f'{json["channel"]["field2"]}\t: {json["feeds"][0]["field2"]}' + '\t[%RH]\n' + f'{json["channel"]["field3"]}\t: {json["feeds"][0]["field3"]}' + '\t[hPa]\n\n' + f'Data pull date\t: {json["feeds"][0]["created_at"]}'

print(info)

