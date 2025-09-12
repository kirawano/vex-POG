all: build upload

practice: CXXFLAGS+=-DPRACTICE
practice: loud all

shell: CXXFLAGS+=-DSHELL
shell: all

red_left: CXXFLAGS+=-DRED_AUTON -DLEFT_AUTON
red_left: all

red_right: CXXFLAGS+=-DRED_AUTON -DRIGHT_AUTON
red_right: all

blue_left: CXXFLAGS+=-DBLUE_AUTON -DLEFT_AUTON
blue_left: all

blue_right: CXXFLAGS+=-DBLUE_AUTON -DRIGHT_AUTON
blue_right: all

build: 
	echo "building..." | figlet | lolcat
	pros build --project src EXTRA_CXXFLAGS+='$(CXXFLAGS)'

upload:
	echo "uploading..." | figlet | lolcat
	pros upload --project src
