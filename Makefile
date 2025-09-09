all: build upload

build:
	echo "building..." | figlet | lolcat
	(cd src ; pros build)

upload:
	echo "uploading..." | figlet | lolcat
	(cd src ; pros upload)
