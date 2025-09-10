all: build upload
CXXFLAGS:

practice: build CXXFLAGS += -PRACTICE
shell: EXTRACXXFLAGS+=-DSHELL
shell: build


red: EXTRA_CXXFLAGS+=-DRED_AUTON
red: build

blue: EXTRA_CXXFLAGS+=-DBLUE_AUTON
blue: build


left: EXTRA_CXXFLAGS+=-DLEFT_AUTON
left: build

right: EXTRA_CXXFLAGS+=-DRIGHT_AUTON
right: build

build: 
	echo "building..." | figlet | lolcat
	pros build EXTRA_CXXFLAGS+=$(CXXFLAGS) --project src

upload:
	echo "uploading..." | figlet | lolcat
	pros upload --project src
