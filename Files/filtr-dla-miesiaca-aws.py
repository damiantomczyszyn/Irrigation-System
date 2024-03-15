import pandas as pd
import glob
from datetime import datetime, timezone
import requests

#http://www.bom.gov.au/climate/maps/averages/uv-index/?period=jan UV dla miesiaca 

stacja_id = 69132 # do zmiany samemu
rok = 2017
miesiac = 5 # do zmiany samemu
miesiac2 = "05"



def fetch_weather_data(latitude, longitude, start_date, end_date, parameters):
    url = f"https://archive-api.open-meteo.com/v1/archive?latitude={latitude}&longitude={longitude}&start_date={start_date}&end_date={end_date}&hourly={parameters}&timeformat=unixtime"
    
    response = requests.get(url)
    
    if response.status_code == 200:
        return response.json()
    else:
        print("Wystąpił problem podczas pobierania danych:", response.text)
        return None

def get_weather(dzien_g):
    latitude = -35.4415
    longitude = 149.7998
    start_date = "2017-05-" + dzien_g
    end_date = "2017-05-" + dzien_g
    parameters = "surface_pressure,soil_temperature_0_to_7cm,soil_temperature_7_to_28cm,soil_moisture_0_to_7cm,soil_moisture_7_to_28cm"

    weather_data = fetch_weather_data(latitude, longitude, start_date, end_date, parameters)

    if weather_data:
        print("Dane pogodowe zostały pomyślnie pobrane.")
        return weather_data



def filter_and_save_to_csv(input_filename, output_filename,epoch_time,api_data):
    # Wczytaj plik CSV do obiektu DataFrame
    df = pd.read_csv(input_filename)

    # Wybierz wiersze, gdzie wartość w kolumnie 'base_time' to 1493596800
    # i wartość w kolumnie 'station_number' to 1491048000
    # oraz wartość w kolumnie 'parameter' to jedna z wymienionych wartości
    filtered_df = df[(df['station_number'] == stacja_id) & 
                     (df['parameter'].isin(['Precip', 'T', 'RH']))]

    # Oblicz średnią dla wartości w kolumnie 'value', gdzie 'parameter' to 'T'
    average_value_T = filtered_df.loc[filtered_df['parameter'] == 'T', 'value'].mean()
    average_value_T = round(average_value_T, 2)

    # Oblicz średnią dla wartości w kolumnie 'value', gdzie 'parameter' to 'RH'
    average_value_RH = filtered_df.loc[filtered_df['parameter'] == 'RH', 'value'].mean()
    average_value_RH = round(average_value_RH, 2)

    # Wybierz wiersze, gdzie wartość kolumny 'parameter' to 'Precip'
    precip_values = df[df['parameter'] == 'Precip']['value']

    # Zsumuj wartości dla parametru 'Precip'
    precip_sum = precip_values.sum()

    # Dodaj nowy wiersz do ramki danych
    new_row = {
        'station_number': stacja_id,
        'unit': 'C',
        'parameter': 'MeanT',
        'value': average_value_T
    }
    filtered_df = filtered_df._append(new_row, ignore_index=True)
    # Dodaj nowy wiersz do ramki danych
    new_row = {
        'station_number': stacja_id,
        'unit': '%',
        'parameter': 'MeanRH',
        'value': average_value_RH
    }
    filtered_df = filtered_df._append(new_row, ignore_index=True)

    # Dodaj nowy wiersz do ramki danych
    new_row = {
        'station_number': stacja_id,
        'unit': '%',
        'parameter': 'Precip24h',
        'value': precip_sum
    }
    filtered_df = filtered_df._append(new_row, ignore_index=True)



    # Tworzymy DataFrame z danych godzinowych
    df_hourly = pd.DataFrame(api_data['hourly'])
    # Zmieniamy nazwę kolumny 'time' na 'valid_start'
    df_hourly = df_hourly.rename(columns={'time': 'valid_start'})

    # Konwertujemy kolumny na wartości wierszy
    df2 = df_hourly.melt(id_vars=['valid_start'], var_name='parameter', value_name='value')
    df2['station_number'] = stacja_id
    
    filtered_df = pd.concat([filtered_df, df2], ignore_index=True)
    
    # Zapisz wybrane wiersze do nowego pliku CSV
    filtered_df.to_csv(output_filename, index=False)

    print("Wiersze spełniające warunki zostały zapisane do pliku", output_filename)

# Wzorzec nazwy pliku

filename_pattern = 'aws_hourly_201705??.csv'

# Pętla dla każdego pliku zgodnego z wzorcem
for input_filename in glob.glob(filename_pattern):
    day_number = input_filename[-6:-4]  # Pobierz numer dnia z nazwy pliku
    output_filename = f'filtered_aws_{miesiac2}_{day_number}.csv'  # Utwórz nazwę pliku wyjściowego

    
    dzien = int(day_number)
    dzien_g = day_number
    #dzien_g2 = int(day_number)+1
    # Utwórz obiekt datetime dla określonej daty
    dt = datetime(rok, miesiac, dzien, tzinfo=timezone.utc)
    # Oblicz czas Epoch Unix dla tej daty
    unix_date = int(dt.timestamp())

    api_data = get_weather(dzien_g)
    
    
    filter_and_save_to_csv(input_filename, output_filename, unix_date, api_data)
