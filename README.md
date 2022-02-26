# Stahanovec

## Build

```
docker build -t stahanovec .
```
	
## Run

```
docker run --restart=always -e TARGET=192.168.1.206 -e PORT=8080 -e DELAY_MS=300 -e THREADS=4 -it stahanovec
```
