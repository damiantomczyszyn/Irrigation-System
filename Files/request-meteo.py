import requests

def fetch_weather_data(latitude, longitude, start_date, end_date, parameters):
    url = f"https://archive-api.open-meteo.com/v1/archive?latitude={latitude}&longitude={longitude}&start_date={start_date}&end_date={end_date}&hourly={parameters}"
    
    response = requests.get(url)
    
    if response.status_code == 200:
        return response.json()
    else:
        print("Wystąpił problem podczas pobierania danych:", response.text)
        return None

def main():
    latitude = -35.4415
    longitude = 149.7998
    start_date = "2017-05-01"
    end_date = "2017-05-02"
    parameters = "surface_pressure,soil_temperature_0_to_7cm,soil_temperature_7_to_28cm,soil_moisture_0_to_7cm,soil_moisture_7_to_28cm"

    weather_data = fetch_weather_data(latitude, longitude, start_date, end_date, parameters)

    if weather_data:
        print("Dane pogodowe zostały pomyślnie pobrane.")

        # Wyświetl parametr hourly
        hourly_data = weather_data.get('hourly', None)
        if hourly_data:
            print("Parametr 'hourly' z danych pogodowych:")
            print(hourly_data)
        else:
            print("Brak danych dla parametru 'hourly'.")

if __name__ == "__main__":
    main()
