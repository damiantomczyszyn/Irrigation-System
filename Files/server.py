from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import os

folderA = "forecast_jsons"
folderB = "station_json"

# Globalne zmienne przechowujące listy plików
files_A = []
files_B = []

# Globalne zmienne przechowujące aktualne indeksy plików
current_index_A = 0
current_index_B = 0


class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        global current_index_A, current_index_B

        if self.path == '/data2':
            if current_index_B < len(files_B):
                filename = os.path.join(folderB, files_B[current_index_B])
                print(filename)
                current_index_B += 1
            else:
                self.send_response(404)
                self.end_headers()
                self.wfile.write(b'404 Not Found')
                return

        elif self.path == '/data1':
            if current_index_A < len(files_A):
                filename = os.path.join(folderA, files_A[current_index_A])
                print(filename)
                current_index_A += 1
            else:
                self.send_response(404)
                self.end_headers()
                self.wfile.write(b'404 Not Found')
                return

        else:
            # Obsługa innych ścieżek URL
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'404 Not Found')
            return

        try:
            with open(filename, 'r') as file:
                data = json.load(file)
        except FileNotFoundError:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'404 Not Found')
            return

        # Ustaw nagłówki odpowiedzi HTTP
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

        # Zwróć zawartość pliku jako dane JSON
        self.wfile.write(json.dumps(data).encode())


def run(server_class=HTTPServer, handler_class=RequestHandler, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f'Starting httpd on port {port}...')
    httpd.serve_forever()


def load_files():
    global files_A, files_B
    files_A = os.listdir(folderA)
    files_A.sort()
    files_B = os.listdir(folderB)
    files_B.sort()


if __name__ == '__main__':
    load_files()
    run()

