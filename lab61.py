from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
import socketserver
import subprocess

# Set the IP address and port for the Weather Monitor (Web Server)
weather_monitor_ip = "192.168.157.66"
weather_monitor_port = 1234

# Set the location for the Weather Station
location = "Scotts+Valley"

# Define the Weather Monitor HTTP request handler
class WeatherMonitorHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/readfile/location.txt':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length).decode('utf-8')
            print(f"Received POST data: {post_data}")
            self.send_response(200)
            self.send_header('Content-type', 'text/plain') 
            self.wfile.write("Data received by Weather Monitor".encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()


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
            response = f"{post_data}: Outdoors T:47F, H:36%, Indoors T:70F, H:40%"
    
            # Send the HTTP response
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(response.encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()

        # Print the received data for debugging purposes
        print(f"Received POST data: {post_data}")

# Start the Weather Monitor Web Server
with socketserver.TCPServer((weather_monitor_ip, weather_monitor_port), WeatherMonitorHandler) as httpd:
    print(f"Server started at http://{weather_monitor_ip}:{weather_monitor_port}")

    # Set up the Hotspot
    # hotspot_setup_command = f"sudo create_hotspot.sh {weather_monitor_ip} 255.255.255.0"
    # subprocess.run(hotspot_setup_command, shell=True)

    #try:
        #httpd.serve_forever()
    #except KeyboardInterrupt:
        #pass

# Set up the server with the handler and port
server_address = ('192.168.157.66', 1234)
httpd = HTTPServer(server_address, WeatherMonitorHandler)

# Print a message indicating the server is running
print('Weather Monitor server is running on port 1234...')

# Start the server
httpd.serve_forever()

# When the server is terminated, stop the Hotspot
#hotspot_stop_command = "sudo stop_hotspot.sh"
#subprocess.run(hotspot_stop_command, shell=True)
