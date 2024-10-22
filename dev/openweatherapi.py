import requests

# Replace with your OpenWeather API key

# Define the base URL for the OpenWeather API
BASE_URL = 'http://api.openweathermap.org/data/2.5/weather'

# Function to get weather data for a city
def get_weather(city):
    # Set up the parameters for the request
    params = {
        'q': city,
        'appid': API_KEY,
        'units': 'metric'  # For temperature in Celsius, change to 'imperial' for Fahrenheit
    }

    # Make the API request
    response = requests.get(BASE_URL, params=params)

    # Check if the request was successful
    if response.status_code == 200:
        data = response.json()
        print(data)
        # Extract relevant information
        weather_description = data['weather'][0]['description']
        temperature = data['main']['temp']
        humidity = data['main']['humidity']
        wind_speed = data['wind']['speed']

        # Print the weather data
        print(f"Weather in {city}:")
        print(f"Description: {weather_description}")
        print(f"Temperature: {temperature}°C")
        print(f"Humidity: {humidity}%")
        print(f"Wind Speed: {wind_speed} m/s")
    else:
        print(f"Error {response.status_code}: Unable to fetch weather data for {city}.")

# Example usage
if __name__ == "__main__":
    city_name = "Harwell, Oxford, UK"
    get_weather(city_name)
