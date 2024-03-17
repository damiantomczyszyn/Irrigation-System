from http.server import BaseHTTPRequestHandler, HTTPServer
import json

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/data2':
            # Odczytaj zawartość pliku
            try:
                with open('filtered_aws_05_01.json', 'r') as file:
                    data = json.load(file)
            except FileNotFoundError:
                self.send_response(404)
                self.end_headers()
                self.wfile.write(b'404 Not Found')
                return

        elif self.path == '/data1':
            # Odczytaj zawartość pliku
            try:
                with open('forecast_2017_05_01.json', 'r') as file:
                    data = json.load(file)
            except FileNotFoundError:
                self.send_response(404)
                self.end_headers()
                self.wfile.write(b'404 Not Found')
                return
        

        else:
            # Obsługa innych ścieżek URL
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'404 Not Found')
        
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

if __name__ == '__main__':
    run()

