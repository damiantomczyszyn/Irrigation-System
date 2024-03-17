import pandas as pd
import json
import os

# Zdefiniuj mapowanie między nazwami parametrów w CSV a ich odpowiednikami w JSON
mapping = {
    'MeanT': 'Temperature',
    'MeanRH': 'Humidity',
    'PressureMean': 'Pressure',
    'soil_moisture_0_to_7cmMean': 'WaterLevel',
    'Precip24h': 'Precip24h'
}

# Pobierz listę plików CSV w folderze
csv_files = [file for file in os.listdir('.') if file.endswith('.csv')]

# Iteruj przez pliki CSV
for csv_file in csv_files:
    # Wczytaj plik CSV
    df = pd.read_csv(csv_file)

    # Inicjalizuj pusty słownik dla danych JSON
    data_json = {}

    # Iteruj przez mapowanie i dodaj odpowiednie wartości do danych JSON
    for csv_param, json_param in mapping.items():
        value = df[df['parameter'] == csv_param]['value'].iloc[0]  # Pobierz wartość dla danego parametru z DataFrame
        data_json[json_param] = value

    # Zapisz dane JSON do pliku
    json_filename = os.path.splitext(csv_file)[0] + '.json'  # Nazwa pliku JSON
    with open(json_filename, 'w') as json_file:
        json.dump(data_json, json_file, indent=4)

    print(f"Dane z pliku {csv_file} zapisane jako {json_filename}")
