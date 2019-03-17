./bin/out: ./main.cpp
	g++ -o ./bin/out ./main.cpp -I/home/pi/Urho3D/include -I/home/pi/Urho3D/include/Urho3D/ThirdParty -I/usr/include -L/home/pi/Urho3D/lib -l Urho3D -l dl -pthread -lGL
