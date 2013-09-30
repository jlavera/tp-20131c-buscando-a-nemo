PATH := /home/utnso/git/tp-20131c-buscando-a-nemo

all: buscando

buscando:
	cd ./libs/lib_commons && make all
	cd ./libs/lib_buscanding && make all 
	cd ./libs/lib_gui && make all
	cd ./libs/lib_memoria && make all
	echo 'Librerias'
	cd ./src/nivel && make all
	cd ./src/personaje && make all
	cd ./src/plataforma && make all
	echo 'src'

clean:
	cd ./libs/lib_commons && make clean
	cd ./libs/lib_buscanding && make clean
	cd ./libs/lib_gui && make clean
	cd ./libs/lib_memoria && make clean
	echo 'Librerias'
	cd ./src/nivel && make clean
	cd ./src/personaje && make clean
	cd ./src/plataforma && make clean
	echo 'src'

export:
	$ export LD_LIBRARY_PATH=$(PATH)/libs/lib_commons:$(PATH)/libs/lib_buscanding:$(PATH)/libs/lib_gui:$(PATH)/libs/lib_memoria