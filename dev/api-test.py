import dotenv
import os
import requests
import json

dotenv.load_dotenv()
key = os.getenv('MAPS_API_KEY')
origin = os.getenv('MAPS_ORIGIN')
destination = os.getenv('MAPS_DESTINATION')


def main():
    url = 'https://routes.googleapis.com/directions/v2:computeRoutes'
    headers = {
        'Content-Type': 'application/json',
        'X-Goog-Api-Key': key,
        'X-Goog-FieldMask': 'routes.duration,routes.distanceMeters'
    }

    data = {
        "origin": {
            "address": origin
        },
        "destination": {
            "address": destination
        },
        "travelMode": "DRIVE",
        "routingPreference": "TRAFFIC_AWARE",
        "computeAlternativeRoutes": False,
        "routeModifiers": {
            "avoidTolls": False,
            "avoidHighways": False,
            "avoidFerries": False
        },
        "languageCode": "en-US",
        "units": "IMPERIAL",
        "routingPreference":"TRAFFIC_AWARE"
    }

    response = requests.post(url, headers=headers, data=json.dumps(data))

    print(response.status_code)
    print(response.json())

    dist = response.json()['routes'][0]["distanceMeters"] / 1609.34
    time_mins = int(response.json()['routes'][0]["duration"][0:-1]) / 60
    print("Time: ", time_mins, "minutes")
    print("Distance: ", dist, "miles")

if __name__ == '__main__':
    main()