from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
import socketserver
import subprocess

# Set the IP address and port for the Weather Monitor (Web Server)
weather_monitor_ip = "172.20.10.2"
weather_monitor_port = 1234

# Set the location for the Weather Station
# location = "Scotts+Valley"

# Define the Weather Monitor HTTP request handler
class WeatherMonitorHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/readfile/location.txt':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length).decode('utf-8')
            print(f"Received POST data: {post_data}")
            self.send_response(200)
            self.send_header('Content-type', 'text/plain') 
            self.end_headers()
            location = "Scotts+Valley\n"
            self.wfile.write("Data received by Weather Monitor".encode('utf-8'))
        elif self.path == "/get_weather":
            weather_response = requests.get(f'https://wttr.in/{location}?format=3')
            if weather_response.status_code == 200
                self.send_response(200)
                self.send_header('Content-type', 'text/plain; charset=utf-8')
                self.end_headers()
                self.wfile.write(weather_response.text.encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'Not Found')

    def do_POST(self):
        # Get the length of the incoming data
        if self.path == '/weather':
            content_length = int(self.headers['Content-Length'])
            
            # Read the POST data
            post_data = self.rfile.read(content_length).decode('utf-8')
    
            # Process the data (customize this part based on your needs)
            location = post_data.split(":")[0]
            temperature = post_data.split("T:")[1].split(",")[0]
            humidity = post_data.split("H:")[1].split("%")[0]
    
            # Generate the response string
            response = f"Received POST request:\n{post_data} \n"
    
            # Send the HTTP response
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(response.encode())
        else:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'Not Found')

        # Print the received data for debugging purposes
        # print(f"Received POST data: {post_data}")

# Start the Weather Monitor Web Server
# with socketserver.TCPServer((weather_monitor_ip, weather_monitor_port), WeatherMonitorHandler) as httpd:
    # print(f"Server started at http://{weather_monitor_ip}:{weather_monitor_port}")

    # Set up the Hotspot
    # hotspot_setup_command = f"sudo create_hotspot.sh {weather_monitor_ip} 255.255.255.0"
    # subprocess.run(hotspot_setup_command, shell=True)

    #try:
        #httpd.serve_forever()
    #except KeyboardInterrupt:
        #pass

def run(server_class = HTTPServer, handler_class = WeatherMonitorHandler, port=1234):
    # Set up the server with the handler and port
    server_address = ('172.20.10.2', port)
    httpd = server_class(server_address, handler_class)

# Print a message indicating the server is running
print(f'Starting Weather Monitor server on port {port}...')

# Start the server
httpd.serve_forever()

# When the server is terminated, stop the Hotspot
#hotspot_stop_command = "sudo stop_hotspot.sh"
#subprocess.run(hotspot_stop_command, shell=True)
if __name__ == '__main__':
    run()
