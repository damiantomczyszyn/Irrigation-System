import pandas as pd
import glob
from datetime import datetime, timezone

#http://www.bom.gov.au/climate/maps/averages/uv-index/?period=jan UV dla miesiaca 

stacja_id = 69132 # do zmiany samemu
rok = 2017
miesiac = 5 # do zmiany samemu
miesiac2 = "05"
average_value_Uv = 11 # do sprawdzenia i zmiany dla miesiaca

def filter_and_save_to_csv(input_filename, output_filename,epoch_time):
    # Wczytaj plik CSV do obiektu DataFrame
    df = pd.read_csv(input_filename)

    # Wybierz wiersze, gdzie wartość w kolumnie 'base_time' to 1493596800
    # i wartość w kolumnie 'station_number' to 1491048000
    # oraz wartość w kolumnie 'parameter' to jedna z wymienionych wartości
    filtered_df = df[(df['base_time'] == epoch_time) & 
                     (df['station_number'] == stacja_id) & 
                     (df['parameter'].isin(['DailyPoP', 'T', 'DailyPrecip', 'RH']))]

    # Oblicz średnią dla wartości w kolumnie 'value', gdzie 'parameter' to 'T'
    average_value_T = filtered_df.loc[filtered_df['parameter'] == 'T', 'value'].mean()
    average_value_T = round(average_value_T, 2)


    # Oblicz średnią dla wartości w kolumnie 'value', gdzie 'parameter' to 'RH'
    average_value_RH = filtered_df.loc[filtered_df['parameter'] == 'RH', 'value'].mean()
    average_value_RH = round(average_value_RH, 2)


    
    # Dodaj nowy wiersz do ramki danych
    new_row = {
        'base_time': epoch_time,
        'station_number': stacja_id,
        'unit': 'C',
        'parameter': 'MeanT',
        'value': average_value_T
    }
    filtered_df = filtered_df._append(new_row, ignore_index=True)

    # Dodaj nowy wiersz do ramki danych
    new_row = {
        'base_time': epoch_time,
        'station_number': stacja_id,
        'unit': '%',
        'parameter': 'MeanRH',
        'value': average_value_RH
    }
    filtered_df = filtered_df._append(new_row, ignore_index=True)
    
    new_row = {
        'base_time': epoch_time,
        'station_number': stacja_id,
        'unit': 'UvIndex',
        'parameter': 'MonthMeanUv',
        'value': average_value_Uv
    }
    filtered_df = filtered_df._append(new_row, ignore_index=True)


    # Zapisz wybrane wiersze do nowego pliku CSV
    filtered_df.to_csv(output_filename, index=False)

    print("Wiersze spełniające warunki zostały zapisane do pliku", output_filename)

# Wzorzec nazwy pliku
filename_pattern = 'Op_Official_201705??.csv'

# Pętla dla każdego pliku zgodnego z wzorcem
for input_filename in glob.glob(filename_pattern):
    day_number = input_filename[-6:-4]  # Pobierz numer dnia z nazwy pliku
    output_filename = f'Op_Filtered_{rok}_{miesiac2}_{day_number}.csv'  # Utwórz nazwę pliku wyjściowego


    dzien = int(day_number)
    # Utwórz obiekt datetime dla określonej daty
    dt = datetime(rok, miesiac, dzien, tzinfo=timezone.utc)
    # Oblicz czas Epoch Unix dla tej daty
    unix_date = int(dt.timestamp())
    

    
    filter_and_save_to_csv(input_filename, output_filename, unix_date)
