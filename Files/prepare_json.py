import pandas as pd
import json

# Wczytanie danych z plików CSV




#dzien="01"
miesiac="09"

for i in range(1, 30):
    day = str(i).zfill(2)  
    dzien=str(i).zfill(2)
    dzien2=str(i+1).zfill(2)
    dzien3=str(i+2).zfill(2)
        

    df1 = pd.read_csv(f'filtered_aws_{miesiac}_{dzien}.csv')
    df2 = pd.read_csv(f'filtered_aws_{miesiac}_{dzien2}.csv')
    df3 = pd.read_csv(f'Op_filtered_2017_{miesiac}_{dzien3}.csv')
    print(i+1)


    # Funkcja do mapowania danych z DataFrame do formatu JSON dla pojedynczego pliku CSV
    def map_to_json(df):
        # Inicjalizacja pustej listy dni
        days = []
        
        # Wczytanie danych do listy dni
        for index, row in df.iterrows():
            day = {
                "datetime": row['valid_start'],
                "temp": df[df['parameter'] == 'MeanT']['value'].iloc[0],
                "humidity": df[df['parameter'] == 'MeanRH']['value'].iloc[0],
                "precip": df[df['parameter'] == 'Precip24h']['value'].iloc[0],
                "pressure": df[df['parameter'] == 'PressureMean']['value'].iloc[0],
                "precipprob": 100 if df[df['parameter'] == 'Precip24h']['value'].iloc[0] != 0 else 0
            }
            days.append(day)
        
        return days

    # Tworzenie danych JSON
    json_data = {
        "days": [
            {
                "datetime": df1['valid_start'].iloc[0],
                "temp": df1[df1['parameter'] == 'MeanT']['value'].iloc[0],
                "humidity": df1[df1['parameter'] == 'MeanRH']['value'].iloc[0],
                "precip": df1[df1['parameter'] == 'Precip24h']['value'].iloc[0],
                "pressure": df1[df1['parameter'] == 'PressureMean']['value'].iloc[0],
                "precipprob": 100 if df1[df1['parameter'] == 'Precip24h']['value'].iloc[0] != 0 else 0
            },
            {
                "datetime": df2['valid_start'].iloc[0],
                "temp": df2[df2['parameter'] == 'MeanT']['value'].iloc[0],
                "humidity": df2[df2['parameter'] == 'MeanRH']['value'].iloc[0],
                "precip": df2[df2['parameter'] == 'Precip24h']['value'].iloc[0],
                "pressure": df2[df2['parameter'] == 'PressureMean']['value'].iloc[0],
                "precipprob": 100 if df2[df2['parameter'] == 'Precip24h']['value'].iloc[0] != 0 else 0
            },
            {
                "datetime": df3['valid_start'].iloc[0],
                "temp": df3[df3['parameter'] == 'MeanT']['value'].iloc[0],
                "humidity": df3[df3['parameter'] == 'MeanRH']['value'].iloc[0],
                "precip": df3[df3['parameter'] == 'DailyPrecip']['value'].iloc[0],
                "precipprob": df3[df3['parameter'] == 'DailyPoP']['value'].iloc[0]
            }
        ]
    }


    # Zapisanie danych JSON do pliku
    with open(f'forecast_2017_{miesiac}_{dzien2}.json', 'w') as json_file:
        json.dump(json_data, json_file, indent=4)

    print("Plik JSON został pomyślnie wygenerowany.")
    
