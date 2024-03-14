import pandas as pd
import pandas as pd
import glob
from datetime import datetime, timezone
stacja_id = 69132 # do zmiany samemu


def display_and_save_rows_with_station_number(input_filename, output_filename):
    pd.set_option('display.max_colwidth', None)
    pd.set_option('display.max_rows', None)
    pd.set_option('display.max_columns', None)
    pd.set_option('display.width', None)
    # Wczytaj plik CSV do obiektu DataFrame
    df = pd.read_csv(input_filename)

    # Wybierz wiersze, gdzie wartość kolumny 'station_number' to 69132
    filtered_df = df[(df['station_number'] == stacja_id) & 
                     (df['parameter'].isin(['Precip', 'T', 'RH']))]

    # Wyświetl wybrane wiersze
    print(filtered_df)
    filtered_df.to_csv(output_filename, index=False)
    

# Nazwa pliku CSV
input_filename = 'aws_hourly_20170501.csv'
output_filename = 'filtered_output.csv'

# Wywołaj funkcję
display_and_save_rows_with_station_number(input_filename, output_filename)


#wilgotnosc ziemi na podstawie opadu z dnia poprzedniego
#pressure
