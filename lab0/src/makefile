build-sharpen: ./a.out

./a.out: image_sharpener.cpp libppm.cpp
	g++ -g image_sharpener.cpp libppm.cpp

run-sharpen: ./a.out
	bash average_times.sh $(INPUT) $(OUTPUT)

clean:
	rm a.out
