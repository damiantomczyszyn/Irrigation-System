import json

treshold_wather_level = 0.2

# Otwieramy plik test.txt w trybie do odczytu
with open("test.txt", "r") as file:
    lines = file.readlines()  # Odczytujemy wszystkie linie
    
    # Tworzymy pustą listę, do której będziemy zapisywać wyniki
    podlewanie = []
    waterLevel = []

    skutecznosc = []
    
    # Ustawiamy flagę, która będzie zmieniała się co 3 linię
    sprawdzaj_linie = False
    
    # Iterujemy przez każdą linię
    for line in lines:
        if sprawdzaj_linie:  # Jeśli flaga jest ustawiona na True
            if line.strip():
                #print(line)
                #print(line)
                last_char = line.strip()[-1]
                # Sprawdzamy czy linia zawiera frazę "Podlewanie włączyło się"
                if last_char == '1':
                    podlewanie.append(1)  # Jeśli tak, dodajemy do listy wartość 1
                elif last_char == '0':
                    podlewanie.append(0)  # W przeciwnym razie dodajemy 0
                else:
                    podlewanie.append(-1)  # Blad
                sprawdzaj_linie = False  # Po sprawdzeniu zmieniamy flagę na False
        else:
            sprawdzaj_linie = True  # Jeśli flaga jest ustawiona na False, zmieniamy na True, aby sprawdzić co trzecią linię

# Wyświetlamy wynik
print("Wynik:", podlewanie)

# Funkcja do odczytywania wartości parametru "WaterLevel" z pliku JSON
def read_water_level(json_file):
    with open(json_file, "r") as file:
        data = json.load(file)  # Wczytujemy zawartość pliku JSON
        water_level = data["WaterLevel"]  # Pobieramy wartość parametru "WaterLevel"

        return water_level
# Iteracja po plikach JSON
for month in range(1, 13):
 
    for day in range(1, 32):  
        json_file = f"filtered_aws_{month:02d}_{day:02d}.json"  # Tworzymy nazwę pliku JSON
        try:
            water_level = read_water_level(json_file)  # Wywołujemy funkcję do odczytu wartości "WaterLevel"
            waterLevel.append(water_level)
            print(f"Dla pliku {json_file}: Wartość parametru WaterLevel: {water_level}")
        except FileNotFoundError:
            pass  # Ignorujemy pliki, które nie istnieją
        except KeyError:
            print(f"Dla pliku {json_file}: Brak parametru WaterLevel")



for i in range(0,151):
    if waterLevel[i] >= treshold_wather_level and podlewanie[i] == 0 or waterLevel[i] < treshold_wather_level and podlewanie[i] == 1:
        # jesli bylo mokro i nie padalo lub jesli bylo sucho i padalo 
        skutecznosc.append(1)
        print(waterLevel[i], "  ", podlewanie[i])
    else:
        skutecznosc.append(0)


print(skutecznosc)
    






