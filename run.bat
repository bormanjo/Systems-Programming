docker build -t systems_programming ./dev_tools/
docker run -it -v "%cd%":/home/ systems_programming